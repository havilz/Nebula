#ifndef NEBULA_IOKIT_NET_E1000_HPP
#define NEBULA_IOKIT_NET_E1000_HPP

#include <iokit/iodevice.hpp>
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace drivers {

/**
 * @brief E1000 RX Ring Descriptor (16 bytes)
 */
struct __attribute__((packed)) e1000_rx_desc_t {
    uint64_t address;
    uint16_t length;
    uint16_t checksum;
    uint8_t  status;
    uint8_t  errors;
    uint16_t special;
};

/**
 * @brief E1000 TX Ring Descriptor (16 bytes)
 */
struct __attribute__((packed)) e1000_tx_desc_t {
    uint64_t address;
    uint16_t length;
    uint8_t  cso;
    uint8_t  cmd;
    uint8_t  status;
    uint8_t  css;
    uint16_t special;
};

/**
 * @brief Intel E1000 PCI Gigabit Network Card Driver Class
 */
class E1000Driver : public nebula::iokit::IODevice {
private:
    uint8_t  m_mac[6];
    bool     m_link_up;
    uint32_t m_mmio_base;
    uint16_t m_rx_cur;
    uint16_t m_tx_cur;

    e1000_rx_desc_t* m_rx_descs;
    e1000_tx_desc_t* m_tx_descs;
    uint8_t*         m_rx_buffers[32];
    uint8_t*         m_tx_buffers[32];

    void write_command(uint16_t address, uint32_t val);
    uint32_t read_command(uint16_t address);
    void detect_eeprom();
    uint16_t eeprom_read(uint8_t addr);

public:
    E1000Driver();
    virtual ~E1000Driver();

    bool init() override;
    bool start() override;
    void stop() override;

    /**
     * @brief Transmit raw Ethernet frame over network card
     * @param data Payload data pointer
     * @param len Packet length in bytes
     */
    bool send_packet(const uint8_t* data, uint16_t len);

    /**
     * @brief Receive raw Ethernet frame from network ring buffer
     * @param buffer Target RAM buffer
     * @return Packet length in bytes, or 0 if no packet received
     */
    uint16_t receive_packet(uint8_t* buffer);

    const uint8_t* get_mac() const { return m_mac; }
    bool is_link_up() const { return m_link_up; }
};

} // namespace drivers
} // namespace nebula

#endif // NEBULA_IOKIT_NET_E1000_HPP
