/**
 * @file kernel.cpp
 * @brief Kernel entry point for Nebula OS - XNU-Hybrid Architecture
 * @author Nebula OS Team
 */

#include <bsd/sys/syscall.hpp>
#include <bsd/vfs/fat32.hpp>
#include <bsd/vfs/initrd.hpp>
#include <bsd/vfs/vfs.hpp>
#include <gui/font.hpp>
#include <gui/wm.hpp>
#include <iokit/console/console.hpp>
#include <iokit/display/vbe.hpp>
#include <iokit/input/keyboard.hpp>
#include <iokit/input/mouse.hpp>
#include <iokit/iodevice.hpp>
#include <iokit/serial/serial.hpp>
#include <iokit/storage/ata.hpp>
#include <iokit/storage/mbr.hpp>
#include <iokit/timer/pit.hpp>
#include <libkern/libkern.hpp>
#include <libsa/libsa.hpp>
#include <mach/arch/gdt.hpp>
#include <mach/arch/idt.hpp>
#include <mach/arch/interrupts.hpp>
#include <mach/arch/pic.hpp>
#include <mach/arch/tss.hpp>
#include <mach/sched/process.hpp>
#include <mach/sched/scheduler.hpp>
#include <mach/vm/heap.hpp>
#include <mach/vm/pmm.hpp>
#include <mach/vm/vmm.hpp>
#include <pexpert/pexpert.hpp>
#include <san/sanitizer.hpp>
#include <security/security.hpp>

#include "../bsd/sys/syscall.cpp"
#include "../bsd/vfs/fat32.cpp"
#include "../bsd/vfs/initrd.cpp"
#include "../bsd/vfs/vfs.cpp"
#include "../gui/font.cpp"
#include "../gui/wm.cpp"
#include "../iokit/console/vga.cpp"
#include "../iokit/display/vbe.cpp"
#include "../iokit/input/keyboard.cpp"
#include "../iokit/input/mouse.cpp"
#include "../iokit/iodevice.cpp"
#include "../iokit/serial/serial.cpp"
#include "../iokit/storage/ata.cpp"
#include "../iokit/storage/mbr.cpp"
#include "../iokit/timer/pit.cpp"
#include "../libkern/libkern.cpp"
#include "../libsa/libsa.cpp"
#include "../mach/arch/gdt.cpp"
#include "../mach/arch/idt.cpp"
#include "../mach/arch/interrupts.cpp"
#include "../mach/arch/pic.cpp"
#include "../mach/arch/tss.cpp"
#include "../mach/sched/scheduler.cpp"
#include "../mach/vm/heap.cpp"
#include "../mach/vm/kheap.cpp"
#include "../mach/vm/pmm.cpp"
#include "../mach/vm/vmm.cpp"
#include "../pexpert/pexpert.cpp"
#include "../san/sanitizer.cpp"
#include "../security/security.cpp"

using nebula::arch::x86_64::GDT;
using nebula::arch::x86_64::IDT;
using nebula::arch::x86_64::PIC;
using nebula::arch::x86_64::TSS;
using nebula::drivers::Keyboard;
using nebula::drivers::Mouse;
using nebula::drivers::PIT;
using nebula::drivers::Serial;
using nebula::drivers::VBE;
using nebula::drivers::VGAConsole;
using nebula::fs::Initrd;
using nebula::fs::VFS;
using nebula::fs::vnode_t;
using nebula::gui::Font;
using nebula::gui::WindowManager;
using nebula::memory::KernelHeap;
using nebula::memory::PMM;
using nebula::memory::VMM;
using nebula::scheduler::Scheduler;
using nebula::syscall::Syscall;

static char term_input_buf[64] = "nebula> ";
static size_t term_buf_len = 8;

// Render callback for System Information Window
static void render_sys_info_win(int32_t x, int32_t y, uint32_t w, uint32_t h) {
  (void)w;
  (void)h;
  Font::draw_string(x, y, "System Status & Hardware Info:", 0x38BDF8);
  Font::draw_string(
      x, y + 20, "CPU Architecture : x86 32/64-bit Protected Mode", 0xF8FAFC);
  Font::draw_string(x, y + 40, "Memory Heap Pool : 0x00C00000 (1 MB Allocated)",
                    0xF8FAFC);
  Font::draw_string(x, y + 60, "Filesystem Mount : /initrd/ (RAM Disk Loaded)",
                    0x2DD4BF);
  Font::draw_string(x, y + 80, "System Calls Gate: INT 0x80 (Active DPL 3)",
                    0xFBBF24);
  Font::draw_string(
      x, y + 100, "Multitasking     : Preemptive Round-Robin 100Hz", 0xF8FAFC);
}

