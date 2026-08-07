/**
 * @file kernel.cpp
 * @brief Kernel entry point for Nebula OS - Phase 2 (GDT, IDT, PIC & Interrupt Verification)
 * @author Nebula OS Team
 */

#include "../../include/kernel/drivers/console.hpp"
#include "../../include/kernel/drivers/serial.hpp"
#include "../../include/kernel/arch/x86_64/gdt.hpp"
#include "../../include/kernel/arch/x86_64/idt.hpp"
#include "../../include/kernel/arch/x86_64/pic.hpp"
#include "../../include/kernel/arch/x86_64/interrupts.hpp"

#include "../drivers/console/vga.cpp"
#include "../drivers/serial/serial.cpp"
#include "../arch/x86_64/gdt/gdt.cpp"
#include "../arch/x86_64/idt/idt.cpp"
#include "../arch/x86_64/interrupts/pic.cpp"
#include "../arch/x86_64/interrupts/interrupts.cpp"

using nebula::drivers::VGAConsole;
using nebula::drivers::Serial;
using nebula::drivers::COLOR_BLUE;
using nebula::drivers::COLOR_CYAN;
using nebula::drivers::COLOR_WHITE;
using nebula::drivers::COLOR_LIGHT_GREEN;
using nebula::drivers::COLOR_YELLOW;
using nebula::arch::x86_64::GDT;
using nebula::arch::x86_64::IDT;
using nebula::arch::x86_64::PIC;

static void klog(const char* str) {
    Serial::write_string(str);
}

static void breakpoint_handler(nebula::arch::x86_64::registers_t* regs) {
    (void)regs;
    klog("[ISR] INT 3 Breakpoint Exception Handler Intercepted Successfully!\n");
    VGAConsole::write_at(0, 11, "[OK] INT 3 Software Interrupt Handler Triggered", COLOR_LIGHT_GREEN, COLOR_BLUE);
}

/**
 * @brief Main entry point of the Nebula OS Kernel
 */
extern "C" void kernel_main() {
    Serial::init();
    klog("[KERNEL] Entered kernel_main()\n");

    klog("[GDT] Initializing 32-bit GDT & reloading segment registers...\n");
    GDT::init();
    klog("[GDT] GDT loaded successfully (CS=0x08, DS=0x10)!\n");

    klog("[IDT] Initializing IDT 256 gates...\n");
    IDT::init();
    klog("[IDT] IDT loaded successfully via LIDT!\n");

    klog("[PIC] Remapping 8259 PIC (Master: 32, Slave: 40)...\n");
    PIC::remap(32, 40);
    klog("[PIC] PIC remapped successfully!\n");

    klog("[ISR] Registering CPU Exception & IRQ stubs...\n");
    nebula::arch::x86_64::interrupts_init();
    nebula::arch::x86_64::register_interrupt_handler(3, breakpoint_handler);
    klog("[ISR] ISR handlers registered successfully!\n");

    klog("[CPU] Enabling interrupts via STI...\n");
    asm volatile ("sti");
    klog("[CPU] Interrupts enabled safely!\n");

    klog("[KERNEL] Clearing VGA screen with blue background...\n");
    VGAConsole::clear(COLOR_BLUE);
    klog("[KERNEL] Screen cleared successfully!\n");

    klog("[KERNEL] Writing Phase 2 VGA banner...\n");
    VGAConsole::write_at(0, 2, "========================================", COLOR_CYAN, COLOR_BLUE);
    VGAConsole::write_at(0, 3, "       Nebula OS Kernel - Phase 2       ", COLOR_WHITE, COLOR_BLUE);
    VGAConsole::write_at(0, 4, "========================================", COLOR_CYAN, COLOR_BLUE);

    VGAConsole::write_at(0, 6, "[OK] GDT 32-bit Reloaded (Code:0x08, Data:0x10)", COLOR_LIGHT_GREEN, COLOR_BLUE);
    VGAConsole::write_at(0, 7, "[OK] IDT 256 Gates & LIDT Loaded", COLOR_LIGHT_GREEN, COLOR_BLUE);
    VGAConsole::write_at(0, 8, "[OK] 8259 PIC Remapped (IRQ 0-15 -> Vectors 32-47)", COLOR_LIGHT_GREEN, COLOR_BLUE);
    VGAConsole::write_at(0, 9, "[OK] CPU Interrupts Enabled (STI Active)", COLOR_LIGHT_GREEN, COLOR_BLUE);

    // Test software interrupt (INT 3)
    klog("[TEST] Testing software interrupt INT 3...\n");
    asm volatile ("int $3");

    VGAConsole::write_at(0, 13, "Phase 2 Initialization Complete!", COLOR_YELLOW, COLOR_BLUE);

    klog("[KERNEL] Phase 2 initialization complete! Entering infinite loop...\n");

    while (true) {
        asm volatile ("hlt");
    }
}
