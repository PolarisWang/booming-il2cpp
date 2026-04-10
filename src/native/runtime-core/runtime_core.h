#ifndef CHAOS_IL2CPP_RUNTIME_CORE_H_
#define CHAOS_IL2CPP_RUNTIME_CORE_H_

#include "runtime_abi.h"

namespace chaos::il2cpp::runtime_core {

struct ManagedExceptionCarrier {
    ExceptionHandle exception;
};

using EngineLifecycleCallback = void (*)(const char* phase_utf8, void* user_data);

const RuntimeAbiV0* GetRuntimeAbiV0();
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

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_RUNTIME_CORE_H_
