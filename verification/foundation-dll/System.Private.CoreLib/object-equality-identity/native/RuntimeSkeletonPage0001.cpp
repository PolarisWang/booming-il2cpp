// family-runtime-skeleton
// runtime skeleton page 1
#include "codegen_bridge.h"
#include "runtime_core.h"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::generated::runtime_skeleton_object_equality_identity
{
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr std::size_t translation_unit_page_number = 1;
constexpr std::size_t translation_unit_page_item_count = 7;

using NativeReferenceAssemblyStubFn = int32_t (CHAOS_RUNTIME_ABI_CALL*)(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);

// Stub for System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object(
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

// Stub for System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object_System_Object(
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

// Stub for System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Object_ReferenceEquals_System_Boolean_System_Object_System_Object(
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

// Stub for System.Private.CoreLib/System.Object::GetHashCode:System.Int32()
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Object_GetHashCode_System_Int32(
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

// Stub for System.Private.CoreLib/System.Object::ToString:System.String()
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Object_ToString_System_String(
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

// Stub for System.Private.CoreLib/System.Object::GetType:System.Type()
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Object_GetType_System_Type(
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

// Stub for System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object(
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
    &NativeReferenceStub_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Object_Equals_System_Boolean_System_Object_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Object_ReferenceEquals_System_Boolean_System_Object_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Object_GetHashCode_System_Int32,
    &NativeReferenceStub_System_Private_CoreLib_System_Object_ToString_System_String,
    &NativeReferenceStub_System_Private_CoreLib_System_Object_GetType_System_Type,
    &NativeReferenceStub_System_Private_CoreLib_System_Object_MemberwiseClone_System_Object,
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
}  // namespace chaos::il2cpp::generated::runtime_skeleton_object_equality_identity
