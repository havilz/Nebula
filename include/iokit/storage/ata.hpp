#ifndef NEBULA_IOKIT_STORAGE_ATA_HPP
#define NEBULA_IOKIT_STORAGE_ATA_HPP

#include <iokit/iodevice.hpp>
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace drivers {

/**
 * @brief ATA Primary Controller Drive Sector Size (512 Bytes)
 */
static const uint32_t ATA_SECTOR_SIZE = 512;

/**
 * @brief Primary ATA Controller Driver inheriting IOKit IODevice
 */
class ATADriver : public nebula::iokit::IODevice {
private:
  bool m_present;
  uint32_t m_total_sectors;

  void wait_bsy();
  void wait_drq();

public:
  ATADriver();
  virtual ~ATADriver();

  bool init() override;
  bool start() override;
  void stop() override;

  /**
   * @brief Send ATA IDENTIFY command (0xEC) to verify drive presence
   */
  bool identify();

  /**
   * @brief Read 512-byte sectors using ATA LBA28 PIO Mode
   * @param lba Starting Logical Block Address
   * @param sector_count Number of sectors to read
   * @param buffer Target RAM buffer (must be at least sector_count * 512 bytes)
   */
  bool read_sectors(uint32_t lba, uint8_t sector_count, uint8_t *buffer);

  /**
   * @brief Write 512-byte sectors using ATA LBA28 PIO Mode
   * @param lba Starting Logical Block Address
   * @param sector_count Number of sectors to write
   * @param buffer Source RAM buffer containing data
   */
  bool write_sectors(uint32_t lba, uint8_t sector_count, const uint8_t *buffer);

  bool is_present() const { return m_present; }
  uint32_t get_total_sectors() const { return m_total_sectors; }
};

} // namespace drivers
} // namespace nebula

#endif // NEBULA_IOKIT_STORAGE_ATA_HPP
