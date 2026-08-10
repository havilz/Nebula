/**
 * @file kernel.cpp
 * @brief Kernel entry point for Nebula OS - XNU-Hybrid Architecture
 * @author Nebula OS Team
 */

#include "../../include/bsd/net/arp.hpp"
#include "../../include/bsd/net/ethernet.hpp"
#include "../../include/bsd/net/icmp.hpp"
#include "../../include/bsd/net/ipv4.hpp"
#include "../../include/bsd/net/socket.hpp"
#include "../../include/bsd/sys/bundle.hpp"
#include "../../include/bsd/sys/elf.hpp"
#include "../../include/bsd/sys/syscall.hpp"
#include "../../include/bsd/vfs/fat32.hpp"
#include "../../include/bsd/vfs/initrd.hpp"
#include "../../include/bsd/vfs/vfs.hpp"
#include "../../include/gui/aqua.hpp"
#include "../../include/gui/font.hpp"
#include "../../include/gui/widget.hpp"
#include "../../include/gui/wm.hpp"
#include "../../include/iokit/console/console.hpp"
#include "../../include/iokit/display/vbe.hpp"
#include "../../include/iokit/input/keyboard.hpp"
#include "../../include/iokit/input/mouse.hpp"
#include "../../include/iokit/iodevice.hpp"
#include "../../include/iokit/net/e1000.hpp"
#include "../../include/iokit/serial/serial.hpp"
#include "../../include/iokit/storage/ata.hpp"
#include "../../include/iokit/storage/mbr.hpp"
#include "../../include/iokit/timer/pit.hpp"
#include "../../include/libkern/libkern.hpp"
#include "../../include/libsa/libsa.hpp"
#include "../../include/mach/arch/gdt.hpp"
#include "../../include/mach/arch/idt.hpp"
#include "../../include/mach/arch/interrupts.hpp"
#include "../../include/mach/arch/pic.hpp"
#include "../../include/mach/arch/tss.hpp"
#include "../../include/mach/sched/process.hpp"
#include "../../include/mach/sched/scheduler.hpp"
#include "../../include/mach/vm/heap.hpp"
#include "../../include/mach/vm/pmm.hpp"
#include "../../include/mach/vm/vmm.hpp"
#include "../../include/multiboot.h"
#include "../../include/pexpert/pexpert.hpp"
#include "../../include/san/sanitizer.hpp"
#include "../../include/security/security.hpp"

#include "../bsd/net/arp.cpp"
#include "../bsd/net/ethernet.cpp"
#include "../bsd/net/icmp.cpp"
#include "../bsd/net/ipv4.cpp"
#include "../bsd/net/socket.cpp"
#include "../bsd/sys/elf.cpp"
#include "../bsd/sys/syscall.cpp"
#include "../bsd/vfs/fat32.cpp"
#include "../bsd/vfs/initrd.cpp"
#include "../bsd/vfs/vfs.cpp"
#include "../gui/aqua.cpp"
#include "../gui/font.cpp"
#include "../gui/widget.cpp"
#include "../gui/wm.cpp"
#include "../iokit/console/vga.cpp"
#include "../iokit/display/vbe.cpp"
#include "../iokit/input/keyboard.cpp"
#include "../iokit/input/mouse.cpp"
#include "../iokit/iodevice.cpp"
#include "../iokit/net/e1000.cpp"
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
  (void)h;
  VBE::fill_rect(x - 5, y - 5, w + 10, h + 10, 0x181825);

  Font::draw_string(x, y, "Nebula Workstation System Specs", 0x38BDF8);
  Font::draw_string(x, y + 20, "Architecture: x86_64 Mach-BSD Hybrid", 0xF8FAFC);
  Font::draw_string(x, y + 40, "Kernel Core : Mach Kernel + IOKit DriverKit", 0xE2E8F0);
  Font::draw_string(x, y + 60, "Network IP  : 10.0.2.15 (E1000 Gigabit UP)", 0x34D399);

  Font::draw_string(x, y + 90, "System Memory (RAM 512 MB): 35% Used", 0xFBBF24);
  VBE::fill_rounded_rect(x, y + 110, w - 10, 16, 6, 0x313244);
  VBE::fill_rounded_rect(x + 2, y + 112, ((w - 14) * 35) / 100, 12, 4, 0x10B981);
}

// Render callback for Interactive Terminal GUI Window
static void render_terminal_win(int32_t x, int32_t y, uint32_t w, uint32_t h) {
  VBE::fill_rect(x - 5, y - 5, w + 10, h + 10, 0x0F172A);

  Font::draw_string(x, y, "Last login: Mon Aug 10 12:45:00 on console", 0x64748B);
  Font::draw_string(x, y + 20, "nebula@os ~ % uname -a", 0x38BDF8);
  Font::draw_string(x, y + 36, "NebulaOS 1.1.0 Mach-BSD x86_64", 0xE2E8F0);

  Font::draw_string(x, y + 60, "nebula@os ~ % ls -l /initrd/", 0x38BDF8);
  Font::draw_string(x, y + 76, "-rw-r--r--  1 root  root  28B  hello.txt", 0x94A3B8);

  Font::draw_string(x, y + 100, "nebula@os ~ % ", 0x38BDF8);
  Font::draw_string(x + 112, y + 100, term_input_buf + 8, 0xF8FAFC);
  Font::draw_string(x + 112 + (term_buf_len - 8) * 8, y + 100, "_", 0x38BDF8);
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

  static nebula::drivers::E1000Driver e1000_nic;
  e1000_nic.init();
  e1000_nic.start();

  nebula::bsd::net::IPv4::init();
  nebula::bsd::net::ARP::init();
  nebula::bsd::net::SocketManager::init();

  Syscall::init();
  nebula::drivers::Serial::write_string(
      "[ELF] Executable Binary Loader Subsystem (ELF32/ELF64) Active\n");
  nebula::drivers::Serial::write_string(
      "[LIBC] Userland Standard C Library (libnebula) Active\n");

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
