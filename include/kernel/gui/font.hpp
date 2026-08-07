#ifndef NEBULA_GUI_FONT_HPP
#define NEBULA_GUI_FONT_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace gui {

/**
 * @brief 8x16 Bitmap Font Engine Class
 */
class Font {
public:
    /**
     * @brief Draw single character at coordinate (x, y)
     */
    static void draw_char(size_t x, size_t y, char c, uint32_t fg_color, uint32_t bg_color = 0x00000000);

    /**
     * @brief Draw string at coordinate (x, y)
     */
    static void draw_string(size_t x, size_t y, const char* str, uint32_t fg_color, uint32_t bg_color = 0x00000000);
};

} // namespace gui
} // namespace nebula

#endif // NEBULA_GUI_FONT_HPP
