#ifndef CHAOS_IL2CPP_GC_HELPERS_H_
#define CHAOS_IL2CPP_GC_HELPERS_H_

#include <chaos/native_types.h>
#include <runtime_abi.h>

namespace chaos::il2cpp::runtime_core {

// Forward declaration — full definition in runtime_core.h.
enum class RuntimeMode;

/* GC allocation helpers for generated code. GcAllocate returns zeroed memory
   (scanned for pointers). GcAllocateAtomic returns zeroed memory that is NOT
   scanned for pointers — use for pointer-free data (e.g. string UTF-8 bytes).

   Fast path (generated code, CHAOS_IL2CPP_NEW_GC macro):
     GcAllocateFast / GcAllocateAtomicFast — __forceinline in gc_alloc_stubs.h,
     no PROFILE_SCOPE, no global atomics, ~30ns/alloc (SHIP config).

   Profiled path (non-hot-path callers, diagnostic use):
     GcAllocateProfiled / GcAllocateAtomicProfiled — out-of-line in
     gc_alloc_stubs.cpp, retains PROFILE_SCOPE + global atomic stats. */
void* GcAllocateFast(CHAOS_IL2CPP_SIZE size);
void* GcAllocateAtomicFast(CHAOS_IL2CPP_SIZE size);
void* GcAllocateProfiled(CHAOS_IL2CPP_SIZE size);
void* GcAllocateAtomicProfiled(CHAOS_IL2CPP_SIZE size);

// Legacy aliases for separately-compiled runtime stubs (string_stubs, etc.)
// that call GcAllocate/GcAllocateAtomic by name.  Resolved to thin wrappers
// in gc_alloc_stubs.cpp that delegate to the profiled variants.
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

/// Register a newly allocated object as needing finalization.
/// Called from generated AOT code when creating an object whose type has a finalizer.
/// The object's type_info provides the finalizer callback at finalization time.
void chaos_gc_register_finalizable(void* obj) noexcept;

}  // namespace chaos::il2cpp::runtime_core

// GC collection declarations (extern "C" at file scope for codegen call sites).
// These are also declared in generated_code_compat.h for native AOT TUs.
extern "C" void chaos_gc_collect() noexcept;
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

/// generation-aware write barrier.  Given both the destination (@a dst)
/// and the stored reference (@a ref), only marks the card when the write is
/// a true old→young cross-generation reference (dst non-gen0 AND ref.gen <
/// dst.gen).  gen0→gen0 and same-mature writes skip the card, saving the hot
/// path.  Faithful to CoreCLR's region write-barrier short-circuit.  Emitted
/// by codegen at stfld / stelem.ref / stobj where the stored ref is available.
extern "C" void chaos_gc_dirty_card_dst_ref(const void* dst, const void* ref) noexcept;

/// Suppress finalization for @a obj (System.GC.SuppressFinalize).
/// Called from managed code when an object's Dispose() has run.
extern "C" void chaos_gc_suppress_finalize(CHAOS_IL2CPP_INTPTR obj) noexcept;

/// Re-register finalization for @a obj (System.GC.ReRegisterForFinalize).
/// Reverses a previous SuppressFinalize call.
extern "C" void chaos_gc_reregister_finalize(CHAOS_IL2CPP_INTPTR obj) noexcept;

#endif  // CHAOS_IL2CPP_GC_HELPERS_H_
