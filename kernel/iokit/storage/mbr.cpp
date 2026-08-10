#include <iokit/storage/mbr.hpp>
#include <iokit/serial/serial.hpp>

namespace nebula {
namespace drivers {

bool MBRParser::parse(ATADriver* ata, uint32_t* out_fat32_lba) {
    if (ata == nullptr || !ata->is_present()) return false;

    uint8_t sector_buffer[512] = {0};
    if (!ata->read_sectors(0, 1, sector_buffer)) {
        Serial::write_string("[MBR] Error: Failed to read Sector 0 (MBR)\n");
        return false;
    }

    mbr_sector_t* mbr = (mbr_sector_t*)sector_buffer;
    if (mbr->boot_signature != 0xAA55) {
        Serial::write_string("[MBR] Warning: Invalid MBR Boot Signature (0xAA55 missing). Raw disk assumes LBA 0 start.\n");
        if (out_fat32_lba) *out_fat32_lba = 0;
        return true; // Direct raw format assumption
    }

    Serial::write_string("[MBR] Valid Master Boot Record (MBR) Signature Detected (0xAA55)\n");

    for (int i = 0; i < 4; i++) {
        uint8_t type = mbr->partitions[i].partition_type;
        if (type == 0x0B || type == 0x0C || type == 0x0E) { // FAT32 variants
            uint32_t lba = mbr->partitions[i].start_lba;
            Serial::write_string("[MBR] Found FAT32 Partition at Partition Entry Index\n");
            if (out_fat32_lba) *out_fat32_lba = lba;
            return true;
        }
    }

    Serial::write_string("[MBR] No explicit FAT32 partition entry found. Falling back to LBA 0.\n");
    if (out_fat32_lba) *out_fat32_lba = 0;
    return true;
}

} // namespace drivers
} // namespace nebula
