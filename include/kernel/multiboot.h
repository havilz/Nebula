#ifndef NEBULA_MULTIBOOT_H
#define NEBULA_MULTIBOOT_H

#include <stdint.h>

#define MULTIBOOT_HEADER_MAGIC          0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002

#define MULTIBOOT_INFO_MEMORY           0x00000001
#define MULTIBOOT_INFO_BOOTDEV          0x00000002
#define MULTIBOOT_INFO_CMDLINE          0x00000004
#define MULTIBOOT_INFO_MODS             0x00000008
#define MULTIBOOT_INFO_MEM_MAP          0x00000040

struct __attribute__((packed)) multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
};

typedef struct multiboot_mmap_entry multiboot_memory_map_t;

struct __attribute__((packed)) multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
};

typedef struct multiboot_info multiboot_info_t;

#endif // NEBULA_MULTIBOOT_H
