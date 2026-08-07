/**
 * @file keyboard.cpp
 * @brief PS/2 Keyboard Driver & Scancode Set 1 Translator implementation for
 * Nebula OS
 * @author Nebula OS Team
 */

#include "../../../include/kernel/drivers/keyboard.hpp"
#include "../../../include/kernel/arch/x86_64/io.hpp"
#include "../../../include/kernel/drivers/serial.hpp"

namespace nebula {
namespace drivers {

char Keyboard::m_buffer[KEYBOARD_BUFFER_SIZE];
size_t Keyboard::m_head = 0;
size_t Keyboard::m_tail = 0;
size_t Keyboard::m_count = 0;

bool Keyboard::m_shift_pressed = false;
bool Keyboard::m_caps_lock = false;
bool Keyboard::m_ctrl_pressed = false;
bool Keyboard::m_alt_pressed = false;

// Scancode Set 1 Lowercase Mapping Table
static const char scancode_ascii_lowercase[128] = {
    0,   27,   '1',  '2', '3',  '4', '5', '6', '7', '8', '9', '0', '-',
    '=', '\b', '\t', 'q', 'w',  'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']',  '\n', 0,   'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', '\'', '`',  0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
    '.', '/',  0,    '*', 0,    ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0,    0,    0,   0,    0,   0,   0,   0,   0,   '-', 0,   0,
    0,   '+',  0,    0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,    0,    0,   0,    0,   0,   0,   0,   0,   0,   0};

// Scancode Set 1 Uppercase Mapping Table
static const char scancode_ascii_uppercase[128] = {
    0,   27,   '!',  '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',
    '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    '{', '}',  '\n', 0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    ':', '"',  '~',  0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',
    '>', '?',  0,    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0,    0,    0,   0,   0,   0,   0,   0,   0,   '-', 0,   0,
    0,   '+',  0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,    0,    0,   0,   0,   0,   0,   0,   0,   0,   0};

void Keyboard::init() {
  m_head = 0;
  m_tail = 0;
  m_count = 0;
  m_shift_pressed = false;
  m_caps_lock = false;

  // Register IRQ 1 handler (Vector 33)
  nebula::arch::x86_64::register_interrupt_handler(33,
                                                   Keyboard::handle_interrupt);
}

char Keyboard::scancode_to_ascii(uint8_t scancode) {
  if (scancode >= 128)
    return 0;

  char lowercase = scancode_ascii_lowercase[scancode];
  char uppercase = scancode_ascii_uppercase[scancode];

  if (lowercase >= 'a' && lowercase <= 'z') {
    bool use_upper = m_shift_pressed ^ m_caps_lock;
    return use_upper ? uppercase : lowercase;
  }

  return m_shift_pressed ? uppercase : lowercase;
}

void Keyboard::handle_interrupt(nebula::arch::x86_64::registers_t *regs) {
  (void)regs;
  using namespace nebula::arch::x86_64;
  uint8_t scancode = inb(KEYBOARD_DATA_PORT);

  // Handle key releases (bit 7 set)
  if (scancode & 0x80) {
    uint8_t released_code = scancode & ~0x80;
    if (released_code == 0x2A || released_code == 0x36) { // Left/Right Shift
      m_shift_pressed = false;
    } else if (released_code == 0x1D) { // Ctrl
      m_ctrl_pressed = false;
    } else if (released_code == 0x38) { // Alt
      m_alt_pressed = false;
    }
    return;
  }

  // Handle key presses
  if (scancode == 0x2A || scancode == 0x36) { // Left/Right Shift
    m_shift_pressed = true;
    return;
  } else if (scancode == 0x3A) { // Caps Lock
    m_caps_lock = !m_caps_lock;
    return;
  } else if (scancode == 0x1D) { // Ctrl
    m_ctrl_pressed = true;
    return;
  } else if (scancode == 0x38) { // Alt
    m_alt_pressed = true;
    return;
  }

  char ch = scancode_to_ascii(scancode);
  if (ch != 0 && m_count < KEYBOARD_BUFFER_SIZE) {
    m_buffer[m_head] = ch;
    m_head = (m_head + 1) % KEYBOARD_BUFFER_SIZE;
    m_count++;
  }
}

bool Keyboard::has_char() { return m_count > 0; }

char Keyboard::get_char() {
  if (m_count == 0)
    return 0;

  char ch = m_buffer[m_tail];
  m_tail = (m_tail + 1) % KEYBOARD_BUFFER_SIZE;
  m_count--;
  return ch;
}

} // namespace drivers
} // namespace nebula

extern "C" {
void keyboard_init() { nebula::drivers::Keyboard::init(); }

bool keyboard_has_char() { return nebula::drivers::Keyboard::has_char(); }

char keyboard_get_char() { return nebula::drivers::Keyboard::get_char(); }
}
