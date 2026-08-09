#ifndef NEBULA_DRIVERS_MOUSE_HPP
#define NEBULA_DRIVERS_MOUSE_HPP

#include <stddef.h>
#include <stdint.h>
#include "../../mach/arch/interrupts.hpp"

namespace nebula {
namespace drivers {

/**
 * @brief PS/2 Mouse State Structure
 */
struct mouse_state_t {
    int32_t x;
    int32_t y;
    bool left_button;
    bool right_button;
    bool middle_button;
};

/**
 * @brief PS/2 Mouse Driver Class (IRQ 12 / Vector 44)
 */
class Mouse {
private:
    static mouse_state_t m_state;
    static uint8_t m_cycle;
    static int8_t m_packet[3];

public:
    /**
     * @brief Initialize PS/2 Mouse device and register IRQ 12 handler
     */
    static void init();

    /**
     * @brief IRQ 12 Interrupt Handler for PS/2 Mouse
     */
    static void handle_interrupt(nebula::arch::x86_64::registers_t* regs);

    /**
     * @brief Get current mouse position and button state
     */
    static mouse_state_t get_state();

    /**
     * @brief Render arrow graphic mouse cursor on VBE backbuffer
     */
    static void draw_cursor(size_t x, size_t y);
};

} // namespace drivers
} // namespace nebula

extern "C" {
    void mouse_init();
}

#endif // NEBULA_DRIVERS_MOUSE_HPP
