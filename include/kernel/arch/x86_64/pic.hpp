#ifndef NEBULA_ARCH_X86_64_PIC_HPP
#define NEBULA_ARCH_X86_64_PIC_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

/**
 * @brief 8259 Programmable Interrupt Controller (PIC) Driver
 */
class PIC {
public:
    static const uint16_t PIC1_COMMAND = 0x20;
    static const uint16_t PIC1_DATA    = 0x21;
    static const uint16_t PIC2_COMMAND = 0xA0;
    static const uint16_t PIC2_DATA    = 0xA1;

    static const uint8_t PIC_EOI        = 0x20;

    /**
     * @brief Remap PIC IRQs (0-15) to custom IDT vectors (default: 32-47)
     * @param master_offset Starting vector offset for Master PIC (IRQ 0-7)
     * @param slave_offset Starting vector offset for Slave PIC (IRQ 8-15)
     */
    static void remap(uint8_t master_offset = 32, uint8_t slave_offset = 40);

    /**
     * @brief Send End-Of-Interrupt (EOI) command to PIC
     * @param irq IRQ line number (0-15)
     */
    static void send_eoi(uint8_t irq);

    /**
     * @brief Disable/Mask 8259 PIC completely
     */
    static void disable();
};

} // namespace x86_64
} // namespace arch
} // namespace nebula

#endif // NEBULA_ARCH_X86_64_PIC_HPP
