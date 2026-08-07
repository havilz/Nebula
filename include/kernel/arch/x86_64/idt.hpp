#ifndef NEBULA_ARCH_X86_64_IDT_HPP
#define NEBULA_ARCH_X86_64_IDT_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

/**
 * @brief IDT Gate Entry structure (8 bytes)
 */
struct __attribute__((packed)) IDTEntry {
    uint16_t base_low;  ///< Lower 16 bits of ISR function address
    uint16_t selector;  ///< Kernel Code Segment Selector (0x08)
    uint8_t  always0;   ///< Reserved (must be 0)
    uint8_t  flags;     ///< Type and Attributes (0x8E = Present, Ring 0, Interrupt Gate 32-bit)
    uint16_t base_high; ///< Upper 16 bits of ISR function address
};

/**
 * @brief IDTR pointer structure (6 bytes)
 */
struct __attribute__((packed)) IDTPointer {
    uint16_t limit; ///< Size of IDT table - 1
    uint32_t base;  ///< Base physical address of IDT array
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
     * @brief Initialize IDT gates and load IDTR register
     */
    static void init();

    /**
     * @brief Configure an IDT gate descriptor
     * @param num Vector index (0-255)
     * @param base Address of ISR entry function
     * @param sel Segment selector (0x08)
     * @param flags Gate attribute flags (0x8E)
     */
    static void set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
};

} // namespace x86_64
} // namespace arch
} // namespace nebula

extern "C" {
    void idt_init();
}

#endif // NEBULA_ARCH_X86_64_IDT_HPP
