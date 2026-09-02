#pragma once
// vtable_registry_internal.h — Shared internal state for vtable_registry
// and vtable_registry_resolve translation units.
//
// Extracted from the anonymous namespace of the original single-file
// vtable_registry.cpp.  Both TUs must observe THE SAME VTableRegistryState
// singleton (GetState()) and the same TCVC/IOC per-thread caches — so the
// state is moved here with inline linkage (single definition across TUs).
//
// NOT part of the public API.  Do not include from outside vtable_registry/.

#include <atomic>
#include <mutex>
#include <new>
#include <vector>

#include "vtable_registry.h"
#include "gc_region.h"
#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <codegen_bridge.h>

namespace chaos::il2cpp::vtable_registry {
namespace internal {

struct VTableRegistryState {
    CHAOS_IL2CPP_SHARED_MUTEX                                         mutex;
    // Primary key: stable_id → vtable
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(CHAOS_IL2CPP_UINT64, const TypeVTable*)       by_stable_id;
    // Secondary key: type_token → vtable
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(CHAOS_IL2CPP_UINT32, const TypeVTable*)       by_type_token;
    // Flat vtable arrays (for AOT codegen direct dispatch)
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(CHAOS_IL2CPP_UINT64, const void**)            flat_vtables;
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(CHAOS_IL2CPP_UINT64, CHAOS_IL2CPP_UINT32)     flat_lengths;
};

inline VTableRegistryState& GetState() {
    static VTableRegistryState s_state;
    return s_state;
}

// ── TCVC: Thread-local VTable Cache (Phase 9a, optimized Phase D-1) ──
//
// ResolveVirtualMethodPointer() is called on EVERY virtual dispatch in
// the interpreter slow path.  The TCVC caches recent resolutions in a
// small thread-local direct-mapped cache (single comparison), bypassing
// the shared_lock + linear scan on cache hit when the global epoch has
// not changed.
//
// Direct-mapped: slot index = key & kTcvcMask.  Single entry per hash
// bucket — no linear scan, no ring buffer.  Power-of-2 size ensures
// cheap mask operation.
//
// Global epoch is incremented by UpdateVTableSlotByMethodToken() when
// any slot is modified (method_replacement, hot-update revert).
//
// Cache entry layout:
//   key   = (instance_type_token << 32) | declared_method_token
//   value = resolved method_pointer
//   epoch = GetVTableEpoch() at time of insertion

inline constexpr CHAOS_IL2CPP_UINT32 kTcvcSize = 128;
inline constexpr CHAOS_IL2CPP_UINT32 kTcvcMask  = kTcvcSize - 1u;

struct TcvcEntry {
    CHAOS_IL2CPP_UINT64 key;
    void*               value;
    CHAOS_IL2CPP_UINT32 epoch;
};

struct TcvcState {
    TcvcEntry entries[kTcvcSize];
};

// Global epoch counter.  Incremented (with release ordering) after every
// successful slot update in UpdateVTableSlotByMethodToken.  Initialized
// to 1 so that zero-initialized cache entries (epoch = 0) never match.
// Function-local static to avoid CRT dynamic initializer ordering issues
// (MSVC 14.42+ C++17: std::atomic ctor is not constexpr for non-zero init).
inline std::atomic<CHAOS_IL2CPP_UINT32>& GetVTableEpoch() noexcept {
    static std::atomic<CHAOS_IL2CPP_UINT32> epoch{1u};
    return epoch;
}

// Per-thread cache state.  Zero-initialized on first access (all entries
// have key=0, epoch=0 — the first ResolveVirtualMethodPointer call will
// have a cache miss since GetVTableEpoch() starts at 1 != 0, so a miss
// populates the cache correctly).
inline thread_local TcvcState tls_tcvc{};

// ── IOC: Interface Offset Cache (Phase 9b+, optimized Phase D-1) ──
//
// Caches (type_token, iface_stable_id) -> vtable_offset lookups to avoid
// linear scanning iface_map entries on repeated interface dispatch.
//
// Separate from TCVC because IOC caches the intermediate interface-offset
// mapping, while TCVC caches the final method_pointer.  IOC entries use
// GetIfaceEpoch() which is bumped only by interface additions (not by
// regular slot updates), so a runtime_iface_map append invalidates IOC
// without invalidating the TCVC for non-interface methods.
//
// Direct-mapped: slot index = key & kIocMask.  Single entry per hash
// bucket — no linear scan, no ring buffer.

inline constexpr CHAOS_IL2CPP_UINT32 kIocSize = 64;
inline constexpr CHAOS_IL2CPP_UINT32 kIocMask  = kIocSize - 1u;

struct IocEntry {
    CHAOS_IL2CPP_UINT64 key;       // (type_token << 32) | (iface_stable_id & 0xFFFFFFFF)
    CHAOS_IL2CPP_UINT32 vtable_offset;
    CHAOS_IL2CPP_UINT32 method_count;
    CHAOS_IL2CPP_UINT32 epoch;     // GetIfaceEpoch() at insertion time
};

struct IocState {
    IocEntry entries[kIocSize];
};

// Global epoch counter for interface-only changes.  Bumped (with release
// ordering) after every successful RegisterTypeVTableRuntimeInterface call.
// Initialized to 1 so zero-initialized cache entries never match.
// Function-local static to avoid CRT dynamic initializer ordering issues.
inline std::atomic<CHAOS_IL2CPP_UINT32>& GetIfaceEpoch() noexcept {
    static std::atomic<CHAOS_IL2CPP_UINT32> epoch{1u};
    return epoch;
}

// Per-thread IOC state.  Zero-initialized on first access.
inline thread_local IocState tls_ioc{};

// (ScanIfaceMapForMethod is defined in vtable_registry_resolve.cpp at
// vtable_registry:: namespace scope, not needed by the registration TU.
// No declaration here to avoid ambiguity at call sites.)

}  // namespace internal
}  // namespace chaos::il2cpp::vtable_registry
