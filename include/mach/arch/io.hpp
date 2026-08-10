#ifndef NEBULA_ARCH_X86_64_IO_HPP
#define NEBULA_ARCH_X86_64_IO_HPP

#include <stdint.h>

namespace nebula {
namespace arch {
namespace x86_64 {

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "d"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val) {
    asm volatile ("outw %0, %1" : : "a"(val), "d"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

static inline void outl(uint16_t port, uint32_t val) {
    asm volatile ("outl %0, %1" : : "a"(val), "d"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ("inl %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

static inline void io_wait() {
    asm volatile ("outb %0, $0x80" : : "a"(static_cast<uint8_t>(0)));
}

} // namespace x86_64
} // namespace arch
} // namespace nebula

#endif // NEBULA_ARCH_X86_64_IO_HPP
