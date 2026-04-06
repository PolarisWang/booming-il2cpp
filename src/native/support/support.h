#ifndef CHAOS_IL2CPP_SUPPORT_H_
#define CHAOS_IL2CPP_SUPPORT_H_

#include "runtime_abi.h"

namespace chaos::il2cpp::support {

const char* TryGetUtf8View(const void* string_object, uintptr_t* out_byte_count);

void* CHAOS_RUNTIME_ABI_CALL ConcatStringPair(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* left_string,
    const void* right_string);

int32_t CHAOS_RUNTIME_ABI_CALL WriteLineString(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* string_object);

}  // namespace chaos::il2cpp::support

#endif  // CHAOS_IL2CPP_SUPPORT_H_
