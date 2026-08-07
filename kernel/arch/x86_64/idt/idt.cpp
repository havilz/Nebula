/**
 * @file idt.cpp
 * @brief Interrupt Descriptor Table (IDT) implementation for x86_64
 * @author Nebula OS Team
 */

#include "../../../../include/kernel/arch/x86_64/idt.hpp"

extern "C" const uint64_t isr_stub_table[48];

namespace nebula {
namespace arch {
namespace x86_64 {

IDTEntry IDT::m_idt[IDT_ENTRIES];
IDTPointer IDT::m_idt_ptr;

void IDT::set_gate(uint8_t vector, uint64_t isr_base, uint16_t selector, uint8_t flags, uint8_t ist) {
    m_idt[vector].offset_low  = static_cast<uint16_t>(isr_base & 0xFFFF);
    m_idt[vector].selector    = selector;
    m_idt[vector].ist         = ist & 0x07;
    m_idt[vector].type_attr   = flags;
    m_idt[vector].offset_mid  = static_cast<uint16_t>((isr_base >> 16) & 0xFFFF);
    m_idt[vector].offset_high = static_cast<uint32_t>((isr_base >> 32) & 0xFFFFFFFF);
    m_idt[vector].zero        = 0;
}

void IDT::init() {
    m_idt_ptr.limit = sizeof(m_idt) - 1;
    m_idt_ptr.base  = reinterpret_cast<uint64_t>(&m_idt);

    // Clear all IDT entries
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&m_idt);
    for (size_t i = 0; i < sizeof(m_idt); i++) {
        ptr[i] = 0;
    }

    // Populate CPU Exception (0-31) and Hardware IRQ (32-47) vectors
    for (size_t i = 0; i < 48; i++) {
        // Gate Type 0x8E = Present | Ring 0 | 64-bit Interrupt Gate
        set_gate(static_cast<uint8_t>(i), isr_stub_table[i], 0x08, 0x8E);
    }

    // Load IDTR using LIDT instruction
    asm volatile ("lidt %0" : : "m"(m_idt_ptr));
}

} // namespace x86_64
} // namespace arch
} // namespace nebula
