# IOKit Timer Driver Implementation

Folder `kernel/iokit/timer/` berisi implementasi driver pewaktu.

## Source Tree

```text
kernel/iokit/timer/
└── pit.cpp  # Implementasi Driver PIT 8254 Timer IRQ 0
```

## Daftar File & Fungsi

| File | Deskripsi & Fungsi |
| :--- | :--- |
| `pit.cpp` | Mengirim divisor frekuensi ke Port `0x43` dan `0x40` untuk pemicuan IRQ 0 100Hz. |
