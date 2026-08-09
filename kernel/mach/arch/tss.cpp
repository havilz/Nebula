/**
 * @file tss.cpp
 * @brief 64-bit Task State Segment (TSS) implementation for Nebula OS
 * @author Nebula OS Team
 */

#include <mach/arch/tss.hpp>
#include <iokit/serial/serial.hpp>

namespace nebula {
namespace arch {
namespace x86_64 {

TSSEntry TSS::m_tss;

void TSS::init(uintptr_t kernel_stack) {
    // Clear TSS memory
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&m_tss);
    for (size_t i = 0; i < sizeof(m_tss); i++) {
        ptr[i] = 0;
    }

    m_tss.rsp0 = kernel_stack;
    m_tss.iomap_base = sizeof(m_tss);
}

void TSS::set_kernel_stack(uintptr_t stack_ptr) {
    m_tss.rsp0 = stack_ptr;
}

TSSEntry* TSS::get_tss() {
    return &m_tss;
}

} // namespace x86_64
} // namespace arch
} // namespace nebula

extern "C" {
    void tss_init(uintptr_t kernel_stack) {
        nebula::arch::x86_64::TSS::init(kernel_stack);
    }

    void tss_set_kernel_stack(uintptr_t stack_ptr) {
        nebula::arch::x86_64::TSS::set_kernel_stack(stack_ptr);
    }
}
