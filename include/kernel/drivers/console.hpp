#ifndef NEBULA_CONSOLE_HPP
#define NEBULA_CONSOLE_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace drivers {

enum VGAColor : uint8_t {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_YELLOW = 14,
    COLOR_WHITE = 15,
};

extern "C" {
    void vga_clear(uint8_t bg);
    void vga_write_at(size_t x, size_t y, const char* str, uint8_t fg, uint8_t bg);
}

class VGAConsole {
public:
    void clear(VGAColor bg = COLOR_BLUE);
    void write_at(size_t x, size_t y, const char* str, VGAColor fg, VGAColor bg);
};

} // namespace drivers
} // namespace nebula

#endif
