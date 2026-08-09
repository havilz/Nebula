/**
 * @file gdt.cpp
 * @brief Global Descriptor Table (GDT) implementation for Nebula OS
 * @author Nebula OS Team
 */

#include <mach/arch/gdt.hpp>

namespace nebula {
namespace arch {
namespace x86_64 {

GDTEntry GDT::m_gdt[GDT::GDT_ENTRIES];
GDTPointer GDT::m_gdt_ptr;

void GDT::set_gate(size_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    if (num >= GDT_ENTRIES) return;

    m_gdt[num].base_low    = static_cast<uint16_t>(base & 0xFFFF);
    m_gdt[num].base_middle = static_cast<uint8_t>((base >> 16) & 0xFF);
    m_gdt[num].base_high   = static_cast<uint8_t>((base >> 24) & 0xFF);

    m_gdt[num].limit_low   = static_cast<uint16_t>(limit & 0xFFFF);
    m_gdt[num].granularity = static_cast<uint8_t>(((limit >> 16) & 0x0F) | (gran & 0xF0));

    m_gdt[num].access      = access;
}

void GDT::init() {
    m_gdt_ptr.limit = static_cast<uint16_t>(sizeof(m_gdt) - 1);
    m_gdt_ptr.base  = reinterpret_cast<uint32_t>(&m_gdt);

    // Initialize all entries to 0
    for (size_t i = 0; i < GDT_ENTRIES; i++) {
        set_gate(i, 0, 0, 0, 0);
    }

    // 0x00: Null Descriptor
    set_gate(0, 0, 0, 0, 0);

    // 0x08: Kernel Code Segment (Ring 0, Executable/Read, 4GB limit)
    set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // 0x10: Kernel Data Segment (Ring 0, Read/Write, 4GB limit)
    set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // 0x18: User Code Segment (Ring 3, Executable/Read, 4GB limit)
    set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // 0x20: User Data Segment (Ring 3, Read/Write, 4GB limit)
    set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    // Reload GDTR and segment registers
    gdt_flush(reinterpret_cast<uint32_t>(&m_gdt_ptr));
}

} // namespace x86_64
} // namespace arch
} // namespace nebula

extern "C" {
    void gdt_flush(uint32_t gdt_ptr_addr) {
        asm volatile (
            "lgdt (%0)\n\t"
            "ljmp $0x08, $1f\n\t"
            "1:\n\t"
            "mov $0x10, %%ax\n\t"
            "mov %%ax, %%ds\n\t"
            "mov %%ax, %%es\n\t"
            "mov %%ax, %%fs\n\t"
            "mov %%ax, %%gs\n\t"
            "mov %%ax, %%ss\n\t"
            :
            : "r"(gdt_ptr_addr)
            : "eax", "memory"
        );
    }

    void gdt_init() {
        nebula::arch::x86_64::GDT::init();
    }
}
