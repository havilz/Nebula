#ifndef NEBULA_MEMORY_HEAP_HPP
#define NEBULA_MEMORY_HEAP_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace memory {

/**
 * @brief Header for each allocated or free memory block in kernel heap (16 bytes)
 */
struct __attribute__((packed)) heap_block_header_t {
    size_t size;                ///< Size of memory block (excluding header size)
    bool is_free;               ///< true if block is available for allocation
    heap_block_header_t* next;  ///< Pointer to next block in heap
    heap_block_header_t* prev;  ///< Pointer to previous block in heap
};

/**
 * @brief Kernel Heap Allocator Class
 */
class KernelHeap {
private:
    static heap_block_header_t* m_start_block;
    static uintptr_t m_heap_start;
    static uintptr_t m_heap_end;

public:
    /**
     * @brief Initialize kernel heap at specific virtual base address
     * @param start_addr Virtual starting address (e.g. 0x00C00000 / 12 MB)
     * @param size Initial size of heap memory pool in bytes (e.g. 1 MB)
     */
    static void init(uintptr_t start_addr = 0x00C00000, size_t size = 0x00100000);

    /**
     * @brief Allocate dynamic memory from kernel heap
     * @param size Number of bytes to allocate
     * @return Pointer to allocated memory, or nullptr if allocation fails
     */
    static void* kmalloc(size_t size);

    /**
     * @brief Free previously allocated memory block
     * @param ptr Pointer to memory returned by kmalloc
     */
    static void kfree(void* ptr);
};

} // namespace memory
} // namespace nebula

extern "C" {
    void* kmalloc(size_t size);
    void kfree(void* ptr);
}

#endif // NEBULA_MEMORY_HEAP_HPP
