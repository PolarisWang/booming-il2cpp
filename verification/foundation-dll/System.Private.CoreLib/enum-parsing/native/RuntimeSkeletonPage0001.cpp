// family-runtime-skeleton
// runtime skeleton page 1
#include "codegen_bridge.h"
#include "runtime_core.h"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::generated::runtime_skeleton_enum_parsing
{
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr std::size_t translation_unit_page_number = 1;
constexpr std::size_t translation_unit_page_item_count = 12;

using NativeReferenceAssemblyStubFn = int32_t (CHAOS_RUNTIME_ABI_CALL*)(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);

// Stub for System.Private.CoreLib/System.Enum::Format:System.String(System.Type,System.Object,System.String)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_Format_System_String_System_Type_System_Object_System_String(
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

// Stub for System.Private.CoreLib/System.Enum::GetName:System.String(System.RuntimeType,System.UInt64)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_GetName_System_String_System_RuntimeType_System_UInt64(
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

// Stub for System.Private.CoreLib/System.Enum::GetName:System.String(System.Type,System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_GetName_System_String_System_Type_System_Object(
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

// Stub for System.Private.CoreLib/System.Enum::GetNames:System.String[](System.Type)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_GetNames_System_String_System_Type(
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

// Stub for System.Private.CoreLib/System.Enum::GetValues:System.Array(System.Type)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_GetValues_System_Array_System_Type(
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

// Stub for System.Private.CoreLib/System.Enum::IsDefined:System.Boolean(System.Type,System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_IsDefined_System_Boolean_System_Type_System_Object(
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

// Stub for System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_Parse_System_Object_System_Type_System_String(
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

// Stub for System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String,System.Boolean)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_Parse_System_Object_System_Type_System_String_System_Boolean(
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

// Stub for System.Private.CoreLib/System.Enum::ToString:System.String()
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_ToString_System_String(
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

// Stub for System.Private.CoreLib/System.Enum::ToString:System.String(System.String)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_ToString_System_String_System_String(
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

// Stub for System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Boolean,System.Object&)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_TryParse_System_Boolean_System_Type_System_String_System_Boolean_System_Object_Ref(
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

// Stub for System.Private.CoreLib/System.Enum::TryParse:System.Boolean(System.Type,System.String,System.Object&)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Enum_TryParse_System_Boolean_System_Type_System_String_System_Object_Ref(
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
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_Format_System_String_System_Type_System_Object_System_String,
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_GetName_System_String_System_RuntimeType_System_UInt64,
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_GetName_System_String_System_Type_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_GetNames_System_String_System_Type,
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_GetValues_System_Array_System_Type,
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_IsDefined_System_Boolean_System_Type_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_Parse_System_Object_System_Type_System_String,
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_Parse_System_Object_System_Type_System_String_System_Boolean,
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_ToString_System_String,
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_ToString_System_String_System_String,
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_TryParse_System_Boolean_System_Type_System_String_System_Boolean_System_Object_Ref,
    &NativeReferenceStub_System_Private_CoreLib_System_Enum_TryParse_System_Boolean_System_Type_System_String_System_Object_Ref,
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
}  // namespace chaos::il2cpp::generated::runtime_skeleton_enum_parsing
