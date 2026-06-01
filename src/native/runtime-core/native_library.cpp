// native_library.cpp — Global native DLL cache with refcounting
//
// Provides LoadLibrary/dlopen caching for P/Invoke. Without this cache
// every P/Invoke call site would repeat LoadLibrary for the same DLL,
// which is expensive (IO + kernel transition) and leaks module handles.
//
// Design:
//   - Global unordered_map: canonical_name → { handle, refcount }
//   - shared_mutex for reads (fast path), exclusive for writes (slow path)
//   - Ref-counted: each NativeLibraryLoad +1, each NativeLibraryFree -1
//   - At refcount == 0 the library is unloaded and the entry removed
//   - No per-entry function-pointer cache (that lives in generated code
//     as static locals)

#include "native_library.h"

#include <chaos/pal/pal_dl.h>
#include <chaos/unordered_dense.h>

#include <cstring>
#include <mutex>
#include <shared_mutex>
#include <string>

namespace chaos::il2cpp::runtime_core {

namespace {

struct NativeLibraryEntry {
    void*      handle   = nullptr;
    uint32_t   refcount = 0;
};

using NativeLibraryMap = CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_STRING, NativeLibraryEntry);

// Global state guarded by a reader-writer lock.
NativeLibraryMap     s_library_map;
std::shared_mutex    s_map_mutex;

}  // anonymous namespace

void* NativeLibraryLoad(const char* name_utf8) {
    if (name_utf8 == nullptr || name_utf8[0] == '\0') return nullptr;

    const char* canonical = chaos::il2cpp::pal::PalDlNormalisePath(name_utf8);
    if (canonical == nullptr || canonical[0] == '\0') return nullptr;

    {
        // Fast path: already loaded.
        std::shared_lock lock(s_map_mutex);
        auto it = s_library_map.find(canonical);
        if (it != s_library_map.end()) {
            ++it->second.refcount;
            return it->second.handle;
        }
    }

    // Slow path: load the library via PAL.
    auto* handle = chaos::il2cpp::pal::PalDlOpen(canonical);

    if (handle == nullptr) return nullptr;

    // Insert into map (still under unique_lock).
    std::unique_lock lock(s_map_mutex);
    auto [it, inserted] = s_library_map.try_emplace(canonical);
    if (inserted) {
        it->second.handle   = handle;
        it->second.refcount = 1;
    } else {
        // Race: another thread loaded the same library between our
        // shared_lock release and unique_lock acquisition.
        // Use their handle and unload ours.
        ++it->second.refcount;
        chaos::il2cpp::pal::PalDlClose(handle);
        return it->second.handle;
    }
    return handle;
}

void* NativeLibraryGetProcAddress(void* handle, const char* symbol_utf8) {
    if (handle == nullptr || symbol_utf8 == nullptr) return nullptr;
    return chaos::il2cpp::pal::PalDlSym(handle, symbol_utf8);
}

bool NativeLibraryFree(void* handle) {
    if (handle == nullptr) return false;

    std::unique_lock lock(s_map_mutex);

    for (auto it = s_library_map.begin(); it != s_library_map.end(); ++it) {
        if (it->second.handle == handle) {
            if (--it->second.refcount == 0) {
                chaos::il2cpp::pal::PalDlClose(handle);
                s_library_map.erase(it);
            }
            return true;
        }
    }
    return false;  // handle not found
}


// ── DllImportResolver (P/Invoke Override) ──────────────────────────

// Global function pointer set by managed runtime via RegisterPInvokeResolverCallback.
// Signature: void* callback(const char* assemblyName, const char* libraryName)
static void* s_pinvoke_resolver_callback = nullptr;

void RegisterPInvokeResolverCallback(void* callback) noexcept {
    s_pinvoke_resolver_callback = callback;
}

void* TryResolveDllImport(const char* assembly_name_utf8, const char* library_name_utf8) noexcept {
    if (s_pinvoke_resolver_callback == nullptr) return nullptr;

    using ResolverFn = CHAOS_IL2CPP_INTPTR (*)(const char*, const char*);
    auto fn = reinterpret_cast<ResolverFn>(s_pinvoke_resolver_callback);
    auto handle = fn(assembly_name_utf8, library_name_utf8);
    return reinterpret_cast<void*>(handle);
}
}  // namespace chaos::il2cpp::runtime_core

// ABI export: P/Invoke resolver callback registration, callable from managed
// code via __Internal DllImport.  Dispatches to the namespaced C++ function.
extern "C" void RegisterPInvokeResolverCallback(void* callback) noexcept {
    chaos::il2cpp::runtime_core::RegisterPInvokeResolverCallback(callback);
}
