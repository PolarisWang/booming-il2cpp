// family-runtime-skeleton
// runtime skeleton page 1
#include "codegen_bridge.h"
#include "runtime_core.h"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::generated::runtime_skeleton_delegate_core_invocation
{
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr std::size_t translation_unit_page_number = 1;
constexpr std::size_t translation_unit_page_item_count = 10;

using NativeReferenceAssemblyStubFn = int32_t (CHAOS_RUNTIME_ABI_CALL*)(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);

// Stub for System.Private.CoreLib/System.Delegate::Combine:System.Delegate(System.Delegate,System.Delegate)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Delegate_Combine_System_Delegate_System_Delegate_System_Delegate(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    // Skeleton stub - method dispatch placeholder
    return CHAOS_BRIDGE_STATUS_OK;
}

// Stub for System.Private.CoreLib/System.Delegate::Remove:System.Delegate(System.Delegate,System.Delegate)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Delegate_Remove_System_Delegate_System_Delegate_System_Delegate(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    // Skeleton stub - method dispatch placeholder
    return CHAOS_BRIDGE_STATUS_OK;
}

// Stub for System.Private.CoreLib/System.Delegate::DynamicInvoke:System.Object(System.Object[])
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Delegate_DynamicInvoke_System_Object_System_Object(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    // Skeleton stub - method dispatch placeholder
    return CHAOS_BRIDGE_STATUS_OK;
}

// Stub for System.Private.CoreLib/System.Delegate::get_Method:System.Reflection.MethodInfo()
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Delegate_get_Method_System_Reflection_MethodInfo(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    // Skeleton stub - method dispatch placeholder
    return CHAOS_BRIDGE_STATUS_OK;
}

// Stub for System.Private.CoreLib/System.Delegate::get_Target:System.Object()
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Delegate_get_Target_System_Object(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    // Skeleton stub - method dispatch placeholder
    return CHAOS_BRIDGE_STATUS_OK;
}

// Stub for System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Object,System.String)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Object_System_String(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    // Skeleton stub - method dispatch placeholder
    return CHAOS_BRIDGE_STATUS_OK;
}

// Stub for System.Private.CoreLib/System.Delegate::CreateDelegate:System.Delegate(System.Type,System.Reflection.MethodInfo)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Reflection_MethodInfo(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    // Skeleton stub - method dispatch placeholder
    return CHAOS_BRIDGE_STATUS_OK;
}

// Stub for System.Private.CoreLib/System.MulticastDelegate::GetInvocationList:System.Delegate[]()
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_MulticastDelegate_GetInvocationList_System_Delegate(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    // Skeleton stub - method dispatch placeholder
    return CHAOS_BRIDGE_STATUS_OK;
}

// Stub for System.Private.CoreLib/System.Delegate::op_Equality:System.Boolean(System.Delegate,System.Delegate)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Delegate_op_Equality_System_Boolean_System_Delegate_System_Delegate(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    // Skeleton stub - method dispatch placeholder
    return CHAOS_BRIDGE_STATUS_OK;
}

// Stub for System.Private.CoreLib/System.Delegate::op_Inequality:System.Boolean(System.Delegate,System.Delegate)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Delegate_op_Inequality_System_Boolean_System_Delegate_System_Delegate(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args)
{
    (void)bridge;
    (void)code_registration;
    (void)metadata_registration;
    (void)options;
    (void)runtime;
    (void)thread;
    (void)managed_args;
    // Skeleton stub - method dispatch placeholder
    return CHAOS_BRIDGE_STATUS_OK;
}

static const NativeReferenceAssemblyStubFn kPageMethodDispatch[] =
{
    &NativeReferenceStub_System_Private_CoreLib_System_Delegate_Combine_System_Delegate_System_Delegate_System_Delegate,
    &NativeReferenceStub_System_Private_CoreLib_System_Delegate_Remove_System_Delegate_System_Delegate_System_Delegate,
    &NativeReferenceStub_System_Private_CoreLib_System_Delegate_DynamicInvoke_System_Object_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Delegate_get_Method_System_Reflection_MethodInfo,
    &NativeReferenceStub_System_Private_CoreLib_System_Delegate_get_Target_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Object_System_String,
    &NativeReferenceStub_System_Private_CoreLib_System_Delegate_CreateDelegate_System_Delegate_System_Type_System_Reflection_MethodInfo,
    &NativeReferenceStub_System_Private_CoreLib_System_MulticastDelegate_GetInvocationList_System_Delegate,
    &NativeReferenceStub_System_Private_CoreLib_System_Delegate_op_Equality_System_Boolean_System_Delegate_System_Delegate,
    &NativeReferenceStub_System_Private_CoreLib_System_Delegate_op_Inequality_System_Boolean_System_Delegate_System_Delegate,
};

int32_t CHAOS_RUNTIME_ABI_CALL DispatchRuntimeSkeletonPage0001(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    std::uint32_t method_slot,
    void* managed_args)
{
    if (method_slot >= (sizeof(kPageMethodDispatch) / sizeof(kPageMethodDispatch[0]))) {
        return CHAOS_BRIDGE_STATUS_NOT_FOUND;
    }
    // Domain is already pushed by the summary entry point.
    return kPageMethodDispatch[method_slot](
        bridge, code_registration, metadata_registration, options, runtime, thread, managed_args);
}
}  // namespace chaos::il2cpp::generated::runtime_skeleton_delegate_core_invocation
