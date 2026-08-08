/**
 * @file vmm.cpp
 * @brief Virtual Memory Manager (VMM) and Paging implementation for Nebula OS
 * @author Nebula OS Team
 */

#include "../../include/kernel/memory/vmm.hpp"
#include "../../include/kernel/memory/pmm.hpp"
#include "../../include/kernel/drivers/serial.hpp"

namespace nebula {
namespace memory {

page_directory_t* VMM::m_current_directory = nullptr;
page_directory_t* VMM::m_kernel_directory = nullptr;

alignas(4096) static page_directory_t initial_page_directory;
alignas(4096) static page_table_t initial_page_tables[64]; // 64 Page Tables = 256 MB Identity Mapping
alignas(4096) static page_table_t bga_page_table1;        // 4 MB Page Table for 0xFD000000
alignas(4096) static page_table_t bga_page_table2;        // 4 MB Page Table for 0xE0000000

void VMM::switch_page_directory(page_directory_t* dir) {
    m_current_directory = dir;
    uintptr_t phys_addr = reinterpret_cast<uintptr_t>(dir);
    asm volatile ("mov %0, %%cr3" : : "r"(phys_addr));
}

void VMM::map_page(uintptr_t virt, uintptr_t phys, uint32_t flags) {
    size_t pd_index = virt >> 22;
    size_t pt_index = (virt >> 12) & 0x3FF;

    page_directory_entry_t* pde = &m_kernel_directory->tables[pd_index];

    page_table_t* table = nullptr;
    if ((pde->entry & PAGE_PRESENT) == 0) {
        // Allocate physical frame for new Page Table
        uintptr_t pt_phys = PMM::allocate_frame();
        if (pt_phys == 0) return;

        table = reinterpret_cast<page_table_t*>(pt_phys);
        for (size_t i = 0; i < 1024; i++) {
            table->pages[i].entry = 0;
        }

        pde->entry = pt_phys | flags | PAGE_PRESENT | PAGE_WRITABLE;
    } else {
        table = reinterpret_cast<page_table_t*>(pde->entry & ~0xFFF);
    }

    table->pages[pt_index].entry = (phys & ~0xFFF) | flags | PAGE_PRESENT;
    asm volatile ("invlpg (%0)" : : "r"(virt) : "memory");
}

void VMM::unmap_page(uintptr_t virt) {
    size_t pd_index = virt >> 22;
    size_t pt_index = (virt >> 12) & 0x3FF;

    page_directory_entry_t* pde = &m_kernel_directory->tables[pd_index];
    if ((pde->entry & PAGE_PRESENT) == 0) return;

    page_table_t* table = reinterpret_cast<page_table_t*>(pde->entry & ~0xFFF);
    table->pages[pt_index].entry = 0;
    asm volatile ("invlpg (%0)" : : "r"(virt) : "memory");
}

void VMM::init() {
    m_kernel_directory = &initial_page_directory;

    // Clear Page Directory entries
    for (size_t i = 0; i < 1024; i++) {
        m_kernel_directory->tables[i].entry = 0;
    }

    // Identity Map 256 MB (64 Page Tables x 1024 pages x 4KB = 256MB)
    for (size_t t = 0; t < 64; t++) {
        page_table_t* pt = &initial_page_tables[t];
        uintptr_t pt_phys = reinterpret_cast<uintptr_t>(pt);

        for (size_t p = 0; p < 1024; p++) {
            uintptr_t phys_addr = (t * 1024 + p) * PAGE_SIZE;
            pt->pages[p].entry = phys_addr | PAGE_PRESENT | PAGE_WRITABLE;
        }

        m_kernel_directory->tables[t].entry = pt_phys | PAGE_PRESENT | PAGE_WRITABLE;
    }

    // Map Bochs BGA Framebuffer LFB regions (0xFD000000 & 0xE0000000) 4MB each
    for (size_t p = 0; p < 1024; p++) {
        bga_page_table1.pages[p].entry = (0xFD000000 + p * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
        bga_page_table2.pages[p].entry = (0xE0000000 + p * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITABLE;
    }
    m_kernel_directory->tables[0xFD000000 >> 22].entry = reinterpret_cast<uintptr_t>(&bga_page_table1) | PAGE_PRESENT | PAGE_WRITABLE;
    m_kernel_directory->tables[0xE0000000 >> 22].entry = reinterpret_cast<uintptr_t>(&bga_page_table2) | PAGE_PRESENT | PAGE_WRITABLE;

    // Switch to kernel page directory in CR3
    switch_page_directory(m_kernel_directory);

    // Enable CPU Paging by setting Bit 31 (PG) in CR0
    uint32_t cr0;
    asm volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile ("mov %0, %%cr0" : : "r"(cr0));
}

} // namespace memory
} // namespace nebula
