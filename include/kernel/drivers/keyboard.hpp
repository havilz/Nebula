#ifndef NEBULA_DRIVERS_KEYBOARD_HPP
#define NEBULA_DRIVERS_KEYBOARD_HPP

#include "../arch/x86_64/interrupts.hpp"
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace drivers {

static const uint16_t KEYBOARD_DATA_PORT = 0x60;
static const uint16_t KEYBOARD_STATUS_PORT = 0x64;
static const size_t KEYBOARD_BUFFER_SIZE = 256;

/**
 * @brief Key event structure storing scancode and ASCII conversion
 */
struct key_event_t {
  uint8_t scancode;
  char ascii;
  bool pressed;
};

/**
 * @brief PS/2 Keyboard Driver & Ring Buffer Queue
 */
class Keyboard {
private:
  static char m_buffer[KEYBOARD_BUFFER_SIZE];
  static size_t m_head;
  static size_t m_tail;
  static size_t m_count;

  static bool m_shift_pressed;
  static bool m_caps_lock;
  static bool m_ctrl_pressed;
  static bool m_alt_pressed;

  static char scancode_to_ascii(uint8_t scancode);

public:
  /**
   * @brief Initialize PS/2 Keyboard driver and register IRQ 1 handler
   */
  static void init();

  /**
   * @brief IRQ 1 Interrupt Callback Handler
   * @param regs CPU registers frame passed by ISR stub
   */
  static void handle_interrupt(nebula::arch::x86_64::registers_t *regs);

  /**
   * @brief Check if there is an unread ASCII character in ring buffer
   */
  static bool has_char();

  /**
   * @brief Get and remove next ASCII character from ring buffer
   * @return ASCII character, or '\0' if buffer is empty
   */
  static char get_char();
};

} // namespace drivers
} // namespace nebula

extern "C" {
void keyboard_init();
bool keyboard_has_char();
char keyboard_get_char();
}

#endif // NEBULA_DRIVERS_KEYBOARD_HPP
