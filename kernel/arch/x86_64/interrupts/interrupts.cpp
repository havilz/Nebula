/**
 * @file interrupts.cpp
 * @brief C++ Interrupt Dispatcher implementation for x86_64
 * @author Nebula OS Team
 */

#include "../../../../include/kernel/arch/x86_64/interrupts.hpp"
#include "../../../../include/kernel/arch/x86_64/pic.hpp"
#include "../../../../include/kernel/drivers/console.hpp"

using nebula::drivers::vga_write_at;
using nebula::drivers::COLOR_WHITE;
using nebula::drivers::COLOR_RED;
using nebula::drivers::COLOR_YELLOW;

namespace nebula {
namespace arch {
namespace x86_64 {

ISRHandler Interrupts::m_handlers[256] = { nullptr };

static const char* exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "BOUND Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved Exception 15",
    "x87 FPU Floating-Point Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved Exception 22",
    "Reserved Exception 23",
    "Reserved Exception 24",
    "Reserved Exception 25",
    "Reserved Exception 26",
    "Reserved Exception 27",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved Exception 31"
};

void Interrupts::register_handler(uint8_t vector, ISRHandler handler) {
    m_handlers[vector] = handler;
}

void Interrupts::dispatch(InterruptRegisters* regs) {
    if (m_handlers[regs->int_no] != nullptr) {
        m_handlers[regs->int_no](regs);
    } else if (regs->int_no < 32) {
        // Unhandled CPU Exception
        vga_write_at(0, 4, "=== CPU EXCEPTION ===", COLOR_WHITE, COLOR_RED);
        vga_write_at(0, 5, exception_messages[regs->int_no], COLOR_YELLOW, COLOR_RED);
        while (true) {
            asm volatile ("cli; hlt");
        }
    }

    // Send EOI for hardware IRQs (vectors 32 - 47)
    if (regs->int_no >= 32 && regs->int_no <= 47) {
        PIC::send_eoi(static_cast<uint8_t>(regs->int_no - 32));
    }
}

} // namespace x86_64
} // namespace arch
} // namespace nebula

extern "C" void cpp_interrupt_handler(nebula::arch::x86_64::InterruptRegisters* regs) {
    nebula::arch::x86_64::Interrupts::dispatch(regs);
}
