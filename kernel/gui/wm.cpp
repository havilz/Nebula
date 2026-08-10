#include <stddef.h>
#include <stdint.h>
#include "../../include/gui/wm.hpp"
#include "../../include/gui/aqua.hpp"
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

    AquaEngine::init();

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

    nebula::drivers::Serial::write_string("[WM] Desktop Compositor & macOS Aqua Engine Initialized\n");
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
    win->bg_color = 0x1E1E2E; // Dark Slate macOS Window Background
    win->render_func = render_func;

    win->is_minimized = false;
    win->is_maximized = false;
    win->is_resizing = false;
    win->saved_x = x;
    win->saved_y = y;
    win->saved_width = w;
    win->saved_height = h;

    m_focused_window_id = win->id;
    m_window_count++;
    m_first_render = true;

    nebula::drivers::Serial::write_string("[WM] Created macOS Aqua Window: ");
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
    Font::draw_string(w / 2 - 120, 60, "Nebula OS - macOS Aqua Desktop v1.1", 0x38BDF8);
    Font::draw_string(w / 2 - 140, 80, "Preemptive Multitasking & VFS Active", 0x94A3B8);
}

void WindowManager::draw_taskbar() {
    // Replaced by Top Global Menu Bar & Floating Dock
}

void WindowManager::process_mouse(int32_t mouse_x, int32_t mouse_y, bool left_down) {
    // Handle Window Dragging & Resizing
    for (size_t i = 0; i < m_window_count; i++) {
        Window* win = &m_windows[i];
        if (win->is_minimized) continue;

        if (win->is_dragging) {
            if (left_down) {
                win->x = mouse_x - win->drag_offset_x;
                win->y = mouse_y - win->drag_offset_y;
                if (win->y < 28) win->y = 28; // Don't drag above Top Menu Bar
                m_first_render = true;
            } else {
                win->is_dragging = false;
            }
        } else if (win->is_resizing) {
            if (left_down) {
                int32_t new_w = mouse_x - win->x;
                int32_t new_h = mouse_y - win->y;
                if (new_w < 180) new_w = 180;
                if (new_h < 120) new_h = 120;
                win->width = static_cast<uint32_t>(new_w);
                win->height = static_cast<uint32_t>(new_h);
                m_first_render = true;
            } else {
                win->is_resizing = false;
            }
        }
    }

    // Handle Mouse Click Down Event
    if (left_down && !m_prev_left_button) {
        // Check Floating Dock Click
        int dock_click = AquaEngine::check_dock_click(mouse_x, mouse_y);
        if (dock_click >= 0) {
            if (dock_click < static_cast<int>(m_window_count)) {
                Window* win = &m_windows[dock_click];
                win->is_minimized = false;
                m_focused_window_id = win->id;
                m_first_render = true;
            }
        }

        for (int i = static_cast<int>(m_window_count) - 1; i >= 0; i--) {
            Window* win = &m_windows[i];
            if (win->is_minimized) continue;

            // Check Traffic Light Buttons Hitbox (🔴 🟡 🟢)
            if (mouse_y >= win->y + 4 && mouse_y <= win->y + 20) {
                // 🔴 Red Close Button
                if (mouse_x >= win->x + 8 && mouse_x <= win->x + 20) {
                    win->is_minimized = true;
                    m_first_render = true;
                    break;
                }
                // 🟡 Yellow Minimize Button
                else if (mouse_x >= win->x + 26 && mouse_x <= win->x + 38) {
                    win->is_minimized = true;
                    m_first_render = true;
                    break;
                }
                // 🟢 Green Maximize Button
                else if (mouse_x >= win->x + 44 && mouse_x <= win->x + 56) {
                    if (!win->is_maximized) {
                        win->saved_x = win->x;
                        win->saved_y = win->y;
                        win->saved_width = win->width;
                        win->saved_height = win->height;
                        win->x = 0;
                        win->y = 28; // Under Top Bar
                        win->width = 800;
                        win->height = 600 - 28 - 66; // Above Dock
                        win->is_maximized = true;
                    } else {
                        win->x = win->saved_x;
                        win->y = win->saved_y;
                        win->width = win->saved_width;
                        win->height = win->saved_height;
                        win->is_maximized = false;
                    }
                    m_first_render = true;
                    break;
                }
            }

            // Check Bottom-Right Resize Handle Corner ($16 \times 16$ px)
            if (mouse_x >= win->x + static_cast<int32_t>(win->width) - 16 &&
                mouse_x <= win->x + static_cast<int32_t>(win->width) &&
                mouse_y >= win->y + static_cast<int32_t>(win->height) - 16 &&
                mouse_y <= win->y + static_cast<int32_t>(win->height)) {

                m_focused_window_id = win->id;
                win->is_resizing = true;
                m_first_render = true;
                break;
            }

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

        const char* active_app_title = "Desktop";

        // 2. Draw Windows (Z-ordered)
        for (size_t i = 0; i < m_window_count; i++) {
            Window* win = &m_windows[i];
            if (win->is_minimized) continue;

            bool is_focused = (win->id == static_cast<uint32_t>(m_focused_window_id));
            if (is_focused) active_app_title = win->title;

            // Window Drop Shadow & Outer Glass Border (Rounded 10px)
            nebula::drivers::VBE::fill_rounded_rect(win->x + 4, win->y + 4, win->width, win->height, 10, 0x050B14);
            nebula::drivers::VBE::fill_rounded_rect(win->x - 1, win->y - 1, win->width + 2, win->height + 2, 11, is_focused ? 0x38BDF8 : 0x45475A);
            nebula::drivers::VBE::fill_rounded_rect(win->x, win->y, win->width, win->height, 10, win->bg_color);

            // Dark macOS Aqua Titlebar Panel (#1E1E2E with rounded top corners)
            uint32_t titlebar_color = is_focused ? 0x1E1E2E : 0x2A2B3D;
            nebula::drivers::VBE::fill_rounded_rect(win->x + 1, win->y + 1, win->width - 2, 28, 8, titlebar_color);
            nebula::drivers::VBE::fill_rect(win->x + 1, win->y + 20, win->width - 2, 8, titlebar_color);
            nebula::drivers::VBE::fill_rect(win->x + 1, win->y + 28, win->width - 2, 1, 0x313244);

            // macOS Traffic Light Window Controls (🔴 🟡 🟢)
            AquaEngine::draw_traffic_lights(win->x, win->y + 2);

            // Window Title Text (Centered nicely)
            Font::draw_string(win->x + 72, win->y + 8, win->title, is_focused ? 0xF8FAFC : 0x94A3B8);

            // Window Client Area Callback
            if (win->render_func != nullptr) {
                win->render_func(win->x + 10, win->y + 32, win->width - 20, win->height - 42);
            }

            // Corner Resize Grip Handle (::: at bottom-right)
            int32_t rx = win->x + static_cast<int32_t>(win->width) - 12;
            int32_t ry = win->y + static_cast<int32_t>(win->height) - 12;
            Font::draw_string(rx, ry, "::", 0x6C7086);
        }

        // 3. Draw Top Global Menu Bar & Floating Dock
        AquaEngine::draw_top_menubar(active_app_title);
        AquaEngine::draw_floating_dock();

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

