/**
 * @file wm.cpp
 * @brief Window Manager & Desktop Compositor implementation for Nebula OS
 * @author Nebula OS Team
 */

#include <stddef.h>
#include <stdint.h>
#include "../../include/gui/wm.hpp"
#include "../../include/iokit/display/vbe.hpp"
#include "../../include/iokit/input/mouse.hpp"
#include "../../include/iokit/serial/serial.hpp"
#include "../../include/iokit/timer/pit.hpp"
#include "../../include/gui/font.hpp"

namespace nebula {
namespace gui {

Window WindowManager::m_windows[MAX_WINDOWS];
size_t WindowManager::m_window_count = 0;
int32_t WindowManager::m_focused_window_id = -1;
bool WindowManager::m_prev_left_button = false;

uint32_t WindowManager::m_wallpaper_cache[800 * 600];
uint32_t WindowManager::m_cursor_backbuffer[24 * 24];
int32_t WindowManager::m_old_mouse_x = -1;
int32_t WindowManager::m_old_mouse_y = -1;
bool WindowManager::m_first_render = true;

static void str_copy(char* dest, const char* src, size_t max_len) {
    if (!dest || !src) return;
    size_t i = 0;
    while (src[i] != '\0' && i < max_len - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void WindowManager::init() {
    m_window_count = 0;
    m_focused_window_id = -1;
    m_prev_left_button = false;
    m_first_render = true;
    m_old_mouse_x = -1;
    m_old_mouse_y = -1;

    // Pre-render Wallpaper Gradient to m_wallpaper_cache once at startup
    size_t w = nebula::drivers::VBE::get_width();
    size_t h = nebula::drivers::VBE::get_height();

    for (size_t y = 0; y < h; y++) {
        uint8_t r = static_cast<uint8_t>(15 + (y * 30) / h);
        uint8_t g = static_cast<uint8_t>(23 + (y * 40) / h);
        uint8_t b = static_cast<uint8_t>(42 + (y * 80) / h);
        uint32_t color = nebula::drivers::make_color(r, g, b);
        for (size_t x = 0; x < w; x++) {
            m_wallpaper_cache[y * w + x] = color;
        }
    }

    nebula::drivers::Serial::write_string("[WM] Desktop Compositor & Offscreen Wallpaper Cache Initialized\n");
}

Window* WindowManager::create_window(int32_t x, int32_t y, uint32_t w, uint32_t h, const char* title, window_render_func_t render_func) {
    if (m_window_count >= MAX_WINDOWS) return nullptr;

    Window* win = &m_windows[m_window_count];
    win->id = static_cast<uint32_t>(m_window_count + 1);
    win->x = x;
    win->y = y;
    win->width = w;
    win->height = h;
    str_copy(win->title, title, sizeof(win->title));
    win->is_focused = true;
    win->is_dragging = false;
    win->drag_offset_x = 0;
    win->drag_offset_y = 0;
    win->bg_color = nebula::drivers::COLOR_BG_NAVY;
    win->render_func = render_func;

    m_focused_window_id = win->id;
    m_window_count++;
    m_first_render = true;

    nebula::drivers::Serial::write_string("[WM] Created GUI Window: ");
    nebula::drivers::Serial::write_string(title);
    nebula::drivers::Serial::write_string("\n");

    return win;
}

void WindowManager::draw_wallpaper() {
    size_t w = nebula::drivers::VBE::get_width();
    size_t h = nebula::drivers::VBE::get_height();
    uint32_t* backbuf = nebula::drivers::VBE::get_backbuffer_ptr();

    // Fast direct pointer copy of pre-rendered wallpaper
    if (backbuf != nullptr) {
        for (size_t i = 0; i < w * h; i++) {
            backbuf[i] = m_wallpaper_cache[i];
        }
    }

    // Centered Desktop Brand Banner
    Font::draw_string(w / 2 - 120, 60, "Nebula OS - Desktop GUI v1.0", 0x38BDF8);
    Font::draw_string(w / 2 - 140, 80, "Preemptive Multitasking & VFS Active", 0x94A3B8);
}

void WindowManager::draw_taskbar() {
    size_t w = nebula::drivers::VBE::get_width();
    size_t h = nebula::drivers::VBE::get_height();
    size_t bar_h = 36;
    size_t bar_y = h - bar_h;

    // Dark Taskbar Panel
    nebula::drivers::VBE::fill_rect(0, bar_y, w, bar_h, 0x020617);
    nebula::drivers::VBE::fill_rect(0, bar_y, w, 2, 0x334155);

    // Start Button
    nebula::drivers::VBE::fill_rect(6, bar_y + 4, 80, bar_h - 8, 0x2563EB);
    Font::draw_string(16, bar_y + 10, "NEBULA", 0xFFFFFF);

    // Uptime Clock
    uint32_t seconds = static_cast<uint32_t>(nebula::drivers::PIT::get_ticks() / 100);
    uint32_t mins = seconds / 60;
    uint32_t secs = seconds % 60;

    char time_str[32];
    time_str[0] = 'U'; time_str[1] = 'P'; time_str[2] = ':'; time_str[3] = ' ';
    time_str[4] = '0' + (mins / 10);
    time_str[5] = '0' + (mins % 10);
    time_str[6] = ':';
    time_str[7] = '0' + (secs / 10);
    time_str[8] = '0' + (secs % 10);
    time_str[9] = '\0';

    Font::draw_string(w - 100, bar_y + 10, time_str, 0xFBBF24);
}

void WindowManager::process_mouse(int32_t mouse_x, int32_t mouse_y, bool left_down) {
    // Handle Window Dragging
    for (size_t i = 0; i < m_window_count; i++) {
        Window* win = &m_windows[i];
        if (win->is_dragging) {
            if (left_down) {
                win->x = mouse_x - win->drag_offset_x;
                win->y = mouse_y - win->drag_offset_y;
                m_first_render = true;
            } else {
                win->is_dragging = false;
            }
        }
    }

    // Handle Mouse Click Down Event
    if (left_down && !m_prev_left_button) {
        for (int i = static_cast<int>(m_window_count) - 1; i >= 0; i--) {
            Window* win = &m_windows[i];

            // Check Titlebar Click (Drag start or Focus)
            if (mouse_x >= win->x && mouse_x <= win->x + static_cast<int32_t>(win->width) &&
                mouse_y >= win->y && mouse_y <= win->y + 24) {

                m_focused_window_id = win->id;
                win->is_dragging = true;
                win->drag_offset_x = mouse_x - win->x;
                win->drag_offset_y = mouse_y - win->y;
                m_first_render = true;
                break;
            }
            // Check Inside Window Body
            else if (mouse_x >= win->x && mouse_x <= win->x + static_cast<int32_t>(win->width) &&
                     mouse_y >= win->y && mouse_y <= win->y + static_cast<int32_t>(win->height)) {
                m_focused_window_id = win->id;
                m_first_render = true;
                break;
            }
        }
    }

    m_prev_left_button = left_down;
}

void WindowManager::render_all() {
    nebula::drivers::mouse_state_t mouse = nebula::drivers::Mouse::get_state();
    process_mouse(mouse.x, mouse.y, mouse.left_button);

    bool mouse_moved = (mouse.x != m_old_mouse_x || mouse.y != m_old_mouse_y);

    if (m_first_render) {
        // 1. Draw Wallpaper
        draw_wallpaper();

        // 2. Draw Windows
        for (size_t i = 0; i < m_window_count; i++) {
            Window* win = &m_windows[i];
            bool is_focused = (win->id == static_cast<uint32_t>(m_focused_window_id));

            // Window Shadow & Outer Border
            nebula::drivers::VBE::fill_rect(win->x + 4, win->y + 4, win->width, win->height, 0x050B14);
            nebula::drivers::VBE::fill_rect(win->x, win->y, win->width, win->height, is_focused ? 0x38BDF8 : 0x475569);
            nebula::drivers::VBE::fill_rect(win->x + 2, win->y + 2, win->width - 4, win->height - 4, win->bg_color);

            // Titlebar Panel
            uint32_t titlebar_color = is_focused ? 0x1E293B : 0x334155;
            nebula::drivers::VBE::fill_rect(win->x + 2, win->y + 2, win->width - 4, 24, titlebar_color);

            // Window Controls (Close X & Minimize - Buttons)
            nebula::drivers::VBE::fill_rect(win->x + win->width - 22, win->y + 6, 16, 16, 0xEF4444);
            Font::draw_char(win->x + win->width - 18, win->y + 6, 'X', 0xFFFFFF);

            nebula::drivers::VBE::fill_rect(win->x + win->width - 42, win->y + 6, 16, 16, 0xF59E0B);
            Font::draw_char(win->x + win->width - 38, win->y + 6, '-', 0xFFFFFF);

            // Title Text
            Font::draw_string(win->x + 10, win->y + 6, win->title, is_focused ? 0xF8FAFC : 0x94A3B8);

            // Window Client Area Callback
            if (win->render_func != nullptr) {
                win->render_func(win->x + 10, win->y + 32, win->width - 20, win->height - 40);
            }
        }

        // 3. Draw Taskbar
        draw_taskbar();

        // 4. Save Full Cursor Backbuffer (24x24)
        for (int ry = 0; ry < 24; ry++) {
            for (int rx = 0; rx < 24; rx++) {
                m_cursor_backbuffer[ry * 24 + rx] = nebula::drivers::VBE::get_pixel(mouse.x + rx, mouse.y + ry);
            }
        }

        // 5. Draw Mouse Cursor
        nebula::drivers::Mouse::draw_cursor(mouse.x, mouse.y);
        nebula::drivers::VBE::swap_buffers();

        m_old_mouse_x = mouse.x;
        m_old_mouse_y = mouse.y;
        m_first_render = false;

    } else if (mouse_moved) {
        // Fast Restore Old Cursor Region (24x24)
        if (m_old_mouse_x >= 0 && m_old_mouse_y >= 0) {
            for (int ry = 0; ry < 24; ry++) {
                for (int rx = 0; rx < 24; rx++) {
                    nebula::drivers::VBE::put_pixel(m_old_mouse_x + rx, m_old_mouse_y + ry, m_cursor_backbuffer[ry * 24 + rx]);
                }
            }
            nebula::drivers::VBE::swap_rect(m_old_mouse_x, m_old_mouse_y, 24, 24);
        }

        // Save New Cursor Region (24x24)
        for (int ry = 0; ry < 24; ry++) {
            for (int rx = 0; rx < 24; rx++) {
                m_cursor_backbuffer[ry * 24 + rx] = nebula::drivers::VBE::get_pixel(mouse.x + rx, mouse.y + ry);
            }
        }

        // Draw Mouse Cursor at New Position
        nebula::drivers::Mouse::draw_cursor(mouse.x, mouse.y);
        nebula::drivers::VBE::swap_rect(mouse.x, mouse.y, 24, 24);

        m_old_mouse_x = mouse.x;
        m_old_mouse_y = mouse.y;
    }
}

} // namespace gui
} // namespace nebula

extern "C" {
    void wm_init() {
        nebula::gui::WindowManager::init();
    }

    void wm_render_all() {
        nebula::gui::WindowManager::render_all();
    }
}
