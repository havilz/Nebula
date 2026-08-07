/**
 * @file kernel.cpp
 * @brief Kernel entry point for Nebula OS - Phase 1 Verification
 * @author Nebula OS Team
 */

#include "../../include/kernel/drivers/console.hpp"
#include "../../include/kernel/drivers/serial.hpp"
#include "../drivers/console/vga.cpp"
#include "../drivers/serial/serial.cpp"

using nebula::drivers::VGAConsole;
using nebula::drivers::Serial;
using nebula::drivers::COLOR_BLUE;
using nebula::drivers::COLOR_CYAN;
using nebula::drivers::COLOR_WHITE;
using nebula::drivers::COLOR_LIGHT_GREEN;
using nebula::drivers::COLOR_YELLOW;

static void klog(const char* str) {
    Serial::write_string(str);
}

/**
 * @brief Main entry point of the Nebula OS Kernel
 */
extern "C" void kernel_main() {
    Serial::init();
    klog("[KERNEL] Entered kernel_main()\n");

    klog("[KERNEL] Clearing VGA screen with blue background...\n");
    VGAConsole::clear(COLOR_BLUE);
    klog("[KERNEL] Screen cleared successfully!\n");

    klog("[KERNEL] Writing VGA banner...\n");
    VGAConsole::write_at(0, 2, "========================================", COLOR_CYAN, COLOR_BLUE);
    VGAConsole::write_at(0, 3, "       Nebula OS Kernel - Phase 1       ", COLOR_WHITE, COLOR_BLUE);
    VGAConsole::write_at(0, 4, "========================================", COLOR_CYAN, COLOR_BLUE);

    VGAConsole::write_at(0, 6, "[OK] VGA Text Mode Console Driver (80x25)", COLOR_LIGHT_GREEN, COLOR_BLUE);
    VGAConsole::write_at(0, 7, "[OK] System V ELF 32-bit Architecture", COLOR_LIGHT_GREEN, COLOR_BLUE);
    VGAConsole::write_at(0, 8, "[OK] Multiboot Header Verified", COLOR_LIGHT_GREEN, COLOR_BLUE);

    VGAConsole::write_at(0, 10, "Phase 1 Initialization Complete!", COLOR_YELLOW, COLOR_BLUE);

    klog("[KERNEL] Phase 1 initialization complete! Entering infinite loop...\n");

    while (true) {
        asm volatile ("hlt");
    }
}
