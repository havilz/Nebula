#include <bsd/net/ethernet.hpp>

namespace nebula {
namespace bsd {
namespace net {

uint16_t Ethernet::htons(uint16_t hostshort) {
    return (uint16_t)((hostshort >> 8) | (hostshort << 8));
}

uint16_t Ethernet::ntohs(uint16_t netshort) {
    return htons(netshort);
}

uint32_t Ethernet::htonl(uint32_t hostlong) {
    return ((hostlong & 0x000000FF) << 24) |
           ((hostlong & 0x0000FF00) << 8)  |
           ((hostlong & 0x00FF0000) >> 8)  |
           ((hostlong & 0xFF000000) >> 24);
}

uint32_t Ethernet::ntohl(uint32_t netlong) {
    return htonl(netlong);
}

} // namespace net
} // namespace bsd
} // namespace nebula
