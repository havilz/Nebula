/**
 * @file vbe.cpp
 * @brief VESA VBE & Bochs BGA Linear Framebuffer Driver implementation for Nebula OS
 * @author Nebula OS Team
 */

#include <stddef.h>
#include <stdint.h>
#include "../../../include/iokit/display/vbe.hpp"
#include "../../../include/iokit/serial/serial.hpp"
#include "../../../include/mach/vm/heap.hpp"
#include "../../../include/libkern/io.hpp"

namespace nebula {
namespace drivers {

uint32_t* VBE::m_framebuffer = nullptr;
uint32_t* VBE::m_backbuffer = nullptr;
size_t VBE::m_width = 800;
size_t VBE::m_height = 600;
size_t VBE::m_pitch = 800 * 4;
size_t VBE::m_bpp = 32;
bool VBE::m_initialized = false;

// Static fallback backbuffer in kernel RAM
static uint32_t fallback_backbuffer[800 * 600] __attribute__((aligned(4096)));

void VBE::bga_write(uint16_t index, uint16_t data) {
    nebula::arch::x86_64::outw(VBE_DISPI_IOPORT_INDEX, index);
    nebula::arch::x86_64::outw(VBE_DISPI_IOPORT_DATA, data);
}

uint16_t VBE::bga_read(uint16_t index) {
    nebula::arch::x86_64::outw(VBE_DISPI_IOPORT_INDEX, index);
    return nebula::arch::x86_64::inw(VBE_DISPI_IOPORT_DATA);
}

void VBE::bga_set_video_mode(uint16_t width, uint16_t height, uint16_t bpp) {
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write(VBE_DISPI_INDEX_XRES, width);
    bga_write(VBE_DISPI_INDEX_YRES, height);
    bga_write(VBE_DISPI_INDEX_BPP, bpp);
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

void VBE::init(vbe_mode_info_t* mode_info) {
    m_width = 800;
    m_height = 600;
    m_bpp = 32;
    m_pitch = m_width * (m_bpp / 8);

    // 1. Program Bochs BGA Hardware Video Mode via Port 0x1CE / 0x1CF
    uint16_t bga_id = bga_read(VBE_DISPI_INDEX_ID);
    nebula::drivers::Serial::write_string("[VBE BGA] Detected Bochs BGA Hardware Version: 0x");
    nebula::drivers::Serial::write_hex32(bga_id);
    nebula::drivers::Serial::write_string("\n");

    bga_set_video_mode(static_cast<uint16_t>(m_width), static_cast<uint16_t>(m_height), static_cast<uint16_t>(m_bpp));

    // 2. Determine Physical Linear Framebuffer (LFB) Address
    if (mode_info != nullptr && mode_info->framebuffer != 0) {
        m_framebuffer = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(mode_info->framebuffer));
    } else {
        // Standard QEMU BGA Physical Framebuffer Address (0xFD000000 or 0xE0000000)
        m_framebuffer = reinterpret_cast<uint32_t*>(0xFD000000);
    }

    // 3. Allocate Backbuffer in Kernel Heap
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

    nebula::drivers::Serial::write_string("[VBE] Linear Framebuffer Connected at 0x");
    nebula::drivers::Serial::write_hex32(reinterpret_cast<uint32_t>(m_framebuffer));
    nebula::drivers::Serial::write_string(" (800x600 32-bit BPP)\n");
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

void VBE::fill_circle(int32_t cx, int32_t cy, int32_t radius, uint32_t color) {
    if (!m_initialized) return;
    for (int32_t dy = -radius; dy <= radius; dy++) {
        for (int32_t dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius) {
                int32_t px = cx + dx;
                int32_t py = cy + dy;
                if (px >= 0 && static_cast<size_t>(px) < m_width && py >= 0 && static_cast<size_t>(py) < m_height) {
                    m_backbuffer[py * m_width + px] = color;
                }
            }
        }
    }
}

void VBE::fill_rounded_rect(int32_t x, int32_t y, uint32_t width, uint32_t height, uint32_t radius, uint32_t color) {
    if (!m_initialized) return;

    int32_t w = static_cast<int32_t>(width);
    int32_t h = static_cast<int32_t>(height);
    int32_t r = static_cast<int32_t>(radius);

    if (r > w / 2) r = w / 2;
    if (r > h / 2) r = h / 2;

    // Center body
    fill_rect(x + r, y, w - 2 * r, h, color);
    // Left & Right columns
    fill_rect(x, y + r, r, h - 2 * r, color);
    fill_rect(x + w - r, y + r, r, h - 2 * r, color);

    // 4 Corner Circles
    fill_circle(x + r, y + r, r, color);
    fill_circle(x + w - r - 1, y + r, r, color);
    fill_circle(x + r, y + h - r - 1, r, color);
    fill_circle(x + w - r - 1, y + h - r - 1, r, color);
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

void VBE::swap_rect(int32_t x, int32_t y, uint32_t width, uint32_t height) {
    if (!m_initialized || m_backbuffer == nullptr || m_framebuffer == nullptr) return;

    if (x < 0) { width += x; x = 0; }
    if (y < 0) { height += y; y = 0; }
    if (static_cast<size_t>(x) >= m_width || static_cast<size_t>(y) >= m_height) return;

    if (static_cast<size_t>(x + width) > m_width) width = m_width - x;
    if (static_cast<size_t>(y + height) > m_height) height = m_height - y;

    for (size_t r = 0; r < height; r++) {
        size_t offset = (y + r) * m_width + x;
        for (size_t c = 0; c < width; c++) {
            m_framebuffer[offset + c] = m_backbuffer[offset + c];
        }
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
