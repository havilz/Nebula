#ifndef NEBULA_ARCH_X86_64_PIC_HPP
#define NEBULA_ARCH_X86_64_PIC_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

/**
 * @brief Programmable Interrupt Controller (8259 PIC) Driver
 */
class PIC {
private:
    static const uint16_t PIC1_COMMAND = 0x20;
    static const uint16_t PIC1_DATA    = 0x21;
    static const uint16_t PIC2_COMMAND = 0xA0;
    static const uint16_t PIC2_DATA    = 0xA1;

    static const uint8_t PIC_EOI        = 0x20;

    static const uint8_t ICW1_ICW4      = 0x01;
    static const uint8_t ICW1_INIT      = 0x10;
    static const uint8_t ICW4_8086      = 0x01;

public:
    /**
     * @brief Remap Master and Slave PIC IRQs to specified IDT vector offsets
     * @param offset1 Master PIC vector offset (e.g. 32 for IRQ 0-7)
     * @param offset2 Slave PIC vector offset (e.g. 40 for IRQ 8-15)
     */
    static void remap(uint8_t offset1 = 32, uint8_t offset2 = 40);

    /**
     * @brief Send End-of-Interrupt (EOI) signal to PIC for finished IRQ
     * @param irq IRQ line number (0-15)
     */
    static void send_eoi(uint8_t irq);

    /**
     * @brief Disable 8259 PIC interrupts (mask all IRQs)
     */
    static void disable();
};

} // namespace x86_64
} // namespace arch
} // namespace nebula

#endif // NEBULA_ARCH_X86_64_PIC_HPP
