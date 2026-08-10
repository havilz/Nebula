#ifndef NEBULA_MEMORY_PMM_HPP
#define NEBULA_MEMORY_PMM_HPP

#include <stddef.h>
#include <stdint.h>
#include "../../multiboot.h"

namespace nebula {
namespace memory {

static const size_t PAGE_SIZE = 4096; // 4 KiB Physical Frame Size

/**
 * @brief Physical Memory Manager (PMM) Bitmap Allocator
 */
class PMM {
private:
    static uint32_t* m_bitmap;
    static size_t m_max_frames;
    static size_t m_used_frames;
    static uintptr_t m_memory_size;

    static void bitmap_set(size_t bit);
    static void bitmap_clear(size_t bit);
    static bool bitmap_test(size_t bit);
    static int32_t find_first_free_frame();

public:
    /**
     * @brief Initialize Physical Memory Manager from Multiboot Memory Map
     * @param mb_info Pointer to Multiboot information structure passed by bootloader
     */
    static void init(multiboot_info_t* mb_info);

    /**
     * @brief Allocate a single 4 KiB physical memory frame
     * @return Physical base address of allocated frame, or 0 if out of memory
     */
    static uintptr_t allocate_frame();

    /**
     * @brief Free a previously allocated 4 KiB physical memory frame
     * @param frame_addr Physical base address of frame to free
     */
    static void free_frame(uintptr_t frame_addr);

    /**
     * @brief Get total system physical memory in Kilobytes
     */
    static size_t get_total_memory_kb();

    /**
     * @brief Get free system physical memory in Kilobytes
     */
    static size_t get_free_memory_kb();

    /**
     * @brief Get count of allocated physical frames
     */
    static size_t get_used_frames_count();
};

} // namespace memory
} // namespace nebula

#endif // NEBULA_MEMORY_PMM_HPP
