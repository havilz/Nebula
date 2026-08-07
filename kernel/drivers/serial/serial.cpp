/**
 * @file serial.cpp
 * @brief Serial COM1 UART Driver implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../../include/kernel/drivers/serial.hpp"

namespace nebula {
namespace drivers {

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "d"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

void Serial::init() {
    outb(COM1_PORT + 1, 0x00); // Disable interrupts
    outb(COM1_PORT + 3, 0x80); // Enable DLAB (set divisor)
    outb(COM1_PORT + 0, 0x03); // Set divisor to 3 (38400 baud)
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(COM1_PORT + 2, 0xC7); // Enable FIFO
    outb(COM1_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

void Serial::write_char(char c) {
    uint8_t status;
    size_t timeout = 10000;
    do {
        status = inb(COM1_PORT + 5);
        if (--timeout == 0) break;
    } while ((status & 0x20) == 0);

    outb(COM1_PORT, static_cast<uint8_t>(c));
}

void Serial::write_string(const char* str) {
    if (str == nullptr) return;
    for (size_t i = 0; str[i] != '\0'; i++) {
        write_char(str[i]);
    }
}

} // namespace drivers
} // namespace nebula

extern "C" {
    void serial_init() {
        nebula::drivers::Serial::init();
    }

    void serial_write_char(char c) {
        nebula::drivers::Serial::write_char(c);
    }

    void serial_write_string(const char* str) {
        nebula::drivers::Serial::write_string(str);
    }
}
