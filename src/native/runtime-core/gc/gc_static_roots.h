#ifndef CHAOS_IL2CPP_GC_STATIC_ROOTS_H_
#define CHAOS_IL2CPP_GC_STATIC_ROOTS_H_

/// @file gc_static_roots.h
/// Explicit static root registration for ALC-isolated static fields.
///
/// AssemblyLoadContexts allocate per-assembly static field storage that
/// may contain object references.  These ranges must be registered as GC
/// roots so the collector finds them during mark/sweep.
///
/// == Lifecycle ==
/// - Register: called from AssemblyManager::LoadAssembly after allocating
///   static_field_ptr.
/// - Unregister: called from AssemblyManager::UnloadAssembly before freeing
///   the static field storage.
/// - Scan: called during GC root scanning (GcScanAllThreadRoots Phase 3).
///
/// == Thread safety ==
/// All operations use a mutex-protected vector.  Registration/Unregistration
/// happens during assembly load/unload (not on hot paths).  Scan happens
/// under a STW safepoint.

#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core {

/// Register a range of static field memory as a GC root.
/// @param start  Start address of the static field storage.
/// @param size   Size in bytes of the static field storage.
/// @param domain_id  Domain identifier (for unload tracking).
void GcRegisterStaticRootRange(void* start, CHAOS_IL2CPP_SIZE size,
                               CHAOS_IL2CPP_UINT32 domain_id) noexcept;

/// Unregister a previously registered static root range.
/// @param start  Start address (must match a previously registered range).
void GcUnregisterStaticRootRange(void* start) noexcept;

/// Unregister all static root ranges for a given domain_id.
/// Called during domain/ALC unload to bulk-remove all roots.
void GcUnregisterDomainStaticRoots(CHAOS_IL2CPP_UINT32 domain_id) noexcept;

/// Scan all registered static root ranges.
/// Calls @a callback for each pointer-aligned slot in each range.
/// @param callback  Root callback (same signature as GcScanAllThreadRoots).
/// @param user_data  Opaque user data forwarded to callback.
void GcScanStaticRoots(void (*callback)(void* root_addr, bool is_interior, void* user_data),
                       void* user_data) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_STATIC_ROOTS_H_
