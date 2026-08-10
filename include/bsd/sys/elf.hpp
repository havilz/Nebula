#ifndef NEBULA_BSD_SYS_ELF_HPP
#define NEBULA_BSD_SYS_ELF_HPP

#include <bsd/vfs/vfs.hpp>
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace bsd {

static const uint8_t ELF_MAG0 = 0x7F;
static const uint8_t ELF_MAG1 = 'E';
static const uint8_t ELF_MAG2 = 'L';
static const uint8_t ELF_MAG3 = 'F';

static const uint16_t ET_EXEC = 2;
static const uint16_t EM_386  = 3;
static const uint32_t PT_LOAD = 1;

/**
 * @brief ELF32 Header Structure
 */
struct __attribute__((packed)) Elf32_Ehdr {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

/**
 * @brief ELF32 Program Header Structure
 */
struct __attribute__((packed)) Elf32_Phdr {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
};

/**
 * @brief Loaded ELF Image Metadata
 */
struct elf_image_t {
    uintptr_t entry_point;
    uintptr_t user_stack;
    uint32_t  memory_size;
    bool      is_valid;
};

/**
 * @brief ELF Executable Binary Loader Class
 */
class ELFLoader {
public:
    /**
     * @brief Validate ELF magic header bytes
     */
    static bool validate_header(const Elf32_Ehdr* header);

    /**
     * @brief Load ELF executable binary from VFS node into Userland memory space
     * @param node VNode pointer to ELF file
     * @param image Out image descriptor
     */
    static bool load_executable(nebula::fs::vnode_t* node, elf_image_t* image);
};

} // namespace bsd
} // namespace nebula

#endif // NEBULA_BSD_SYS_ELF_HPP
