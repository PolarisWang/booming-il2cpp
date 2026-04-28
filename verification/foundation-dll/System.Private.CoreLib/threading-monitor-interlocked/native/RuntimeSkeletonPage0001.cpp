// family-runtime-skeleton
// runtime skeleton page 1
#include "codegen_bridge.h"
#include "runtime_core.h"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::generated::runtime_skeleton_threading_monitor_interlocked
{
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr std::size_t translation_unit_page_number = 1;
constexpr std::size_t translation_unit_page_item_count = 14;

using NativeReferenceAssemblyStubFn = int32_t (CHAOS_RUNTIME_ABI_CALL*)(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args);

// Stub for System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_Enter_System_Void_System_Object(
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

// Stub for System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_Exit_System_Void_System_Object(
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

// Stub for System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object(
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

// Stub for System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object_System_Int32(
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

// Stub for System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_Pulse_System_Void_System_Object(
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

// Stub for System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_PulseAll_System_Void_System_Object(
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

// Stub for System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_Wait_System_Boolean_System_Object(
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

// Stub for System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_Increment_System_Int32_System_Int32_Ref(
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

// Stub for System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_Decrement_System_Int32_System_Int32_Ref(
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

// Stub for System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_Exchange_System_Int32_System_Int32_Ref_System_Int32(
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

// Stub for System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_CompareExchange_System_Int32_System_Int32_Ref_System_Int32_System_Int32(
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

// Stub for System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_Add_System_Int32_System_Int32_Ref_System_Int32(
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

// Stub for System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_MemoryBarrier_System_Void(
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

// Stub for System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)
int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_System_Private_CoreLib_System_Threading_Volatile_Read_System_Int32_System_Int32_Ref(
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
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_Enter_System_Void_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_Exit_System_Void_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_TryEnter_System_Boolean_System_Object_System_Int32,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_Pulse_System_Void_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_PulseAll_System_Void_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Monitor_Wait_System_Boolean_System_Object,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_Increment_System_Int32_System_Int32_Ref,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_Decrement_System_Int32_System_Int32_Ref,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_Exchange_System_Int32_System_Int32_Ref_System_Int32,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_CompareExchange_System_Int32_System_Int32_Ref_System_Int32_System_Int32,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_Add_System_Int32_System_Int32_Ref_System_Int32,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Interlocked_MemoryBarrier_System_Void,
    &NativeReferenceStub_System_Private_CoreLib_System_Threading_Volatile_Read_System_Int32_System_Int32_Ref,
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
}  // namespace chaos::il2cpp::generated::runtime_skeleton_threading_monitor_interlocked
