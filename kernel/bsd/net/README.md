# BSD TCP/IP Network Protocol Stack Implementation

Folder `kernel/bsd/net/` berisi implementasi BSD TCP/IP Network Stack dan Socket API.

## Source Tree

```text
kernel/bsd/net/
├── ethernet.cpp # Implementasi Konversi Byte Order & Ethernet Layer
├── arp.cpp      # Implementasi ARP Resolution & Table Caching
├── ipv4.cpp     # Implementasi IPv4 Packet Processing & Routing (10.0.2.15)
├── icmp.cpp     # Implementasi Balasan ICMP Ping Echo Request
├── socket.cpp   # Implementasi BSD Socket API System Calls
└── README.md    # BSD Network Stack Implementation Documentation
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `ethernet.cpp` | Konversi `htons`, `ntohs`, `htonl`, `ntohl`. |
| `arp.cpp` | Pemrosesan paket resolusi alamat hardware ARP. |
| `ipv4.cpp` | Kalkulasi checksum IPv4 dan penetapan IP lokal `10.0.2.15`. |
| `icmp.cpp` | Penanganan pesan balasan ICMP Echo Reply (Response terhadap `ping`). |
| `socket.cpp` | Pengaturan antarmuka socket BSD pada kernel. |
