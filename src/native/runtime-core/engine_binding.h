#ifndef CHAOS_IL2CPP_ENGINE_BINDING_H_
#define CHAOS_IL2CPP_ENGINE_BINDING_H_

#include <chaos/native_types.h>
#include "runtime_abi.h"
#include "runtime_capability.h"

namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INT32 EngineLogWrite(
    const char* category_utf8,
    const char* message_utf8);
CHAOS_IL2CPP_UINTPTR CreateEngineObjectHandle(void* object_instance);
void* ResolveEngineObjectHandle(CHAOS_IL2CPP_UINTPTR handle);
bool RegisterEngineLifecycleCallback(
    const char* phase_utf8,
    EngineLifecycleCallback callback,
    void* user_data);
bool DispatchEngineLifecycleCallbacks(const char* phase_utf8);
bool IsMainThreadLane();
bool ThreadStaticInt32Add(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const char* slot_key_utf8,
    CHAOS_IL2CPP_INT32 delta,
    CHAOS_IL2CPP_INT32* out_value);
bool MonitorEnter(void* monitor_target);
bool MonitorExit(void* monitor_target);
bool GcSafepoint(
    RuntimeState* runtime_state,
    ThreadState* thread_state);
CHAOS_IL2CPP_SIZE ReportThreadRoot(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* root_address,
    CHAOS_IL2CPP_SIZE root_size);
bool EnqueueFinalizer(
    RuntimeState* runtime_state,
    void* object_instance,
    FinalizerCallback finalizer);
CHAOS_IL2CPP_SIZE DrainFinalizerQueue(RuntimeState* runtime_state);
CHAOS_IL2CPP_INTPTR MarshalAllocHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR size);
CHAOS_IL2CPP_INTPTR MarshalAllocCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INT32 size);
CHAOS_IL2CPP_INTPTR MarshalReAllocHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_INTPTR size);
CHAOS_IL2CPP_INTPTR MarshalReAllocCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_INT32 size);
bool MarshalFreeHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory);
bool MarshalFreeCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory);
bool MarshalZeroFreeCoTaskMemUtf8(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory);
void* MarshalPtrToStringUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR utf8_buffer,
    CHAOS_IL2CPP_INT32 length,
    bool has_explicit_length);
CHAOS_IL2CPP_INTPTR MarshalStringToCoTaskMemUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* managed_string);
CHAOS_IL2CPP_UINT8 MarshalReadByte(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INT16 MarshalReadInt16(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INT32 MarshalReadInt32(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INT64 MarshalReadInt64(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INTPTR MarshalReadIntPtr(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
void MarshalWriteByte(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_UINT8 value);
void MarshalWriteInt16(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT16 value);
void MarshalWriteInt32(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 value);
void MarshalWriteInt64(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT64 value);
void MarshalWriteIntPtr(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INTPTR value);
CHAOS_IL2CPP_INT32 TaskKernelNewId();
CHAOS_IL2CPP_INT32 InteropKernel32GetLastError();
CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentProcessId();
CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentThreadId();
CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentProcess();
CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentThread();
bool InteropKernel32CloseHandle(CHAOS_IL2CPP_INTPTR handle);
bool InteropKernel32FreeLibrary(CHAOS_IL2CPP_INTPTR handle);
ValueTypeKernelBackendKind GetCharClassificationKernelBackend();
ValueTypeKernelBackendKind GetFloatingClassificationKernelBackend();
ValueTypeKernelBackendKind GetBitArithmeticKernelBackend();
ValueTypeKernelBackendKind GetTickArithmeticKernelBackend();
VectorKernelBackendKind GetVectorKernelBackend();

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_ENGINE_BINDING_H_
