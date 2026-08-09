#ifndef NEBULA_LIBKERN_LIBKERN_HPP
#define NEBULA_LIBKERN_LIBKERN_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace libkern {

/**
 * @brief OSObject - Base class for XNU RTTI Object System
 */
class OSObject {
private:
    mutable uint32_t m_ref_count;

public:
    OSObject();
    virtual ~OSObject();

    virtual void retain() const;
    virtual void release() const;
    virtual uint32_t get_ref_count() const;
};

/**
 * @brief Initialize Libkern Utility Subsystem
 */
void init();

} // namespace libkern
} // namespace nebula

#endif // NEBULA_LIBKERN_LIBKERN_HPP
