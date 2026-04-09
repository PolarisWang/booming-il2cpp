#ifndef CHAOS_IL2CPP_RUNTIME_CORE_H_
#define CHAOS_IL2CPP_RUNTIME_CORE_H_

#include "runtime_abi.h"

namespace chaos::il2cpp::runtime_core {

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

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_RUNTIME_CORE_H_
