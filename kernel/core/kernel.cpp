/**
 * @file kernel.cpp
 * @brief Kernel entry point for Nebula OS - Phase 4 Verification (Input & Timer
 * Drivers)
 * @author Nebula OS Team
 */

#include "../../include/kernel/arch/x86_64/gdt.hpp"
#include "../../include/kernel/arch/x86_64/idt.hpp"
#include "../../include/kernel/arch/x86_64/interrupts.hpp"
#include "../../include/kernel/arch/x86_64/pic.hpp"
#include "../../include/kernel/drivers/console.hpp"
#include "../../include/kernel/drivers/keyboard.hpp"
#include "../../include/kernel/drivers/pit.hpp"
#include "../../include/kernel/drivers/serial.hpp"
#include "../../include/kernel/memory/heap.hpp"
#include "../../include/kernel/memory/pmm.hpp"
#include "../../include/kernel/memory/vmm.hpp"

#include "../arch/x86_64/gdt/gdt.cpp"
#include "../arch/x86_64/idt/idt.cpp"
#include "../arch/x86_64/interrupts/interrupts.cpp"
#include "../arch/x86_64/interrupts/pic.cpp"
#include "../drivers/console/vga.cpp"
#include "../drivers/input/keyboard.cpp"
#include "../drivers/serial/serial.cpp"
#include "../drivers/timer/pit.cpp"
#include "../memory/heap.cpp"
#include "../memory/kheap.cpp"
#include "../memory/pmm.cpp"
#include "../memory/vmm.cpp"

using nebula::arch::x86_64::GDT;
using nebula::arch::x86_64::IDT;
using nebula::arch::x86_64::PIC;
using nebula::drivers::COLOR_BLUE;
using nebula::drivers::COLOR_CYAN;
using nebula::drivers::COLOR_LIGHT_GREEN;
using nebula::drivers::COLOR_WHITE;
using nebula::drivers::COLOR_YELLOW;
using nebula::drivers::Keyboard;
using nebula::drivers::PIT;
using nebula::drivers::Serial;
using nebula::drivers::VGAConsole;
using nebula::memory::KernelHeap;
using nebula::memory::PMM;
using nebula::memory::VMM;

static void klog(const char *str) { Serial::write_string(str); }

/**
 * @brief Main entry point of the Nebula OS Kernel
 */
extern "C" void kernel_main(uint32_t magic, multiboot_info_t *mb_info) {
  (void)magic;
  Serial::init();
  klog("[KERNEL] Entered kernel_main()\n");

  klog("[GDT] Initializing 32-bit GDT & reloading segment registers...\n");
  GDT::init();

  klog("[IDT] Initializing IDT 256 gates...\n");
  IDT::init();

  klog("[PIC] Remapping 8259 PIC (Master: 32, Slave: 40)...\n");
  PIC::remap(32, 40);

  klog("[ISR] Registering CPU Exception & IRQ stubs...\n");
  nebula::arch::x86_64::interrupts_init();

  klog("[PMM] Initializing Physical Memory Manager (Bitmap Allocator)...\n");
  PMM::init(mb_info);

  klog("[VMM] Initializing Virtual Memory Manager & Identity Paging "
       "(0-256MB)...\n");
  VMM::init();

  klog("[HEAP] Initializing Kernel Heap Allocator at 0x00C00000 (1 MB "
       "pool)...\n");
  KernelHeap::init(0x00C00000, 0x00100000);

  klog("[PIT] Initializing 8254 Timer at 100 Hz (IRQ 0 / Vector 32)...\n");
  PIT::init(100);

  klog("[KEYBOARD] Initializing PS/2 Keyboard Driver (IRQ 1 / Vector 33)...\n");
  Keyboard::init();

  klog("[CPU] Enabling interrupts via STI...\n");
  asm volatile("sti");
  klog("[CPU] Interrupts enabled safely!\n");

  klog("[KERNEL] Clearing VGA screen with blue background...\n");
  VGAConsole::clear(COLOR_BLUE);

  klog("[KERNEL] Writing Phase 4 VGA banner...\n");
  VGAConsole::write_at(
      0, 2, "========================================", COLOR_CYAN, COLOR_BLUE);
  VGAConsole::write_at(0, 3, "       Nebula OS Kernel - Phase 4       ",
                       COLOR_WHITE, COLOR_BLUE);
  VGAConsole::write_at(
      0, 4, "========================================", COLOR_CYAN, COLOR_BLUE);

  VGAConsole::write_at(0, 6, "[OK] PIT 8254 Timer Initialized (100 Hz / IRQ 0)",
                       COLOR_LIGHT_GREEN, COLOR_BLUE);
  VGAConsole::write_at(0, 7, "[OK] PS/2 Keyboard Driver Initialized (IRQ 1)",
                       COLOR_LIGHT_GREEN, COLOR_BLUE);
  VGAConsole::write_at(0, 8, "[OK] Interactive Shell Ready - Type on Keyboard!",
                       COLOR_YELLOW, COLOR_BLUE);

  VGAConsole::write_at(0, 10, "nebula> ", COLOR_CYAN, COLOR_BLUE);

  klog("[KERNEL] Phase 4 initialization complete! Interactive shell active.\n");

  size_t col = 8;
  size_t row = 10;

  while (true) {
    if (Keyboard::has_char()) {
      char c = Keyboard::get_char();
      if (c == '\n') {
        row++;
        col = 0;
        VGAConsole::write_at(col, row, "nebula> ", COLOR_CYAN, COLOR_BLUE);
        col = 8;
        Serial::write_string("\n[INPUT] Line Submitted\nnebula> ");
      } else if (c == '\b') {
        if (col > 8) {
          col--;
          VGAConsole::write_at(col, row, " ", COLOR_WHITE, COLOR_BLUE);
        }
      } else {
        char str_buf[2] = {c, '\0'};
        VGAConsole::write_at(col, row, str_buf, COLOR_WHITE, COLOR_BLUE);
        Serial::write_char(c);
        col++;
        if (col >= 80) {
          col = 0;
          row++;
        }
      }
    }
    asm volatile("hlt");
  }
}
