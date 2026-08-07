/**
 * @file kernel.cpp
 * @brief Kernel entry point for Nebula OS - Phase 6 Verification (VFS, Initrd RAM Disk, & Ring 3 Syscalls)
 * @author Nebula OS Team
 */

#include "../../include/kernel/drivers/console.hpp"
#include "../../include/kernel/drivers/serial.hpp"
#include "../../include/kernel/drivers/pit.hpp"
#include "../../include/kernel/drivers/keyboard.hpp"
#include "../../include/kernel/arch/x86_64/gdt.hpp"
#include "../../include/kernel/arch/x86_64/idt.hpp"
#include "../../include/kernel/arch/x86_64/pic.hpp"
#include "../../include/kernel/arch/x86_64/interrupts.hpp"
#include "../../include/kernel/arch/x86_64/tss.hpp"
#include "../../include/kernel/memory/pmm.hpp"
#include "../../include/kernel/memory/vmm.hpp"
#include "../../include/kernel/memory/heap.hpp"
#include "../../include/kernel/process/process.hpp"
#include "../../include/kernel/scheduler/scheduler.hpp"
#include "../../include/kernel/fs/vfs.hpp"
#include "../../include/kernel/fs/initrd.hpp"
#include "../../include/kernel/syscall/syscall.hpp"

#include "../drivers/console/vga.cpp"
#include "../drivers/serial/serial.cpp"
#include "../drivers/timer/pit.cpp"
#include "../drivers/input/keyboard.cpp"
#include "../arch/x86_64/gdt/gdt.cpp"
#include "../arch/x86_64/idt/idt.cpp"
#include "../arch/x86_64/interrupts/pic.cpp"
#include "../arch/x86_64/interrupts/interrupts.cpp"
#include "../arch/x86_64/tss/tss.cpp"
#include "../memory/pmm.cpp"
#include "../memory/vmm.cpp"
#include "../memory/heap.cpp"
#include "../memory/kheap.cpp"
#include "../scheduler/scheduler.cpp"
#include "../fs/vfs.cpp"
#include "../fs/initrd.cpp"
#include "../syscall/syscall.cpp"

using nebula::drivers::VGAConsole;
using nebula::drivers::Serial;
using nebula::drivers::PIT;
using nebula::drivers::Keyboard;
using nebula::drivers::COLOR_BLUE;
using nebula::drivers::COLOR_CYAN;
using nebula::drivers::COLOR_WHITE;
using nebula::drivers::COLOR_LIGHT_GREEN;
using nebula::drivers::COLOR_YELLOW;
using nebula::arch::x86_64::GDT;
using nebula::arch::x86_64::IDT;
using nebula::arch::x86_64::PIC;
using nebula::arch::x86_64::TSS;
using nebula::memory::PMM;
using nebula::memory::VMM;
using nebula::memory::KernelHeap;
using nebula::scheduler::Scheduler;
using nebula::fs::VFS;
using nebula::fs::Initrd;
using nebula::fs::vnode_t;
using nebula::syscall::Syscall;

static void klog(const char* str) {
    Serial::write_string(str);
}

// Background Kernel Thread Alpha
static void kernel_thread_alpha() {
    size_t count = 0;
    while (true) {
        count++;
        if (count % 50000000 == 0) {
            Serial::write_string("[THREAD ALPHA] Running in background (TID 1)\n");
            VGAConsole::write_at(0, 12, "[THREAD ALPHA] Active (Preemptive Switch OK)", COLOR_LIGHT_GREEN, COLOR_BLUE);
        }
    }
}

// Background Kernel Thread Beta
static void kernel_thread_beta() {
    size_t count = 0;
    while (true) {
        count++;
        if (count % 50000000 == 0) {
            Serial::write_string("[THREAD BETA] Running in background (TID 2)\n");
            VGAConsole::write_at(0, 13, "[THREAD BETA] Active (Preemptive Switch OK)", COLOR_YELLOW, COLOR_BLUE);
        }
    }
}

/**
 * @brief Main entry point of the Nebula OS Kernel
 */
