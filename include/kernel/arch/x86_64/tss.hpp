#ifndef NEBULA_ARCH_X86_64_TSS_HPP
#define NEBULA_ARCH_X86_64_TSS_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

/**
 * @brief 64-bit Task State Segment (TSS) Structure (104 bytes)
 */
struct __attribute__((packed)) TSSEntry {
    uint32_t reserved0;
    uint64_t rsp0;            ///< Stack pointer for Privilege Level 0 (Kernel Stack)
    uint64_t rsp1;            ///< Stack pointer for Privilege Level 1
    uint64_t rsp2;            ///< Stack pointer for Privilege Level 2
    uint64_t reserved1;
    uint64_t ist[7];          ///< Interrupt Stack Table (IST1 - IST7)
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;      ///< I/O Map Base Address
};

/**
 * @brief 64-bit TSS Descriptor Structure in GDT (16 bytes)
 */
struct __attribute__((packed)) TSSDescriptor {
    uint16_t limit_low;       ///< Lower 16 bits of segment limit
    uint16_t base_low;        ///< Lower 16 bits of base address
    uint8_t  base_middle;     ///< Middle 8 bits of base address
    uint8_t  access;          ///< Access flags and TSS type
    uint8_t  granularity;     ///< Granularity and upper 4 bits of limit
    uint8_t  base_high;       ///< Middle-high 8 bits of base address
    uint32_t base_upper;      ///< Highest 32 bits of 64-bit base address
    uint32_t reserved;        ///< Reserved bits (must be 0)
};

/**
 * @brief 64-bit Task State Segment Manager Class
 */
class TSS {
private:
    static TSSEntry m_tss;

public:
    /**
     * @brief Initialize 64-bit TSS entry and load Task Register via LTR
     * @param kernel_stack Kernel stack pointer for RSP0
     */
    static void init(uintptr_t kernel_stack);

    /**
     * @brief Set Kernel Stack Pointer (RSP0) for Ring 3 to Ring 0 transitions
     * @param stack_ptr Pointer to top of kernel stack
     */
    static void set_kernel_stack(uintptr_t stack_ptr);

    /**
     * @brief Get pointer to static TSSEntry instance
     */
    static TSSEntry* get_tss();
};

} // namespace x86_64
} // namespace arch
} // namespace nebula

extern "C" {
    void tss_init(uintptr_t kernel_stack);
    void tss_set_kernel_stack(uintptr_t stack_ptr);
}

#endif // NEBULA_ARCH_X86_64_TSS_HPP
