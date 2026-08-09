#ifndef NEBULA_MEMORY_VMM_HPP
#define NEBULA_MEMORY_VMM_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace memory {

static const uint32_t PAGE_PRESENT  = 0x1;
static const uint32_t PAGE_WRITABLE = 0x2;
static const uint32_t PAGE_USER     = 0x4;

/**
 * @brief Page Table Entry (PTE) structure (4 bytes)
 */
struct __attribute__((packed)) page_table_entry_t {
    uint32_t entry;
};

/**
 * @brief Page Directory Entry (PDE) structure (4 bytes)
 */
struct __attribute__((packed)) page_directory_entry_t {
    uint32_t entry;
};

/**
 * @brief Page Table structure (1024 PTEs, 4 KiB size)
 */
struct __attribute__((packed)) page_table_t {
    page_table_entry_t pages[1024];
};

/**
 * @brief Page Directory structure (1024 PDEs, 4 KiB size)
 */
struct __attribute__((packed)) page_directory_t {
    page_directory_entry_t tables[1024];
};

/**
 * @brief Virtual Memory Manager (VMM) & Paging Class
 */
class VMM {
private:
    static page_directory_t* m_current_directory;
    static page_directory_t* m_kernel_directory;

public:
    /**
     * @brief Initialize Virtual Memory Manager, identity map kernel space, and enable CPU paging
     */
    static void init();

    /**
     * @brief Map a virtual page to a physical frame
     * @param virt Virtual page address
     * @param phys Physical frame address
     * @param flags Page flags (PAGE_PRESENT, PAGE_WRITABLE, PAGE_USER)
     */
    static void map_page(uintptr_t virt, uintptr_t phys, uint32_t flags);

    /**
     * @brief Unmap a virtual page
     * @param virt Virtual page address
     */
    static void unmap_page(uintptr_t virt);

    /**
     * @brief Switch active page directory register (CR3)
     * @param dir Pointer to target Page Directory
     */
    static void switch_page_directory(page_directory_t* dir);
};

} // namespace memory
} // namespace nebula

#endif // NEBULA_MEMORY_VMM_HPP
