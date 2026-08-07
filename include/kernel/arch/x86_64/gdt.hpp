#ifndef NEBULA_ARCH_X86_64_GDT_HPP
#define NEBULA_ARCH_X86_64_GDT_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

/**
 * @brief Standard 8-byte GDT Entry structure for x86_64 Long Mode
 */
struct __attribute__((packed)) GDTEntry {
    uint16_t limit_low;       ///< Lower 16 bits of segment limit
    uint16_t base_low;        ///< Lower 16 bits of base address
    uint8_t  base_middle;     ///< Middle 8 bits of base address
    uint8_t  access;          ///< Access flags and privilege level
    uint8_t  granularity;     ///< Granularity and upper 4 bits of limit
    uint8_t  base_high;       ///< Upper 8 bits of base address
};

/**
 * @brief System 16-byte TSS Descriptor structure for 64-bit GDT
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
 * @brief 64-bit Task State Segment (TSS) structure (104 bytes)
 */
struct __attribute__((packed)) TSSEntry {
    uint32_t reserved0;
    uint64_t rsp0;            ///< Stack pointer for Privilege Level 0
    uint64_t rsp1;            ///< Stack pointer for Privilege Level 1
    uint64_t rsp2;            ///< Stack pointer for Privilege Level 2
    uint64_t reserved1;
    uint64_t ist[7];          ///< Interrupt Stack Table (IST1 - IST7)
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;      ///< I/O Map Base Address
};

/**
 * @brief GDT Pointer structure for LGDT instruction
 */
struct __attribute__((packed)) GDTPointer {
    uint16_t limit;           ///< Table limit (size - 1)
    uint64_t base;            ///< Base linear address of GDT
};

/**
 * @brief GDT Management Class
 */
class GDT {
private:
    static const size_t GDT_ENTRIES = 7;
    static GDTEntry m_gdt[GDT_ENTRIES];
    static GDTPointer m_gdt_ptr;
    static TSSEntry m_tss;

    static void set_gate(size_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
    static void set_tss_gate(size_t num, uint64_t base, uint32_t limit, uint8_t access, uint8_t gran);

public:
    /**
     * @brief Initialize GDT entries and TSS, then load GDTR
     */
    static void init();
};

} // namespace x86_64
} // namespace arch
} // namespace nebula

#endif // NEBULA_ARCH_X86_64_GDT_HPP
