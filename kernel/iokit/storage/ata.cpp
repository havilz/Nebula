#include <iokit/storage/ata.hpp>
#include <iokit/serial/serial.hpp>
#include <libkern/io.hpp>

using nebula::arch::x86_64::inb;
using nebula::arch::x86_64::outb;
using nebula::arch::x86_64::inw;
using nebula::arch::x86_64::outw;

namespace nebula {
namespace drivers {

static const uint16_t ATA_PRIMARY_DATA         = 0x1F0;
static const uint16_t ATA_PRIMARY_ERROR        = 0x1F1;
static const uint16_t ATA_PRIMARY_SECCOUNT     = 0x1F2;
static const uint16_t ATA_PRIMARY_LBA_LO       = 0x1F3;
static const uint16_t ATA_PRIMARY_LBA_MID      = 0x1F4;
static const uint16_t ATA_PRIMARY_LBA_HI       = 0x1F5;
static const uint16_t ATA_PRIMARY_DRIVE_HEAD   = 0x1F6;
static const uint16_t ATA_PRIMARY_COMMAND      = 0x1F7;
static const uint16_t ATA_PRIMARY_STATUS       = 0x1F7;
static const uint16_t ATA_PRIMARY_CONTROL      = 0x3F6;

static const uint8_t ATA_CMD_READ_PIO          = 0x20;
static const uint8_t ATA_CMD_WRITE_PIO         = 0x30;
static const uint8_t ATA_CMD_IDENTIFY          = 0xEC;

static const uint8_t ATA_SR_BSY                = 0x80;
static const uint8_t ATA_SR_DRQ                = 0x08;
static const uint8_t ATA_SR_ERR                = 0x01;

ATADriver::ATADriver()
    : IODevice("ATAPrimaryController"), m_present(false), m_total_sectors(0) {}

ATADriver::~ATADriver() {}

void ATADriver::wait_bsy() {
    while (inb(ATA_PRIMARY_STATUS) & ATA_SR_BSY) {
        // Spin until BSY clears
    }
}

void ATADriver::wait_drq() {
    while (!(inb(ATA_PRIMARY_STATUS) & ATA_SR_DRQ)) {
        // Spin until DRQ sets
    }
}

bool ATADriver::init() {
    Serial::write_string("[ATA] Initializing Primary ATA/IDE Storage Controller...\n");
    return true;
}

bool ATADriver::start() {
    m_started = true;
    m_present = identify();
    if (m_present) {
        Serial::write_string("[ATA] Primary Harddisk Drive Identified & Ready (LBA28 PIO Mode)\n");
    } else {
        Serial::write_string("[ATA] No ATA Harddisk Drive Detected on Primary Bus\n");
    }
    return m_present;
}

void ATADriver::stop() {
    m_started = false;
    m_present = false;
}

bool ATADriver::identify() {
    outb(ATA_PRIMARY_DRIVE_HEAD, 0xA0);
    outb(ATA_PRIMARY_SECCOUNT, 0);
    outb(ATA_PRIMARY_LBA_LO, 0);
    outb(ATA_PRIMARY_LBA_MID, 0);
    outb(ATA_PRIMARY_LBA_HI, 0);
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_IDENTIFY);

    uint8_t status = inb(ATA_PRIMARY_STATUS);
    if (status == 0) return false; // Drive does not exist

    wait_bsy();

    if (inb(ATA_PRIMARY_LBA_MID) != 0 || inb(ATA_PRIMARY_LBA_HI) != 0) {
        return false; // Not ATA (could be ATAPI)
    }

    uint8_t drq_err = 0;
    while (true) {
        status = inb(ATA_PRIMARY_STATUS);
        if (status & ATA_SR_ERR) {
            drq_err = 1;
            break;
        }
        if (status & ATA_SR_DRQ) break;
    }

    if (drq_err) return false;

    // Read 256 16-bit words (512 bytes) of IDENTIFY data
    uint16_t identify_data[256];
    for (int i = 0; i < 256; i++) {
        identify_data[i] = inw(ATA_PRIMARY_DATA);
    }

    m_total_sectors = (uint32_t)identify_data[60] | ((uint32_t)identify_data[61] << 16);
    return true;
}

bool ATADriver::read_sectors(uint32_t lba, uint8_t sector_count, uint8_t* buffer) {
    if (buffer == nullptr || sector_count == 0) return false;

    wait_bsy();

    outb(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, sector_count);
    outb(ATA_PRIMARY_LBA_LO, (uint8_t)(lba & 0xFF));
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_PRIMARY_LBA_HI, (uint8_t)((lba >> 16) & 0xFF));
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_READ_PIO);

    uint16_t* ptr = (uint16_t*)buffer;

    for (int s = 0; s < sector_count; s++) {
        wait_bsy();
        wait_drq();

        for (int i = 0; i < 256; i++) {
            ptr[s * 256 + i] = inw(ATA_PRIMARY_DATA);
        }
    }

    return true;
}

bool ATADriver::write_sectors(uint32_t lba, uint8_t sector_count, const uint8_t* buffer) {
    if (buffer == nullptr || sector_count == 0) return false;

    wait_bsy();

    outb(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, sector_count);
    outb(ATA_PRIMARY_LBA_LO, (uint8_t)(lba & 0xFF));
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_PRIMARY_LBA_HI, (uint8_t)((lba >> 16) & 0xFF));
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_WRITE_PIO);

    const uint16_t* ptr = (const uint16_t*)buffer;

    for (int s = 0; s < sector_count; s++) {
        wait_bsy();
        wait_drq();

        for (int i = 0; i < 256; i++) {
            outw(ATA_PRIMARY_DATA, ptr[s * 256 + i]);
        }
    }

    outb(ATA_PRIMARY_COMMAND, 0xE7); // Cache Flush Command
    wait_bsy();
    return true;
}

} // namespace drivers
} // namespace nebula
