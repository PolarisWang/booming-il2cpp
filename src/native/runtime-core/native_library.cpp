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

#include <chaos/unordered_dense.h>

#include <cstring>
#include <mutex>
#include <shared_mutex>
#include <string>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

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

// ── Canonicalise a module name ──────────────────────────────────────
// Strip directory separators, normalise case on Windows.
static std::string CanonicaliseName(const char* name) {
    std::string result;
    if (name == nullptr) return result;

    // Determine length.
    const auto len = std::strlen(name);
    result.reserve(len);

    for (size_t i = 0; i < len; ++i) {
        char c = name[i];
#if defined(_WIN32)
        // Normalise separators to '\' (Windows native).
        if (c == '/') c = '\\';
        // Case-insensitive: fold to lowercase.
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
#else
        // POSIX: normalise separators to '/'.
        if (c == '\\') c = '/';
#endif
        result.push_back(c);
    }
    return result;
}

}  // anonymous namespace

void* NativeLibraryLoad(const char* name_utf8) {
    if (name_utf8 == nullptr || name_utf8[0] == '\0') return nullptr;

    const auto canonical = CanonicaliseName(name_utf8);
    if (canonical.empty()) return nullptr;

    {
        // Fast path: already loaded.
        std::shared_lock lock(s_map_mutex);
        auto it = s_library_map.find(canonical);
        if (it != s_library_map.end()) {
            ++it->second.refcount;
            return it->second.handle;
        }
    }

    // Slow path: load the library.
#if defined(_WIN32)
    // Try the canonical name first; if it doesn't contain '.', try
    // appending ".dll" so that "kernel32" → "kernel32.dll" works.
    auto search_name = canonical;
    if (search_name.find('.') == std::string::npos) {
        search_name += ".dll";
    }
    auto* handle = static_cast<void*>(::LoadLibraryA(search_name.c_str()));
#else
    // POSIX: dlopen with RTLD_LAZY | RTLD_LOCAL.
    auto* handle = ::dlopen(canonical.c_str(), RTLD_LAZY | RTLD_LOCAL);
#endif

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
#if defined(_WIN32)
        ::FreeLibrary(static_cast<HMODULE>(handle));
#else
        ::dlclose(handle);
#endif
        return it->second.handle;
    }
    return handle;
}

void* NativeLibraryGetProcAddress(void* handle, const char* symbol_utf8) {
    if (handle == nullptr || symbol_utf8 == nullptr) return nullptr;

#if defined(_WIN32)
    return reinterpret_cast<void*>(
        ::GetProcAddress(static_cast<HMODULE>(handle), symbol_utf8));
#else
    return ::dlsym(handle, symbol_utf8);
#endif
}

bool NativeLibraryFree(void* handle) {
    if (handle == nullptr) return false;

    std::unique_lock lock(s_map_mutex);

    for (auto it = s_library_map.begin(); it != s_library_map.end(); ++it) {
        if (it->second.handle == handle) {
            if (--it->second.refcount == 0) {
#if defined(_WIN32)
                ::FreeLibrary(static_cast<HMODULE>(handle));
#else
                ::dlclose(handle);
#endif
                s_library_map.erase(it);
            }
            return true;
        }
    }
    return false;  // handle not found
}

}  // namespace chaos::il2cpp::runtime_core
