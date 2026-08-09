#ifndef NEBULA_BSD_NET_ICMP_HPP
#define NEBULA_BSD_NET_ICMP_HPP

#include "ipv4.hpp"
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace bsd {
namespace net {

/**
 * @brief ICMP Ping Packet Header (8 bytes)
 */
struct __attribute__((packed)) icmp_header_t {
    uint8_t  type; // 8 = Echo Request, 0 = Echo Reply
    uint8_t  code;
    uint16_t checksum;
    uint16_t id;
    uint16_t sequence;
};

class ICMP {
public:
    static void handle_icmp(const uint8_t* packet, uint16_t len);
};

} // namespace net
} // namespace bsd
} // namespace nebula

#endif // NEBULA_BSD_NET_ICMP_HPP
