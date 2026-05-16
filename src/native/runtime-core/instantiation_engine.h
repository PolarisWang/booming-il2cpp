#ifndef CHAOS_IL2CPP_RUNTIME_CORE_RUNTIME_INSTANTIATION_H_
#define CHAOS_IL2CPP_RUNTIME_CORE_RUNTIME_INSTANTIATION_H_

#include <runtime_abi.h>
#include "reflection_query_model.h"
#include <runtime_instantiation.h>
#include <interpreter_vm.h>

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <vector>

namespace chaos::il2cpp::runtime_instantiation {

// ── Interpreter dispatch context ──────────────────────────────────

/// Maximum interpreter dispatch recursion depth.
constexpr int kInterpreterMaxDispatchDepth = 200;

/// Per-call dispatch context passed to InterpreterDispatch callback.
struct InterpreterDispatchContext {
    RuntimeState* runtime_state   = nullptr;
    ThreadState*  thread_state    = nullptr;
    int           recursion_depth = 0;
};

/// DispatchCallback implementation for InterpreterVM::Execute.
/// Routes Call/CallVirt/CallBridge through MethodInvoke (AOT or interpreted).
/// Declared here so InterpreterEntryDirect (interpreter_entry.cpp) can also
/// wire it up as the dispatch_fn for patched-method execution frames.
interpreter::DispatchResult InterpreterDispatch(
    void*                               call_target,
    const interpreter::InterpreterValue* call_args,
    CHAOS_IL2CPP_UINT32                 arg_count,
    bool                                is_instance_call,
    void*                               dispatch_context);

/// Result of a raw dispatch (no InterpreterValue round-trip).
struct RawDispatchResult {
    bool    has_value       = false;
    uint64_t value           = 0;
    uint8_t tag             = 0;        // ValueTag
    bool    threw_exception = false;
    void*   exception_obj   = nullptr;
    void*   struct_data     = nullptr;  // heap-allocated struct return (caller frees)
    uint32_t struct_size    = 0;
};

/// Pre-computed metadata for a single Call instruction's target.
/// Populated during IR lowering to avoid runtime reflection queries.
struct CachedCallInfo {
    uint8_t  ret_tag          = 0xFF;   // ValueTag for return type (0xFF = uninit)
    bool     is_struct_ret    = false;  // true = struct (value type) return
    uint32_t struct_size      = 0;      // pre-computed struct buffer size (0 = scalar)

    // A2: direct native function pointer for same-module calls.
    // When non-null and !is_patched, Handle_Call can call direct_ptr directly
    // instead of going through method_invoke (save ~1500ns).
    // direct_ptr is nullptr for cross-module calls (must use method_invoke).
    void*    direct_ptr       = nullptr;  // AOT native function (void(*)())
    bool     is_patched       = false;    // true = method has active patch
    uint32_t module_id        = 0;        // module owning the target method
    uint32_t slot             = 0;        // dispatch table slot index

    // ── CallVirt MIC (Monomorphic Inline Cache) ────────────────────────────
    // Runtime-populated fields for virtual call optimization.  Handle_CallVirt
    // caches the vtable-resolved function pointer + receiver type_token so
    // monomorphic call sites (same receiver type repeated) skip the ~2200ns
    // vtable walk.  Updated under benign races (all racers compute same value).
    std::atomic<void*>   mic_dispatch_ptr {nullptr};  // cached vtable-resolved fn ptr
    std::atomic<uint32_t> mic_type_token   {0};        // cached receiver type_token
    std::atomic<uint64_t> mic_generation   {0};        // g_patch_generation at cache-fill time

