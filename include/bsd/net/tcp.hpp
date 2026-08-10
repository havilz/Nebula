#ifndef NEBULA_BSD_NET_TCP_HPP
#define NEBULA_BSD_NET_TCP_HPP

#include "ipv4.hpp"
#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace bsd {
namespace net {

/**
 * @brief TCP Segment Header (20 bytes)
 */
struct __attribute__((packed)) tcp_header_t {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint8_t  data_offset;
    uint8_t  flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_ptr;
};

class TCP {
public:
    static void handle_tcp(const uint8_t* packet, uint16_t len);
};

} // namespace net
} // namespace bsd
} // namespace nebula

#endif // NEBULA_BSD_NET_TCP_HPP
