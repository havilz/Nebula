#ifndef NEBULA_BSD_NET_IPV4_HPP
#define NEBULA_BSD_NET_IPV4_HPP

#include "ethernet.hpp"
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace bsd {
namespace net {

static const uint8_t IP_PROTO_ICMP = 1;
static const uint8_t IP_PROTO_TCP  = 6;
static const uint8_t IP_PROTO_UDP  = 17;

/**
 * @brief IPv4 Packet Header (20 bytes)
 */
struct __attribute__((packed)) ipv4_header_t {
    uint8_t  version_ihl;
    uint8_t  tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t flags_fragment;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
};

class IPv4 {
public:
    static void init();
    static uint16_t checksum(const void* vdata, size_t length);
    static void handle_ip(const uint8_t* packet, uint16_t len);
};

} // namespace net
} // namespace bsd
} // namespace nebula

#endif // NEBULA_BSD_NET_IPV4_HPP
