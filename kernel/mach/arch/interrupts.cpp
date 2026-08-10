/**
 * @file interrupts.cpp
 * @brief Interrupt service routine dispatchers and registration for Nebula OS
 * @author Nebula OS Team
 */

#include <mach/arch/interrupts.hpp>
#include <mach/arch/idt.hpp>
#include <mach/arch/pic.hpp>
#include <iokit/serial/serial.hpp>
#include <mach/sched/scheduler.hpp>

extern "C" {
    // 32 CPU Exception Stubs
    void isr0();  void isr1();  void isr2();  void isr3();
    void isr4();  void isr5();  void isr6();  void isr7();
    void isr8();  void isr9();  void isr10(); void isr11();
    void isr12(); void isr13(); void isr14(); void isr15();
    void isr16(); void isr17(); void isr18(); void isr19();
    void isr20(); void isr21(); void isr22(); void isr23();
    void isr24(); void isr25(); void isr26(); void isr27();
    void isr28(); void isr29(); void isr30(); void isr31();
    void isr128();

    // 16 IRQ Hardware Stubs
    void irq0();  void irq1();  void irq2();  void irq3();
    void irq4();  void irq5();  void irq6();  void irq7();
    void irq8();  void irq9();  void irq10(); void irq11();
    void irq12(); void irq13(); void irq14(); void irq15();

    uint32_t isr_handler(nebula::arch::x86_64::registers_t* regs);
    uint32_t irq_handler(nebula::arch::x86_64::registers_t* regs);
}

namespace nebula {
namespace arch {
namespace x86_64 {

using nebula::drivers::Serial;

typedef void (*isr_stub_t)();

// Array of 32 CPU Exception stub function pointers
static const isr_stub_t isr_stubs[32] = {
    isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7,
    isr8,  isr9,  isr10, isr11, isr12, isr13, isr14, isr15,
    isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
    isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
};

// Array of 16 IRQ Hardware stub function pointers
static const isr_stub_t irq_stubs[16] = {
    irq0,  irq1,  irq2,  irq3,  irq4,  irq5,  irq6,  irq7,
    irq8,  irq9,  irq10, irq11, irq12, irq13, irq14, irq15
};

static isr_t interrupt_handlers[256] = { nullptr };

static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault (#GP)",
    "Page Fault (#PF)",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Security Exception",
    "Reserved"
};

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
    if (n == 128) {
        // Set System Call IDT Vector 128 (0x80) with DPL 3 (0xEE)
        IDT::set_gate(128, reinterpret_cast<uint32_t>(isr128), 0x08, 0xEE);
    }
}

void interrupts_init() {
    // Register 32 CPU Exception gates (Vectors 0-31)
    for (uint8_t i = 0; i < 32; i++) {
        IDT::set_gate(i, reinterpret_cast<uint32_t>(isr_stubs[i]), 0x08, 0x8E);
    }

    // Register 16 IRQ Hardware gates (Vectors 32-47)
    for (uint8_t i = 0; i < 16; i++) {
        IDT::set_gate(32 + i, reinterpret_cast<uint32_t>(irq_stubs[i]), 0x08, 0x8E);
    }

    // Register System Call Vector 128 (0x80) with DPL 3
    IDT::set_gate(128, reinterpret_cast<uint32_t>(isr128), 0x08, 0xEE);
}

} // namespace x86_64
} // namespace arch
} // namespace nebula

extern "C" {

uint32_t isr_handler(nebula::arch::x86_64::registers_t* regs) {
    if (regs == nullptr) return reinterpret_cast<uint32_t>(regs);

    if (nebula::arch::x86_64::interrupt_handlers[regs->int_no] != nullptr) {
        nebula::arch::x86_64::interrupt_handlers[regs->int_no](regs);
    } else {
        nebula::drivers::Serial::write_string("\n[CPU EXCEPTION] ");
        if (regs->int_no < 32) {
            nebula::drivers::Serial::write_string(nebula::arch::x86_64::exception_messages[regs->int_no]);
        } else {
            nebula::drivers::Serial::write_string("Unhandled Vector ");
            nebula::drivers::Serial::write_dec(regs->int_no);
        }

        if (regs->int_no == 14) { // Page Fault
            uint32_t faulting_address;
            asm volatile ("mov %%cr2, %0" : "=r"(faulting_address));
            nebula::drivers::Serial::write_string(" at Faulting Linear Address: ");
            nebula::drivers::Serial::write_hex32(faulting_address);
        }

        nebula::drivers::Serial::write_string("\n[CPU HALT] System halted to prevent exception loop.\n");
        asm volatile ("cli; hlt");
    }
    return reinterpret_cast<uint32_t>(regs);
}

uint32_t irq_handler(nebula::arch::x86_64::registers_t* regs) {
    if (regs == nullptr) return reinterpret_cast<uint32_t>(regs);

    // Send EOI to PIC before calling handler
    if (regs->int_no >= 40) {
        nebula::arch::x86_64::PIC::send_eoi(regs->int_no - 32);
    } else {
        nebula::arch::x86_64::PIC::send_eoi(regs->int_no - 32);
    }

    if (nebula::arch::x86_64::interrupt_handlers[regs->int_no] != nullptr) {
        nebula::arch::x86_64::interrupt_handlers[regs->int_no](regs);
    }

    if (regs->int_no == 32) { // IRQ 0 Timer Tick
        return nebula::scheduler::Scheduler::handle_timer_tick(regs);
    }

    return reinterpret_cast<uint32_t>(regs);
}

}
