/**
 * @file serial.cpp
 * @brief Serial COM1 UART Driver implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../../include/kernel/drivers/serial.hpp"
#include "../../../include/kernel/arch/x86_64/io.hpp"

namespace nebula {
namespace drivers {

void Serial::init() {
    using namespace nebula::arch::x86_64;
    outb(COM1_PORT + 1, 0x00); // Disable interrupts
    outb(COM1_PORT + 3, 0x80); // Enable DLAB (set divisor)
    outb(COM1_PORT + 0, 0x03); // Set divisor to 3 (38400 baud)
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(COM1_PORT + 2, 0xC7); // Enable FIFO
    outb(COM1_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

void Serial::write_char(char c) {
    using namespace nebula::arch::x86_64;
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

void Serial::write_hex32(uint32_t val) {
    static const char hex_chars[] = "0123456789ABCDEF";
    write_string("0x");
    for (int i = 28; i >= 0; i -= 4) {
        write_char(hex_chars[(val >> i) & 0xF]);
    }
}

void Serial::write_dec(size_t val) {
    if (val == 0) {
        write_char('0');
        return;
    }
    char buf[32];
    int i = 0;
    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }
    while (i > 0) {
        write_char(buf[--i]);
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
