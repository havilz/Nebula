# IOKit Network Card Driver Implementation

Folder `kernel/iokit/net/` berisi implementasi driver perangkat keras kartu jaringan (NIC).

## Source Tree

```text
kernel/iokit/net/
├── e1000.cpp    # Implementasi Driver Intel E1000 Gigabit Network Card
└── README.md    # Network Card Driver Implementation Documentation
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `e1000.cpp` | Inisialisasi PCI MMIO, alokasi 32 RX/TX ring descriptors buffer, ekstraksi MAC Address, dan pengiriman paket Ethernet. |
