#include <bsd/net/arp.hpp>
#include <iokit/serial/serial.hpp>

namespace nebula {
namespace bsd {
namespace net {

void ARP::init() {
    nebula::drivers::Serial::write_string("[ARP] Address Resolution Protocol Handler Active\n");
}

void ARP::handle_arp(const uint8_t* packet, uint16_t len) {
    if (packet == nullptr || len < sizeof(arp_header_t)) return;
    nebula::drivers::Serial::write_string("[ARP] Processing Incoming ARP Resolution Packet\n");
}

} // namespace net
} // namespace bsd
} // namespace nebula
