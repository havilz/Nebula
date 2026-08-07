/**
 * @file pmm.cpp
 * @brief Physical Memory Manager (PMM) Bitmap Allocator implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../include/kernel/memory/pmm.hpp"
#include "../../include/kernel/drivers/serial.hpp"

namespace nebula {
namespace memory {

uint32_t* PMM::m_bitmap = nullptr;
size_t PMM::m_max_frames = 0;
size_t PMM::m_used_frames = 0;
uintptr_t PMM::m_memory_size = 0;

static uint32_t internal_pmm_bitmap[32768]; // Static storage for up to 128 MB RAM tracking

void PMM::bitmap_set(size_t bit) {
    m_bitmap[bit / 32] |= (1U << (bit % 32));
}

void PMM::bitmap_clear(size_t bit) {
    m_bitmap[bit / 32] &= ~(1U << (bit % 32));
}

bool PMM::bitmap_test(size_t bit) {
    return (m_bitmap[bit / 32] & (1U << (bit % 32))) != 0;
}

int32_t PMM::find_first_free_frame() {
    for (size_t i = 0; i < m_max_frames / 32; i++) {
        if (m_bitmap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                uint32_t bit = 1U << j;
                if ((m_bitmap[i] & bit) == 0) {
                    return static_cast<int32_t>(i * 32 + j);
                }
            }
        }
    }
    return -1;
}

void PMM::init(multiboot_info_t* mb_info) {
    m_bitmap = internal_pmm_bitmap;

    if (mb_info && (mb_info->flags & MULTIBOOT_INFO_MEMORY)) {
        m_memory_size = (mb_info->mem_lower + mb_info->mem_upper) * 1024;
    } else {
        m_memory_size = 128 * 1024 * 1024; // Default fallback to 128 MB RAM
    }

    m_max_frames = m_memory_size / PAGE_SIZE;
    m_used_frames = m_max_frames; // Initially mark all as used/reserved

    // Fill bitmap with 1s (reserved)
    for (size_t i = 0; i < m_max_frames / 32; i++) {
        m_bitmap[i] = 0xFFFFFFFF;
    }

    // Parse Multiboot mmap to mark available memory regions as free
    if (mb_info && (mb_info->flags & MULTIBOOT_INFO_MEM_MAP)) {
        multiboot_memory_map_t* mmap = reinterpret_cast<multiboot_memory_map_t*>(mb_info->mmap_addr);
        uint32_t mmap_end = mb_info->mmap_addr + mb_info->mmap_length;

        while (reinterpret_cast<uint32_t>(mmap) < mmap_end) {
            if (mmap->type == 1) { // Type 1 = Available RAM
                uint64_t base = mmap->addr;
                uint64_t length = mmap->len;

                for (uint64_t addr = base; addr < base + length; addr += PAGE_SIZE) {
                    size_t frame = static_cast<size_t>(addr / PAGE_SIZE);
                    if (frame < m_max_frames) {
                        bitmap_clear(frame);
                        m_used_frames--;
                    }
                }
            }
            mmap = reinterpret_cast<multiboot_memory_map_t*>(
                reinterpret_cast<uint32_t>(mmap) + mmap->size + sizeof(mmap->size)
            );
        }
    } else {
        // Fallback: mark 1 MB to 128 MB as available
        for (uint64_t addr = 0x100000; addr < m_memory_size; addr += PAGE_SIZE) {
            size_t frame = static_cast<size_t>(addr / PAGE_SIZE);
            if (frame < m_max_frames) {
                bitmap_clear(frame);
                m_used_frames--;
            }
        }
    }

    // Reserve 1 MB kernel base (0x0 - 0x100000)
    for (size_t i = 0; i < 256; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            m_used_frames++;
        }
    }
}

uintptr_t PMM::allocate_frame() {
    int32_t frame = find_first_free_frame();
    if (frame < 0) {
        return 0; // Out of memory
    }

    bitmap_set(static_cast<size_t>(frame));
    m_used_frames++;
    return static_cast<uintptr_t>(frame * PAGE_SIZE);
}

void PMM::free_frame(uintptr_t frame_addr) {
    size_t frame = frame_addr / PAGE_SIZE;
    if (frame >= m_max_frames) return;

    if (bitmap_test(frame)) {
        bitmap_clear(frame);
        if (m_used_frames > 0) {
            m_used_frames--;
        }
    }
}

size_t PMM::get_total_memory_kb() {
    return m_memory_size / 1024;
}

size_t PMM::get_free_memory_kb() {
    return (m_max_frames - m_used_frames) * (PAGE_SIZE / 1024);
}

size_t PMM::get_used_frames_count() {
    return m_used_frames;
}

} // namespace memory
} // namespace nebula
