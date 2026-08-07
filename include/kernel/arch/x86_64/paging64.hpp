#ifndef NEBULA_ARCH_X86_64_PAGING64_HPP
#define NEBULA_ARCH_X86_64_PAGING64_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

static const uint64_t PAGE64_PRESENT   = 0x01;
static const uint64_t PAGE64_WRITABLE  = 0x02;
static const uint64_t PAGE64_USER      = 0x04;
static const uint64_t PAGE64_HUGE      = 0x80;

/**
 * @brief 64-bit Page Map Level 4 (PML4) Entry Structure (8 bytes)
 */
struct __attribute__((packed)) pml4_entry_t {
    uint64_t value;
};

/**
 * @brief 64-bit Page Directory Pointer Table (PDPT) Entry Structure (8 bytes)
 */
struct __attribute__((packed)) pdpt_entry_t {
    uint64_t value;
};

/**
 * @brief 64-bit Page Directory Entry (PDE) Structure (8 bytes)
 */
struct __attribute__((packed)) pd_entry64_t {
    uint64_t value;
};

/**
 * @brief 64-bit Page Table Entry (PTE) Structure (8 bytes)
 */
struct __attribute__((packed)) pt_entry64_t {
    uint64_t value;
};

/**
 * @brief 64-bit Page Map Level 4 Structure (512 entries, 4 KiB size)
 */
struct __attribute__((packed)) pml4_table_t {
    pml4_entry_t entries[512];
};

/**
 * @brief 64-bit Page Directory Pointer Table Structure (512 entries, 4 KiB size)
 */
struct __attribute__((packed)) pdpt_table_t {
    pdpt_entry_t entries[512];
};

/**
 * @brief 64-bit Page Directory Structure (512 entries, 4 KiB size)
 */
struct __attribute__((packed)) pd_table64_t {
    pd_entry64_t entries[512];
};

} // namespace x86_64
} // namespace arch
} // namespace nebula

#endif // NEBULA_ARCH_X86_64_PAGING64_HPP
