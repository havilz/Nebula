/**
 * @file vmm.cpp
 * @brief Virtual Memory Manager (VMM) 4-Level Paging implementation
 * @author Nebula OS Team
 */

#include "../../include/kernel/memory/vmm.hpp"
#include "../../include/kernel/memory/pmm.hpp"

namespace nebula {
namespace memory {

uint32_t* VMM::m_current_pml4 = nullptr;

void VMM::init() {
    // Read current CR3 page table directory
    uintptr_t cr3;
    asm volatile ("mov %%cr3, %0" : "=r"(cr3));
    m_current_pml4 = reinterpret_cast<uint32_t*>(cr3);
}

void VMM::switch_pml4(uintptr_t pml4_phys) {
    m_current_pml4 = reinterpret_cast<uint32_t*>(pml4_phys);
    asm volatile ("mov %0, %%cr3" : : "r"(pml4_phys) : "memory");
}

bool VMM::map_page(uintptr_t virt, uintptr_t phys, uint32_t flags) {
    if (virt % PAGE_SIZE != 0 || phys % PAGE_SIZE != 0) {
        return false;
    }

    size_t page_dir_index = (virt >> 22) & 0x3FF;
    size_t page_tbl_index = (virt >> 12) & 0x3FF;

    uint32_t* page_directory = m_current_pml4;
    uint32_t pde = page_directory[page_dir_index];

    uint32_t* page_table = nullptr;

    if (!(pde & PAGE_PRESENT)) {
        // Allocate a new physical frame for Page Table
        uintptr_t new_table_phys = PMM::alloc_block();
        if (!new_table_phys) return false;

        page_table = reinterpret_cast<uint32_t*>(new_table_phys);
        for (size_t i = 0; i < 1024; i++) {
            page_table[i] = 0;
        }

        page_directory[page_dir_index] = static_cast<uint32_t>(new_table_phys) | PAGE_PRESENT | PAGE_WRITABLE | (flags & PAGE_USER);
    } else {
        page_table = reinterpret_cast<uint32_t*>(pde & ~0xFFF);
    }

    page_table[page_tbl_index] = static_cast<uint32_t>(phys) | (flags & 0xFFF);
    flush_tlb(virt);

    return true;
}

bool VMM::unmap_page(uintptr_t virt) {
    if (virt % PAGE_SIZE != 0) return false;

    size_t page_dir_index = (virt >> 22) & 0x3FF;
    size_t page_tbl_index = (virt >> 12) & 0x3FF;

    uint32_t* page_directory = m_current_pml4;
    uint32_t pde = page_directory[page_dir_index];

    if (!(pde & PAGE_PRESENT)) return false;

    uint32_t* page_table = reinterpret_cast<uint32_t*>(pde & ~0xFFF);
    if (!(page_table[page_tbl_index] & PAGE_PRESENT)) return false;

    page_table[page_tbl_index] = 0;
    flush_tlb(virt);

    return true;
}

uintptr_t VMM::get_physical(uintptr_t virt) {
    size_t page_dir_index = (virt >> 22) & 0x3FF;
    size_t page_tbl_index = (virt >> 12) & 0x3FF;

    uint32_t* page_directory = m_current_pml4;
    uint32_t pde = page_directory[page_dir_index];

    if (!(pde & PAGE_PRESENT)) return 0;

    uint32_t* page_table = reinterpret_cast<uint32_t*>(pde & ~0xFFF);
    uint32_t pte = page_table[page_tbl_index];

    if (!(pte & PAGE_PRESENT)) return 0;

    return static_cast<uintptr_t>(pte & ~0xFFF) + (virt & 0xFFF);
}

} // namespace memory
} // namespace nebula
