# IOKit Storage Subsystem Implementation

Folder `kernel/iokit/storage/` berisi kode sumber implementasi C++ untuk driver pengontrol media penyimpanan dan parser tabel partisi MBR.

## Source Tree

```text
kernel/iokit/storage/
├── ata.cpp      # Implementasi ATA/IDE LBA28 PIO Mode Sector Read/Write
├── mbr.cpp      # Implementasi MBR Sector 0 Partition Table Scanner
└── README.md    # Storage Subsystem Implementation Documentation
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `ata.cpp` | Pemrograman Port I/O Primary ATA (`0x1F0`-`0x1F7`), pembacaan identifikasi drive `0xEC`, dan transfer sektor PIO. |
| `mbr.cpp` | Pemrosesan sektor boot Sektor 0, pengesahan *boot signature* `0xAA55`, dan ekstraksi LBA awal partisi FAT32. |
