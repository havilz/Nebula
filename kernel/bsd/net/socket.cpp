#include <stddef.h>
#include <stdint.h>
#include "../../../include/bsd/net/socket.hpp"
#include "../../../include/iokit/serial/serial.hpp"

namespace nebula {
namespace bsd {
namespace net {

static const size_t MAX_SOCKETS = 16;
static bsd_socket_t s_sockets[MAX_SOCKETS];

void SocketManager::init() {
  for (size_t i = 0; i < MAX_SOCKETS; i++) {
    s_sockets[i].in_use = false;
    s_sockets[i].domain = 0;
    s_sockets[i].type = 0;
    s_sockets[i].protocol = 0;
    s_sockets[i].local_port = 0;
    s_sockets[i].local_ip = 0;
  }
  nebula::drivers::Serial::write_string(
      "[NET] BSD Socket Subsystem Initialized (16 Control Slots)\n");
}

int SocketManager::sys_socket(int domain, int type, int protocol) {
  for (size_t i = 0; i < MAX_SOCKETS; i++) {
    if (!s_sockets[i].in_use) {
      s_sockets[i].in_use = true;
      s_sockets[i].domain = domain;
      s_sockets[i].type = type;
      s_sockets[i].protocol = protocol;
      return static_cast<int>(i + 3);
    }
  }
  return -1;
}

int SocketManager::sys_bind(int sockfd, const sockaddr_in *addr,
                            uint32_t addrlen) {
  (void)addrlen;
  int idx = sockfd - 3;
  if (idx < 0 || idx >= static_cast<int>(MAX_SOCKETS) ||
      !s_sockets[idx].in_use || addr == nullptr) {
    return -1;
  }

  s_sockets[idx].local_port = addr->sin_port;
  s_sockets[idx].local_ip = addr->sin_addr;
  return 0;
}

} // namespace net
} // namespace bsd
} // namespace nebula
