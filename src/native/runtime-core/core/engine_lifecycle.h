#ifndef CHAOS_IL2CPP_ENGINE_LIFECYCLE_H_
#define CHAOS_IL2CPP_ENGINE_LIFECYCLE_H_

#include <chaos/native_types.h>
#include <runtime_abi.h>

namespace chaos::il2cpp::runtime_core {

// Lifecycle callback type used by engine lifecycle registrations.
using EngineLifecycleCallback = CHAOS_IL2CPP_INT32(CHAOS_RUNTIME_ABI_CALL*)(
    const char* phase_utf8, void* user_data);

struct EngineLifecycleRegistration {
    CHAOS_IL2CPP_STRING phase;
    EngineLifecycleCallback callback;
    void* user_data;
};

// GC handle table entry (defined in engine_lifecycle.cpp, referenced by
// abi_reflection.cpp and GC scan code at runtime_core namespace scope).
struct GcHandleEntry {
    void* object_instance;
    bool pinned;
    bool weak;
    bool track_resurrection;  // WeakTrackResurrection: defer nullification
                              // until after finalization can resurrect
    bool async_pinned;        // AsyncPinned: strong + pinned for async ops
    bool points_to_nursery;   // Cached: object_instance's region is the nursery.
                              // Maintained by GcSetHandleTarget / creation /
                              // weak forwarding so generation-aware scans can
                              // prune WITHOUT re-querying RegionManager.
};

// ABI constants at runtime_core namespace scope (shared across core files).
extern const char* kEngineObservePrefix;

// Engine-global state.
extern CHAOS_IL2CPP_MUTEX g_engine_binding_mutex;
extern CHAOS_IL2CPP_UINTPTR g_next_engine_handle;
extern CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINTPTR, void*) g_engine_handles;
extern CHAOS_IL2CPP_VECTOR(EngineLifecycleRegistration) g_engine_lifecycle_registrations;
extern const CHAOS_IL2CPP_THREAD::id g_main_thread_id;

// GC handle state (sharded implementation in engine_lifecycle.cpp).
// Use GcCreateStrongHandle / GcFreeHandle / etc. for thread-safe access.
// Direct extern access to internals is deprecated — update callers to use the API.

// ── Internal handle API (no RuntimeState required, for tests / internal use) ──

/// Create a strong handle (object will be kept alive by GC).
/// Returns a nonzero handle ID on success, 0 on failure.
CHAOS_IL2CPP_UINT64 GcCreateStrongHandle(void* object_instance) noexcept;

/// Create a weak handle (object can be collected; handle nulled when dead).
/// Returns a nonzero handle ID on success, 0 on failure.
CHAOS_IL2CPP_UINT64 GcCreateWeakHandle(void* object_instance) noexcept;

/// Create a long weak handle (WeakTrackResurrection): the handle is NOT
/// nullified until the BGC cycle AFTER finalization has run, giving
/// finalizers a chance to resurrect the object.  Returns a nonzero handle
/// ID on success, 0 on failure.
CHAOS_IL2CPP_UINT64 GcCreateLongWeakHandle(void* object_instance) noexcept;

/// Create a pinned handle (object will not be moved by young GC).
/// Returns a nonzero handle ID on success, 0 on failure.
CHAOS_IL2CPP_UINT64 GcCreatePinnedHandle(void* object_instance) noexcept;

/// Create an async pinned handle (strong + pinned, for async operations).
/// Semantically equivalent to GCHandleType::AsyncPinned: the referenced
/// object is kept alive and will not be moved.  Returns a nonzero handle
/// ID on success, 0 on failure.
CHAOS_IL2CPP_UINT64 GcCreateAsyncPinnedHandle(void* object_instance) noexcept;

/// Free a handle created by any of the above.
void GcFreeHandle(CHAOS_IL2CPP_UINT64 handle_id) noexcept;

/// Read the target object of any handle type (strong, weak, pinned).
/// Returns nullptr if handle_id is invalid or the target was collected.
void* GcGetHandleTarget(CHAOS_IL2CPP_UINT64 handle_id) noexcept;

/// Overwrite the target object of any handle type.
void GcSetHandleTarget(CHAOS_IL2CPP_UINT64 handle_id, void* new_target) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_ENGINE_LIFECYCLE_H_
