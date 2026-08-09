#include <bsd/sys/elf.hpp>
#include <iokit/serial/serial.hpp>
#include <mach/vm/heap.hpp>
#include <mach/vm/vmm.hpp>

namespace nebula {
namespace bsd {

bool ELFLoader::validate_header(const Elf32_Ehdr* header) {
    if (header == nullptr) return false;
    if (header->e_ident[0] != ELF_MAG0 ||
        header->e_ident[1] != ELF_MAG1 ||
        header->e_ident[2] != ELF_MAG2 ||
        header->e_ident[3] != ELF_MAG3) {
        return false;
    }
    return (header->e_type == ET_EXEC && header->e_machine == EM_386);
}

bool ELFLoader::load_executable(nebula::fs::vnode_t* node, elf_image_t* image) {
    if (node == nullptr || image == nullptr) return false;

    image->is_valid = false;
    image->entry_point = 0;
    image->user_stack = 0;
    image->memory_size = 0;

    Elf32_Ehdr ehdr;
    if (nebula::fs::VFS::read(node, 0, sizeof(Elf32_Ehdr), (uint8_t*)&ehdr) != sizeof(Elf32_Ehdr)) {
        nebula::drivers::Serial::write_string("[ELF] Error: Failed to read ELF Header from VFS node\n");
        return false;
    }

    if (!validate_header(&ehdr)) {
        nebula::drivers::Serial::write_string("[ELF] Warning: Invalid ELF Header or unsupported architecture\n");
        return false;
    }

    nebula::drivers::Serial::write_string("[ELF] Valid ELF32 Executable Binary Header Recognized\n");

    // Read Program Header Table
    uint32_t ph_size = ehdr.e_phnum * sizeof(Elf32_Phdr);
    Elf32_Phdr* ph_table = (Elf32_Phdr*)nebula::memory::KernelHeap::kmalloc(ph_size);
    if (ph_table == nullptr) return false;

    if (nebula::fs::VFS::read(node, ehdr.e_phoff, ph_size, (uint8_t*)ph_table) != ph_size) {
        nebula::memory::KernelHeap::kfree(ph_table);
        return false;
    }

    uint32_t total_mem = 0;
    for (int i = 0; i < ehdr.e_phnum; i++) {
        if (ph_table[i].p_type == PT_LOAD) {
            uint32_t vaddr = ph_table[i].p_vaddr;
            uint32_t memsz = ph_table[i].p_memsz;
            uint32_t filesz = ph_table[i].p_filesz;
            uint32_t offset = ph_table[i].p_offset;

            if (vaddr > 0 && memsz > 0) {
                // Read segment from VFS file
                uint8_t* seg_buf = (uint8_t*)nebula::memory::KernelHeap::kmalloc(memsz);
                if (seg_buf != nullptr) {
                    for (uint32_t b = 0; b < memsz; b++) seg_buf[b] = 0;
                    if (filesz > 0) {
                        nebula::fs::VFS::read(node, offset, filesz, seg_buf);
                    }
                    total_mem += memsz;
                }
            }
        }
    }

    nebula::memory::KernelHeap::kfree(ph_table);

    // Allocate 16 KiB User Stack
    void* user_stack_mem = nebula::memory::KernelHeap::kmalloc(16384);
    uintptr_t stack_top = (uintptr_t)user_stack_mem + 16384 - 16;

    image->entry_point = ehdr.e_entry;
    image->user_stack = stack_top;
    image->memory_size = total_mem;
    image->is_valid = true;

    nebula::drivers::Serial::write_string("[ELF] Executable Segment Load Complete\n");
    return true;
}

} // namespace bsd
} // namespace nebula
