// family-runtime-skeleton
// runtime skeleton page 1
#include "codegen_bridge.h"
#include "runtime_core.h"
#include <atomic>
#include <chaos/native_types.h>
#include <chaos/native_types.h>
#include <cstring>

namespace chaos::il2cpp::generated::runtime_skeleton_binder_type_style
{
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr CHAOS_IL2CPP_SIZE translation_unit_page_number = 1;
constexpr CHAOS_IL2CPP_SIZE translation_unit_page_item_count = 3;

using NativeReferenceAssemblyStubFn = CHAOS_IL2CPP_INT32 (CHAOS_RUNTIME_ABI_CALL*)(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);

// Stub for System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::SerializationBinder::Void()
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_SerializationBinder_Void(
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

// Stub for System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToName:System.Void(P0, P1, P2)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_BindToName_System_Void_P0_P1_P2(
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

// Stub for System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SerializationBinder::BindToType:System.Void(P0, P1)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_BindToType_System_Void_P0_P1(
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
    &NativeReferenceStub_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_SerializationBinder_Void,
    &NativeReferenceStub_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_BindToName_System_Void_P0_P1_P2,
    &NativeReferenceStub_System_Runtime_Serialization_Formatters_System_Runtime_Serialization_SerializationBinder_BindToType_System_Void_P0_P1,
};

int32_t CHAOS_RUNTIME_ABI_CALL DispatchRuntimeSkeletonPage0001(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    CHAOS_IL2CPP_UINT32 method_slot,
    void* managed_args)
{
    if (method_slot >= (sizeof(kPageMethodDispatch) / sizeof(kPageMethodDispatch[0]))) {
        return CHAOS_BRIDGE_STATUS_NOT_FOUND;
    }
    // Domain is already pushed by the summary entry point.
    return kPageMethodDispatch[method_slot](
        bridge, code_registration, metadata_registration, options, runtime, thread, managed_args);
}
}  // namespace chaos::il2cpp::generated::runtime_skeleton_binder_type_style
