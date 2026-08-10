#include <libsa/libsa.hpp>
#include <iokit/serial/serial.hpp>

namespace nebula {
namespace libsa {

void init() {
    nebula::drivers::Serial::write_string("[LIBSA] Standalone Kernel Infrastructure & Module Loader Initialized\n");
}

} // namespace libsa
} // namespace nebula
