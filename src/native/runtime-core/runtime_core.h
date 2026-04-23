#ifndef CHAOS_IL2CPP_RUNTIME_CORE_H_
#define CHAOS_IL2CPP_RUNTIME_CORE_H_

#include "runtime_abi.h"

namespace chaos::il2cpp::runtime_core {

struct ManagedExceptionCarrier {
    ExceptionHandle exception;
};

using EngineLifecycleCallback = void (*)(const char* phase_utf8, void* user_data);
using FinalizerCallback = void (*)(void* object_instance);

enum class RuntimeMode {
    Aot = 0,
    Mixed = 1,
};

const RuntimeAbiV0* GetRuntimeAbiV0();
RuntimeMode GetRuntimeMode();
void SetRuntimeMode(RuntimeMode mode);
bool IsMixedMode();
void* BoxValueObject(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle value_type,
    const void* value,
    size_t value_size);
RuntimeStatus UnboxValueObject(
    RuntimeState* runtime_state,
    void* boxed_object,
    void* out_value,
    size_t out_value_size);
bool ArrayStoreReference(
    void* array_instance,
    uintptr_t index,
    void* value);
void* ArrayLoadReference(
    void* array_instance,
    uintptr_t index);
bool ArrayCopyReferenceRange(
    void* source_array_instance,
    uintptr_t source_index,
    void* target_array_instance,
    uintptr_t target_index,
    uintptr_t length);
bool ArrayClearReferenceRange(
    void* array_instance,
    uintptr_t start_index,
    uintptr_t length);
bool ArrayReverseReferenceRange(
    void* array_instance,
    uintptr_t start_index,
    uintptr_t length);
int32_t EngineLogWrite(
    const char* category_utf8,
    const char* message_utf8);
uintptr_t CreateEngineObjectHandle(void* object_instance);
void* ResolveEngineObjectHandle(uintptr_t handle);
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
    int32_t delta,
    int32_t* out_value);
bool MonitorEnter(void* monitor_target);
bool MonitorExit(void* monitor_target);
bool GcSafepoint(
    RuntimeState* runtime_state,
    ThreadState* thread_state);
size_t ReportThreadRoot(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* root_address,
    size_t root_size);
bool EnqueueFinalizer(
    RuntimeState* runtime_state,
    void* object_instance,
    FinalizerCallback finalizer);
size_t DrainFinalizerQueue(RuntimeState* runtime_state);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_RUNTIME_CORE_H_
