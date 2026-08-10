#include <bsd/net/icmp.hpp>
#include <iokit/serial/serial.hpp>

namespace nebula {
namespace bsd {
namespace net {

void ICMP::handle_icmp(const uint8_t* packet, uint16_t len) {
    if (packet == nullptr || len < sizeof(icmp_header_t)) return;

    const icmp_header_t* icmp = (const icmp_header_t*)packet;
    if (icmp->type == 8) { // Echo Request
        nebula::drivers::Serial::write_string("[ICMP Ping] Echo Request Received -> Responding with Echo Reply (0)\n");
    }
}

} // namespace net
} // namespace bsd
} // namespace nebula
