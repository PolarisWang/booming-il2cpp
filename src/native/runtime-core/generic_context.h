#ifndef CHAOS_IL2CPP_GENERIC_CONTEXT_H_
#define CHAOS_IL2CPP_GENERIC_CONTEXT_H_

#include "runtime_abi.h"
#include <chaos/native_types.h>

/* Forward declaration from codegen_bridge.h */
struct ModuleGenericRegistrationV0;

namespace chaos::il2cpp::generic_context {

// ── Handle-based generic type instantiation registration ──

/// Register a closed generic type instantiation.
/// `open_type`   — handle of the open generic definition (e.g. List<>)
/// `closed_type` — handle of the closed instantiation (e.g. List<int>)
/// `type_args`   — array of type argument handles
/// `arg_count`   — length of type_args
/// Thread-safe, idempotent (duplicate closed_type is a no-op).
void RegisterGenericInstantiation(
    TypeInfoHandle open_type,
    TypeInfoHandle closed_type,
    const TypeInfoHandle* type_args,
    CHAOS_IL2CPP_UINT32 arg_count);

// ── Method generic context ──

/// Look up the generic context (class + method type arguments) for a method
/// token.  Returns nullptr if no context is registered.
GenericContextHandle GetGenericContextForMethod(CHAOS_IL2CPP_UINT32 method_token);

/// Register a generic context for a specific method instantiation.
/// `method_token`     — metadata token of the closed generic method
/// `class_type_args`  — class-level type argument handles
/// `class_arg_count`  — number of class type arguments
/// `method_type_args` — method-level type argument handles
/// `method_arg_count` — number of method type arguments
/// Thread-safe, idempotent.
void RegisterMethodGenericContext(
    CHAOS_IL2CPP_UINT32 method_token,
    const TypeInfoHandle* class_type_args, CHAOS_IL2CPP_UINT32 class_arg_count,
    const TypeInfoHandle* method_type_args, CHAOS_IL2CPP_UINT32 method_arg_count);

// ── Accessors ──

CHAOS_IL2CPP_UINT32 GetClassTypeArgCount(GenericContextHandle generic_context);
TypeInfoHandle      GetClassTypeArg(GenericContextHandle generic_context, CHAOS_IL2CPP_UINT32 index);
CHAOS_IL2CPP_UINT32 GetMethodTypeArgCount(GenericContextHandle generic_context);
TypeInfoHandle      GetMethodTypeArg(GenericContextHandle generic_context, CHAOS_IL2CPP_UINT32 index);

// ── Bulk registration and module lifecycle ──

/// Register all generic instantiations described by a ModuleGenericRegistrationV0.
/// Token→handle resolution is performed using `reg.source_image` and the
/// CodegenBridgeV0 resolve_type_by_token helper.
/// `module_id` is stored in each entry for later bulk unregistration.
void RegisterModuleGenerics(const struct ModuleGenericRegistrationV0* reg);

/// Unregister all generic instantiations and method contexts that belong to
/// the given module.  Called during hot-update package unloading.
void UnregisterModuleGenerics(CHAOS_IL2CPP_UINT32 module_id);

/// Returns the total number of registered generic type instantiations.
CHAOS_IL2CPP_UINT32 GetRegisteredInstantiationCount();

// ── Fast-path lookup for RuntimeInstantiationBridge ──

/// Try to find a closed generic type by its open definition and type arguments.
/// Returns the closed_type handle or nullptr on miss.
/// O(n) in the number of instantiations for the given open type (typically small).
TypeInfoHandle TryResolveClosedType(
    TypeInfoHandle open_type,
    const TypeInfoHandle* type_args,
    CHAOS_IL2CPP_UINT32 arg_count);

}  // namespace chaos::il2cpp::generic_context

#endif  // CHAOS_IL2CPP_GENERIC_CONTEXT_H_
