# BSD TCP/IP Network Protocol Stack Headers

Folder `include/bsd/net/` berisi deklarasi header untuk BSD TCP/IP Network Stack dan Socket API.

## Source Tree

```text
include/bsd/net/
├── ethernet.hpp # Ethernet II Frame Header & Byte Order Helper
├── arp.hpp      # Address Resolution Protocol Header
├── ipv4.hpp     # IPv4 Packet Header & Checksum
├── icmp.hpp     # ICMP Ping Echo Protocol Header
├── udp.hpp      # UDP Transport Protocol Header
├── tcp.hpp      # TCP Transport Protocol Header
├── socket.hpp   # BSD Socket API Abstraction Header
└── README.md    # BSD Network Stack Header Documentation
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `ethernet.hpp` | Header frame Ethernet II 14-byte (`ethernet_header_t`) dan pembantu konversi byte order `htons`/`ntohs`. |
| `arp.hpp` | Header ARP 28-byte (`arp_header_t`) untuk pemetaan alamat IP ke MAC Address. |
| `ipv4.hpp` | Header paket IPv4 20-byte (`ipv4_header_t`) dan fungsi checksum internet. |
| `icmp.hpp` | Header paket ICMP (`icmp_header_t`) untuk penanganan pesan `ping`. |
| `udp.hpp` | Header datagram UDP (`udp_header_t`). |
| `tcp.hpp` | Header segment TCP (`tcp_header_t`). |
| `socket.hpp` | Abstraksi BSD Socket API (`sys_socket`, `sys_bind`). |
