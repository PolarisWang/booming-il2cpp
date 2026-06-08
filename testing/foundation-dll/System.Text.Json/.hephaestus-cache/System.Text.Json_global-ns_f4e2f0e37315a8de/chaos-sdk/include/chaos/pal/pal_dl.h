#ifndef CHAOS_IL2CPP_PAL_DL_H_
#define CHAOS_IL2CPP_PAL_DL_H_

// ── Dynamic library loading ──────────────────────────────────────────
// Platform-independent wrappers for dlopen / LoadLibrary.
// ============================================================================

#include <chaos/pal/pal_types.h>

namespace chaos::il2cpp::pal {

/// Open a dynamic library by path or name.
/// On Windows, if @a path has no extension, ".dll" is appended automatically.
/// Returns nullptr on failure (call PalDlError for diagnostic).
PalLibraryPtr PalDlOpen(const char* path) noexcept;

/// Look up a symbol (function or data) in an open library.
/// Returns nullptr if the symbol is not found.
PalProcAddress PalDlSym(PalLibraryPtr handle, const char* symbol) noexcept;

/// Close a library previously opened with PalDlOpen.
/// Returns true on success, false if the handle was invalid.
bool PalDlClose(PalLibraryPtr handle) noexcept;

/// Return a human-readable error string from the last failed PalDlOpen/Sym call.
/// The returned pointer is valid only until the next PAL call on the same thread.
const char* PalDlError() noexcept;

/// Normalise a module path: convert separators, fold case (on Windows).
/// Returns a copy in a thread-local buffer (valid until next call).
const char* PalDlNormalisePath(const char* path) noexcept;

}  // namespace chaos::il2cpp::pal

#endif  // CHAOS_IL2CPP_PAL_DL_H_
