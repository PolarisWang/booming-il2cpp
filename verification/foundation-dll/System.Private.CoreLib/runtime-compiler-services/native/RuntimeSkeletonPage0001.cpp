// family-runtime-skeleton
// runtime skeleton page 1
#include "codegen_bridge.h"
#include "runtime_core.h"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::generated::runtime_skeleton_runtime_compiler_services
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

// Stub for System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode:System.Int32(System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetHashCode_System_Int32_System_Object(
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

// Stub for System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::Equals:System.Boolean(System.Object,System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_Equals_System_Boolean_System_Object_System_Object(
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

// Stub for System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object(
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

// Stub for System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::RunClassConstructor:System.Void(System.RuntimeTypeHandle)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_RunClassConstructor_System_Void_System_RuntimeTypeHandle(
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

// Stub for System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray:System.Void(System.Array,System.RuntimeFieldHandle)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle(
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

// Stub for System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetSubArray:T[](T[],System.Range)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetSubArray_T_T_System_Range(
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

// Stub for System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::BoxEnum:System.Object(T)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_BoxEnum_System_Object_T(
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

// Stub for System.Private.CoreLib/System.Runtime.CompilerServices.FormattableStringFactory::Create:System.Runtime.CompilerServices.FormattableString(System.String,System.Object[])
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_FormattableStringFactory_Create_System_Runtime_CompilerServices_FormattableString_System_String_System_Object(
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

// Stub for System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::.ctor:System.Void(System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_ctor_System_Void_System_Object(
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

// Stub for System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException:System.Object()
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_get_WrappedException_System_Object(
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
    &NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetHashCode_System_Int32_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_Equals_System_Boolean_System_Object_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetObjectValue_System_Object_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_RunClassConstructor_System_Void_System_RuntimeTypeHandle,
    &NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray_System_Void_System_Array_System_RuntimeFieldHandle,
    &NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_GetSubArray_T_T_System_Range,
    &NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeHelpers_BoxEnum_System_Object_T,
    &NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_FormattableStringFactory_Create_System_Runtime_CompilerServices_FormattableString_System_String_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_ctor_System_Void_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Runtime_CompilerServices_RuntimeWrappedException_get_WrappedException_System_Object,
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
}  // namespace chaos::il2cpp::generated::runtime_skeleton_runtime_compiler_services
