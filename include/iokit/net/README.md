# IOKit Network Card Driver Headers

Folder `include/iokit/net/` berisi deklarasi header untuk driver perangkat keras kartu jaringan (NIC).

## Source Tree

```text
include/iokit/net/
├── e1000.hpp    # Intel E1000 PCI Gigabit Network Driver Header
└── README.md    # Network Card Driver Header Documentation
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `e1000.hpp` | Deklarasi kelas `E1000Driver`, `e1000_rx_desc_t`, `e1000_tx_desc_t`, dan pengiriman/penerimaan paket data mentah Ethernet. |
