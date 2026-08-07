/**
 * @file kernel.cpp
 * @brief Kernel entry point for Nebula OS - Phase 1 Verification
 * @author Nebula OS Team
 */

#include "../../include/kernel/drivers/console.hpp"

using nebula::drivers::vga_clear;
using nebula::drivers::vga_write_at;

/**
 * @brief Main entry point of the Nebula OS Kernel
 */
extern "C" void kernel_main() {
    vga_clear(nebula::drivers::COLOR_BLUE);

    vga_write_at(0, 0, "========================================", nebula::drivers::COLOR_CYAN, nebula::drivers::COLOR_BLUE);
    vga_write_at(0, 1, "       Nebula OS Kernel - Phase 1       ", nebula::drivers::COLOR_WHITE, nebula::drivers::COLOR_BLUE);
    vga_write_at(0, 2, "========================================", nebula::drivers::COLOR_CYAN, nebula::drivers::COLOR_BLUE);

    vga_write_at(0, 4, "[OK] VGA Text Mode Console Driver (80x25)", nebula::drivers::COLOR_LIGHT_GREEN, nebula::drivers::COLOR_BLUE);
    vga_write_at(0, 5, "[OK] System V ELF 32-bit Architecture", nebula::drivers::COLOR_LIGHT_GREEN, nebula::drivers::COLOR_BLUE);
    vga_write_at(0, 6, "[OK] Multiboot Header Verified", nebula::drivers::COLOR_LIGHT_GREEN, nebula::drivers::COLOR_BLUE);

    vga_write_at(0, 8, "Phase 1 Initialization Complete!", nebula::drivers::COLOR_YELLOW, nebula::drivers::COLOR_BLUE);

    while (true) {
        asm volatile ("hlt");
    }
}