    // PrecacheCallTarget returns by value; std::atomic members delete the
    // implicit copy ctor, so we provide an explicit one.
    CachedCallInfo() = default;
    CachedCallInfo(const CachedCallInfo& other) noexcept
        : ret_tag(other.ret_tag)
        , is_struct_ret(other.is_struct_ret)
        , struct_size(other.struct_size)
        , direct_ptr(other.direct_ptr)
        , is_patched(other.is_patched)
        , module_id(other.module_id)
        , slot(other.slot)
        , mic_dispatch_ptr(other.mic_dispatch_ptr.load(std::memory_order_relaxed))
        , mic_type_token(other.mic_type_token.load(std::memory_order_relaxed))
        , mic_generation(other.mic_generation.load(std::memory_order_relaxed))
    {}
    CachedCallInfo(CachedCallInfo&& other) noexcept
        : ret_tag(other.ret_tag)
        , is_struct_ret(other.is_struct_ret)
        , struct_size(other.struct_size)
        , direct_ptr(other.direct_ptr)
        , is_patched(other.is_patched)
        , module_id(other.module_id)
        , slot(other.slot)
        , mic_dispatch_ptr(other.mic_dispatch_ptr.load(std::memory_order_relaxed))
        , mic_type_token(other.mic_type_token.load(std::memory_order_relaxed))
        , mic_generation(other.mic_generation.load(std::memory_order_relaxed))
    {
        other.ret_tag = 0xFF;
        other.direct_ptr = nullptr;
        other.mic_dispatch_ptr.store(nullptr, std::memory_order_relaxed);
        other.mic_type_token.store(0, std::memory_order_relaxed);
        other.mic_generation.store(0, std::memory_order_relaxed);
    }
    CachedCallInfo& operator=(const CachedCallInfo& other) noexcept
    {
        ret_tag = other.ret_tag;
        is_struct_ret = other.is_struct_ret;
        struct_size = other.struct_size;
        direct_ptr = other.direct_ptr;
        is_patched = other.is_patched;
        module_id = other.module_id;
        slot = other.slot;
        mic_dispatch_ptr.store(other.mic_dispatch_ptr.load(std::memory_order_relaxed), std::memory_order_relaxed);
        mic_type_token.store(other.mic_type_token.load(std::memory_order_relaxed), std::memory_order_relaxed);
        mic_generation.store(other.mic_generation.load(std::memory_order_relaxed), std::memory_order_relaxed);
        return *this;
    }
};

/// Pre-compute call metadata for a single call_target (MethodInfoHandle).
/// Resolves ret_tag, struct-ness, and struct size so InterpreterDispatchRaw
/// can skip all reflection queries at runtime.
CachedCallInfo PrecacheCallTarget(void* call_target) noexcept;

/// Dispatch directly from raw uint64_t values + ValueTag tags, skipping the
/// InterpreterValue round-trip.  Args are in call order (arg[0] = first arg).
/// Used by FastFrame::Handle_Call (fast_dispatch.cpp) to inline the call dispatch.
RawDispatchResult InterpreterDispatchRaw(
    void*                               call_target,
    const uint64_t*                     raw_args,
    const uint8_t*                      arg_tags,
    CHAOS_IL2CPP_UINT32                 arg_count,
    bool                                is_instance_call,
    void*                               dispatch_context,
    const CachedCallInfo*               cache_info = nullptr);

// ── Runtime-instantiated type ─────────────────────────────────────────────

/// Heap-allocated descriptor for a MakeGenericType-created type.
/// Owns the ReflectionQueryTypeDescriptor + type argument storage.
/// Lifecycle: allocated on first miss, lives for process lifetime.
struct RuntimeInstantiatedType {
    /// Core type descriptor (heap-allocated, embedded).
    chaos::il2cpp::runtime_core::ReflectionQueryTypeDescriptor descriptor;

    /// Type arguments used to close the generic type.
    TypeInfoHandle* type_args = nullptr;
    CHAOS_IL2CPP_UINT32  arg_count = 0u;

    /// Value type layout (valid when the closed type is a value type).
    CHAOS_IL2CPP_UINT32  value_size = 0u;
    CHAOS_IL2CPP_UINT32* field_offsets = nullptr;
    CHAOS_IL2CPP_UINT32  field_offset_count = 0u;

    /// Resolved field type handles (heap-allocated, optional cache).
    /// Phase 3: each field's resolved TypeInfoHandle after generic parameter
    /// substitution.  Populated by the LayoutEngine during ComputeValueTypeLayout.
    TypeInfoHandle*       resolved_field_types = nullptr;
    CHAOS_IL2CPP_UINT32  resolved_field_count = 0u;

    /// Module ownership (0 = AOT root).
    CHAOS_IL2CPP_UINT32  module_id = 0u;

