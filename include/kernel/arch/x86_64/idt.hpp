#ifndef NEBULA_ARCH_X86_64_IDT_HPP
#define NEBULA_ARCH_X86_64_IDT_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

/**
 * @brief 16-byte IDT Entry structure for x86_64 Long Mode
 */
struct __attribute__((packed)) IDTEntry {
    uint16_t offset_low;      ///< Lower 16 bits of ISR offset
    uint16_t selector;        ///< Target code segment selector (e.g. 0x08)
    uint8_t  ist;             ///< Interrupt Stack Table offset (bits 0..2)
    uint8_t  type_attr;       ///< Gate type, DPL, and Present flag
    uint16_t offset_mid;      ///< Middle 16 bits of ISR offset
    uint32_t offset_high;     ///< Upper 32 bits of ISR offset
    uint32_t zero;            ///< Reserved (must be zero)
};

/**
 * @brief IDT Pointer structure for LIDT instruction
 */
struct __attribute__((packed)) IDTPointer {
    uint16_t limit;           ///< Table limit (size - 1)
    uint64_t base;            ///< Linear base address of IDT
};

/**
 * @brief IDT Management Class
 */
class IDT {
private:
    static const size_t IDT_ENTRIES = 256;
    static IDTEntry m_idt[IDT_ENTRIES];
    static IDTPointer m_idt_ptr;

public:
    /**
     * @brief Set an IDT entry for a specific vector
     * @param vector Interrupt vector index (0 - 255)
     * @param isr_base Address of Interrupt Service Routine handler
     * @param selector Segment selector (0x08 for kernel code)
     * @param flags Type and attribute flags
     * @param ist Interrupt Stack Table offset (0 for default)
     */
    static void set_gate(uint8_t vector, uint64_t isr_base, uint16_t selector, uint8_t flags, uint8_t ist = 0);

    /**
     * @brief Initialize IDT gates and load IDTR
     */
    static void init();
};

} // namespace x86_64
} // namespace arch
} // namespace nebula

#endif // NEBULA_ARCH_X86_64_IDT_HPP
