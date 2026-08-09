#ifndef NEBULA_DRIVERS_VBE_HPP
#define NEBULA_DRIVERS_VBE_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace drivers {

/**
 * @brief Bochs VBE Dispi Interface Constants
 */
static const uint16_t VBE_DISPI_IOPORT_INDEX = 0x01CE;
static const uint16_t VBE_DISPI_IOPORT_DATA  = 0x01CF;

static const uint16_t VBE_DISPI_INDEX_ID          = 0;
static const uint16_t VBE_DISPI_INDEX_XRES        = 1;
static const uint16_t VBE_DISPI_INDEX_YRES        = 2;
static const uint16_t VBE_DISPI_INDEX_BPP         = 3;
static const uint16_t VBE_DISPI_INDEX_ENABLE      = 4;
static const uint16_t VBE_DISPI_INDEX_BANK        = 5;
static const uint16_t VBE_DISPI_INDEX_VIRT_WIDTH  = 6;
static const uint16_t VBE_DISPI_INDEX_VIRT_HEIGHT = 7;
static const uint16_t VBE_DISPI_INDEX_X_OFFSET    = 8;
static const uint16_t VBE_DISPI_INDEX_Y_OFFSET    = 9;

static const uint16_t VBE_DISPI_DISABLED          = 0x00;
static const uint16_t VBE_DISPI_ENABLED           = 0x01;
static const uint16_t VBE_DISPI_LFB_ENABLED       = 0x40;

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
 * @brief VESA VBE & Bochs BGA Linear Framebuffer Driver Class
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
     * @brief Write 16-bit word to Bochs BGA I/O Port
     */
    static void bga_write(uint16_t index, uint16_t data);

    /**
     * @brief Read 16-bit word from Bochs BGA I/O Port
     */
    static uint16_t bga_read(uint16_t index);

    /**
     * @brief Configure Bochs BGA hardware video mode (width, height, bpp)
     */
    static void bga_set_video_mode(uint16_t width, uint16_t height, uint16_t bpp);

    /**
     * @brief Initialize VBE Framebuffer driver, Bochs BGA, and Backbuffer
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

    /**
     * @brief Swap local dirty rectangle from backbuffer to physical linear framebuffer
     */
    static void swap_rect(int32_t x, int32_t y, uint32_t width, uint32_t height);

    static size_t get_width() { return m_width; }
    static size_t get_height() { return m_height; }
    static uint32_t* get_backbuffer_ptr() { return m_backbuffer; }
    static bool is_initialized() { return m_initialized; }
};

} // namespace drivers
} // namespace nebula

extern "C" {
    void vbe_init(nebula::drivers::vbe_mode_info_t* mode_info);
    void vbe_swap_buffers();
}

#endif // NEBULA_DRIVERS_VBE_HPP
