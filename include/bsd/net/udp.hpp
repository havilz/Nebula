#ifndef NEBULA_BSD_NET_UDP_HPP
#define NEBULA_BSD_NET_UDP_HPP

#include "ipv4.hpp"
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace bsd {
namespace net {

/**
 * @brief UDP Packet Header (8 bytes)
 */
struct __attribute__((packed)) udp_header_t {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
};

class UDP {
public:
    static void handle_udp(const uint8_t* packet, uint16_t len);
};

} // namespace net
} // namespace bsd
} // namespace nebula

#endif // NEBULA_BSD_NET_UDP_HPP
