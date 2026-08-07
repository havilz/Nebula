/**
 * @file pic.cpp
 * @brief 8259 Programmable Interrupt Controller implementation for x86_64
 * @author Nebula OS Team
 */

#include "../../../../include/kernel/arch/x86_64/pic.hpp"

namespace nebula {
namespace arch {
namespace x86_64 {

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "d"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

static inline void io_wait() {
    asm volatile ("jmp 1f; 1:");
}

static void pic_log(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        outb(0x3F8, str[i]);
    }
}

void PIC::remap(uint8_t master_offset, uint8_t slave_offset) {
    pic_log("[PIC] Reading masks...\n");
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    pic_log("[PIC] ICW1...\n");
    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();

    pic_log("[PIC] ICW2...\n");
    outb(PIC1_DATA, master_offset);
    io_wait();
    outb(PIC2_DATA, slave_offset);
    io_wait();

    pic_log("[PIC] ICW3...\n");
    outb(PIC1_DATA, 0x04);
    io_wait();
    outb(PIC2_DATA, 0x02);
    io_wait();

    pic_log("[PIC] ICW4...\n");
    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();

    pic_log("[PIC] Restoring masks...\n");
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);

    pic_log("[PIC] Remap complete!\n");
}

void PIC::disable() {
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void PIC::send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

} // namespace x86_64
} // namespace arch
} // namespace nebula
