#ifndef CHAOS_IL2CPP_RUNTIME_INSTANTIATION_H_
#define CHAOS_IL2CPP_RUNTIME_INSTANTIATION_H_

#include "runtime_abi.h"
#include "codegen_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CHAOS_RUNTIME_INSTANTIATION_BRIDGE_V0 0u

/*
 * Runtime instantiation bridge.
 *
 * Provides MakeGenericType / MakeGenericMethod support for the Gen-2 Handle
 * generic registration scheme.  The fast path resolves from the
 * GenericContextRegistry (handle-based, O(1) pointer compare).  On registry
 * miss the instantiate function falls back to the interpreted engine which
 * heap-allocates a concrete type descriptor and registers it so subsequent
 * lookups hit the fast path.
 */
typedef struct RuntimeInstantiationBridgeV0 {
    uint32_t abi_version;
    uint32_t struct_size;

    /*
     * Resolve or instantiate a closed generic type.
     *
     * 1. Queries GenericContextRegistry by (open_type_definition, type_args).
     * 2. If found → returns the existing closed_type handle (fast path).
     * 3. If not found → heap-allocates a RuntimeInstantiatedType, builds a
     *    ReflectionQueryTypeDescriptor for the closed type, computes value
     *    type layout if needed, creates interpreted method stubs, registers
     *    the new type into the registry, and returns its handle.
     */
    TypeInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_or_instantiate_type)(
        TypeInfoHandle         open_type_definition,
        const TypeInfoHandle*  type_args,
        uint32_t               arg_count);

    /*
     * Resolve or instantiate a closed generic method.
     *
     * Same strategy as resolve_or_instantiate_type but for methods.
     * Registry miss → create an interpreted method body + context.
     */
    MethodInfoHandle (CHAOS_RUNTIME_ABI_CALL* resolve_or_instantiate_method)(
        MethodInfoHandle       open_method_definition,
        const TypeInfoHandle*  type_args,
        uint32_t               arg_count);

    /*
     * Unregister all generic instantiations belonging to a module.
     * Called during hot-update package unloading.
     */
    void (CHAOS_RUNTIME_ABI_CALL* unregister_module_generics)(
        uint32_t module_id);

    /*
     * Interpret a runtime-instantiated generic method call.
     *
     * Called by MethodInvoke when FindInvokerPointer returns nullptr for a
     * runtime-allocated token (>= 0x80000000).  The bridge lazily lowers the
     * open method's IL body on first call and then dispatches via the
     * InterpreterVM.
     *
     * `method` is the MethodInfoHandle (used to recover RuntimeInstantiatedMethod*
     * via container_of on TryDecodeReflectionQueryMethodHandle).
     *
     * Returns CHAOS_RUNTIME_STATUS_OK on success,
     *         CHAOS_RUNTIME_STATUS_MANAGED_EXCEPTION on exception,
     *         CHAOS_RUNTIME_STATUS_NOT_FOUND if the method is unloaded.
     */
    RuntimeStatus (CHAOS_RUNTIME_ABI_CALL* interpret_method_call)(
        RuntimeState*           runtime_state,
        ThreadState*            thread_state,
        MethodInfoHandle        method,
        void*                   object_instance,
        void* const*            argv,
        CHAOS_IL2CPP_UINT32    argc,
        void*                   out_return_value,
        CHAOS_IL2CPP_SIZE       out_return_value_size,
        ExceptionHandle*        out_exception);

    /* Diagnostic counters. */
    uint32_t runtime_instantiation_count;
    uint32_t interpreted_method_call_count;
} RuntimeInstantiationBridgeV0;

/* Returns the process-wide v0 instantiation bridge or null. */
CHAOS_RUNTIME_ABI_EXPORT const RuntimeInstantiationBridgeV0*
CHAOS_RUNTIME_ABI_CALL ChaosRuntimeInstantiationGetBridgeV0(void);

#ifdef __cplusplus
}
#endif

#endif /* CHAOS_IL2CPP_RUNTIME_INSTANTIATION_H_ */
