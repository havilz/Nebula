#ifndef NEBULA_MEMORY_VMM_HPP
#define NEBULA_MEMORY_VMM_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace memory {

enum VMMFlags : uint32_t {
    PAGE_PRESENT   = 1 << 0,  ///< Page is present in memory
    PAGE_WRITABLE  = 1 << 1,  ///< Read/Write allowed (0 = Read-Only)
    PAGE_USER      = 1 << 2,  ///< Userland Ring 3 accessible
    PAGE_PWT       = 1 << 3,  ///< Page-level Write-Through
    PAGE_PCD       = 1 << 4,  ///< Page-level Cache Disable
    PAGE_ACCESSED  = 1 << 5,  ///< Set by CPU on read/write
    PAGE_DIRTY     = 1 << 6,  ///< Set by CPU on write
    PAGE_HUGE      = 1 << 7,  ///< 2MB / 1GB Huge Page
    PAGE_GLOBAL    = 1 << 8,  ///< Global page (prevent TLB flush on CR3 load)
};

/**
 * @brief Virtual Memory Manager (VMM) for 4-Level Paging Architecture
 */
class VMM {
private:
    static uint32_t* m_current_pml4;

    static inline void flush_tlb(uintptr_t virt) {
        asm volatile ("invlpg (%0)" : : "r"(virt) : "memory");
    }

public:
    /**
     * @brief Initialize Virtual Memory Manager and identity-map kernel memory
     */
    static void init();

    /**
     * @brief Map a virtual address page to a physical address frame
     * @param virt Virtual linear address to map
     * @param phys Physical base address
     * @param flags Page attributes (PAGE_PRESENT | PAGE_WRITABLE etc)
     */
    static bool map_page(uintptr_t virt, uintptr_t phys, uint32_t flags);

    /**
     * @brief Unmap a virtual address page
     * @param virt Virtual linear address to unmap
     */
    static bool unmap_page(uintptr_t virt);

    /**
     * @brief Translate a virtual address to its underlying physical address
     * @param virt Virtual address
     * @return Physical address, or 0 if unmapped
     */
    static uintptr_t get_physical(uintptr_t virt);

    /**
     * @brief Switch current CR3 page table directory
     * @param pml4_phys Physical address of PML4 directory
     */
    static void switch_pml4(uintptr_t pml4_phys);
};

} // namespace memory
} // namespace nebula

#endif // NEBULA_MEMORY_VMM_HPP
