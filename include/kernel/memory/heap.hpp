#ifndef NEBULA_MEMORY_HEAP_HPP
#define NEBULA_MEMORY_HEAP_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace memory {

static const uint32_t HEAP_MAGIC = 0x48454150; // "HEAP"

/**
 * @brief Kernel Heap Block Boundary Tag Header Structure
 */
struct HeapBlock {
    uint32_t magic;           ///< Magic signature for heap corruption detection
    size_t size;              ///< Size of data payload in bytes
    bool is_free;             ///< Free flag (true = available, false = allocated)
    HeapBlock* next;          ///< Pointer to next adjacent heap block
    HeapBlock* prev;          ///< Pointer to previous adjacent heap block
};

/**
 * @brief Kernel Dynamic Heap Allocator Class (First-Fit with Coalescing)
 */
class Heap {
private:
    static HeapBlock* m_start_block;
    static uintptr_t m_heap_start;
    static uintptr_t m_heap_end;
    static size_t m_total_bytes;
    static size_t m_used_bytes;

    static void split_block(HeapBlock* block, size_t size);
    static void coalesce(HeapBlock* block);

public:
    /**
     * @brief Initialize Kernel Heap Allocator with memory region
     * @param base Starting virtual address of heap region
     * @param size Size of heap region in bytes
     */
    static void init(uintptr_t base, size_t size);

    /**
     * @brief Allocate a dynamic memory block from kernel heap
     * @param size Number of bytes to allocate
     * @return Pointer to allocated memory payload, or nullptr if OOM
     */
    static void* kmalloc(size_t size);

    /**
     * @brief Free an allocated dynamic memory block back to kernel heap
     * @param ptr Pointer to memory payload previously returned by kmalloc
     */
    static void kfree(void* ptr);

    /**
     * @brief Allocate zero-initialized memory array from kernel heap
     */
    static void* kcalloc(size_t num, size_t size);

    /**
     * @brief Resize an existing dynamic memory block
     */
    static void* krealloc(void* ptr, size_t new_size);

    static size_t get_total_bytes() { return m_total_bytes; }
    static size_t get_used_bytes()  { return m_used_bytes; }
    static size_t get_free_bytes()  { return m_total_bytes - m_used_bytes; }
};

} // namespace memory
} // namespace nebula

extern "C" {
    void* kmalloc(size_t size);
    void  kfree(void* ptr);
    void* kcalloc(size_t num, size_t size);
    void* krealloc(void* ptr, size_t size);
}

#endif // NEBULA_MEMORY_HEAP_HPP
