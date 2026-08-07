/**
 * @file vga.cpp
 * @brief VGA Text Mode Console Driver implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../../include/kernel/drivers/console.hpp"

namespace nebula {
namespace drivers {

void VGAConsole::clear(VGAColor bg) {
    vga_clear(static_cast<uint8_t>(bg));
}

void VGAConsole::write_at(size_t x, size_t y, const char* str, VGAColor fg, VGAColor bg) {
    vga_write_at(x, y, str, static_cast<uint8_t>(fg), static_cast<uint8_t>(bg));
}

} // namespace drivers
} // namespace nebula

extern "C" {

void vga_clear(uint8_t bg) {
    volatile uint16_t* vga_buffer = reinterpret_cast<volatile uint16_t*>(0xB8000);
    uint16_t entry = static_cast<uint16_t>(' ') | (static_cast<uint16_t>((bg << 4) | 0x0F) << 8);
    for (size_t i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = entry;
    }
}

void vga_write_at(size_t x, size_t y, const char* str, uint8_t fg, uint8_t bg) {
    volatile uint16_t* vga_buffer = reinterpret_cast<volatile uint16_t*>(0xB8000);
    uint8_t color = fg | (bg << 4);
    y = y % 25;
    x = x % 80;
    size_t index = y * 80 + x;
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (index >= 80 * 25) index = index % (80 * 25);
        vga_buffer[index++] = static_cast<uint16_t>(str[i]) | (static_cast<uint16_t>(color) << 8);
    }
}

}