    /// True once registered in the GenericContextRegistry.
    bool is_registered = false;
};

// ── Token management ──────────────────────────────────────────────────────

/// Allocate a unique runtime metadata token (0x80000000 and above).
/// These tokens never collide with AOT-allocated tokens.
CHAOS_IL2CPP_UINT32 AllocateRuntimeToken();

// ── Closed type descriptor construction ────────────────────────────────────

/// Build a heap-allocated RuntimeInstantiatedType for a closed generic type.
///
/// Copies the open type's ReflectionQueryTypeDescriptor, replaces
/// metadata_token with a runtime-allocated token, sets generic_type_definition,
/// and stores type_args for later lookup and value-type layout computation.
///
/// Returns nullptr if open_type cannot be decoded (not a reflection-query type).
RuntimeInstantiatedType* BuildClosedDescriptor(
    TypeInfoHandle         open_type_definition,
    const TypeInfoHandle*  type_args,
    CHAOS_IL2CPP_UINT32   arg_count);

/// Build a subject_id string for the closed type: "OpenType[arg1,arg2,...]"
/// The returned buffer is heap-allocated; caller takes ownership.
char* BuildClosedSubjectId(
    const chaos::il2cpp::runtime_core::ReflectionQueryTypeDescriptor* open_desc,
    const TypeInfoHandle*                type_args,
    CHAOS_IL2CPP_UINT32                  arg_count);

// ── Value type layout computation ──────────────────────────────────────────

/// Compute value_size and field_offsets for a closed value type.
/// Uses the open type's field descriptors with type argument substitution.
void ComputeValueTypeLayout(RuntimeInstantiatedType* rt_type);

// ── Bridge access ──────────────────────────────────────────────────────────

/// Returns the process-wide RuntimeInstantiationBridgeV0 instance.
/// The struct is defined in contracts/native/v0/runtime_instantiation.h
/// (typedef struct inside extern "C", at global scope).  No local forward
/// declaration needed -- the include above provides the full definition.
const RuntimeInstantiationBridgeV0* GetBridgeV0();

// ── AOT method map (generic method promotion) ────────────────────────────

/// Register per-module AOT method entries for runtime query.
/// Stores raw pointers to the codegen-emitted static arrays (no copy).
/// @param module_id   The module owning these entries.
/// @param entries     Pointer to GenericMethodAotEntryV0[] (must live for
///                    process lifetime — emitted as static const C++ array).
/// @param count       Number of entries.
/// @param args        Flat type-arg token pool (uint32_t[]).
/// @param arg_count   Number of tokens in the args pool.
void RegisterMethodAotEntries(
    CHAOS_IL2CPP_UINT32                module_id,
    const GenericMethodAotEntryV0*     entries,
    CHAOS_IL2CPP_UINT32                count,
    const uint32_t*                    args,
    CHAOS_IL2CPP_UINT32                arg_count);

/// Query the per-module AOT method map for a pre-compiled generic method.
///
/// Per-module binary search by open_token, then token-based type_args scan.
/// Type argument matching compares GetTypeToken(type_args[i]) against the
/// stored metadata tokens from the AOT entries' flat token pool.  This works
/// correctly when the type arguments belong to the same module as the generic
/// method definition (the common case).  Cross-module type arguments may
/// produce false negatives (graceful interpreter fallback).
///
/// @param module_id          Module owning the entries.
/// @param open_method_token  Metadata token of the open generic method.
/// @param type_args          Concrete type arguments (TypeInfoHandle[]).
/// @param arg_count          Number of type arguments.
/// @return Token-based MethodInfoHandle suitable for MethodInvoke dispatch,
///         or 0u if not found in the AOT map.
MethodInfoHandle QueryAotMethod(
    CHAOS_IL2CPP_UINT32                module_id,
    CHAOS_IL2CPP_UINT32                open_method_token,
    const TypeInfoHandle*              type_args,
    CHAOS_IL2CPP_UINT32                arg_count);

}  // namespace chaos::il2cpp::runtime_instantiation

#endif  // CHAOS_IL2CPP_RUNTIME_CORE_RUNTIME_INSTANTIATION_H_
