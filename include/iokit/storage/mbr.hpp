#ifndef NEBULA_IOKIT_STORAGE_MBR_HPP
#define NEBULA_IOKIT_STORAGE_MBR_HPP

#include <iokit/storage/ata.hpp>
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace drivers {

/**
 * @brief Master Boot Record Partition Entry (16 bytes)
 */
struct __attribute__((packed)) mbr_partition_entry_t {
    uint8_t  boot_indicator;  // 0x80 = Active / Bootable
    uint8_t  start_chs[3];    // Starting CHS Address
    uint8_t  partition_type;  // 0x0C = FAT32 LBA, 0x83 = Linux Ext2/3/4
    uint8_t  end_chs[3];      // Ending CHS Address
    uint32_t start_lba;       // Logical Block Address Start
    uint32_t sector_count;    // Total Sectors in Partition
};

/**
 * @brief MBR Sector 0 Structure (512 bytes)
 */
struct __attribute__((packed)) mbr_sector_t {
    uint8_t  bootstrap_code[446];
    mbr_partition_entry_t partitions[4];
    uint16_t boot_signature;  // 0xAA55
};

/**
 * @brief MBR Partition Table Parser Class
 */
class MBRParser {
public:
    /**
     * @brief Parse LBA 0 of ATA drive to locate FAT32 partition starting LBA
     * @param ata ATADriver instance
     * @param out_fat32_lba Pointer to uint32_t to store starting LBA
     * @return true if FAT32 partition is found, false otherwise
     */
    static bool parse(ATADriver* ata, uint32_t* out_fat32_lba);
};

} // namespace drivers
} // namespace nebula

#endif // NEBULA_IOKIT_STORAGE_MBR_HPP
