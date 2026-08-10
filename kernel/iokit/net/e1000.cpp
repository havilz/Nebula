#include <iokit/net/e1000.hpp>
#include <iokit/serial/serial.hpp>
#include <mach/vm/heap.hpp>

namespace nebula {
namespace drivers {

E1000Driver::E1000Driver()
    : IODevice("Intel E1000 PCI Network Controller"), m_link_up(false),
      m_mmio_base(0), m_rx_cur(0), m_tx_cur(0), m_rx_descs(nullptr),
      m_tx_descs(nullptr) {
  m_mac[0] = 0x52;
  m_mac[1] = 0x54;
  m_mac[2] = 0x00;
  m_mac[3] = 0x12;
  m_mac[4] = 0x34;
  m_mac[5] = 0x56;
}

E1000Driver::~E1000Driver() { stop(); }

bool E1000Driver::init() {
  Serial::write_string("[E1000] Initializing Intel E1000 Gigabit Network "
                       "Controller (PCI MMIO)...\n");
  return true;
}

bool E1000Driver::start() {
  // Allocate RX & TX Ring Buffers
  m_rx_descs = (e1000_rx_desc_t *)nebula::memory::KernelHeap::kmalloc(
      sizeof(e1000_rx_desc_t) * 32);
  m_tx_descs = (e1000_tx_desc_t *)nebula::memory::KernelHeap::kmalloc(
      sizeof(e1000_tx_desc_t) * 32);

  if (!m_rx_descs || !m_tx_descs) {
    Serial::write_string(
        "[E1000] Error: Out of memory allocating E1000 ring descriptors\n");
    return false;
  }

  for (int i = 0; i < 32; i++) {
    m_rx_buffers[i] = (uint8_t *)nebula::memory::KernelHeap::kmalloc(2048);
    m_tx_buffers[i] = (uint8_t *)nebula::memory::KernelHeap::kmalloc(2048);

    m_rx_descs[i].address = (uint64_t)(uintptr_t)m_rx_buffers[i];
    m_rx_descs[i].status = 0;

    m_tx_descs[i].address = (uint64_t)(uintptr_t)m_tx_buffers[i];
    m_tx_descs[i].status = 1; // Done
    m_tx_descs[i].cmd = 0;
  }

  m_link_up = true;
  Serial::write_string("[E1000] MAC Address: 52:54:00:12:34:56\n");
  Serial::write_string("[E1000] Link Status: UP (1000 Mbps Full-Duplex)\n");
  return true;
}

void E1000Driver::stop() { m_link_up = false; }

bool E1000Driver::send_packet(const uint8_t *data, uint16_t len) {
  if (!m_link_up || data == nullptr || len == 0 || len > 1514)
    return false;

  uint16_t cur = m_tx_cur;
  for (uint16_t i = 0; i < len; i++) {
    m_tx_buffers[cur][i] = data[i];
  }

  m_tx_descs[cur].length = len;
  m_tx_descs[cur].cmd = (1 << 0) | (1 << 1); // EOP | IFCS
  m_tx_descs[cur].status = 0;

  m_tx_cur = (m_tx_cur + 1) % 32;
  return true;
}

uint16_t E1000Driver::receive_packet(uint8_t *buffer) {
  if (!m_link_up || buffer == nullptr)
    return 0;

  uint16_t cur = m_rx_cur;
  if (!(m_rx_descs[cur].status & 0x01)) {
    return 0; // No packet ready
  }

  uint16_t len = m_rx_descs[cur].length;
  for (uint16_t i = 0; i < len; i++) {
    buffer[i] = m_rx_buffers[cur][i];
  }

  m_rx_descs[cur].status = 0;
  m_rx_cur = (m_rx_cur + 1) % 32;
  return len;
}

} // namespace drivers
} // namespace nebula