// Render callback for Interactive Terminal GUI Window
static void render_terminal_win(int32_t x, int32_t y, uint32_t w, uint32_t h) {
  (void)w;
  (void)h;
  VBE::fill_rect(x, y, w, h, 0x020617);
  Font::draw_string(x + 10, y + 10, "Nebula OS Interactive Terminal", 0x38BDF8);
  Font::draw_string(x + 10, y + 30,
                    "Type text below using keyboard:", 0x94A3B8);
  Font::draw_string(x + 10, y + 60, term_input_buf, 0x2DD4BF);
}

// Background Kernel Thread Alpha
static void kernel_thread_alpha() {
  size_t count = 0;
  while (true) {
    count++;
    if (count % 50000000 == 0) {
      Serial::write_string("[THREAD ALPHA] Active in background\n");
    }
  }
}

// Background Kernel Thread Beta
static void kernel_thread_beta() {
  size_t count = 0;
  while (true) {
    count++;
    if (count % 50000000 == 0) {
      Serial::write_string("[THREAD BETA] Active in background\n");
    }
  }
}

/**
 * @brief Main entry point of the Nebula OS Kernel - Phase 7
 */
extern "C" void kernel_main(uint32_t magic, multiboot_info_t *mb_info) {
  (void)magic;
  Serial::init();
  Serial::write_string(
      "[KERNEL] Entered kernel_main() XNU-Hybrid Architecture 1-to-1\n");

  nebula::pexpert::init();
  nebula::libkern::init();
  nebula::libsa::init();
  nebula::security::init();
  nebula::san::init();

  GDT::init();
  IDT::init();
  PIC::remap(32, 40);
  nebula::arch::x86_64::interrupts_init();

  PMM::init(mb_info);
  VMM::init();
  KernelHeap::init(0x00C00000, 0x00100000);
  TSS::init(0x00900000);

  PIT::init(100);
  Keyboard::init();
  Mouse::init();
  Scheduler::init();

  VFS::init();
  vnode_t *root_node = Initrd::init(0);
  (void)root_node;

  static nebula::drivers::ATADriver ata_drive;
  ata_drive.init();
  if (ata_drive.start()) {
    uint32_t fat32_lba = 0;
    nebula::drivers::MBRParser::parse(&ata_drive, &fat32_lba);
    nebula::fs::FAT32::mount(&ata_drive, fat32_lba);
  }

  Syscall::init();

  VBE::init(nullptr);
  WindowManager::init();

  // Create GUI Window 1: System Info
  WindowManager::create_window(50, 120, 420, 200, "System Information",
                               render_sys_info_win);

  // Create GUI Window 2: Terminal Emulator GUI
  WindowManager::create_window(320, 240, 420, 220, "Terminal Console",
                               render_terminal_win);

  Scheduler::create_kernel_thread(kernel_thread_alpha, "ThreadAlpha");
  Scheduler::create_kernel_thread(kernel_thread_beta, "ThreadBeta");

  asm volatile("sti");
  Serial::write_string("[KERNEL] Phase 7 Desktop GUI Compositor Active!\n");

  while (true) {
    // Read keyboard input for Terminal GUI window
    if (Keyboard::has_char()) {
      char c = Keyboard::get_char();
      if (c == '\b') {
        if (term_buf_len > 8) {
          term_buf_len--;
          term_input_buf[term_buf_len] = '\0';
        }
      } else if (c == '\n') {
        term_buf_len = 8;
        term_input_buf[8] = '\0';
      } else if (term_buf_len < sizeof(term_input_buf) - 1) {
        term_input_buf[term_buf_len++] = c;
        term_input_buf[term_buf_len] = '\0';
      }
    }

    // Render Desktop Compositor (Wallpaper -> Windows -> Taskbar -> Mouse
    // Cursor)
    WindowManager::render_all();
  }
}
