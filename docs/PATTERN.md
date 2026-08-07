# PATTERN.md - Pola Desain Arsitektur C++ Bare-Metal Nebula OS

Dokumen ini mencatat pola desain (Design Patterns) dan idiom arsitektural yang diterapkan dalam basis kode **Nebula OS** untuk menjaga konsistensi dan modularitas sistem.

---

## 1. Hardware Abstraction Layer (HAL)

### Tujuan:
Memisahkan kode spesifik arsitektur CPU (seperti register x86_64, instruksi `inb`/`outb`, PML4 paging) dari logika bisnis kernel (seperti VFS dan Scheduler).

### Implementasi:
* Kelas antarmuka abstrak didefinisikan di `include/kernel/`.
* Implementasi konkrit diletakkan di `kernel/arch/x86_64/`.

```cpp
// Contoh: HAL Interrupt Controller
class IInterruptController {
public:
    virtual void enable_irq(uint8_t irq) = 0;
    virtual void disable_irq(uint8_t irq) = 0;
    virtual void send_eoi(uint8_t irq) = 0;
};
```

---

## 2. Kernel Singleton / Static Engine Pattern

### Tujuan:
Memastikan manajer utama kernel (seperti `PhysicalMemoryManager`, `InterruptManager`, `Scheduler`) hanya memiliki tepat 1 instans terpusat di seluruh sistem.

### Implementasi:
Menggunakan instans statis dengan inisialisasi eksplisit (bukan Meyers Singleton bertipe `static` lokal yang memerlukan thread-safe initialization runtime).

```cpp
class PhysicalMemoryManager {
private:
    static PhysicalMemoryManager* s_instance;

public:
    static void initialize(size_t total_memory);
    static PhysicalMemoryManager& instance() {
        return *s_instance;
    }

    uintptr_t allocate_frame();
    void free_frame(uintptr_t frame_addr);
};
```

---

## 3. RAII (Resource Acquisition Is Initialization)

### Tujuan:
Mencegah Memory Leak dan Spinlock Deadlock di dalam kernel dengan memanfaatkan destruktor otomatis C++.

### Implementasi:
1. **Lock Guard (Spinlock RAII)**:
```cpp
class SpinlockGuard {
private:
    Spinlock& m_lock;

public:
    explicit SpinlockGuard(Spinlock& lock) : m_lock(lock) {
        m_lock.acquire();
    }
    ~SpinlockGuard() {
        m_lock.release();
    }
};
```

2. **Smart Pointers Custom (`UniquePtr` / `SharedPtr`)**:
Membungkus alokasi memori heap kernel (`kmalloc`) sehingga memori dibebaskan secara otomatis saat keluar dari scope fungsi.

---

## 4. Result Pattern (`Result<T, E>`)

### Tujuan:
Menggantikan C++ Exception (yang dilarang) dalam menangani kegagalan operasi kernel secara aman dan eksplisit.

### Implementasi:
```cpp
template <typename T, typename E>
class Result {
private:
    T m_value;
    E m_error;
    bool m_isOk;

public:
    static Result ok(T val) { ... }
    static Result err(E err) { ... }

    bool is_ok() const { return m_isOk; }
    T value() const { return m_value; }
    E error() const { return m_error; }
};
```

---

## 5. Event Callback / Observer Pattern (Driver & UI Window Engine)

### Tujuan:
Menghubungkan interupsi hardware (seperti Keyboard PS/2 atau Mouse) dengan GUI Compositor / Shell tanpa memunculkan dependensi sirkular.

### Implementasi:
Driver mendaftarkan listener fungsi/lambda yang akan dipanggil saat kejadian hardware terjadi.
