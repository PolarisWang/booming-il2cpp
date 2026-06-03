// pal_io_posix.cpp — POSIX I/O utility implementations
// On POSIX, stdin/stdout are already in binary mode, so PalSetBinaryMode
// is a no-op.

#include <chaos/pal/pal_io.h>

namespace chaos::il2cpp::pal {

void PalSetBinaryMode() noexcept {
    // stdin/stdout are already in binary mode on POSIX. No-op.
}

}  // namespace chaos::il2cpp::pal
