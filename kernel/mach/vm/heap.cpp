/**
 * @file heap.cpp
 * @brief Kernel Heap Allocator implementation for Nebula OS
 * @author Nebula OS Team
 */

#include <mach/vm/heap.hpp>

namespace nebula {
namespace memory {

heap_block_header_t* KernelHeap::m_start_block = nullptr;
uintptr_t KernelHeap::m_heap_start = 0;
uintptr_t KernelHeap::m_heap_end = 0;

void KernelHeap::init(uintptr_t start_addr, size_t size) {
    m_heap_start = start_addr;
    m_heap_end = start_addr + size;

    m_start_block = reinterpret_cast<heap_block_header_t*>(m_heap_start);
    m_start_block->size = size - sizeof(heap_block_header_t);
    m_start_block->is_free = true;
    m_start_block->next = nullptr;
    m_start_block->prev = nullptr;
}

void* KernelHeap::kmalloc(size_t size) {
    if (size == 0) return nullptr;

    // Align size to 8-byte boundary
    size = (size + 7) & ~7;

    heap_block_header_t* current = m_start_block;
    while (current != nullptr) {
        if (current->is_free && current->size >= size) {
            // Check if block can be split
            if (current->size >= size + sizeof(heap_block_header_t) + 16) {
                heap_block_header_t* next_block = reinterpret_cast<heap_block_header_t*>(
                    reinterpret_cast<uintptr_t>(current) + sizeof(heap_block_header_t) + size
                );

                next_block->size = current->size - size - sizeof(heap_block_header_t);
                next_block->is_free = true;
                next_block->next = current->next;
                next_block->prev = current;

                if (current->next != nullptr) {
                    current->next->prev = next_block;
                }

                current->next = next_block;
                current->size = size;
            }

            current->is_free = false;
            return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(current) + sizeof(heap_block_header_t));
        }

        current = current->next;
    }

    return nullptr; // Out of heap memory
}

void KernelHeap::kfree(void* ptr) {
    if (ptr == nullptr) return;

    heap_block_header_t* block = reinterpret_cast<heap_block_header_t*>(
        reinterpret_cast<uintptr_t>(ptr) - sizeof(heap_block_header_t)
    );

    block->is_free = true;

    // Coalesce with next block if free
    if (block->next != nullptr && block->next->is_free) {
        block->size += sizeof(heap_block_header_t) + block->next->size;
        block->next = block->next->next;
        if (block->next != nullptr) {
            block->next->prev = block;
        }
    }

    // Coalesce with prev block if free
    if (block->prev != nullptr && block->prev->is_free) {
        block->prev->size += sizeof(heap_block_header_t) + block->size;
        block->prev->next = block->next;
        if (block->next != nullptr) {
            block->next->prev = block->prev;
        }
    }
}

} // namespace memory
} // namespace nebula

extern "C" {
    void* kmalloc(size_t size) {
        return nebula::memory::KernelHeap::kmalloc(size);
    }

    void kfree(void* ptr) {
        nebula::memory::KernelHeap::kfree(ptr);
    }
}
