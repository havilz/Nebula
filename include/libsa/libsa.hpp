#ifndef NEBULA_LIBSA_LIBSA_HPP
#define NEBULA_LIBSA_LIBSA_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace libsa {

/**
 * @brief Dynamic Kernel Extension (Kext) Module Descriptor
 */
struct kext_module_t {
    const char* name;
    const char* version;
    bool is_loaded;
    void* entry_point;
};

/**
 * @brief Initialize Libsa Standalone Kernel Infrastructure & Module Loader
 */
void init();

} // namespace libsa
} // namespace nebula

#endif // NEBULA_LIBSA_LIBSA_HPP
