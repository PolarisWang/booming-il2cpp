// pal_io.h — I/O utilities (stdin/stdout binary mode, etc.)
#pragma once

namespace chaos::il2cpp::pal {

/// Set stdin/stdout to binary mode.
/// On Windows: calls _setmode(_fileno(stdin), _O_BINARY) to disable CR/LF
/// translation for binary protocol transport (debugger, eventpipe pipes).
/// On POSIX: no-op (stdin/stdout are already in binary mode).
void PalSetBinaryMode() noexcept;

}  // namespace chaos::il2cpp::pal