extern "C" void kernel_main(uint32_t magic, multiboot_info_t* mb_info) {
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

    klog("[VMM] Initializing Virtual Memory Manager & Identity Paging (0-256MB)...\n");
    VMM::init();

    klog("[HEAP] Initializing Kernel Heap Allocator at 0x00C00000 (1 MB pool)...\n");
    KernelHeap::init(0x00C00000, 0x00100000);

    klog("[TSS] Initializing Task State Segment (TSS 64-bit)...\n");
    TSS::init(0x00900000);

    klog("[PIT] Initializing 8254 Timer at 100 Hz (IRQ 0 / Vector 32)...\n");
    PIT::init(100);

    klog("[KEYBOARD] Initializing PS/2 Keyboard Driver (IRQ 1 / Vector 33)...\n");
    Keyboard::init();

    klog("[SCHEDULER] Initializing Preemptive Round-Robin Scheduler...\n");
    Scheduler::init();

    klog("[VFS] Initializing Virtual File System...\n");
    VFS::init();

    klog("[INITRD] Mounting Initrd RAM Disk at /initrd/...\n");
    vnode_t* root_node = Initrd::init(0);

    klog("[SYSCALL] Initializing System Call Vector (INT 0x80)...\n");
    Syscall::init();

    // Test Reading File from VFS Initrd RAM Disk
    klog("[TEST VFS] Searching for /initrd/hello.txt...\n");
    vnode_t* file_node = VFS::finddir(root_node, "hello.txt");
    if (file_node != nullptr) {
        char read_buf[128];
        for (size_t i = 0; i < sizeof(read_buf); i++) read_buf[i] = 0;
        uint32_t bytes = VFS::read(file_node, 0, sizeof(read_buf) - 1, reinterpret_cast<uint8_t*>(read_buf));
        klog("[TEST VFS] Successfully Read /initrd/hello.txt (");
        Serial::write_dec(bytes);
        klog(" bytes):\n");
        klog(read_buf);
    }

    // Test System Call INT 0x80 (SYS_WRITE)
    klog("[TEST SYSCALL] Triggering INT 0x80 System Call (SYS_WRITE)...\n");
    const char* sys_msg = "[SYSCALL TEST] Hello via INT 0x80 System Call!\n";
    asm volatile (
        "mov $2, %%eax\n" // SYS_WRITE = 2
        "mov $1, %%ebx\n" // fd = 1
        "mov %0, %%ecx\n" // str
        "mov $45, %%edx\n"
        "int $0x80\n"
        :
        : "r"(sys_msg)
        : "eax", "ebx", "ecx", "edx"
    );

    klog("[SCHEDULER] Creating Kernel Thread Alpha...\n");
    Scheduler::create_kernel_thread(kernel_thread_alpha, "ThreadAlpha");

    klog("[SCHEDULER] Creating Kernel Thread Beta...\n");
    Scheduler::create_kernel_thread(kernel_thread_beta, "ThreadBeta");

    klog("[CPU] Enabling interrupts via STI...\n");
    asm volatile ("sti");
    klog("[CPU] Interrupts enabled safely!\n");

    klog("[KERNEL] Clearing VGA screen with blue background...\n");
    VGAConsole::clear(COLOR_BLUE);

    klog("[KERNEL] Writing Phase 6 VGA banner...\n");
    VGAConsole::write_at(0, 2, "========================================", COLOR_CYAN, COLOR_BLUE);
    VGAConsole::write_at(0, 3, "       Nebula OS Kernel - Phase 6       ", COLOR_WHITE, COLOR_BLUE);
    VGAConsole::write_at(0, 4, "========================================", COLOR_CYAN, COLOR_BLUE);

    VGAConsole::write_at(0, 6, "[OK] VFS & Initrd RAM Disk Mounted (/initrd/hello.txt)", COLOR_LIGHT_GREEN, COLOR_BLUE);
    VGAConsole::write_at(0, 7, "[OK] System Call Gate INT 0x80 Active (SYS_WRITE / SYS_READ)", COLOR_LIGHT_GREEN, COLOR_BLUE);
    VGAConsole::write_at(0, 8, "[OK] Multitasking & Userland Infrastructure Ready!", COLOR_YELLOW, COLOR_BLUE);

    VGAConsole::write_at(0, 15, "nebula> ", COLOR_CYAN, COLOR_BLUE);

    klog("[KERNEL] Phase 6 VFS & System Call Infrastructure Active!\n");

    size_t col = 8;
    size_t row = 15;

    while (true) {
        if (Keyboard::has_char()) {
            char c = Keyboard::get_char();
            if (c == '\n') {
                row++;
                if (row >= 24) row = 15;
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
                char str_buf[2] = { c, '\0' };
                VGAConsole::write_at(col, row, str_buf, COLOR_WHITE, COLOR_BLUE);
                Serial::write_char(c);
                col++;
                if (col >= 80) {
                    col = 0;
                    row++;
                    if (row >= 24) row = 15;
                }
            }
        }
        asm volatile ("hlt");
    }
}
