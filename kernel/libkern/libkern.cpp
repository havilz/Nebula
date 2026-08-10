#include <libkern/libkern.hpp>
#include <iokit/serial/serial.hpp>

namespace nebula {
namespace libkern {

OSObject::OSObject() : m_ref_count(1) {}

OSObject::~OSObject() {}

void OSObject::retain() const {
    __atomic_fetch_add(&m_ref_count, 1, __ATOMIC_RELAXED);
}

void OSObject::release() const {
    if (__atomic_sub_fetch(&m_ref_count, 1, __ATOMIC_RELEASE) == 0) {
        delete this;
    }
}

uint32_t OSObject::get_ref_count() const {
    return m_ref_count;
}

void init() {
    nebula::drivers::Serial::write_string("[LIBKERN] Runtime C++ Utility & OSObject Engine Initialized\n");
}

} // namespace libkern
} // namespace nebula

extern "C" {
    void* memset(void* dest, int val, size_t count) {
        uint8_t* ptr = (uint8_t*)dest;
        for (size_t i = 0; i < count; i++) {
            ptr[i] = (uint8_t)val;
        }
        return dest;
    }

    void* memcpy(void* dest, const void* src, size_t count) {
        uint8_t* d = (uint8_t*)dest;
        const uint8_t* s = (const uint8_t*)src;
        for (size_t i = 0; i < count; i++) {
            d[i] = s[i];
        }
        return dest;
    }

    int memcmp(const void* s1, const void* s2, size_t count) {
        const uint8_t* p1 = (const uint8_t*)s1;
        const uint8_t* p2 = (const uint8_t*)s2;
        for (size_t i = 0; i < count; i++) {
            if (p1[i] != p2[i]) return p1[i] - p2[i];
        }
        return 0;
    }

    void ___chkstk_ms() {}
    int __cxa_guard_acquire(uint64_t *g) { return !*(char *)(g); }
    void __cxa_guard_release(uint64_t *g) { *(char *)(g) = 1; }
    void __cxa_guard_abort(uint64_t *) {}
    int atexit(void (*)(void)) { return 0; }
}
