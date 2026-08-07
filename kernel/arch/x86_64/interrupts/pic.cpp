/**
 * @file pic.cpp
 * @brief 8259 Programmable Interrupt Controller (PIC) implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../../../include/kernel/arch/x86_64/pic.hpp"
#include "../../../../include/kernel/arch/x86_64/io.hpp"

namespace nebula {
namespace arch {
namespace x86_64 {

void PIC::remap(uint8_t offset1, uint8_t offset2) {
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    // ICW1: Start initialization sequence
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // ICW2: Vector offsets
    outb(PIC1_DATA, offset1); // Master PIC IRQ 0-7 -> vectors offset1..offset1+7
    io_wait();
    outb(PIC2_DATA, offset2); // Slave PIC IRQ 8-15 -> vectors offset2..offset2+7
    io_wait();

    // ICW3: Master/Slave cascade connection
    outb(PIC1_DATA, 4); // Tell Master PIC that Slave PIC is at IRQ2 (0000 0100)
    io_wait();
    outb(PIC2_DATA, 2); // Tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    // ICW4: 8086/88 Mode
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Restore saved masks
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void PIC::send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

void PIC::disable() {
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

} // namespace x86_64
} // namespace arch
} // namespace nebula

extern "C" {
    void pic_remap(uint8_t offset1, uint8_t offset2) {
        nebula::arch::x86_64::PIC::remap(offset1, offset2);
    }

    void pic_send_eoi(uint8_t irq) {
        nebula::arch::x86_64::PIC::send_eoi(irq);
    }
}
