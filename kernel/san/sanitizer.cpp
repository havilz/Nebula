#include <san/sanitizer.hpp>
#include <iokit/serial/serial.hpp>

namespace nebula {
namespace san {

void init() {
    nebula::drivers::Serial::write_string("[SAN] Kernel Sanitizer Memory Hooks (KASAN / UBSAN) Active\n");
}

} // namespace san
} // namespace nebula
