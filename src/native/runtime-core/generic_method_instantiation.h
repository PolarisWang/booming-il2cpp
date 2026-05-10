#ifndef CHAOS_IL2CPP_GENERIC_METHOD_INSTANTIATION_H_
#define CHAOS_IL2CPP_GENERIC_METHOD_INSTANTIATION_H_

#include "aot_core_ir_reader.h"  // DeserializeAotCoreIrMethod, ResolveSubjectIdFn
#include "reflection_query_model.h"
#include "codegen_bridge.h"     // CodegenBridgeV0, ImageHandle
#include <chaos/native_types.h>

namespace chaos::il2cpp::layout { class LayoutEngine; }

namespace chaos::il2cpp::runtime_instantiation {

// ── Runtime-instantiated method ─────────────────────────────────────────────

/// Heap-allocated descriptor for a MakeGenericMethod-created method.
/// Owns the ReflectionQueryMethodDescriptor + type argument storage.
/// Lifecycle: allocated on first miss, lives for process lifetime.
struct RuntimeInstantiatedMethod {
    /// Core method descriptor (heap-allocated, embedded).
    /// Must be first field so that the struct* doubles as descriptor* for
    /// EncodeReflectionQueryMethodHandle / TryDecodeReflectionQueryMethodHandle.
    chaos::il2cpp::runtime_core::ReflectionQueryMethodDescriptor descriptor;

    /// Type arguments used to close the generic method.
    TypeInfoHandle* type_args = nullptr;
    CHAOS_IL2CPP_UINT32  arg_count = 0u;

    /// The open method definition handle.
    MethodInfoHandle open_method_definition = 0u;

    /// Module ownership (0 = AOT root).
    CHAOS_IL2CPP_UINT32  module_id = 0u;

    /// True once registered in the GenericContextRegistry.
    bool is_registered = false;

    /// Set to true by UnregisterModuleGenerics when the owning module is unloaded.
    /// The interpret_method_call bridge checks this flag before executing.
    bool is_unloaded = false;

    /// AotCoreIr JSON of the open method definition (not owned; must stay alive
    /// for the process lifetime of this instantiation).
    const char*         aot_core_ir_json = nullptr;
    CHAOS_IL2CPP_SIZE   aot_core_ir_json_length = 0u;

    /// IR method body cache.  Populated on first call to LowerMethodBody().
    /// Heap-allocated interpreter::IRMethod; delete via ~IRMethod() + std::free.
    chaos::il2cpp::interpreter::IRMethod* ir_method_body = nullptr;

    /// Process-wide codegen bridge for metadata token resolution.
    const CodegenBridgeV0* bridge = nullptr;

    /// Source image handle for token resolution (assembly/module context).
    ImageHandle source_image = 0u;

    /// Layout engine for field offset resolution (optional, may be nullptr).
    chaos::il2cpp::layout::LayoutEngine* layout_engine = nullptr;
};

// ── Closed method descriptor construction ────────────────────────────────────

/// Build a heap-allocated RuntimeInstantiatedMethod for a closed generic method.
RuntimeInstantiatedMethod* CreateClosedMethodDescriptor(
    MethodInfoHandle         open_method_definition,
    const TypeInfoHandle*    type_args,
    CHAOS_IL2CPP_UINT32     arg_count);

// ── Deferred AotCoreIr JSON → IR deserialization ────────────────────────────

/// Deserialize the open method's AotCoreIr JSON into an IRMethod for this closed
/// generic method instantiation.
///
/// On success the result is cached in rt_method->ir_method_body.  Subsequent
/// calls are no-ops (the cache is returned immediately).
///
/// \param resolve_fn   callback to resolve subject IDs to call_target pointers
/// \param resolve_ctx  passed through to resolve_fn
///
/// Returns true on success or if the body was already cached.
bool LowerMethodBody(
    RuntimeInstantiatedMethod*                      rt_method,
    const char*                                     aot_core_ir_json,
    CHAOS_IL2CPP_SIZE                               aot_core_ir_json_length,
    chaos::il2cpp::runtime_core::ResolveSubjectIdFn  resolve_fn,
    void*                                           resolve_ctx);

}  // namespace chaos::il2cpp::runtime_instantiation

#endif  // CHAOS_IL2CPP_GENERIC_METHOD_INSTANTIATION_H_