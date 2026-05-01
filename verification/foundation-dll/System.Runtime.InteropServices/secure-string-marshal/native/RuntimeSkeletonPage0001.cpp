// family-runtime-skeleton
// runtime skeleton page 1
#include "codegen_bridge.h"
#include "runtime_core.h"
#include <atomic>
#include <chaos/native_types.h>
#include <chaos/native_types.h>
#include <cstring>

namespace chaos::il2cpp::generated::runtime_skeleton_secure_string_marshal
{
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr CHAOS_IL2CPP_SIZE translation_unit_page_number = 1;
constexpr CHAOS_IL2CPP_SIZE translation_unit_page_item_count = 4;

using NativeReferenceAssemblyStubFn = CHAOS_IL2CPP_INT32 (CHAOS_RUNTIME_ABI_CALL*)(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);

// Stub for System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemAnsi:System.Void(P0)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToCoTaskMemAnsi_System_Void_P0(
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

// Stub for System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToCoTaskMemUnicode:System.Void(P0)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToCoTaskMemUnicode_System_Void_P0(
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

// Stub for System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocAnsi:System.Void(P0)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToGlobalAllocAnsi_System_Void_P0(
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

// Stub for System.Runtime.InteropServices/System.Security.SecureStringMarshal::SecureStringToGlobalAllocUnicode:System.Void(P0)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToGlobalAllocUnicode_System_Void_P0(
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
    &NativeReferenceStub_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToCoTaskMemAnsi_System_Void_P0,
    &NativeReferenceStub_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToCoTaskMemUnicode_System_Void_P0,
    &NativeReferenceStub_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToGlobalAllocAnsi_System_Void_P0,
    &NativeReferenceStub_System_Runtime_InteropServices_System_Security_SecureStringMarshal_SecureStringToGlobalAllocUnicode_System_Void_P0,
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
}  // namespace chaos::il2cpp::generated::runtime_skeleton_secure_string_marshal
