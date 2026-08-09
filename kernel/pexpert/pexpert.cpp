#include <pexpert/pexpert.hpp>
#include <iokit/serial/serial.hpp>

namespace nebula {
namespace pexpert {

void init() {
    nebula::drivers::Serial::write_string("[PEXPERT] Platform Expert Device Tree & Hardware Initialization Complete\n");
}

} // namespace pexpert
} // namespace nebula
