#ifndef NEBULA_BSD_NET_ETHERNET_HPP
#define NEBULA_BSD_NET_ETHERNET_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace bsd {
namespace net {

static const uint16_t ETHERTYPE_IPV4 = 0x0800;
static const uint16_t ETHERTYPE_ARP  = 0x0806;

/**
 * @brief Ethernet II Frame Header (14 bytes)
 */
struct __attribute__((packed)) ethernet_header_t {
    uint8_t  dest_mac[6];
    uint8_t  src_mac[6];
    uint16_t ethertype; // Big-Endian
};

class Ethernet {
public:
    static uint16_t htons(uint16_t hostshort);
    static uint16_t ntohs(uint16_t netshort);
    static uint32_t htonl(uint32_t hostlong);
    static uint32_t ntohl(uint32_t netlong);
};

} // namespace net
} // namespace bsd
} // namespace nebula

#endif // NEBULA_BSD_NET_ETHERNET_HPP
