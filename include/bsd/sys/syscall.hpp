#ifndef NEBULA_SYSCALL_SYSCALL_HPP
#define NEBULA_SYSCALL_SYSCALL_HPP

#include <stddef.h>
#include <stdint.h>
#include "../../mach/arch/interrupts.hpp"

namespace nebula {
namespace syscall {

static const uint32_t SYS_READ  = 1;
static const uint32_t SYS_WRITE = 2;
static const uint32_t SYS_OPEN  = 3;
static const uint32_t SYS_CLOSE = 4;
static const uint32_t SYS_YIELD = 5;
static const uint32_t SYS_EXIT  = 6;

/**
 * @brief System Call Dispatcher & Manager Class
 */
class Syscall {
public:
    /**
     * @brief Initialize System Call Vector (INT 0x80) with DPL 3 (User accessible)
     */
    static void init();

    /**
     * @brief System Call Interrupt Vector 0x80 Handler
     * @param regs CPU registers frame passed by ISR stub
     */
    static void handle_syscall(nebula::arch::x86_64::registers_t* regs);
};

} // namespace syscall
} // namespace nebula

extern "C" {
    void syscall_init();
}

#endif // NEBULA_SYSCALL_SYSCALL_HPP
