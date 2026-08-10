#ifndef NEBULA_GUI_AQUA_HPP
#define NEBULA_GUI_AQUA_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace gui {

struct DockItem {
    const char* name;
    const char* icon_symbol;
    uint32_t color;
    bool is_open;
    bool is_active;
    int32_t window_id;
};

/**
 * @brief macOS Aqua UI Design Engine for Nebula OS
 */
class AquaEngine {
private:
    static DockItem m_dock_items[4];
    static size_t m_dock_count;

public:
    /**
     * @brief Initialize Aqua UI Engine & Dock Items
     */
    static void init();

    /**
     * @brief Render Top Global Menu Bar (Height 28px)
     */
    static void draw_top_menubar(const char* active_app_name);

    /**
     * @brief Render Floating Translucent macOS Dock (Height 48px)
     */
    static void draw_floating_dock();

    /**
     * @brief Render macOS Traffic Light Window Controls (🔴 🟡 🟢)
     * @param x Window top-left X
     * @param y Window top-left Y
     * @param hovered_button 0=None, 1=Close (Red), 2=Minimize (Yellow), 3=Maximize (Green)
     */
    static void draw_traffic_lights(int32_t x, int32_t y, int hovered_button = 0);

    /**
     * @brief Check click on Top Dock items
     * @return Index of clicked dock item, or -1 if none
     */
    static int check_dock_click(int32_t mouse_x, int32_t mouse_y);
};

} // namespace gui
} // namespace nebula

#endif // NEBULA_GUI_AQUA_HPP
