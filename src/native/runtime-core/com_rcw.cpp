#include <cstring>
#include <cstdlib>
#include <mutex>
#include <chaos/native_types.h>
#include <chaos/unordered_dense.h>
#include <chaos/log.h>
#include "com_rcw.h"

// On Windows, QueryInterface uses the real Windows GUID/IUnknown types.
#if defined(_WIN32)
#include <objbase.h>
#endif

namespace chaos::il2cpp::com_rcw {
namespace {

// ── Global RCW table ──────────────────────────────────────────────
// Maps IUnknown identity pointer → ComRcwNative*.
// Thread-safe: all operations hold s_rcw_table_mutex.
CHAOS_IL2CPP_MUTEX s_rcw_table_mutex;
CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(void*, ComRcwNative*) s_rcw_table;

}  // anonymous namespace

ComRcwNative* FindOrCreateRcw(void* unknown_ptr) noexcept {
    if (unknown_ptr == nullptr) return nullptr;

    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_rcw_table_mutex);

    // Check existing.
    auto it = s_rcw_table.find(unknown_ptr);
    if (it != s_rcw_table.end()) {
        it->second->wrapper_refcount++;
        return it->second;
    }

    // Allocate new RCW.
    auto* rcw = static_cast<ComRcwNative*>(std::malloc(sizeof(ComRcwNative)));
    if (rcw == nullptr) return nullptr;

    rcw->magic = kComRcwMagic;
    rcw->identity_unknown = unknown_ptr;
    rcw->wrapper_refcount = 1;
    rcw->cache_count = 0;

    // Clear the interface cache.
    for (CHAOS_IL2CPP_SIZE i = 0; i < kMaxInterfaceCache; ++i) {
        rcw->interface_cache[i].iid = nullptr;
        rcw->interface_cache[i].interface_ptr = nullptr;
        rcw->interface_cache[i].refcount = 0;
    }

    // AddRef the canonical IUnknown on first creation.
#if defined(_WIN32)
    if (unknown_ptr != nullptr) {
        auto* vtbl = *static_cast<chaos::il2cpp::com_abi::IUnknownVtbl**>(unknown_ptr);
        vtbl->AddRef(unknown_ptr);
    }
#endif

    s_rcw_table[unknown_ptr] = rcw;
    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "Created RCW {0} for IUnknown {1}",
                              static_cast<void*>(rcw), unknown_ptr);
    return rcw;
}

void ReleaseRcw(ComRcwNative* rcw) noexcept {
    if (rcw == nullptr) return;

    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_rcw_table_mutex);

    if (rcw->wrapper_refcount == 0 || --rcw->wrapper_refcount > 0) {
        return;
    }

    // Release all cached interface pointers.
    for (CHAOS_IL2CPP_SIZE i = 0; i < rcw->cache_count; ++i) {
        auto& entry = rcw->interface_cache[i];
        if (entry.interface_ptr != nullptr) {
#if defined(_WIN32)
            auto* vtbl = *static_cast<chaos::il2cpp::com_abi::IUnknownVtbl**>(entry.interface_ptr);
            vtbl->Release(entry.interface_ptr);
#endif
            entry.interface_ptr = nullptr;
        }
    }

    // Remove from global table.
    s_rcw_table.erase(rcw->identity_unknown);

    // Release the canonical IUnknown.
#if defined(_WIN32)
    if (rcw->identity_unknown != nullptr) {
        auto* vtbl = *static_cast<chaos::il2cpp::com_abi::IUnknownVtbl**>(rcw->identity_unknown);
        vtbl->Release(rcw->identity_unknown);
    }
#endif

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "Released RCW {0} for IUnknown {1}",
                              static_cast<void*>(rcw), rcw->identity_unknown);
    std::free(rcw);
}

void* QueryInterfaceCached(ComRcwNative* rcw, const void* iid_bytes) noexcept {
    if (rcw == nullptr || iid_bytes == nullptr) return nullptr;

    // Check cache first (no lock needed — cache is per-RCW and only
    // grows; entries are written-once).
    for (CHAOS_IL2CPP_SIZE i = 0; i < rcw->cache_count; ++i) {
        auto& entry = rcw->interface_cache[i];
        if (std::memcmp(entry.iid, iid_bytes, 16) == 0) {
            entry.refcount++;
            return entry.interface_ptr;
        }
    }

    // Cache miss — call QueryInterface.
#if defined(_WIN32)
    GUID iid;
    std::memcpy(&iid, iid_bytes, sizeof(GUID));

    void* result = nullptr;
    auto* vtbl = *static_cast<chaos::il2cpp::com_abi::IUnknownVtbl**>(rcw->identity_unknown);
    HRESULT hr = vtbl->QueryInterface(rcw->identity_unknown, &iid, &result);
    if (FAILED(hr) || result == nullptr) return nullptr;

    // Cache the result if there's room.
    if (rcw->cache_count < kMaxInterfaceCache) {
        CHAOS_IL2CPP_SIZE idx = rcw->cache_count++;
        rcw->interface_cache[idx].iid = iid_bytes;
        rcw->interface_cache[idx].interface_ptr = result;
        // result is already AddRef'd by QI — set refcount to 1 for the cache.
        rcw->interface_cache[idx].refcount = 1;
    }

    return result;
#else
    (void)iid_bytes;
    return nullptr;
#endif
}

}  // namespace chaos::il2cpp::com_rcw
