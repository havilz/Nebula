/**
 * @file heap.cpp
 * @brief Kernel Dynamic Heap Allocator implementation (First-Fit with Coalescing)
 * @author Nebula OS Team
 */

#include "../../include/kernel/memory/heap.hpp"

namespace nebula {
namespace memory {

HeapBlock* Heap::m_start_block = nullptr;
uintptr_t Heap::m_heap_start = 0;
uintptr_t Heap::m_heap_end = 0;
size_t Heap::m_total_bytes = 0;
size_t Heap::m_used_bytes = 0;

void Heap::init(uintptr_t base, size_t size) {
    m_heap_start = base;
    m_heap_end   = base + size;
    m_total_bytes = size;
    m_used_bytes  = 0;

    m_start_block = reinterpret_cast<HeapBlock*>(base);
    m_start_block->magic   = HEAP_MAGIC;
    m_start_block->size    = size - sizeof(HeapBlock);
    m_start_block->is_free = true;
    m_start_block->next    = nullptr;
    m_start_block->prev    = nullptr;
}

void Heap::split_block(HeapBlock* block, size_t size) {
    if (block->size >= size + sizeof(HeapBlock) + 16) {
        uintptr_t new_block_addr = reinterpret_cast<uintptr_t>(block) + sizeof(HeapBlock) + size;
        HeapBlock* new_block = reinterpret_cast<HeapBlock*>(new_block_addr);

        new_block->magic   = HEAP_MAGIC;
        new_block->size    = block->size - size - sizeof(HeapBlock);
        new_block->is_free = true;
        new_block->next    = block->next;
        new_block->prev    = block;

        if (block->next != nullptr) {
            block->next->prev = new_block;
        }

        block->next = new_block;
        block->size = size;
    }
}

void Heap::coalesce(HeapBlock* block) {
    // Coalesce with next block if free
    if (block->next != nullptr && block->next->is_free) {
        block->size += sizeof(HeapBlock) + block->next->size;
        block->next = block->next->next;
        if (block->next != nullptr) {
            block->next->prev = block;
        }
    }

    // Coalesce with previous block if free
    if (block->prev != nullptr && block->prev->is_free) {
        block->prev->size += sizeof(HeapBlock) + block->size;
        block->prev->next = block->next;
        if (block->next != nullptr) {
            block->next->prev = block->prev;
        }
    }
}

void* Heap::kmalloc(size_t size) {
    if (size == 0) return nullptr;

    // Align allocation size to 8-byte boundary
    if (size % 8 != 0) {
        size += (8 - (size % 8));
    }

    HeapBlock* curr = m_start_block;
    while (curr != nullptr) {
        if (curr->magic != HEAP_MAGIC) {
            // Heap corruption detected
            return nullptr;
        }

        if (curr->is_free && curr->size >= size) {
            split_block(curr, size);
            curr->is_free = false;
            m_used_bytes += curr->size + sizeof(HeapBlock);
            return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(curr) + sizeof(HeapBlock));
        }
        curr = curr->next;
    }

    // Out of Heap Memory
    return nullptr;
}

void Heap::kfree(void* ptr) {
    if (ptr == nullptr) return;

    uintptr_t block_addr = reinterpret_cast<uintptr_t>(ptr) - sizeof(HeapBlock);
    HeapBlock* block = reinterpret_cast<HeapBlock*>(block_addr);

    if (block->magic != HEAP_MAGIC || block->is_free) {
        // Double free or invalid pointer
        return;
    }

    block->is_free = true;
    if (m_used_bytes >= block->size + sizeof(HeapBlock)) {
        m_used_bytes -= (block->size + sizeof(HeapBlock));
    }

    coalesce(block);
}

void* Heap::kcalloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = kmalloc(total);
    if (ptr != nullptr) {
        uint8_t* byte_ptr = static_cast<uint8_t*>(ptr);
        for (size_t i = 0; i < total; i++) {
            byte_ptr[i] = 0;
        }
    }
    return ptr;
}

void* Heap::krealloc(void* ptr, size_t new_size) {
    if (ptr == nullptr) return kmalloc(new_size);
    if (new_size == 0) {
        kfree(ptr);
        return nullptr;
    }

    uintptr_t block_addr = reinterpret_cast<uintptr_t>(ptr) - sizeof(HeapBlock);
    HeapBlock* block = reinterpret_cast<HeapBlock*>(block_addr);

    if (block->size >= new_size) return ptr;

    void* new_ptr = kmalloc(new_size);
    if (new_ptr != nullptr) {
        uint8_t* src = static_cast<uint8_t*>(ptr);
        uint8_t* dst = static_cast<uint8_t*>(new_ptr);
        for (size_t i = 0; i < block->size; i++) {
            dst[i] = src[i];
        }
        kfree(ptr);
    }
    return new_ptr;
}

} // namespace memory
} // namespace nebula

extern "C" {
    void* kmalloc(size_t size) {
        return nebula::memory::Heap::kmalloc(size);
    }

    void kfree(void* ptr) {
        nebula::memory::Heap::kfree(ptr);
    }

    void* kcalloc(size_t num, size_t size) {
        return nebula::memory::Heap::kcalloc(num, size);
    }

    void* krealloc(void* ptr, size_t size) {
        return nebula::memory::Heap::krealloc(ptr, size);
    }
}
