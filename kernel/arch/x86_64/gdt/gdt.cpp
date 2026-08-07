/**
 * @file gdt.cpp
 * @brief Global Descriptor Table (GDT) implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../../../include/kernel/arch/x86_64/gdt.hpp"

namespace nebula {
namespace arch {
namespace x86_64 {

// Static member definitions
GDTEntry GDT::m_gdt[GDT_ENTRIES];
GDTPointer GDT::m_gdt_ptr;
TSSEntry GDT::m_tss;

/**
 * @brief Helper assembly function to load GDTR and reload segment registers
 */
static void gdt_flush(uint32_t gdtr_addr) {
    asm volatile (
        "movl %0, %%eax\n\t"
        "lgdt (%%eax)\n\t"
        "movw $0x10, %%ax\n\t"
        "movw %%ax, %%ds\n\t"
        "movw %%ax, %%es\n\t"
        "movw %%ax, %%fs\n\t"
        "movw %%ax, %%gs\n\t"
        "movw %%ax, %%ss\n\t"
        "pushl $0x08\n\t"
        "pushl $1f\n\t"
        "lret\n\t"
        "1:\n\t"
        :
        : "r"(gdtr_addr)
        : "eax", "memory"
    );
}

/**
 * @brief Helper assembly function to load Task Register (TR)
 */
static void tss_flush(uint16_t selector) {
    asm volatile (
        "ltr %0"
        :
        : "r"(selector)
    );
}

void GDT::set_gate(size_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    m_gdt[num].base_low    = (base & 0xFFFF);
    m_gdt[num].base_middle = (base >> 16) & 0xFF;
    m_gdt[num].base_high   = (base >> 24) & 0xFF;

    m_gdt[num].limit_low   = (limit & 0xFFFF);
    m_gdt[num].granularity = (limit >> 16) & 0x0F;

    m_gdt[num].granularity |= gran & 0xF0;
    m_gdt[num].access      = access;
}

void GDT::set_tss_gate(size_t num, uint64_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    set_gate(num, (uint32_t)base, limit, access, gran);
}

void GDT::init() {
    // 1. Setup GDT Entries
    set_gate(0, 0, 0, 0, 0);                 // Null descriptor
    set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);  // Kernel Code Segment 0x08
    set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);  // Kernel Data Segment 0x10
    set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);  // User Code Segment 0x18
    set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);  // User Data Segment 0x20
    set_tss_gate(5, (uint32_t)&m_tss, sizeof(TSSEntry) - 1, 0x89, 0x00); // TSS Segment 0x28

    // Initialize TSS fields
    m_tss.rsp0 = 0x00;
    m_tss.iomap_base = sizeof(TSSEntry);

    // 2. Setup GDT Register (GDTR)
    m_gdt_ptr.limit = sizeof(m_gdt) - 1;
    m_gdt_ptr.base  = (uint32_t)&m_gdt;

    // 3. Load GDT using lgdt instruction
    gdt_flush((uint32_t)&m_gdt_ptr);

    // 4. Load Task Register (TSS)
    tss_flush(0x28);
}

} // namespace x86_64
} // namespace arch
} // namespace nebula
