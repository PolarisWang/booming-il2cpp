#ifndef CHAOS_IL2CPP_NATIVE_LIBRARY_H_
#define CHAOS_IL2CPP_NATIVE_LIBRARY_H_

#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core {

/// Load a native DLL by name. Returns an opaque handle (HMODULE on Windows,
/// dlopen handle on POSIX). The module is ref-counted: each call adds a
/// reference. Call NativeLibraryFree to release.
/// Thread-safe: concurrent calls with the same name return the same handle.
void* NativeLibraryLoad(const char* name_utf8);

/// Resolve a symbol (function or data) from a previously loaded library.
/// Returns a function pointer or nullptr if not found.
/// Thread-safe.
void* NativeLibraryGetProcAddress(void* handle, const char* symbol_utf8);

/// Release one reference to a native library. When the ref-count reaches
/// zero the library is unloaded (FreeLibrary / dlclose).
/// Returns true on success, false if the handle was invalid.
bool NativeLibraryFree(void* handle);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_NATIVE_LIBRARY_H_
