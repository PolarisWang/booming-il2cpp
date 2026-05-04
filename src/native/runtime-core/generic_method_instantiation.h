#ifndef CHAOS_IL2CPP_GENERIC_METHOD_INSTANTIATION_H_
#define CHAOS_IL2CPP_GENERIC_METHOD_INSTANTIATION_H_

#include "il_to_ir_lowerer.h"  // ILTokenResolver, LowerILToIR
#include "reflection_query_model.h"
#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_instantiation {

// ── Runtime-instantiated method ─────────────────────────────────────────────

/// Heap-allocated descriptor for a MakeGenericMethod-created method.
/// Owns the ReflectionQueryMethodDescriptor + type argument storage.
/// Lifecycle: allocated on first miss, lives for process lifetime.
struct RuntimeInstantiatedMethod {
    /// Core method descriptor (heap-allocated, embedded).
    /// Must be first field so that the struct* doubles as descriptor* for
    /// EncodeReflectionQueryMethodHandle / TryDecodeReflectionQueryMethodHandle.
    ReflectionQueryMethodDescriptor descriptor;

    /// Type arguments used to close the generic method.
    TypeInfoHandle* type_args = nullptr;
    CHAOS_IL2CPP_UINT32  arg_count = 0u;

    /// The open method definition handle.
    MethodInfoHandle open_method_definition = 0u;

    /// Module ownership (0 = AOT root).
    CHAOS_IL2CPP_UINT32  module_id = 0u;

    /// True once registered in the GenericContextRegistry.
    bool is_registered = false;

    /// IL bytecodes of the open method definition (not owned; must stay alive
    /// for the process lifetime of this instantiation).
    const void*         il_bytes = nullptr;
    CHAOS_IL2CPP_SIZE   il_length = 0u;

    /// IR method body cache.  Populated on first call to LowerMethodBody().
    /// Heap-allocated interpreter::IRMethod; delete via ~IRMethod() + std::free.
    chaos::il2cpp::interpreter::IRMethod* ir_method_body = nullptr;
};

// ── Closed method descriptor construction ────────────────────────────────────

/// Build a heap-allocated RuntimeInstantiatedMethod for a closed generic method.
///
/// Copies the open method's ReflectionQueryMethodDescriptor, replaces
/// metadata_token with a runtime-allocated token, builds a closed subject_id,
/// and stores type_args for later lookup and IL body instantiation.
///
/// Returns nullptr if open_method cannot be decoded (not a reflection-query method).
RuntimeInstantiatedMethod* CreateClosedMethodDescriptor(
    MethodInfoHandle         open_method_definition,
    const TypeInfoHandle*    type_args,
    CHAOS_IL2CPP_UINT32     arg_count);

// ── Deferred IL→IR lowering ─────────────────────────────────────────────────

/// Lower the open method's IL bytecodes into an IRMethod for this closed
/// generic method instantiation.
///
/// The input buffer (`il_bytes` / `il_length`) is auto-detected as either:
///   - A raw ECMA-335 method body header (tiny or fat format) followed by
///     IL code and optional SEH sections, OR
///   - Raw IL code bytes alone (backward compat with callers that pre-strip
///     the header).
///
/// The method header is parsed to extract max_stack, code_size (for correct
/// SEH section location) and local_var_sig_tok.  If no valid header is
/// detected the input is treated as raw code with max_stack=8.
///
/// On success the result is cached in rt_method->ir_method_body.  Subsequent
/// calls are no-ops (the cache is returned immediately).
///
/// \param token_resolver  passed through to LowerILToIR; resolves metadata
///                        tokens in the context of the closed generic method
/// \param user_data       passed through to LowerILToIR; e.g. a pointer to
///                        the RuntimeInstantiatedMethod for token resolution
///
/// Returns true on success or if the body was already cached.
bool LowerMethodBody(
    RuntimeInstantiatedMethod*                      rt_method,
    const void*                                     il_bytes,
    CHAOS_IL2CPP_SIZE                               il_length,
    chaos::il2cpp::interpreter::ILTokenResolver      token_resolver,
    void*                                           user_data);

}  // namespace chaos::il2cpp::runtime_instantiation

#endif  // CHAOS_IL2CPP_GENERIC_METHOD_INSTANTIATION_H_
