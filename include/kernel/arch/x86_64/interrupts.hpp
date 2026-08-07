#ifndef NEBULA_ARCH_X86_64_INTERRUPTS_HPP
#define NEBULA_ARCH_X86_64_INTERRUPTS_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

/**
 * @brief Registers pushed on stack during interrupt entry
 */
struct __attribute__((packed)) registers_t {
    uint32_t ds;                                     ///< Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; ///< Pushed by pusha
    uint32_t int_no, err_code;                       ///< Interrupt number and error code
    uint32_t eip, cs, eflags, useresp, ss;           ///< Pushed by CPU automatically
};

typedef void (*isr_t)(registers_t* regs);

/**
 * @brief Register callback handler for interrupt vector
 * @param n Vector index (0-255)
 * @param handler Callback function pointer
 */
void register_interrupt_handler(uint8_t n, isr_t handler);

/**
 * @brief Initialize ISR exception stubs, IRQ stubs, and IDT gates
 */
void interrupts_init();

} // namespace x86_64
} // namespace arch
} // namespace nebula

extern "C" {
    uint32_t isr_handler(nebula::arch::x86_64::registers_t* regs);
    uint32_t irq_handler(nebula::arch::x86_64::registers_t* regs);
}

#endif // NEBULA_ARCH_X86_64_INTERRUPTS_HPP
