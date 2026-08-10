#ifndef NEBULA_SAN_SANITIZER_HPP
#define NEBULA_SAN_SANITIZER_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace san {

/**
 * @brief Initialize Kernel Memory Sanitizer Hooks (KASAN / UBSAN)
 */
void init();

} // namespace san
} // namespace nebula

#endif // NEBULA_SAN_SANITIZER_HPP
