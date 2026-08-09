#ifndef NEBULA_BSD_NET_ARP_HPP
#define NEBULA_BSD_NET_ARP_HPP

#include "ethernet.hpp"
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace bsd {
namespace net {

/**
 * @brief ARP Packet Header (28 bytes)
 */
struct __attribute__((packed)) arp_header_t {
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t  hardware_size;
    uint8_t  protocol_size;
    uint16_t opcode; // 1 = Request, 2 = Reply
    uint8_t  sender_mac[6];
    uint32_t sender_ip;
    uint8_t  target_mac[6];
    uint32_t target_ip;
};

class ARP {
public:
    static void init();
    static void handle_arp(const uint8_t* packet, uint16_t len);
};

} // namespace net
} // namespace bsd
} // namespace nebula

#endif // NEBULA_BSD_NET_ARP_HPP
