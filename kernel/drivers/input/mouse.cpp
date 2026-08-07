/**
 * @file mouse.cpp
 * @brief PS/2 Mouse Driver implementation (IRQ 12) for Nebula OS
 * @author Nebula OS Team
 */

#include "../../../include/kernel/drivers/mouse.hpp"
#include "../../../include/kernel/drivers/serial.hpp"
#include "../../../include/kernel/drivers/vbe.hpp"
#include "../../../include/kernel/arch/x86_64/io.hpp"

namespace nebula {
namespace drivers {

mouse_state_t Mouse::m_state = { 400, 300, false, false, false };
uint8_t Mouse::m_cycle = 0;
int8_t Mouse::m_packet[3] = { 0, 0, 0 };

static inline void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if ((nebula::arch::x86_64::inb(0x64) & 1) == 1) return;
        }
    } else {
        while (timeout--) {
            if ((nebula::arch::x86_64::inb(0x64) & 2) == 0) return;
        }
    }
}

static inline void mouse_write(uint8_t write_byte) {
    mouse_wait(1);
    nebula::arch::x86_64::outb(0x64, 0xD4);
    mouse_wait(1);
    nebula::arch::x86_64::outb(0x60, write_byte);
}

static inline uint8_t mouse_read() {
    mouse_wait(0);
    return nebula::arch::x86_64::inb(0x60);
}

void Mouse::init() {
    m_state.x = 400;
    m_state.y = 300;
    m_state.left_button = false;
    m_state.right_button = false;
    m_state.middle_button = false;
    m_cycle = 0;

    // Enable auxiliary mouse device
    mouse_wait(1);
    nebula::arch::x86_64::outb(0x64, 0xA8);

    // Enable mouse interrupt in controller command byte
    mouse_wait(1);
    nebula::arch::x86_64::outb(0x64, 0x20);
    mouse_wait(0);
    uint8_t status = (nebula::arch::x86_64::inb(0x60) | 2);
    mouse_wait(1);
    nebula::arch::x86_64::outb(0x64, 0x60);
    mouse_wait(1);
    nebula::arch::x86_64::outb(0x60, status);

    // Set default mouse settings & enable streaming
    mouse_write(0xF6);
    mouse_read(); // Acknowledge

    mouse_write(0xF4);
    mouse_read(); // Acknowledge

    // Register IRQ 12 (Vector 44)
    nebula::arch::x86_64::register_interrupt_handler(44, Mouse::handle_interrupt);
    nebula::drivers::Serial::write_string("[MOUSE] PS/2 Mouse Driver Initialized (IRQ 12)\n");
}

void Mouse::handle_interrupt(nebula::arch::x86_64::registers_t* regs) {
    (void)regs;
    uint8_t status = nebula::arch::x86_64::inb(0x64);
    if (!(status & 1) || !(status & 0x20)) return;

    int8_t mouse_byte = static_cast<int8_t>(nebula::arch::x86_64::inb(0x60));

    switch (m_cycle) {
        case 0:
            m_packet[0] = mouse_byte;
            if (mouse_byte & 0x08) { // Valid bit 3 alignment
                m_cycle++;
            }
            break;
        case 1:
            m_packet[1] = mouse_byte;
            m_cycle++;
            break;
        case 2:
            m_packet[2] = mouse_byte;
            m_cycle = 0;

            m_state.left_button   = (m_packet[0] & 0x01) != 0;
            m_state.right_button  = (m_packet[0] & 0x02) != 0;
            m_state.middle_button = (m_packet[0] & 0x04) != 0;

            m_state.x += m_packet[1];
            m_state.y -= m_packet[2];

            // Clamp coordinate bounds inside screen
            if (m_state.x < 0) m_state.x = 0;
            if (m_state.y < 0) m_state.y = 0;
            if (m_state.x >= static_cast<int32_t>(VBE::get_width())) {
                m_state.x = static_cast<int32_t>(VBE::get_width()) - 1;
            }
            if (m_state.y >= static_cast<int32_t>(VBE::get_height())) {
                m_state.y = static_cast<int32_t>(VBE::get_height()) - 1;
            }
            break;
    }
}

mouse_state_t Mouse::get_state() {
    return m_state;
}

// Graphic Arrow Mouse Cursor (12x18 pixels)
static const char cursor_bitmap[18][13] = {
    "#           ",
    "##          ",
    "#.#         ",
    "#..#        ",
    "#...#       ",
    "#....#      ",
    "#.....#     ",
    "#......#    ",
    "#.......#   ",
    "#........#  ",
    "#.....####  ",
    "#..##..#    ",
    "#.#  #..#   ",
    "##   #..#   ",
    "#     #..#  ",
    "      #..#  ",
    "       ##   ",
    "            "
};

void Mouse::draw_cursor(size_t x, size_t y) {
    uint32_t WHITE = 0xFFFFFF;
    uint32_t BLACK = 0x000000;

    for (size_t r = 0; r < 18; r++) {
        for (size_t c = 0; c < 12; c++) {
            char pixel = cursor_bitmap[r][c];
            if (pixel == '#') {
                VBE::put_pixel(x + c, y + r, BLACK);
            } else if (pixel == '.') {
                VBE::put_pixel(x + c, y + r, WHITE);
            }
        }
    }
}

} // namespace drivers
} // namespace nebula

extern "C" {
    void mouse_init() {
        nebula::drivers::Mouse::init();
    }
}
