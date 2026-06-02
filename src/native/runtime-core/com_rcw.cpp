#include <cstring>
#include <cstdlib>
#include <mutex>
#include <chaos/native_types.h>
#include <chaos/unordered_dense.h>
#include <chaos/log.h>
#include <chaos/pal/pal_com.h>
#include "com_rcw.h"
#include "memory_domain.h"

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
    auto* rcw = static_cast<ComRcwNative*>(memory_domain::DomainCurrentAllocateTagged(sizeof(ComRcwNative)));
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
    chaos::il2cpp::pal::PalComAddRef(unknown_ptr);

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
            chaos::il2cpp::pal::PalComRelease(entry.interface_ptr);
            entry.interface_ptr = nullptr;
        }
    }

    // Remove from global table.
    s_rcw_table.erase(rcw->identity_unknown);

    // Release the canonical IUnknown.
    chaos::il2cpp::pal::PalComRelease(rcw->identity_unknown);

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "Released RCW {0} for IUnknown {1}",
                              static_cast<void*>(rcw), rcw->identity_unknown);
    memory_domain::DomainFreeTagged(rcw);
}

void* QueryInterfaceCached(ComRcwNative* rcw, const void* iid_bytes) noexcept {
    if (rcw == nullptr || iid_bytes == nullptr) return nullptr;

    // Check cache first (no lock needed — cache is per-RCW and only
    // grows; entries are written-once).
    for (CHAOS_IL2CPP_SIZE i = 0; i < rcw->cache_count; ++i) {
        auto& entry = rcw->interface_cache[i];
        if (CHAOS_IL2CPP_MEMCMP(entry.iid, iid_bytes, 16) == 0) {
            entry.refcount++;
            return entry.interface_ptr;
        }
    }

    // Cache miss — call QueryInterface.
    void* result = nullptr;
    CHAOS_IL2CPP_INT32 hr = chaos::il2cpp::pal::PalComQueryInterface(
        rcw->identity_unknown, iid_bytes, &result);
    if (hr < 0 || result == nullptr) return nullptr;

    // Cache the result if there's room.
    if (rcw->cache_count < kMaxInterfaceCache) {
        CHAOS_IL2CPP_SIZE idx = rcw->cache_count++;
        rcw->interface_cache[idx].iid = iid_bytes;
        rcw->interface_cache[idx].interface_ptr = result;
        // result is already AddRef'd by QI — set refcount to 1 for the cache.
        rcw->interface_cache[idx].refcount = 1;
    }

    return result;
}

}  // namespace chaos::il2cpp::com_rcw
