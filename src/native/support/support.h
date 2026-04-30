#ifndef CHAOS_IL2CPP_SUPPORT_H_
#define CHAOS_IL2CPP_SUPPORT_H_

#include "runtime_abi.h"
#include <chaos/native_types.h>

namespace chaos::il2cpp::support {

const char* TryGetUtf8View(const void* string_object, CHAOS_IL2CPP_INTPTR* out_byte_count);

void* CHAOS_RUNTIME_ABI_CALL ConcatStringPair(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* left_string,
    const void* right_string);

void* CHAOS_RUNTIME_ABI_CALL ConcatStringTriple(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* first_string,
    const void* second_string,
    const void* third_string);

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL WriteLineString(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* string_object);

/// Compute TimeSpan ticks from milliseconds with rounding and overflow check.
/// Returns ticks via out parameter. The TimeSpan value type is opaque at this level.
void CHAOS_RUNTIME_ABI_CALL TimeSpanFromMilliseconds(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_FLOAT64 milliseconds,
    CHAOS_IL2CPP_INT64* out_ticks);

}  // namespace chaos::il2cpp::support

#endif  // CHAOS_IL2CPP_SUPPORT_H_
