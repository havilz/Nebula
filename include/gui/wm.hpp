#ifndef NEBULA_GUI_WM_HPP
#define NEBULA_GUI_WM_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace gui {

typedef void (*window_render_func_t)(int32_t content_x, int32_t content_y, uint32_t content_w, uint32_t content_h);

/**
 * @brief GUI Window Data Structure
 */
struct Window {
    uint32_t id;
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
    char title[64];
    bool is_focused;
    bool is_dragging;
    int32_t drag_offset_x;
    int32_t drag_offset_y;
    uint32_t bg_color;
    window_render_func_t render_func;

    // macOS Aqua Window State Flags
    bool is_minimized;
    bool is_maximized;
    bool is_resizing;
    int32_t saved_x;
    int32_t saved_y;
    uint32_t saved_width;
    uint32_t saved_height;
};

static const size_t MAX_WINDOWS = 8;

/**
 * @brief Window Manager & Desktop Compositor Class
 */
class WindowManager {
private:
    static Window m_windows[MAX_WINDOWS];
    static size_t m_window_count;
    static int32_t m_focused_window_id;
    static bool m_prev_left_button;

    static uint32_t m_wallpaper_cache[800 * 600];
    static uint32_t m_cursor_backbuffer[24 * 24];
    static int32_t m_old_mouse_x;
    static int32_t m_old_mouse_y;
    static bool m_first_render;

public:
    /**
     * @brief Initialize Window Manager & Desktop Compositor
     */
    static void init();

    /**
     * @brief Create a new desktop GUI window
     */
    static Window* create_window(int32_t x, int32_t y, uint32_t w, uint32_t h, const char* title, window_render_func_t render_func);

    /**
     * @brief Render Wallpaper, Taskbar, Windows (Z-ordered), and Mouse Cursor
     */
    static void render_all();

    /**
     * @brief Handle mouse interaction for window focus and dragging
     */
    static void process_mouse(int32_t mouse_x, int32_t mouse_y, bool left_down);

    /**
     * @brief Render Deep Space gradient wallpaper
     */
    static void draw_wallpaper();

    /**
     * @brief Render Taskbar at bottom of screen
     */
    static void draw_taskbar();
};

} // namespace gui
} // namespace nebula

extern "C" {
    void wm_init();
    void wm_render_all();
}

#endif // NEBULA_GUI_WM_HPP
