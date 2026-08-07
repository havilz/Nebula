/**
 * @file idt.cpp
 * @brief Interrupt Descriptor Table (IDT) implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../../../include/kernel/arch/x86_64/idt.hpp"

namespace nebula {
namespace arch {
namespace x86_64 {

IDTEntry IDT::m_idt[IDT::IDT_ENTRIES];
IDTPointer IDT::m_idt_ptr;

void IDT::set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    m_idt[num].base_low  = static_cast<uint16_t>(base & 0xFFFF);
    m_idt[num].base_high = static_cast<uint16_t>((base >> 16) & 0xFFFF);

    m_idt[num].selector  = sel;
    m_idt[num].always0   = 0;
    m_idt[num].flags     = flags;
}

void IDT::init() {
    m_idt_ptr.limit = static_cast<uint16_t>(sizeof(m_idt) - 1);
    m_idt_ptr.base  = reinterpret_cast<uint32_t>(&m_idt);

    for (size_t i = 0; i < IDT_ENTRIES; i++) {
        set_gate(static_cast<uint8_t>(i), 0, 0, 0);
    }

    // Execute lidt instruction
    asm volatile ("lidt %0" : : "m"(m_idt_ptr));
}

} // namespace x86_64
} // namespace arch
} // namespace nebula

extern "C" {
    void idt_init() {
        nebula::arch::x86_64::IDT::init();
    }
}
