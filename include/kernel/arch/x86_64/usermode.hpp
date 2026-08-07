#ifndef NEBULA_ARCH_X86_64_USERMODE_HPP
#define NEBULA_ARCH_X86_64_USERMODE_HPP

#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

/**
 * @brief Switch CPU execution from Ring 0 Kernel Mode to Ring 3 User Mode
 * @param user_entry Pointer to entry function of user program
 * @param user_stack Pointer to top of user mode stack
 */
extern "C" void enter_usermode(uintptr_t user_entry, uintptr_t user_stack);

} // namespace x86_64
} // namespace arch
} // namespace nebula

#endif // NEBULA_ARCH_X86_64_USERMODE_HPP
