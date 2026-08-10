#include <stddef.h>
#include <stdint.h>
#include "../../../include/bsd/vfs/fat32.hpp"
#include "../../../include/iokit/serial/serial.hpp"
#include "../../../include/mach/vm/heap.hpp"

namespace nebula {
namespace fs {

nebula::drivers::ATADriver* FAT32::s_ata = nullptr;
uint32_t FAT32::s_lba_start = 0;
uint16_t FAT32::s_bytes_per_sector = 512;
uint8_t  FAT32::s_sectors_per_cluster = 8;
uint16_t FAT32::s_reserved_sectors = 32;
uint8_t  FAT32::s_fat_count = 2;
uint32_t FAT32::s_fat_size_32 = 0;
uint32_t FAT32::s_root_cluster = 2;
uint32_t FAT32::s_fat_lba = 0;
uint32_t FAT32::s_cluster_lba = 0;
bool     FAT32::s_mounted = false;

vnode_t FAT32::s_root_vnode;
vnode_operations_t FAT32::s_fat32_ops = {
    FAT32::read,
    FAT32::write,
    nullptr,
    nullptr,
    FAT32::finddir
};

vnode_t* FAT32::mount(nebula::drivers::ATADriver* ata, uint32_t lba_start) {
    if (ata == nullptr || !ata->is_present()) return nullptr;

    s_ata = ata;
    s_lba_start = lba_start;

    uint8_t sector_buffer[512] = {0};
    if (!s_ata->read_sectors(s_lba_start, 1, sector_buffer)) {
        drivers::Serial::write_string("[FAT32] Error: Failed to read Boot Sector\n");
        return nullptr;
    }

    fat32_bpb_t* bpb = (fat32_bpb_t*)sector_buffer;
    if (bpb->bytes_per_sector != 512) {
        drivers::Serial::write_string("[FAT32] Warning: Non-512 sector size detected or unformatted volume\n");
    }

    s_bytes_per_sector = bpb->bytes_per_sector > 0 ? bpb->bytes_per_sector : 512;
    s_sectors_per_cluster = bpb->sectors_per_cluster > 0 ? bpb->sectors_per_cluster : 8;
    s_reserved_sectors = bpb->reserved_sectors > 0 ? bpb->reserved_sectors : 32;
    s_fat_count = bpb->fat_count > 0 ? bpb->fat_count : 2;
    s_fat_size_32 = bpb->fat_size_32;
    s_root_cluster = bpb->root_cluster > 0 ? bpb->root_cluster : 2;

    s_fat_lba = s_lba_start + s_reserved_sectors;
    s_cluster_lba = s_fat_lba + (s_fat_count * s_fat_size_32);

    s_root_vnode.flags = VFS_DIRECTORY;
    s_root_vnode.length = 0;
    s_root_vnode.inode = s_root_cluster;
    s_root_vnode.ops = &s_fat32_ops;
    s_root_vnode.ptr = nullptr;

    // Copy "fat32" name to root vnode
    const char* name = "fat32";
    for (int i = 0; name[i] != '\0' && i < 127; i++) {
        s_root_vnode.name[i] = name[i];
        s_root_vnode.name[i+1] = '\0';
    }

    s_mounted = true;
    drivers::Serial::write_string("[FAT32] Volume Successfully Mounted at /fat32/\n");
    return &s_root_vnode;
}

uint32_t FAT32::get_next_cluster(uint32_t current_cluster) {
    if (!s_mounted || s_ata == nullptr || current_cluster < 2) return 0x0FFFFFFF;

    uint32_t fat_offset = current_cluster * 4;
    uint32_t fat_sector = s_fat_lba + (fat_offset / s_bytes_per_sector);
    uint32_t entry_offset = fat_offset % s_bytes_per_sector;

    uint8_t sector_buf[512] = {0};
    if (!s_ata->read_sectors(fat_sector, 1, sector_buf)) {
        return 0x0FFFFFFF;
    }

    uint32_t next_cluster = *reinterpret_cast<uint32_t*>(&sector_buf[entry_offset]);
    return next_cluster & 0x0FFFFFFF;
}

uint32_t FAT32::read(vnode_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node == nullptr || buffer == nullptr || size == 0 || !s_mounted) return 0;

    uint32_t cluster_size = s_sectors_per_cluster * s_bytes_per_sector;
    uint32_t current_cluster = node->inode;

    // Skip clusters up to offset
    uint32_t cluster_offset = offset / cluster_size;
    uint32_t byte_in_cluster = offset % cluster_size;

    for (uint32_t i = 0; i < cluster_offset; i++) {
        current_cluster = get_next_cluster(current_cluster);
        if (current_cluster >= 0x0FFFFFF8 || current_cluster < 2) return 0;
    }

    uint32_t total_read = 0;
    uint32_t bytes_remaining = size;

    if (offset + bytes_remaining > node->length && node->length > 0) {
        bytes_remaining = (node->length > offset) ? (node->length - offset) : 0;
    }

    uint8_t cluster_buf[4096]; // Max 8 sectors per cluster

