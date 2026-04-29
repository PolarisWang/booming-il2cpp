#ifndef CHAOS_IL2CPP_GENERIC_CONTEXT_H_
#define CHAOS_IL2CPP_GENERIC_CONTEXT_H_

#include "runtime_abi.h"
#include <chaos/native_types.h>

namespace chaos::il2cpp::generic_context {

/// Registers a closed generic type instantiation.
/// `open_token`   — token of the open generic type (e.g., List<>)
/// `closed_token` — token of the specific instantiation (e.g., List<int>)
/// `type_args`    — array of type argument tokens
/// `arg_count`    — length of type_args
void RegisterGenericInstantiation(
    CHAOS_IL2CPP_UINT32 open_token,
    CHAOS_IL2CPP_UINT32 closed_token,
    const CHAOS_IL2CPP_UINT32* type_args,
    CHAOS_IL2CPP_UINT32 arg_count);

/// Look up the generic context (type+method type arguments) for a method token.
/// Returns nullptr if no generic context is registered for this method.
GenericContextHandle GetGenericContextForMethod(CHAOS_IL2CPP_UINT32 method_token);

/// Register a generic context for a specific method instantiation.
/// The context records both class and method type arguments.
void RegisterMethodGenericContext(
    CHAOS_IL2CPP_UINT32 method_token,
    const TypeInfoHandle* class_type_args, CHAOS_IL2CPP_UINT32 class_arg_count,
    const TypeInfoHandle* method_type_args, CHAOS_IL2CPP_UINT32 method_arg_count);

CHAOS_IL2CPP_UINT32 GetClassTypeArgCount(GenericContextHandle generic_context);
TypeInfoHandle GetClassTypeArg(GenericContextHandle generic_context, CHAOS_IL2CPP_UINT32 index);
CHAOS_IL2CPP_UINT32 GetMethodTypeArgCount(GenericContextHandle generic_context);
TypeInfoHandle GetMethodTypeArg(GenericContextHandle generic_context, CHAOS_IL2CPP_UINT32 index);

/// Returns the number of registered generic instantiations.
CHAOS_IL2CPP_UINT32 GetRegisteredInstantiationCount();

}  // namespace chaos::il2cpp::generic_context

#endif  // CHAOS_IL2CPP_GENERIC_CONTEXT_H_
