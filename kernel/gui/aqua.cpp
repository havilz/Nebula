/**
 * @file aqua.cpp
 * @brief macOS Aqua UI Design Engine implementation for Nebula OS
 * @author Nebula OS Team
 */

#include <stddef.h>
#include <stdint.h>
#include "../../include/gui/aqua.hpp"
#include "../../include/gui/font.hpp"
#include "../../include/iokit/display/vbe.hpp"
#include "../../include/iokit/serial/serial.hpp"
#include "../../include/iokit/timer/pit.hpp"

namespace nebula {
namespace gui {

DockItem AquaEngine::m_dock_items[4];
size_t AquaEngine::m_dock_count = 0;

void AquaEngine::init() {
    m_dock_items[0] = { "Terminal", "TERM", 0x38BDF8, true, true, 2 };
    m_dock_items[1] = { "Files", "FILE", 0xFBBF24, false, false, -1 };
    m_dock_items[2] = { "SysMon", "STAT", 0x34D399, true, false, 1 };
    m_dock_items[3] = { "Settings", "SETT", 0xA78BFA, false, false, -1 };
    m_dock_count = 4;

    nebula::drivers::Serial::write_string("[AQUA] macOS Aqua Design Engine Initialized\n");
}

void AquaEngine::draw_top_menubar(const char* active_app_name) {
    size_t screen_w = nebula::drivers::VBE::get_width();
    size_t bar_h = 28;

    // Top Bar Translucent Dark Background (#181825) & Bottom Accent Border (#313244)
    nebula::drivers::VBE::fill_rect(0, 0, screen_w, bar_h, 0x181825);
    nebula::drivers::VBE::fill_rect(0, bar_h - 1, screen_w, 1, 0x313244);

    // Left Section: Apple/Nebula Logo & App Name
    Font::draw_char(10, 6, 'O', 0x38BDF8); // Nebula Brand Glyph
    Font::draw_string(24, 6, "Nebula", 0xF8FAFC);

    if (active_app_name != nullptr && active_app_name[0] != '\0') {
        Font::draw_string(85, 6, "|", 0x475569);
        Font::draw_string(98, 6, active_app_name, 0xE2E8F0);
    }

    // Standard macOS Top Menus
    Font::draw_string(210, 6, "File", 0x94A3B8);
    Font::draw_string(255, 6, "Edit", 0x94A3B8);
    Font::draw_string(300, 6, "View", 0x94A3B8);
    Font::draw_string(345, 6, "Window", 0x94A3B8);
    Font::draw_string(410, 6, "Help", 0x94A3B8);

    // Right Section: Network IP, Net Status, & Clock
    Font::draw_string(screen_w - 240, 6, "IP: 10.0.2.15", 0x34D399);
    Font::draw_string(screen_w - 130, 6, "E1000: UP", 0x38BDF8);

    // Uptime Clock
    uint32_t seconds = static_cast<uint32_t>(nebula::drivers::PIT::get_ticks() / 100);
    uint32_t mins = seconds / 60;
    uint32_t secs = seconds % 60;

    char clock_str[16];
    clock_str[0] = '0' + (mins / 10);
    clock_str[1] = '0' + (mins % 10);
    clock_str[2] = ':';
    clock_str[3] = '0' + (secs / 10);
    clock_str[4] = '0' + (secs % 10);
    clock_str[5] = '\0';

    Font::draw_string(screen_w - 50, 6, clock_str, 0xFBBF24);
}

void AquaEngine::draw_floating_dock() {
    size_t screen_w = nebula::drivers::VBE::get_width();
    size_t screen_h = nebula::drivers::VBE::get_height();

    size_t item_w = 48;
    size_t item_h = 44;
    size_t spacing = 12;
    size_t total_w = (m_dock_count * item_w) + ((m_dock_count + 1) * spacing);
    size_t dock_h = 56;

    int32_t dock_x = static_cast<int32_t>((screen_w - total_w) / 2);
    int32_t dock_y = static_cast<int32_t>(screen_h - dock_h - 10);

    // Outer Shadow & Rounded Translucent Pill Background (#181825 with #45475A Silver Glass Border)
    nebula::drivers::VBE::fill_rounded_rect(dock_x + 2, dock_y + 2, total_w, dock_h, 14, 0x050B14);
    nebula::drivers::VBE::fill_rounded_rect(dock_x - 1, dock_y - 1, total_w + 2, dock_h + 2, 15, 0x45475A);
    nebula::drivers::VBE::fill_rounded_rect(dock_x, dock_y, total_w, dock_h, 14, 0x181825);

    // Render Dock Items
    for (size_t i = 0; i < m_dock_count; i++) {
        int32_t ix = dock_x + spacing + i * (item_w + spacing);
        int32_t iy = dock_y + 6;

        DockItem* item = &m_dock_items[i];

        // Rounded Icon Container (radius 10px)
        nebula::drivers::VBE::fill_rounded_rect(ix, iy, item_w, item_h, 10, 0x2A2B3D);
        nebula::drivers::VBE::fill_rounded_rect(ix + 2, iy + 2, item_w - 4, item_h - 4, 8, item->color);

        // Icon Label Text
        Font::draw_string(ix + 8, iy + 14, item->icon_symbol, 0x050B14);

        // Glowing Circle Active Indicator under open app icon (macOS Aqua style)
        if (item->is_open) {
            nebula::drivers::VBE::fill_circle(ix + item_w / 2, dock_y + dock_h - 6, 2, 0x38BDF8);
        }
    }
}

void AquaEngine::draw_traffic_lights(int32_t x, int32_t y, int hovered_button) {
    // 🔴 Red Close Disc (Radius 6px)
    uint32_t red_color = (hovered_button == 1) ? 0xE0443E : 0xFF5F56;
    nebula::drivers::VBE::fill_circle(x + 14, y + 12, 6, red_color);

    // 🟡 Yellow Minimize Disc (Radius 6px)
    uint32_t yellow_color = (hovered_button == 2) ? 0xDFA023 : 0xFFBD2E;
    nebula::drivers::VBE::fill_circle(x + 32, y + 12, 6, yellow_color);

    // 🟢 Green Maximize Disc (Radius 6px)
    uint32_t green_color = (hovered_button == 3) ? 0x1AAB29 : 0x27C93F;
    nebula::drivers::VBE::fill_circle(x + 50, y + 12, 6, green_color);
}

int AquaEngine::check_dock_click(int32_t mouse_x, int32_t mouse_y) {
    size_t screen_w = nebula::drivers::VBE::get_width();
    size_t screen_h = nebula::drivers::VBE::get_height();

    size_t item_w = 48;
    size_t spacing = 12;
    size_t total_w = (m_dock_count * item_w) + ((m_dock_count + 1) * spacing);
    size_t dock_h = 56;

    int32_t dock_x = static_cast<int32_t>((screen_w - total_w) / 2);
    int32_t dock_y = static_cast<int32_t>(screen_h - dock_h - 10);

    if (mouse_x >= dock_x && mouse_x <= dock_x + static_cast<int32_t>(total_w) &&
        mouse_y >= dock_y && mouse_y <= dock_y + static_cast<int32_t>(dock_h)) {

        for (size_t i = 0; i < m_dock_count; i++) {
            int32_t ix = dock_x + spacing + i * (item_w + spacing);
            if (mouse_x >= ix && mouse_x <= ix + static_cast<int32_t>(item_w)) {
                return static_cast<int>(i);
            }
        }
    }
    return -1;
}

} // namespace gui
} // namespace nebula
