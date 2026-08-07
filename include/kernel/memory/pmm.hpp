#ifndef NEBULA_MEMORY_PMM_HPP
#define NEBULA_MEMORY_PMM_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace memory {

static const size_t PAGE_SIZE = 4096;
static const size_t PAGE_SHIFT = 12;

/**
 * @brief Physical Memory Manager (PMM) Class using Bitmap Frame Allocator
 */
class PMM {
private:
    static uint32_t* m_bitmap;
    static size_t m_max_blocks;
    static size_t m_used_blocks;
    static size_t m_bitmap_size;

    static inline void set_bit(size_t bit) {
        m_bitmap[bit / 32] |= (1 << (bit % 32));
    }

    static inline void clear_bit(size_t bit) {
        m_bitmap[bit / 32] &= ~(1 << (bit % 32));
    }

    static inline bool test_bit(size_t bit) {
        return (m_bitmap[bit / 32] & (1 << (bit % 32))) != 0;
    }

    static int first_free_block();
    static int first_free_blocks(size_t count);

public:
    /**
     * @brief Initialize Physical Memory Manager with bitmap location and memory size
     * @param bitmap_addr Pointer to bitmap array memory location
     * @param mem_size Total physical memory size in bytes
     */
    static void init(uintptr_t bitmap_addr, size_t mem_size);

    /**
     * @brief Mark a physical memory region as free
     */
    static void init_region(uintptr_t base, size_t size);

    /**
     * @brief Mark a physical memory region as reserved/used
     */
    static void reserve_region(uintptr_t base, size_t size);

    /**
     * @brief Allocate a single 4KB physical memory page frame
     * @return Physical base address of allocated frame, or 0 if out of memory
     */
    static uintptr_t alloc_block();

    /**
     * @brief Free a single 4KB physical memory page frame
     * @param phys_addr Physical address of page frame to free
     */
    static void free_block(uintptr_t phys_addr);

    /**
     * @brief Allocate contiguous 4KB physical memory page frames
     * @param count Number of contiguous frames to allocate
     * @return Base physical address of allocated block, or 0 if out of memory
     */
    static uintptr_t alloc_blocks(size_t count);

    /**
     * @brief Free contiguous 4KB physical memory page frames
     * @param phys_addr Base physical address of block to free
     * @param count Number of contiguous frames to free
     */
    static void free_blocks(uintptr_t phys_addr, size_t count);

    static size_t get_total_blocks() { return m_max_blocks; }
    static size_t get_used_blocks()  { return m_used_blocks; }
    static size_t get_free_blocks()  { return m_max_blocks - m_used_blocks; }
};

} // namespace memory
} // namespace nebula

#endif // NEBULA_MEMORY_PMM_HPP
