# PATTERN.md - Pola Desain Arsitektur C++ Bare-Metal Nebula OS

Dokumen ini mencatat pola desain (Design Patterns) dan idiom arsitektural yang diterapkan dalam basis kode **Nebula OS** (XNU-Hybrid Architecture Model) untuk menjaga konsistensi dan modularitas sistem.

---

## 1. Hardware Abstraction Layer (HAL) & XNU Layering

### Tujuan:
Memisahkan kode spesifik arsitektur CPU (seperti register x86_64, instruksi `inb`/`outb`, Paging) dari layanan POSIX OS (VFS dan Syscall).

### Implementasi:
* Kelas antarmuka Mach Core didefinisikan di `include/mach/` dan diimplementasikan di `kernel/mach/`.
* Layanan POSIX BSD OS Services didefinisikan di `include/bsd/` dan diimplementasikan di `kernel/bsd/`.
* Driver C++ Berorientasi Objek didefinisikan di `include/iokit/` dan diimplementasikan di `kernel/iokit/`.

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

## 2. IOKit C++ Object-Oriented Driver Pattern (`IODevice`)

### Tujuan:
Menyediakan kelas dasar berorientasi objek (OOP) untuk seluruh driver hardware dengan *lifecycle management* yang terstruktur (`init()`, `start()`, `stop()`).

### Implementasi:
```cpp
namespace nebula {
namespace iokit {

class IODevice {
protected:
    const char* m_name;
    bool m_started;

public:
    IODevice(const char* name) : m_name(name), m_started(false) {}
    virtual ~IODevice() {}

    virtual bool init() = 0;
    virtual bool start() = 0;
    virtual void stop() = 0;

    const char* get_name() const { return m_name; }
    bool is_started() const { return m_started; }
};

} // namespace iokit
} // namespace nebula
```

---

## 3. Libkern `OSObject` Reference Counting Pattern

### Tujuan:
Menyediakan manajemen *lifecycle* objek C++ terhitung (*refcounted*) tanpa bergantung pada RTTI atau Standard C++ Library (`std::`).

### Implementasi:
```cpp
namespace nebula {
namespace libkern {

class OSObject {
private:
    mutable uint32_t m_ref_count;

public:
    OSObject() : m_ref_count(1) {}
    virtual ~OSObject() {}

    virtual void retain() const {
        __atomic_fetch_add(&m_ref_count, 1, __ATOMIC_RELAXED);
    }
    virtual void release() const {
        if (__atomic_sub_fetch(&m_ref_count, 1, __ATOMIC_RELEASE) == 0) {
            delete this;
        }
    }
    virtual uint32_t get_ref_count() const { return m_ref_count; }
};

} // namespace libkern
} // namespace nebula
```

---

## 4. Kernel Singleton / Static Engine Pattern

### Tujuan:
Memastikan manajer utama kernel (seperti `PMM`, `VMM`, `Scheduler`) hanya memiliki tepat 1 instans terpusat di seluruh sistem.

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

## 5. RAII (Resource Acquisition Is Initialization)

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

## 6. Result Pattern (`Result<T, E>`)

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

## 7. Event Callback / Observer Pattern (Driver & UI Window Engine)

### Tujuan:
Menghubungkan interupsi hardware (seperti Keyboard PS/2 atau Mouse) dengan GUI Compositor / Shell tanpa memunculkan dependensi sirkular.

### Implementasi:
Driver mendaftarkan listener fungsi/lambda yang akan dipanggil saat kejadian hardware terjadi.
