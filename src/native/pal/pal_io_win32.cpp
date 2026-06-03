// pal_io_win32.cpp — Windows I/O utility implementations
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include <chaos/pal/pal_io.h>

namespace chaos::il2cpp::pal {

void PalSetBinaryMode() noexcept {
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
}

}  // namespace chaos::il2cpp::pal
