#ifndef NEBULA_SECURITY_SECURITY_HPP
#define NEBULA_SECURITY_SECURITY_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace security {

/**
 * @brief User Credentials (ucred) Structure
 */
struct ucred_t {
    uint32_t uid;
    uint32_t gid;
    uint32_t euid;
    uint32_t egid;
};

/**
 * @brief Initialize Mandatory Access Control (MAC) Security Framework
 */
void init();

/**
 * @brief Check permission for process access
 */
bool check_permission(const ucred_t* cred, uint32_t requested_flags);

} // namespace security
} // namespace nebula

#endif // NEBULA_SECURITY_SECURITY_HPP
