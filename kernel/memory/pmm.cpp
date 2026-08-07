/**
 * @file pmm.cpp
 * @brief Bitmap Physical Memory Manager implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../include/kernel/memory/pmm.hpp"

namespace nebula {
namespace memory {

uint32_t* PMM::m_bitmap = nullptr;
size_t PMM::m_max_blocks = 0;
size_t PMM::m_used_blocks = 0;
size_t PMM::m_bitmap_size = 0;

void PMM::init(uintptr_t bitmap_addr, size_t mem_size) {
    m_max_blocks  = mem_size / PAGE_SIZE;
    m_used_blocks = m_max_blocks;
    m_bitmap      = reinterpret_cast<uint32_t*>(bitmap_addr);
    m_bitmap_size = m_max_blocks / 32;

    if (m_max_blocks % 32 != 0) {
        m_bitmap_size++;
    }

    // Default: Mark all physical memory blocks as used/reserved
    for (size_t i = 0; i < m_bitmap_size; i++) {
        m_bitmap[i] = 0xFFFFFFFF;
    }
}

void PMM::init_region(uintptr_t base, size_t size) {
    size_t align = base / PAGE_SIZE;
    size_t blocks = size / PAGE_SIZE;

    for (size_t i = 0; i < blocks; i++) {
        if (test_bit(align + i)) {
            clear_bit(align + i);
            m_used_blocks--;
        }
    }

    // Always reserve block 0 (null address protection)
    set_bit(0);
}

void PMM::reserve_region(uintptr_t base, size_t size) {
    size_t align = base / PAGE_SIZE;
    size_t blocks = size / PAGE_SIZE;

    for (size_t i = 0; i < blocks; i++) {
        if (!test_bit(align + i)) {
            set_bit(align + i);
            m_used_blocks++;
        }
    }
}

int PMM::first_free_block() {
    for (size_t i = 0; i < m_bitmap_size; i++) {
        if (m_bitmap[i] != 0xFFFFFFFF) {
            for (int bit = 0; bit < 32; bit++) {
                if (!(m_bitmap[i] & (1 << bit))) {
                    return static_cast<int>(i * 32 + bit);
                }
            }
        }
    }
    return -1;
}

int PMM::first_free_blocks(size_t count) {
    if (count == 0) return -1;
    if (count == 1) return first_free_block();

    for (size_t i = 0; i < m_bitmap_size; i++) {
        if (m_bitmap[i] != 0xFFFFFFFF) {
            for (int bit = 0; bit < 32; bit++) {
                size_t start_block = i * 32 + bit;
                size_t free_count = 0;

                for (size_t j = 0; j < count; j++) {
                    if (start_block + j >= m_max_blocks) break;
                    if (!test_bit(start_block + j)) {
                        free_count++;
                    } else {
                        break;
                    }
                }

                if (free_count == count) {
                    return static_cast<int>(start_block);
                }
            }
        }
    }
    return -1;
}

uintptr_t PMM::alloc_block() {
    if (get_free_blocks() == 0) return 0;

    int frame = first_free_block();
    if (frame < 0) return 0;

    set_bit(static_cast<size_t>(frame));
    m_used_blocks++;

    return static_cast<uintptr_t>(frame) * PAGE_SIZE;
}

void PMM::free_block(uintptr_t phys_addr) {
    size_t frame = phys_addr / PAGE_SIZE;

    if (frame < m_max_blocks && test_bit(frame)) {
        clear_bit(frame);
        m_used_blocks--;
    }
}

uintptr_t PMM::alloc_blocks(size_t count) {
    if (get_free_blocks() < count) return 0;

    int frame = first_free_blocks(count);
    if (frame < 0) return 0;

    for (size_t i = 0; i < count; i++) {
        set_bit(static_cast<size_t>(frame) + i);
    }
    m_used_blocks += count;

    return static_cast<uintptr_t>(frame) * PAGE_SIZE;
}

void PMM::free_blocks(uintptr_t phys_addr, size_t count) {
    size_t frame = phys_addr / PAGE_SIZE;

    for (size_t i = 0; i < count; i++) {
        if (frame + i < m_max_blocks && test_bit(frame + i)) {
            clear_bit(frame + i);
            m_used_blocks--;
        }
    }
}

} // namespace memory
} // namespace nebula
