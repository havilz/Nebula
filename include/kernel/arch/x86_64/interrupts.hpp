#ifndef NEBULA_ARCH_X86_64_INTERRUPTS_HPP
#define NEBULA_ARCH_X86_64_INTERRUPTS_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

/**
 * @brief CPU Register Stack Frame saved during Interrupt Service Routine (ISR) - 32-bit
 */
struct __attribute__((packed)) InterruptRegisters {
    uint32_t edi;             ///< Saved EDI from pusha
    uint32_t esi;             ///< Saved ESI from pusha
    uint32_t ebp;             ///< Saved EBP from pusha
    uint32_t esp;             ///< Saved ESP from pusha
    uint32_t ebx;             ///< Saved EBX from pusha
    uint32_t edx;             ///< Saved EDX from pusha
    uint32_t ecx;             ///< Saved ECX from pusha
    uint32_t eax;             ///< Saved EAX from pusha

    uint32_t int_no;          ///< Interrupt Vector number (0-255)
    uint32_t err_code;        ///< Error Code (dummy or hardware)

    uint32_t eip;             ///< Instruction Pointer pushed by hardware
    uint32_t cs;              ///< Code Segment pushed by hardware
    uint32_t eflags;          ///< EFLAGS register pushed by hardware
};

typedef void (*ISRHandler)(InterruptRegisters* regs);

/**
 * @brief Interrupt Manager Class
 */
class Interrupts {
private:
    static ISRHandler m_handlers[256];

public:
    /**
     * @brief Register a custom C++ handler function for an interrupt vector
     * @param vector Vector index (0-255)
     * @param handler Function pointer to handler
     */
    static void register_handler(uint8_t vector, ISRHandler handler);

    /**
     * @brief C++ main interrupt dispatcher invoked by assembly ISR stubs
     * @param regs Pointer to register stack frame
     */
    static void dispatch(InterruptRegisters* regs);
};

} // namespace x86_64
} // namespace arch
} // namespace nebula

extern "C" void cpp_interrupt_handler(nebula::arch::x86_64::InterruptRegisters* regs);

#endif // NEBULA_ARCH_X86_64_INTERRUPTS_HPP
