// ── T2.1: managed-object ↔ native-handle binding for threading primitives ──
//
// THE PROBLEM
// -----------
// The managed primitives are INSTANCE types whose state lives behind a native
// handle: `SemaphoreSlim.Wait()` is an instance method (receiver + timeout +
// CancellationToken), while `SemaphoreSlimWait(sem_id, timeout)` is a plain
// handle function over `g_semaphores[1024]`.  Something has to store the
// mapping from the managed instance to that slot id, and something has to
// decide when the slot is released.
//
// WHY THIS IS NOT A NEW MECHANISM (contra the original roadmap)
// -------------------------------------------------------------
// The roadmap called this "新机制，无先例".  That is wrong, and the correction
// matters because inventing infrastructure here would be gratuitous.  Four
// precedents already exist:
//
//   * wait_handle.cpp      — unordered_map<uint32_t, unique_ptr<...>> under a
//                            std::shared_mutex; monotonic ids, `active` flag
//   * synchronization.cpp  — fixed arrays g_semaphores/g_rwlocks/g_barriers/
//                            g_countdown_events[1024]; handle == array index
//   * engine_binding.cpp   — CreateEngineObjectHandle / ResolveEngineObjectHandle
//   * synchronization_context.cpp — `chaos_synchronization_context_create`
//                            returns a native pointer that managed code then
//                            passes back through get_Current/set_Current
//
// The LAST one is the shape this file follows, because it is the only existing
// precedent for *managed object identity held natively*: the native create call
// hands out an opaque INTPTR, the managed instance carries it, and every
// instance method passes it back.  No side table keyed by object address is
// needed, and none is added here.
//
// WHERE THE HANDLE LIVES — AND WHY THAT IS THE WHOLE DESIGN POINT
// ---------------------------------------------------------------
// The handle is stored in a field ON THE MANAGED OBJECT itself, and this file
// reads and writes it at a FIXED OFFSET from the receiver pointer.  That is the
// same technique the registry already uses for `Nullable<T>::get_HasValue`
// (see RuntimeHelperShapeRegistry.CoreStubs.Part1.S17.cs: `*reinterpret_cast<
// CHAOS_IL2CPP_INT32*>(chaos_arg_0)`) and for the inline `List<T>` view in
// generated_code_compat.h (`chaos_list_fields`, "embedded in the GC object right
// after ThinLockableHeader (offset 8)").
//
// The offset is `sizeof(ThinLockableHeader)`, i.e. 8 on this runtime: the header
// is TypeInfo* at [0] plus a removed sync_state (see generated_code_compat.h).
//
// ── The GC question, answered ──
// Roadmap's stated risk for T2.1 was "托管对象被 GC 回收后，native 槽位由谁释放".
// Storing the handle IN the object is what makes that answerable: the object
// owns its handle the way it owns any other field, so the release point is the
// object's own teardown — `Dispose()` / `Close()` / finalization — not a
// separate mapping table that would need its own liveness reasoning and its own
// way to be told an object died.  A side table keyed by object address would
// additionally be UNSOUND under a moving/compacting collector, since the key
// would change under it.  An offset read has no such problem.
//
// ── Which of the two fields this reads, and the trap in it ──
// The handle field is read through a helper that DISAMBIGUATES "field is 0
// because the object was never initialized" from "field is 0 because it was
// disposed".  Both are 0.  They are not the same state: calling into a
// never-initialized instance should create the native object on demand, while
// calling into a disposed one must fail rather than silently resurrect it.
// `NativeHandleSlot` is that disambiguation point; see its comment below.

#pragma once

#include <chaos/native_types.h>

extern "C" {

// ══════════════════════════════════════════════════════════════════════
// Handle storage on the managed instance
// ══════════════════════════════════════════════════════════════════════

/// Bind a native handle to a managed instance by storing it in the instance's
/// handle field.  Returns the handle for convenience.
///
/// `instance` is the managed `this` pointer.  A null instance is rejected
/// (returns 0) rather than writing through it — the offset write would
/// otherwise be an arbitrary small-address store.
CHAOS_IL2CPP_INTPTR ChaosManagedHandleStore(CHAOS_IL2CPP_INTPTR instance,
                                            CHAOS_IL2CPP_INTPTR handle) noexcept;

/// Read the handle bound to a managed instance.  Returns 0 when unbound.
CHAOS_IL2CPP_INTPTR ChaosManagedHandleLoad(CHAOS_IL2CPP_INTPTR instance) noexcept;

/// Release the binding: reads the handle, clears the field, and calls the
/// supplied native destroy function on it.
///
/// Clearing BEFORE destroying is deliberate.  A concurrent call that observes
/// the handle between the two steps would find 0 and fail cleanly, whereas
/// clearing after would let it observe an id that is about to die — the
/// use-after-release window that `wait_handle.cpp`'s `active` flag also exists
/// to close.
///
/// Idempotent: releasing twice is a no-op the second time (field already 0),
/// because `destroy` is only invoked for a nonzero handle.
///
/// Returns 1 if a handle was released, 0 if there was nothing to release.
CHAOS_IL2CPP_INT32 ChaosManagedHandleRelease(CHAOS_IL2CPP_INTPTR instance,
                                             CHAOS_IL2CPP_INTPTR destroy_fn) noexcept;

// ══════════════════════════════════════════════════════════════════════
// Lazy binding — create-on-first-use
// ══════════════════════════════════════════════════════════════════════

/// Return the instance's handle, creating it via `create_fn` if unbound.
///
/// WHY THIS EXISTS.  A managed `SemaphoreSlim` has two constructors — the
/// parameterless one and `(int initialCount, int maxCount)` — and only the
/// second knows the counts.  If the handle were created eagerly in `.ctor()`,
/// the parameterless ctor could not create it (no arguments to pass) and would
/// have to create a *wrong* one.  Creating on first use instead means the
/// arguments are whatever the first real operation supplies, and the
/// parameterless ctor simply leaves the field 0.
///
/// `create_fn` is `CHAOS_IL2CPP_INTPTR (*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)`
/// — (a, b) supplied by the caller so this one entry point serves every
/// primitive rather than one per type.
///
/// Returns the bound handle (nonzero) on success, or 0 if creation failed.
CHAOS_IL2CPP_INTPTR ChaosManagedHandleGetOrCreate(CHAOS_IL2CPP_INTPTR instance,
                                                  CHAOS_IL2CPP_INTPTR create_fn,
                                                  CHAOS_IL2CPP_INTPTR arg0,
                                                  CHAOS_IL2CPP_INTPTR arg1) noexcept;

}  // extern "C"
