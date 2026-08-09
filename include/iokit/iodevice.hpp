#ifndef NEBULA_IOKIT_IODEVICE_HPP
#define NEBULA_IOKIT_IODEVICE_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace iokit {

/**
 * @brief Abstract Base Class for all IOKit C++ Hardware Devices & Drivers (XNU Model)
 */
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

#endif // NEBULA_IOKIT_IODEVICE_HPP
