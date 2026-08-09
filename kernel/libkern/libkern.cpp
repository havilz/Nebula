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
