#ifndef NEBULA_GUI_WIDGET_HPP
#define NEBULA_GUI_WIDGET_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace gui {

typedef void (*button_click_handler_t)();

/**
 * @brief Base GUI Widget Class
 */
class Widget {
protected:
    int32_t m_x;
    int32_t m_y;
    uint32_t m_width;
    uint32_t m_height;
    bool m_visible;

public:
    Widget(int32_t x, int32_t y, uint32_t w, uint32_t h)
        : m_x(x), m_y(y), m_width(w), m_height(h), m_visible(true) {}

    virtual ~Widget() {}

    virtual void draw() = 0;
    virtual bool handle_click(int32_t mouse_x, int32_t mouse_y) {
        (void)mouse_x; (void)mouse_y;
        return false;
    }

    void set_position(int32_t x, int32_t y) { m_x = x; m_y = y; }
    void set_size(uint32_t w, uint32_t h) { m_width = w; m_height = h; }
    bool is_visible() const { return m_visible; }
    void set_visible(bool visible) { m_visible = visible; }
};

/**
 * @brief macOS Aqua Styled Button Widget
 */
class AquaButton : public Widget {
private:
    char m_label[32];
    uint32_t m_bg_color;
    uint32_t m_text_color;
    button_click_handler_t m_on_click;
    bool m_is_hovered;

public:
    AquaButton(int32_t x, int32_t y, uint32_t w, uint32_t h, const char* label, button_click_handler_t on_click = nullptr);

    void draw() override;
    bool handle_click(int32_t mouse_x, int32_t mouse_y) override;
    void set_colors(uint32_t bg, uint32_t text) { m_bg_color = bg; m_text_color = text; }
};

/**
 * @brief macOS Aqua Styled Label Widget
 */
class AquaLabel : public Widget {
private:
    char m_text[64];
    uint32_t m_color;

public:
    AquaLabel(int32_t x, int32_t y, const char* text, uint32_t color = 0xF8FAFC);

    void draw() override;
    void set_text(const char* text);
};

/**
 * @brief macOS Aqua Styled Progress Bar Widget
 */
class AquaProgressBar : public Widget {
private:
    uint32_t m_percentage;
    uint32_t m_fill_color;

public:
    AquaProgressBar(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t initial_pct = 0);

    void draw() override;
    void set_percentage(uint32_t pct);
};

} // namespace gui
} // namespace nebula

#endif // NEBULA_GUI_WIDGET_HPP
