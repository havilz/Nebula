#ifndef NEBULA_DRIVERS_PIT_HPP
#define NEBULA_DRIVERS_PIT_HPP

#include <stddef.h>
#include <stdint.h>
#include <mach/arch/interrupts.hpp>

namespace nebula {
namespace drivers {

static const uint16_t PIT_CHANNEL0_PORT = 0x40;
static const uint16_t PIT_COMMAND_PORT  = 0x43;
static const uint32_t PIT_BASE_FREQUENCY = 1193182; // 1.193182 MHz base oscillator

/**
 * @brief Programmable Interval Timer (PIT 8254) Driver
 */
class PIT {
private:
    static uint64_t m_ticks;
    static uint32_t m_frequency;

public:
    /**
     * @brief Initialize PIT timer with target frequency in Hertz
     * @param frequency_hz Target tick frequency (e.g. 100 Hz = 10 ms per tick)
     */
    static void init(uint32_t frequency_hz = 100);

    /**
     * @brief IRQ 0 Interrupt Callback Handler
     * @param regs CPU registers frame passed by ISR stub
     */
    static void handle_interrupt(nebula::arch::x86_64::registers_t* regs);

    /**
     * @brief Get total system ticks elapsed since boot
     */
    static uint64_t get_ticks();

    /**
     * @brief Get current tick frequency in Hz
     */
    static uint32_t get_frequency();

    /**
     * @brief Sleep/delay execution for specified milliseconds
     * @param milliseconds Duration in ms to pause
     */
    static void sleep_ms(uint32_t milliseconds);
};

} // namespace drivers
} // namespace nebula

extern "C" {
    void pit_init(uint32_t frequency_hz);
    uint64_t pit_get_ticks();
    void pit_sleep_ms(uint32_t milliseconds);
}

#endif // NEBULA_DRIVERS_PIT_HPP
