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

void Serial::init() {
    // QEMU virtual COM1 UART is pre-initialized by system BIOS.
}

void Serial::write_char(char c) {
    outb(COM1_PORT, c);
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
