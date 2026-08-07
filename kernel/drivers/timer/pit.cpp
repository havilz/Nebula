/**
 * @file pit.cpp
 * @brief Programmable Interval Timer (PIT 8254) Driver implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../../include/kernel/drivers/pit.hpp"
#include "../../../include/kernel/drivers/serial.hpp"
#include "../../../include/kernel/arch/x86_64/io.hpp"
#include "../../../include/kernel/scheduler/scheduler.hpp"

namespace nebula {
namespace drivers {

using nebula::arch::x86_64::outb;

uint64_t PIT::m_ticks = 0;
uint32_t PIT::m_frequency = 100;

void PIT::init(uint32_t frequency_hz) {
    using namespace nebula::arch::x86_64;
    if (frequency_hz == 0) frequency_hz = 100;
    m_frequency = frequency_hz;

    // Calculate divisor for PIT
    uint32_t divisor = PIT_BASE_FREQUENCY / m_frequency;
    if (divisor > 65535) divisor = 65535;

    // Command byte: Channel 0, Access Mode lobyte/hibyte, Mode 3 (Square Wave Generator), 16-bit binary
    // 0x36 = 00 11 011 0
    outb(PIT_COMMAND_PORT, 0x36);

    // Send divisor low byte then high byte
    outb(PIT_CHANNEL0_PORT, static_cast<uint8_t>(divisor & 0xFF));
    outb(PIT_CHANNEL0_PORT, static_cast<uint8_t>((divisor >> 8) & 0xFF));

    // Register IRQ 0 handler (Vector 32)
    nebula::arch::x86_64::register_interrupt_handler(32, PIT::handle_interrupt);
}

void PIT::handle_interrupt(nebula::arch::x86_64::registers_t* regs) {
    m_ticks++;
    nebula::scheduler::Scheduler::handle_timer_tick(regs);
}

uint64_t PIT::get_ticks() {
    return m_ticks;
}

uint32_t PIT::get_frequency() {
    return m_frequency;
}

void PIT::sleep_ms(uint32_t milliseconds) {
    uint64_t start_ticks = m_ticks;
    uint32_t ticks_to_wait = (milliseconds * m_frequency) / 1000;
    if (ticks_to_wait == 0 && milliseconds > 0) ticks_to_wait = 1;

    while ((m_ticks - start_ticks) < ticks_to_wait) {
        asm volatile ("hlt");
    }
}

} // namespace drivers
} // namespace nebula

extern "C" {
    void pit_init(uint32_t frequency_hz) {
        nebula::drivers::PIT::init(frequency_hz);
    }

    uint64_t pit_get_ticks() {
        return nebula::drivers::PIT::get_ticks();
    }

    void pit_sleep_ms(uint32_t milliseconds) {
        nebula::drivers::PIT::sleep_ms(milliseconds);
    }
}
