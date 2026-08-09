#include <bsd/vfs/fat32.hpp>
#include <iokit/serial/serial.hpp>
#include <mach/vm/heap.hpp>

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

uint32_t FAT32::read(vnode_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node == nullptr || buffer == nullptr || size == 0 || !s_mounted) return 0;

    uint32_t cluster = node->inode;
    uint32_t cluster_size = s_sectors_per_cluster * s_bytes_per_sector;
    uint32_t cluster_lba = s_cluster_lba + ((cluster - 2) * s_sectors_per_cluster);

    uint8_t cluster_buf[4096]; // Max 8 sectors cluster buffer
    if (s_sectors_per_cluster <= 8) {
        s_ata->read_sectors(cluster_lba, s_sectors_per_cluster, cluster_buf);
        
        uint32_t bytes_to_copy = size;
        if (offset + bytes_to_copy > node->length) {
            bytes_to_copy = node->length > offset ? node->length - offset : 0;
        }

        for (uint32_t i = 0; i < bytes_to_copy && (offset + i) < cluster_size; i++) {
            buffer[i] = cluster_buf[offset + i];
        }
        return bytes_to_copy;
    }
    return 0;
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

vnode_t* FAT32::finddir(vnode_t* node, const char* name) {
    (void)node;
    (void)name;
    return nullptr;
}

} // namespace fs
} // namespace nebula
