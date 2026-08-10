#include <bsd/net/ipv4.hpp>
#include <iokit/serial/serial.hpp>

namespace nebula {
namespace bsd {
namespace net {

void IPv4::init() {
    nebula::drivers::Serial::write_string("[IPv4] Protocol Layer Initialized (Assigned IP: 10.0.2.15)\n");
}

uint16_t IPv4::checksum(const void* vdata, size_t length) {
    const uint16_t* data = (const uint16_t*)vdata;
    uint32_t acc = 0xffff;

    for (size_t i = 0; i < length / 2; i++) {
        acc += Ethernet::ntohs(data[i]);
        if (acc > 0xffff) acc -= 0xffff;
    }

    if (length & 1) {
        uint16_t word = 0;
        *(uint8_t*)&word = *(const uint8_t*)((const uint8_t*)data + length - 1);
        acc += Ethernet::ntohs(word);
        if (acc > 0xffff) acc -= 0xffff;
    }

    return Ethernet::htons(~acc);
}

void IPv4::handle_ip(const uint8_t* packet, uint16_t len) {
    if (packet == nullptr || len < sizeof(ipv4_header_t)) return;
    nebula::drivers::Serial::write_string("[IPv4] Incoming IPv4 Packet Received\n");
}

} // namespace net
} // namespace bsd
} // namespace nebula
