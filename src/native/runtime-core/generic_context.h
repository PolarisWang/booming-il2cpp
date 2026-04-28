#ifndef CHAOS_IL2CPP_GENERIC_CONTEXT_H_
#define CHAOS_IL2CPP_GENERIC_CONTEXT_H_

#include "runtime_abi.h"
#include <stdint.h>

namespace chaos::il2cpp::generic_context {

/// Registers a closed generic type instantiation.
/// `open_token`   — token of the open generic type (e.g., List<>)
/// `closed_token` — token of the specific instantiation (e.g., List<int>)
/// `type_args`    — array of type argument tokens
/// `arg_count`    — length of type_args
void RegisterGenericInstantiation(
    uint32_t open_token,
    uint32_t closed_token,
    const uint32_t* type_args,
    uint32_t arg_count);

/// Look up the generic context (type+method type arguments) for a method token.
/// Returns nullptr if no generic context is registered for this method.
GenericContextHandle GetGenericContextForMethod(uint32_t method_token);

/// Register a generic context for a specific method instantiation.
/// The context records both class and method type arguments.
void RegisterMethodGenericContext(
    uint32_t method_token,
    const TypeInfoHandle* class_type_args, uint32_t class_arg_count,
    const TypeInfoHandle* method_type_args, uint32_t method_arg_count);

uint32_t GetClassTypeArgCount(GenericContextHandle generic_context);
TypeInfoHandle GetClassTypeArg(GenericContextHandle generic_context, uint32_t index);
uint32_t GetMethodTypeArgCount(GenericContextHandle generic_context);
TypeInfoHandle GetMethodTypeArg(GenericContextHandle generic_context, uint32_t index);

/// Returns the number of registered generic instantiations.
uint32_t GetRegisteredInstantiationCount();

}  // namespace chaos::il2cpp::generic_context

#endif  // CHAOS_IL2CPP_GENERIC_CONTEXT_H_
