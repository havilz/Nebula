# IOKit Storage Subsystem Headers

Folder `include/iokit/storage/` berisi deklarasi header untuk subsistem pengontrol media penyimpanan (*Storage Controllers*) dan pemroses tabel partisi (*Partition Table Parsers*).

## Source Tree

```text
include/iokit/storage/
├── ata.hpp      # Primary ATA/IDE Controller Driver Header
├── mbr.hpp      # Master Boot Record (MBR) Partition Table Parser Header
└── README.md    # Storage Subsystem Header Documentation
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `ata.hpp` | Deklarasi kelas `ATADriver` berorientasi objek turunan `IODevice` (LBA28 PIO Sector Read/Write 512-byte). |
| `mbr.hpp` | Deklarasi parser MBR Sektor 0 (`mbr_partition_entry_t`) untuk mendeteksi lokasi fisik LBA partisi FAT32. |
