#ifndef NEBULA_BSD_VFS_FAT32_HPP
#define NEBULA_BSD_VFS_FAT32_HPP

#include <bsd/vfs/vfs.hpp>
#include <iokit/storage/ata.hpp>
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace fs {

/**
 * @brief FAT32 BIOS Parameter Block (BPB) Header Structure (512 bytes)
 */
struct __attribute__((packed)) fat32_bpb_t {
    uint8_t  jmp_boot[3];
    char     oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  fat_count;
    uint16_t root_dir_entries;
    uint16_t total_sectors_16;
    uint8_t  media_type;
    uint16_t fat_size_16;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint32_t fat_size_32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;
    uint16_t fs_info;
    uint16_t backup_boot_sector;
    uint8_t  reserved[12];
    uint8_t  drive_number;
    uint8_t  reserved1;
    uint8_t  boot_signature;
    uint32_t volume_id;
    char     volume_label[11];
    char     fs_type[8];
};

/**
 * @brief FAT32 Standard Directory Entry Structure (32 bytes)
 */
struct __attribute__((packed)) fat32_dir_entry_t {
    char     filename[11];      // 8 chars name + 3 chars ext
    uint8_t  attributes;        // 0x10 = Directory, 0x20 = Archive File
    uint8_t  nt_reserved;
    uint8_t  creation_time_tenth;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_hi;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_lo;
    uint32_t file_size;
};

/**
 * @brief FAT32 Filesystem Driver Class
 */
class FAT32 {
private:
    static drivers::ATADriver* s_ata;
    static uint32_t s_lba_start;
    static uint16_t s_bytes_per_sector;
    static uint8_t  s_sectors_per_cluster;
    static uint16_t s_reserved_sectors;
    static uint8_t  s_fat_count;
    static uint32_t s_fat_size_32;
    static uint32_t s_root_cluster;
    static uint32_t s_fat_lba;
    static uint32_t s_cluster_lba;
    static bool     s_mounted;

    static vnode_t  s_root_vnode;
    static vnode_operations_t s_fat32_ops;

public:
    /**
     * @brief Mount FAT32 partition from ATA drive
     * @param ata ATADriver instance pointer
     * @param lba_start Partition LBA start offset
     * @return vnode_t* Pointer to FAT32 root directory vnode
     */
    static vnode_t* mount(nebula::drivers::ATADriver* ata, uint32_t lba_start);

    /**
     * @brief Read file operations callback
     */
    static uint32_t read(vnode_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);

    /**
     * @brief Write file operations callback
     */
    static uint32_t write(vnode_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer);

    /**
     * @brief Directory lookup callback
     */
    static vnode_t* finddir(vnode_t* node, const char* name);

    static bool is_mounted() { return s_mounted; }
};

} // namespace fs
} // namespace nebula

#endif // NEBULA_BSD_VFS_FAT32_HPP
