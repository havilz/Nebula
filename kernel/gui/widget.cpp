/**
 * @file widget.cpp
 * @brief macOS Aqua Styled Widget Components implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../include/gui/widget.hpp"
#include "../../include/gui/font.hpp"
#include "../../include/iokit/display/vbe.hpp"
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace gui {

namespace {
static void widget_str_copy(char *dest, const char *src, size_t max_len) {
  if (!dest || !src)
    return;
  size_t i = 0;
  while (src[i] != '\0' && i < max_len - 1) {
    dest[i] = src[i];
    i++;
  }
  dest[i] = '\0';
}
} // namespace

AquaButton::AquaButton(int32_t x, int32_t y, uint32_t w, uint32_t h,
                       const char *label, button_click_handler_t on_click)
    : Widget(x, y, w, h), m_bg_color(0x2563EB), m_text_color(0xFFFFFF),
      m_on_click(on_click), m_is_hovered(false) {
  widget_str_copy(m_label, label, sizeof(m_label));
}

void AquaButton::draw() {
  if (!m_visible)
    return;

  uint32_t bg = m_is_hovered ? 0x3B82F6 : m_bg_color;

  // Outer Border & Background Fill
  nebula::drivers::VBE::fill_rect(m_x, m_y, m_width, m_height, 0x1D4ED8);
  nebula::drivers::VBE::fill_rect(m_x + 1, m_y + 1, m_width - 2, m_height - 2,
                                  bg);

  // Centered Button Label
  size_t label_len = 0;
  while (m_label[label_len] != '\0')
    label_len++;

  int32_t text_x =
      m_x +
      (static_cast<int32_t>(m_width) - static_cast<int32_t>(label_len * 8)) / 2;
  int32_t text_y = m_y + (static_cast<int32_t>(m_height) - 16) / 2;

  if (text_x < m_x + 4)
    text_x = m_x + 4;
  if (text_y < m_y + 2)
    text_y = m_y + 2;

  Font::draw_string(text_x, text_y, m_label, m_text_color);
}

bool AquaButton::handle_click(int32_t mouse_x, int32_t mouse_y) {
  if (!m_visible)
    return false;

  if (mouse_x >= m_x && mouse_x <= m_x + static_cast<int32_t>(m_width) &&
      mouse_y >= m_y && mouse_y <= m_y + static_cast<int32_t>(m_height)) {

    if (m_on_click != nullptr) {
      m_on_click();
    }
    return true;
  }
  return false;
}

AquaLabel::AquaLabel(int32_t x, int32_t y, const char *text, uint32_t color)
    : Widget(x, y, 100, 16), m_color(color) {
  widget_str_copy(m_text, text, sizeof(m_text));
}

void AquaLabel::draw() {
  if (!m_visible)
    return;
  Font::draw_string(m_x, m_y, m_text, m_color);
}

void AquaLabel::set_text(const char *text) {
  widget_str_copy(m_text, text, sizeof(m_text));
}

AquaProgressBar::AquaProgressBar(int32_t x, int32_t y, uint32_t w, uint32_t h,
                                 uint32_t initial_pct)
    : Widget(x, y, w, h), m_percentage(initial_pct), m_fill_color(0x10B981) {
  if (m_percentage > 100)
    m_percentage = 100;
}

void AquaProgressBar::draw() {
  if (!m_visible)
    return;

  // Track Border & Dark Fill (#181825)
  nebula::drivers::VBE::fill_rect(m_x, m_y, m_width, m_height, 0x313244);
  nebula::drivers::VBE::fill_rect(m_x + 1, m_y + 1, m_width - 2, m_height - 2,
                                  0x181825);

  // Active Fill Portion
  uint32_t fill_w = ((m_width - 4) * m_percentage) / 100;
  if (fill_w > 0) {
    nebula::drivers::VBE::fill_rect(m_x + 2, m_y + 2, fill_w, m_height - 4,
                                    m_fill_color);
  }
}

void AquaProgressBar::set_percentage(uint32_t pct) {
  m_percentage = pct > 100 ? 100 : pct;
}

} // namespace gui
} // namespace nebula
