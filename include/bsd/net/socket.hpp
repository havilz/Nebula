#ifndef NEBULA_BSD_NET_SOCKET_HPP
#define NEBULA_BSD_NET_SOCKET_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace bsd {
namespace net {

static const int AF_INET     = 2;
static const int SOCK_STREAM = 1;
static const int SOCK_DGRAM  = 2;

struct sockaddr_in {
    uint16_t sin_family;
    uint16_t sin_port;
    uint32_t sin_addr;
    char     sin_zero[8];
};

struct bsd_socket_t {
    bool in_use;
    int domain;
    int type;
    int protocol;
    uint16_t local_port;
    uint32_t local_ip;
};

class SocketManager {
public:
    static void init();
    static int sys_socket(int domain, int type, int protocol);
    static int sys_bind(int sockfd, const sockaddr_in* addr, uint32_t addrlen);
};

} // namespace net
} // namespace bsd
} // namespace nebula

#endif // NEBULA_BSD_NET_SOCKET_HPP
