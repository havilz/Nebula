#ifndef NEBULA_DRIVERS_SERIAL_HPP
#define NEBULA_DRIVERS_SERIAL_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace drivers {

static const uint16_t COM1_PORT = 0x3F8;

/**
 * @brief Serial COM1 UART 16550 Driver for Kernel Debug Logging
 */
class Serial {
public:
    /**
     * @brief Initialize COM1 serial port (38400 baud rate, 8N1 mode)
     */
    static void init();

    /**
     * @brief Transmit a single character over COM1 serial port
     * @param c Character byte to transmit
     */
    static void write_char(char c);

    /**
     * @brief Transmit a null-terminated string over COM1 serial port
     * @param str Pointer to null-terminated C-string
     */
    static void write_string(const char* str);

    /**
     * @brief Transmit a 32-bit hexadecimal number over COM1 serial port
     * @param val 32-bit integer value to print
     */
    static void write_hex32(uint32_t val);

    /**
     * @brief Transmit a decimal number over COM1 serial port
     * @param val Decimal integer value to print
     */
    static void write_dec(size_t val);
};

} // namespace drivers
} // namespace nebula

extern "C" {
    void serial_init();
    void serial_write_char(char c);
    void serial_write_string(const char* str);
}

#endif // NEBULA_DRIVERS_SERIAL_HPP
