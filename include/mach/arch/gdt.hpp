#ifndef NEBULA_ARCH_X86_64_GDT_HPP
#define NEBULA_ARCH_X86_64_GDT_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

/**
 * @brief Standard 8-byte GDT Entry structure
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
 * @brief GDT Pointer structure for LGDT instruction (6 bytes)
 */
struct __attribute__((packed)) GDTPointer {
    uint16_t limit;           ///< Table limit (size - 1)
    uint32_t base;            ///< Base linear address of GDT
};

/**
 * @brief GDT Management Class
 */
class GDT {
private:
    static const size_t GDT_ENTRIES = 5;
    static GDTEntry m_gdt[GDT_ENTRIES];
    static GDTPointer m_gdt_ptr;

    static void set_gate(size_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

public:
    /**
     * @brief Initialize GDT entries and load GDTR
     */
    static void init();
};

} // namespace x86_64
} // namespace arch
} // namespace nebula

extern "C" {
    /**
     * @brief Assembly routine to reload GDTR register and segment registers
     * @param gdt_ptr_addr Physical address of GDTPointer
     */
    void gdt_flush(uint32_t gdt_ptr_addr);
}

#endif // NEBULA_ARCH_X86_64_GDT_HPP
