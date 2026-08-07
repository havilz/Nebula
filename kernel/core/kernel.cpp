/**
 * @file kernel.cpp
 * @brief Kernel entry point for Nebula OS - Phase 3 Verification (Memory Management)
 * @author Nebula OS Team
 */

#include "../../include/kernel/drivers/console.hpp"
#include "../../include/kernel/drivers/serial.hpp"
#include "../../include/kernel/arch/x86_64/gdt.hpp"
#include "../../include/kernel/arch/x86_64/idt.hpp"
#include "../../include/kernel/arch/x86_64/pic.hpp"
#include "../../include/kernel/arch/x86_64/interrupts.hpp"
#include "../../include/kernel/memory/pmm.hpp"
#include "../../include/kernel/memory/vmm.hpp"
#include "../../include/kernel/memory/heap.hpp"

#include "../drivers/console/vga.cpp"
#include "../drivers/serial/serial.cpp"
#include "../arch/x86_64/gdt/gdt.cpp"
#include "../arch/x86_64/idt/idt.cpp"
#include "../arch/x86_64/interrupts/pic.cpp"
#include "../arch/x86_64/interrupts/interrupts.cpp"
#include "../memory/pmm.cpp"
#include "../memory/vmm.cpp"
#include "../memory/heap.cpp"
#include "../memory/kheap.cpp"

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
using nebula::memory::PMM;
using nebula::memory::VMM;
using nebula::memory::KernelHeap;

static void klog(const char* str) {
    Serial::write_string(str);
}

class TestMemoryObject {
public:
    uint32_t value1;
    uint32_t value2;

    TestMemoryObject(uint32_t v1, uint32_t v2) : value1(v1), value2(v2) {}
};

/**
 * @brief Main entry point of the Nebula OS Kernel
 */
extern "C" void kernel_main(uint32_t magic, multiboot_info_t* mb_info) {
    (void)magic;
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
    klog("[ISR] ISR handlers registered successfully!\n");

    klog("[CPU] Enabling interrupts via STI...\n");
    asm volatile ("sti");
    klog("[CPU] Interrupts enabled safely!\n");

    klog("[PMM] Initializing Physical Memory Manager (Bitmap Allocator)...\n");
    PMM::init(mb_info);
    klog("[PMM] PMM Initialized Successfully!\n");

    klog("[VMM] Initializing Virtual Memory Manager & Identity Paging (0-16MB)...\n");
    VMM::init();
    klog("[VMM] CPU Paging Enabled Safely (CR3 & CR0.PG Active)!\n");

    klog("[HEAP] Initializing Kernel Heap Allocator at 0x00C00000 (1 MB pool)...\n");
    KernelHeap::init(0x00C00000, 0x00100000);
    klog("[HEAP] Kernel Heap Initialized Successfully!\n");

    klog("[KERNEL] Clearing VGA screen with blue background...\n");
    VGAConsole::clear(COLOR_BLUE);
    klog("[KERNEL] Screen cleared successfully!\n");

    klog("[KERNEL] Writing Phase 3 VGA banner...\n");
    VGAConsole::write_at(0, 2, "========================================", COLOR_CYAN, COLOR_BLUE);
    VGAConsole::write_at(0, 3, "       Nebula OS Kernel - Phase 3       ", COLOR_WHITE, COLOR_BLUE);
    VGAConsole::write_at(0, 4, "========================================", COLOR_CYAN, COLOR_BLUE);

    VGAConsole::write_at(0, 6, "[OK] PMM Bitmap Frame Allocator Initialized", COLOR_LIGHT_GREEN, COLOR_BLUE);
    VGAConsole::write_at(0, 7, "[OK] VMM Identity Paging (16 MB) & CR0.PG Active", COLOR_LIGHT_GREEN, COLOR_BLUE);
    VGAConsole::write_at(0, 8, "[OK] Kernel Heap Allocator (0x00C00000 / 1 MB)", COLOR_LIGHT_GREEN, COLOR_BLUE);

    // Test Memory Allocations
    klog("[TEST] Testing kmalloc(512)...\n");
    void* ptr1 = kmalloc(512);
    if (ptr1 != nullptr) {
        klog("[TEST] kmalloc(512) Allocation Successful!\n");
        VGAConsole::write_at(0, 9, "[OK] kmalloc(512) Dynamic Allocation Success", COLOR_LIGHT_GREEN, COLOR_BLUE);
        kfree(ptr1);
        klog("[TEST] kfree(ptr1) Freed Successfully!\n");
    }

    klog("[TEST] Testing C++ Global Operator new...\n");
    TestMemoryObject* obj = new TestMemoryObject(0x12345678, 0x9ABCDEF0);
    if (obj != nullptr && obj->value1 == 0x12345678) {
        klog("[TEST] C++ Operator new TestMemoryObject Success!\n");
        VGAConsole::write_at(0, 10, "[OK] C++ Global operator new & delete Success", COLOR_LIGHT_GREEN, COLOR_BLUE);
        delete obj;
        klog("[TEST] delete obj Freed Successfully!\n");
    }

    VGAConsole::write_at(0, 12, "Phase 3 Initialization Complete!", COLOR_YELLOW, COLOR_BLUE);

    klog("[KERNEL] Phase 3 initialization complete! Entering infinite loop...\n");

    while (true) {
        asm volatile ("hlt");
    }
}
