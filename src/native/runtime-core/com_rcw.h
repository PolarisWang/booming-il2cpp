#ifndef CHAOS_IL2CPP_COM_RCW_H_
#define CHAOS_IL2CPP_COM_RCW_H_

#include <cstdint>
#include <chaos/native_types.h>
#include <chaos/collection.h>
#include <chaos/unordered_dense.h>
#include "com_abi.h"

namespace chaos::il2cpp::com_rcw {

// ── Constants ──────────────────────────────────────────────────────

/// Magic number for ComRcwNative header, used by IsComRcwHandle to
/// distinguish RCW handles from raw COM pointers at runtime.
constexpr uint32_t kComRcwMagic = 0x52435721u;  // "RCW!"

/// Maximum number of cached interface pointers per RCW.
/// Game engine scenario: typically 1-3 interfaces per object.
constexpr CHAOS_IL2CPP_SIZE kMaxInterfaceCache = 8;

// ── Data structures ────────────────────────────────────────────────

/// A single cached QueryInterface result.
struct InterfaceCacheEntry {
    const void* iid;           // Pointer to static IID bytes (16-byte GUID).
    void* interface_ptr;       // Cached interface pointer (already AddRef'd).
    CHAOS_IL2CPP_UINT32 refcount;  // Additional refs from managed wrappers.
};

/// Native RCW object — wraps a single COM IUnknown identity.
/// Allocated via domain heap (not GC heap). Lifetime managed by
/// reference count from managed SafeHandle wrappers.
struct ComRcwNative {
    CHAOS_IL2CPP_UINT32 magic;                    // = kComRcwMagic
    void* identity_unknown;                       // Canonical IUnknown* for identity.
    CHAOS_IL2CPP_UINT32 wrapper_refcount;         // Managed wrapper reference count.
    CHAOS_IL2CPP_UINT32 cache_count;              // Valid entries in interface_cache.
    InterfaceCacheEntry interface_cache[kMaxInterfaceCache];
};

// ── Runtime detection ──────────────────────────────────────────────

/// Check whether a pointer is a ComRcwNative (by magic value).
inline bool IsComRcwHandle(CHAOS_IL2CPP_INTPTR ptr) noexcept {
    if (ptr == 0) return false;
    const auto* magic = static_cast<const CHAOS_IL2CPP_UINT32*>(
        reinterpret_cast<const void*>(static_cast<uintptr_t>(ptr)));
    return *magic == kComRcwMagic;
}

// ── RCW lifecycle functions ────────────────────────────────────────

/// Find an existing RCW for the given IUnknown*, or create a new one.
/// Calls unknown->AddRef() on first creation.
/// Thread-safe (global table is mutex-protected).
/// Returns nullptr on allocation failure.
ComRcwNative* FindOrCreateRcw(void* unknown_ptr) noexcept;

/// Release a managed wrapper's reference on an RCW.
/// When wrapper_refcount reaches zero, releases all cached interface
/// pointers, releases identity_unknown, and frees the RCW.
void ReleaseRcw(ComRcwNative* rcw) noexcept;

/// Look up or query a specific interface on an RCW.
/// Checks the per-RCW cache first, then falls back to QueryInterface.
/// Returns the interface pointer (already AddRef'd) or nullptr.
void* QueryInterfaceCached(ComRcwNative* rcw, const void* iid_bytes) noexcept;

}  // namespace chaos::il2cpp::com_rcw

#endif  // CHAOS_IL2CPP_COM_RCW_H_