    while (bytes_remaining > 0 && current_cluster >= 2 && current_cluster < 0x0FFFFFF8) {
        uint32_t cluster_lba = s_cluster_lba + ((current_cluster - 2) * s_sectors_per_cluster);
        if (!s_ata->read_sectors(cluster_lba, s_sectors_per_cluster, cluster_buf)) {
            break;
        }

        uint32_t chunk = cluster_size - byte_in_cluster;
        if (chunk > bytes_remaining) chunk = bytes_remaining;

        for (uint32_t i = 0; i < chunk; i++) {
            buffer[total_read + i] = cluster_buf[byte_in_cluster + i];
        }

        total_read += chunk;
        bytes_remaining -= chunk;
        byte_in_cluster = 0;

        if (bytes_remaining > 0) {
            current_cluster = get_next_cluster(current_cluster);
        }
    }

    return total_read;
}

uint32_t FAT32::write(vnode_t* node, uint32_t offset, uint32_t size, const uint8_t* buffer) {
    (void)offset;
    if (node == nullptr || buffer == nullptr || size == 0 || !s_mounted) return 0;

    uint32_t cluster = node->inode;
    uint32_t cluster_lba = s_cluster_lba + ((cluster - 2) * s_sectors_per_cluster);

    uint8_t cluster_buf[4096];
    for (uint32_t i = 0; i < 4096; i++) cluster_buf[i] = 0;

    for (uint32_t i = 0; i < size && i < 4096; i++) {
        cluster_buf[i] = buffer[i];
    }

    s_ata->write_sectors(cluster_lba, s_sectors_per_cluster, cluster_buf);
    node->length = size;
    return size;
}

static char to_upper(char c) {
    if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
    return c;
}

vnode_t* FAT32::finddir(vnode_t* node, const char* name) {
    if (node == nullptr || name == nullptr || !s_mounted) return nullptr;

    // Convert input search name to FAT 8.3 formatted space-padded 11-byte filename
    char target_83[11];
    for (int i = 0; i < 11; i++) target_83[i] = ' ';

    int name_idx = 0;
    int target_idx = 0;

    // Copy base filename (up to 8 chars or before dot)
    while (name[name_idx] != '\0' && name[name_idx] != '.' && target_idx < 8) {
        target_83[target_idx++] = to_upper(name[name_idx++]);
    }

    // Skip extra chars before dot
    while (name[name_idx] != '\0' && name[name_idx] != '.') {
        name_idx++;
    }

    // If extension exists, copy up to 3 chars
    if (name[name_idx] == '.') {
        name_idx++;
        target_idx = 8;
        while (name[name_idx] != '\0' && target_idx < 11) {
            target_83[target_idx++] = to_upper(name[name_idx++]);
        }
    }

    uint32_t current_cluster = node->inode;
    uint32_t cluster_size = s_sectors_per_cluster * s_bytes_per_sector;
    uint8_t cluster_buf[4096];

    while (current_cluster >= 2 && current_cluster < 0x0FFFFFF8) {
        uint32_t cluster_lba = s_cluster_lba + ((current_cluster - 2) * s_sectors_per_cluster);
        if (!s_ata->read_sectors(cluster_lba, s_sectors_per_cluster, cluster_buf)) {
            break;
        }

        size_t entry_count = cluster_size / sizeof(fat32_dir_entry_t);
        fat32_dir_entry_t* entries = reinterpret_cast<fat32_dir_entry_t*>(cluster_buf);

        for (size_t i = 0; i < entry_count; i++) {
            fat32_dir_entry_t* entry = &entries[i];

            // 0x00 = End of directory entries
            if (static_cast<uint8_t>(entry->filename[0]) == 0x00) {
                return nullptr;
            }

            // 0xE5 = Deleted entry, 0x0F = Long File Name (LFN) entry
            if (static_cast<uint8_t>(entry->filename[0]) == 0xE5 || entry->attributes == 0x0F) {
                continue;
            }

            // Match 11-byte 8.3 filename
            bool match = true;
            for (int k = 0; k < 11; k++) {
                if (entry->filename[k] != target_83[k]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                void* vnode_mem = memory::KernelHeap::kmalloc(sizeof(vnode_t));
                if (vnode_mem == nullptr) return nullptr;

                vnode_t* new_vnode = static_cast<vnode_t*>(vnode_mem);
                new_vnode->flags = (entry->attributes & 0x10) ? VFS_DIRECTORY : VFS_FILE;
                new_vnode->length = entry->file_size;
                new_vnode->inode = (static_cast<uint32_t>(entry->first_cluster_hi) << 16) | entry->first_cluster_lo;
                new_vnode->ops = &s_fat32_ops;
                new_vnode->ptr = nullptr;

                for (size_t n = 0; name[n] != '\0' && n < 127; n++) {
                    new_vnode->name[n] = name[n];
                    new_vnode->name[n+1] = '\0';
                }

                return new_vnode;
            }
        }

        current_cluster = get_next_cluster(current_cluster);
    }

    return nullptr;
}

} // namespace fs
} // namespace nebula
