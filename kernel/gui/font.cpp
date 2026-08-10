/**
 * @file font.cpp
 * @brief 8x16 Bitmap Font Engine implementation for Nebula OS
 * @author Nebula OS Team
 */

#include <gui/font.hpp>
#include <iokit/display/vbe.hpp>

namespace nebula {
namespace gui {

// Standard 8x16 VGA Font Glyph Patterns
static const uint8_t font_vga_8x16[128][16] = {
    // ASCII 0-31: Blank / Control
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},

    // 32 ' ' (Space)
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 33 '!'
    {0x00,0x00,0x18,0x3C,0x3C,0x3C,0x18,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00},
    // 65 'A'
    {0x00,0x00,0x18,0x3C,0x66,0x66,0xCC,0xCC,0xFF,0xCC,0xCC,0xCC,0xCC,0x00,0x00,0x00},
    // 66 'B'
    {0x00,0x00,0xFC,0x66,0x66,0x66,0x7C,0x66,0x66,0x66,0x66,0xFC,0x00,0x00,0x00,0x00},
    // 67 'C'
    {0x00,0x00,0x3E,0x66,0xCC,0xC0,0xC0,0xC0,0xC0,0xCC,0x66,0x3E,0x00,0x00,0x00,0x00},
};

void Font::draw_char(size_t x, size_t y, char c, uint32_t fg_color, uint32_t bg_color) {
    uint8_t u_c = static_cast<uint8_t>(c);
    if (u_c >= 128) u_c = '?';

    // Simple pixel glyph drawing for 8x16 font
    for (size_t row = 0; row < 16; row++) {
        uint8_t bits = (u_c >= 32 && u_c < 128) ? font_vga_8x16[u_c][row] : 0xFF;
        if (bits == 0 && u_c >= 'a' && u_c <= 'z') bits = font_vga_8x16['A' + (u_c - 'a')][row]; // Fallback uppercase glyph

        for (size_t col = 0; col < 8; col++) {
            bool pixel_on = (bits & (1 << (7 - col))) != 0;
            if (pixel_on) {
                nebula::drivers::VBE::put_pixel(x + col, y + row, fg_color);
            } else if (bg_color != 0x00000000) {
                nebula::drivers::VBE::put_pixel(x + col, y + row, bg_color);
            }
        }
    }
}

void Font::draw_string(size_t x, size_t y, const char* str, uint32_t fg_color, uint32_t bg_color) {
    if (str == nullptr) return;
    size_t curr_x = x;
    size_t curr_y = y;

    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            curr_x = x;
            curr_y += 16;
        } else {
            draw_char(curr_x, curr_y, str[i], fg_color, bg_color);
            curr_x += 8;
        }
    }
}

} // namespace gui
} // namespace nebula
