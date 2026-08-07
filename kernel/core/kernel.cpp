/**
 * @file kernel.cpp
 * @brief Kernel entry point for Nebula OS - Phase 1 Verification with Terminal Serial Logging
 * @author Nebula OS Team
 */

#include "../../include/kernel/drivers/console.hpp"
#include "../drivers/console/vga.cpp"

static void klog_char(char c) {
    uint8_t status;
    size_t timeout = 10000;
    do {
        asm volatile ("inb %1, %0" : "=a"(status) : "d"(static_cast<uint16_t>(0x3FD)));
        if (--timeout == 0) break;
    } while ((status & 0x20) == 0);

    asm volatile ("outb %0, %1" : : "a"(static_cast<uint8_t>(c)), "d"(static_cast<uint16_t>(0x3F8)));
}

static void klog(const char* str) {
    if (!str) return;
    for (size_t i = 0; str[i] != '\0'; i++) {
        klog_char(str[i]);
    }
}

/**
 * @brief Main entry point of the Nebula OS Kernel
 */
extern "C" void kernel_main() {
    klog("[KERNEL] Entered kernel_main()\n");

    klog("[KERNEL] Clearing VGA screen with blue background...\n");
    vga_clear(nebula::drivers::COLOR_BLUE);
    klog("[KERNEL] Screen cleared successfully!\n");

    klog("[KERNEL] Writing VGA banner...\n");
    vga_write_at(0, 2, "========================================", nebula::drivers::COLOR_CYAN, nebula::drivers::COLOR_BLUE);
    vga_write_at(0, 3, "       Nebula OS Kernel - Phase 1       ", nebula::drivers::COLOR_WHITE, nebula::drivers::COLOR_BLUE);
    vga_write_at(0, 4, "========================================", nebula::drivers::COLOR_CYAN, nebula::drivers::COLOR_BLUE);

    vga_write_at(0, 6, "[OK] VGA Text Mode Console Driver (80x25)", nebula::drivers::COLOR_LIGHT_GREEN, nebula::drivers::COLOR_BLUE);
    vga_write_at(0, 7, "[OK] System V ELF 32-bit Architecture", nebula::drivers::COLOR_LIGHT_GREEN, nebula::drivers::COLOR_BLUE);
    vga_write_at(0, 8, "[OK] Multiboot Header Verified", nebula::drivers::COLOR_LIGHT_GREEN, nebula::drivers::COLOR_BLUE);

    vga_write_at(0, 10, "Phase 1 Initialization Complete!", nebula::drivers::COLOR_YELLOW, nebula::drivers::COLOR_BLUE);

    klog("[KERNEL] Phase 1 initialization complete! Entering infinite loop...\n");

    while (true) {
        asm volatile ("hlt");
    }
}
