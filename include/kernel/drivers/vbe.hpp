#ifndef NEBULA_DRIVERS_VBE_HPP
#define NEBULA_DRIVERS_VBE_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace drivers {

/**
 * @brief VESA VBE Mode Info Structure
 */
struct __attribute__((packed)) vbe_mode_info_t {
    uint16_t attributes;
    uint8_t  window_a;
    uint8_t  window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch;              ///< Bytes per scanline
    uint16_t width;              ///< Screen width in pixels
    uint16_t height;             ///< Screen height in pixels
    uint8_t  w_char;
    uint8_t  y_char;
    uint8_t  planes;
    uint8_t  bpp;                 ///< Bits per pixel (32-bit)
    uint8_t  banks;
    uint8_t  memory_model;
    uint8_t  bank_size;
    uint8_t  image_pages;
    uint8_t  reserved0;
    uint8_t  red_mask;
    uint8_t  red_position;
    uint8_t  green_mask;
    uint8_t  green_position;
    uint8_t  blue_mask;
    uint8_t  blue_position;
    uint8_t  reserved_mask;
    uint8_t  reserved_position;
    uint8_t  direct_color_attributes;
    uint32_t framebuffer;        ///< Physical Linear Framebuffer Address
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t  reserved1[206];
};

/**
 * @brief Color helper factory
 */
static inline uint32_t make_color(uint8_t r, uint8_t g, uint8_t b) {
    return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | static_cast<uint32_t>(b);
}

// Preset Color Palette
static const uint32_t COLOR_BG_DARK     = 0x0F172A; // Slate Dark 900
static const uint32_t COLOR_BG_NAVY     = 0x1E293B; // Slate Navy 800
static const uint32_t COLOR_TITLEBAR    = 0x334155; // Titlebar Slate 700
static const uint32_t COLOR_TEXT_WHITE  = 0xF8FAFC;
static const uint32_t COLOR_TEXT_MUTED  = 0x94A3B8;
static const uint32_t COLOR_ACCENT_BLUE = 0x38BDF8; // Sky Blue
static const uint32_t COLOR_ACCENT_CYAN = 0x2DD4BF; // Teal Cyan
static const uint32_t COLOR_ACCENT_GOLD = 0xFBBF24; // Amber Gold
static const uint32_t COLOR_BTN_CLOSE   = 0xEF4444; // Red Close Btn
static const uint32_t COLOR_BTN_MIN     = 0xF59E0B; // Amber Min Btn

/**
 * @brief VESA VBE Linear Framebuffer Driver Class
 */
class VBE {
private:
    static uint32_t* m_framebuffer;
    static uint32_t* m_backbuffer;
    static size_t m_width;
    static size_t m_height;
    static size_t m_pitch;
    static size_t m_bpp;
    static bool m_initialized;

public:
    /**
     * @brief Initialize VBE Framebuffer driver and allocate Backbuffer
     */
    static void init(vbe_mode_info_t* mode_info);

    /**
     * @brief Put a 32-bit RGB pixel into backbuffer
     */
    static void put_pixel(size_t x, size_t y, uint32_t color);

    /**
     * @brief Get 32-bit RGB pixel color from backbuffer
     */
    static uint32_t get_pixel(size_t x, size_t y);

    /**
     * @brief Draw a hollow rectangle boundary
     */
    static void draw_rect(size_t x, size_t y, size_t width, size_t height, uint32_t color);

    /**
     * @brief Draw a filled solid rectangle
     */
    static void fill_rect(size_t x, size_t y, size_t width, size_t height, uint32_t color);

    /**
     * @brief Clear backbuffer with background color
     */
    static void clear(uint32_t color);

    /**
     * @brief Swap backbuffer memory to physical linear framebuffer (flicker-free)
     */
    static void swap_buffers();

    static size_t get_width() { return m_width; }
    static size_t get_height() { return m_height; }
    static bool is_initialized() { return m_initialized; }
};

} // namespace drivers
} // namespace nebula

extern "C" {
    void vbe_init(nebula::drivers::vbe_mode_info_t* mode_info);
    void vbe_swap_buffers();
}

#endif // NEBULA_DRIVERS_VBE_HPP
