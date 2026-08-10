#ifndef NEBULA_BSD_SYS_BUNDLE_HPP
#define NEBULA_BSD_SYS_BUNDLE_HPP

#include <stddef.h>
#include <stdint.h>

namespace nebula {
namespace bsd {

/**
 * @brief Native App Bundle (.app / .neb) Metadata Structure
 */
struct app_bundle_t {
    char name[64];
    char version[16];
    char exec_path[128];
    char icon_path[128];
    bool is_valid;
};

/**
 * @brief App Bundle Parser & Package Manager Class
 */
class AppBundleManager {
public:
    /**
     * @brief Parse application bundle metadata from path
     */
    static bool parse_bundle(const char* bundle_path, app_bundle_t* bundle_out);
};

} // namespace bsd
} // namespace nebula

#endif // NEBULA_BSD_SYS_BUNDLE_HPP
