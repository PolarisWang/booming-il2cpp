#ifndef CHAOS_IL2CPP_GC_HELPERS_H_
#define CHAOS_IL2CPP_GC_HELPERS_H_

#include <chaos/native_types.h>
#include <runtime_abi.h>

namespace chaos::il2cpp::runtime_core {

// Forward declaration — full definition in runtime_core.h.
enum class RuntimeMode;

/* GC allocation helpers for generated code. GcAllocate returns zeroed memory
   (scanned for pointers). GcAllocateAtomic returns zeroed memory that is NOT
   scanned for pointers — use for pointer-free data (e.g. string UTF-8 bytes). */
void* GcAllocate(CHAOS_IL2CPP_SIZE size);
void* GcAllocateAtomic(CHAOS_IL2CPP_SIZE size);
RuntimeStatus TypeQueryCapability(TypeInfoHandle type, RuntimeTypeCapabilityInfoV0* out_capability_info);
RuntimeMode GetRuntimeMode();
void SetRuntimeMode(RuntimeMode mode);
bool IsMixedMode();
void* BoxValueObject(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle value_type,
    const void* value,
    CHAOS_IL2CPP_SIZE value_size);
RuntimeStatus UnboxValueObject(
    RuntimeState* runtime_state,
    void* boxed_object,
    void* out_value,
    CHAOS_IL2CPP_SIZE out_value_size);
bool ArrayStoreReference(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR index,
    void* value);
void* ArrayLoadReference(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR index);
bool ArrayCopyReferenceRange(
    void* source_array_instance,
    CHAOS_IL2CPP_UINTPTR source_index,
    void* target_array_instance,
    CHAOS_IL2CPP_UINTPTR target_index,
    CHAOS_IL2CPP_UINTPTR length);
bool ArrayClearReferenceRange(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR start_index,
    CHAOS_IL2CPP_UINTPTR length);
bool ArrayReverseReferenceRange(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR start_index,
    CHAOS_IL2CPP_UINTPTR length);

/// Mark the object as changed (for incremental GC write barrier).
/// C3 stub: will be wired for incremental GC in later milestones.
inline void GcEndStubbornChange(const void* /*obj*/) noexcept {
    // No-op: CRAG write barrier uses card table.
    // C3+ will implement the actual write-barrier logic here.
}

/// String duplicate through the current TLS domain heap.
/// Falls back to std::malloc when no domain is active.
/// Returns nullptr on OOM or null input.
char* DomainStrDup(const char* src);

// ── GC collection API (extern "C" for codegen inline body) ─────────

/// Trigger a full blocking GC collection from managed code (System.GC.Collect()).
/// Runs young collection + full old-gen mark-sweep + pending finalizers.
extern "C" void chaos_gc_collect() noexcept;

/// Wait for pending finalizers (System.GC.WaitForPendingFinalizers()).
extern "C" void chaos_gc_wait_for_pending_finalizers() noexcept;

/// GC.KeepAlive — prevents the GC from collecting the object before
/// this call.  The generated code emits a direct call to this function
/// from codegen wrappers for System.GC.KeepAlive.  No-op in CRAG (no
/// precise concurrent GC), but serves as the observable side-effect
/// boundary that the JIT/GC uses for lifetime extension.
extern "C" void chaos_gc_keepalive(CHAOS_IL2CPP_INTPTR obj) noexcept;

/// Get the total allocated managed memory (System.GC.GetTotalMemory()).
/// Register external memory pressure (System.GC.AddMemoryPressure()).
/// Unregister external memory pressure (System.GC.RemoveMemoryPressure()).
// All three are now declared in gc_api.h with CHAOS_RUNTIME_ABI_CALL.

/// Write barrier: dirty the card table for @a obj (for generational GC).
/// Used by codegen stfld/stelem.ref/stobj to keep the card table consistent.
extern "C" void chaos_gc_dirty_card(const void* obj) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_HELPERS_H_
