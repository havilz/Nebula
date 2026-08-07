/**
 * @file kernel.cpp
 * @brief Kernel entry point for Nebula OS - Phase 7 (VBE Framebuffer & GUI
 * Window Manager)
 * @author Nebula OS Team
 */

#include "../../include/kernel/arch/x86_64/gdt.hpp"
#include "../../include/kernel/arch/x86_64/idt.hpp"
#include "../../include/kernel/arch/x86_64/interrupts.hpp"
#include "../../include/kernel/arch/x86_64/pic.hpp"
#include "../../include/kernel/arch/x86_64/tss.hpp"
#include "../../include/kernel/drivers/console.hpp"
#include "../../include/kernel/drivers/keyboard.hpp"
#include "../../include/kernel/drivers/mouse.hpp"
#include "../../include/kernel/drivers/pit.hpp"
#include "../../include/kernel/drivers/serial.hpp"
#include "../../include/kernel/drivers/vbe.hpp"
#include "../../include/kernel/fs/initrd.hpp"
#include "../../include/kernel/fs/vfs.hpp"
#include "../../include/kernel/gui/font.hpp"
#include "../../include/kernel/gui/wm.hpp"
#include "../../include/kernel/memory/heap.hpp"
#include "../../include/kernel/memory/pmm.hpp"
#include "../../include/kernel/memory/vmm.hpp"
#include "../../include/kernel/process/process.hpp"
#include "../../include/kernel/scheduler/scheduler.hpp"
#include "../../include/kernel/syscall/syscall.hpp"

#include "../arch/x86_64/gdt/gdt.cpp"
#include "../arch/x86_64/idt/idt.cpp"
#include "../arch/x86_64/interrupts/interrupts.cpp"
#include "../arch/x86_64/interrupts/pic.cpp"
#include "../arch/x86_64/tss/tss.cpp"
#include "../drivers/console/vga.cpp"
#include "../drivers/gui/vbe.cpp"
#include "../drivers/input/keyboard.cpp"
#include "../drivers/input/mouse.cpp"
#include "../drivers/serial/serial.cpp"
#include "../drivers/timer/pit.cpp"
#include "../fs/initrd.cpp"
#include "../fs/vfs.cpp"
#include "../gui/font.cpp"
#include "../gui/wm.cpp"
#include "../memory/heap.cpp"
#include "../memory/kheap.cpp"
#include "../memory/pmm.cpp"
#include "../memory/vmm.cpp"
#include "../scheduler/scheduler.cpp"
#include "../syscall/syscall.cpp"

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
  Serial::write_string("[KERNEL] Entered kernel_main() Phase 7 VBE GUI\n");

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
