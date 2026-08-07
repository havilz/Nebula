/**
 * @file vbe.cpp
 * @brief VESA VBE Linear Framebuffer Driver implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../../include/kernel/drivers/vbe.hpp"
#include "../../../include/kernel/drivers/serial.hpp"
#include "../../../include/kernel/memory/heap.hpp"

namespace nebula {
namespace drivers {

uint32_t* VBE::m_framebuffer = nullptr;
uint32_t* VBE::m_backbuffer = nullptr;
size_t VBE::m_width = 800;
size_t VBE::m_height = 600;
size_t VBE::m_pitch = 800 * 4;
size_t VBE::m_bpp = 32;
bool VBE::m_initialized = false;

// Static fallback memory for VBE Framebuffer
static uint32_t fallback_framebuffer[800 * 600] __attribute__((aligned(4096)));
static uint32_t fallback_backbuffer[800 * 600] __attribute__((aligned(4096)));

void VBE::init(vbe_mode_info_t* mode_info) {
    if (mode_info != nullptr && mode_info->framebuffer != 0) {
        m_width = mode_info->width;
        m_height = mode_info->height;
        m_pitch = mode_info->pitch;
        m_bpp = mode_info->bpp;
        m_framebuffer = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(mode_info->framebuffer));
    } else {
        // Fallback software VBE buffer
        m_width = 800;
        m_height = 600;
        m_pitch = 800 * 4;
        m_bpp = 32;
        m_framebuffer = fallback_framebuffer;
    }

    // Allocate Double Buffering Backbuffer in Kernel Heap or static memory
    size_t buffer_size = m_width * m_height * sizeof(uint32_t);
    void* heap_buf = nebula::memory::KernelHeap::kmalloc(buffer_size);
    if (heap_buf != nullptr) {
        m_backbuffer = static_cast<uint32_t*>(heap_buf);
    } else {
        m_backbuffer = fallback_backbuffer;
    }

    m_initialized = true;
    clear(COLOR_BG_DARK);
    swap_buffers();

    nebula::drivers::Serial::write_string("[VBE] Framebuffer Driver Initialized (");
    nebula::drivers::Serial::write_dec(m_width);
    nebula::drivers::Serial::write_string("x");
    nebula::drivers::Serial::write_dec(m_height);
    nebula::drivers::Serial::write_string(" 32-bit BPP)\n");
}

void VBE::put_pixel(size_t x, size_t y, uint32_t color) {
    if (!m_initialized || x >= m_width || y >= m_height || m_backbuffer == nullptr) return;
    m_backbuffer[y * m_width + x] = color;
}

uint32_t VBE::get_pixel(size_t x, size_t y) {
    if (!m_initialized || x >= m_width || y >= m_height || m_backbuffer == nullptr) return 0;
    return m_backbuffer[y * m_width + x];
}

void VBE::draw_rect(size_t x, size_t y, size_t width, size_t height, uint32_t color) {
    if (!m_initialized) return;
    for (size_t i = x; i < x + width && i < m_width; i++) {
        put_pixel(i, y, color);
        put_pixel(i, y + height - 1, color);
    }
    for (size_t j = y; j < y + height && j < m_height; j++) {
        put_pixel(x, j, color);
        put_pixel(x + width - 1, j, color);
    }
}

void VBE::fill_rect(size_t x, size_t y, size_t width, size_t height, uint32_t color) {
    if (!m_initialized) return;
    for (size_t j = y; j < y + height && j < m_height; j++) {
        for (size_t i = x; i < x + width && i < m_width; i++) {
            m_backbuffer[j * m_width + i] = color;
        }
    }
}

void VBE::clear(uint32_t color) {
    if (!m_initialized || m_backbuffer == nullptr) return;
    size_t total = m_width * m_height;
    for (size_t i = 0; i < total; i++) {
        m_backbuffer[i] = color;
    }
}

void VBE::swap_buffers() {
    if (!m_initialized || m_backbuffer == nullptr || m_framebuffer == nullptr) return;
    size_t total = m_width * m_height;
    for (size_t i = 0; i < total; i++) {
        m_framebuffer[i] = m_backbuffer[i];
    }
}

} // namespace drivers
} // namespace nebula

extern "C" {
    void vbe_init(nebula::drivers::vbe_mode_info_t* mode_info) {
        nebula::drivers::VBE::init(mode_info);
    }

    void vbe_swap_buffers() {
        nebula::drivers::VBE::swap_buffers();
    }
}
