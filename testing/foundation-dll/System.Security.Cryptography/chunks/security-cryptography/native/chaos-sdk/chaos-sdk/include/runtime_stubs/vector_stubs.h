#ifndef CHAOS_IL2CPP_VECTOR_STUBS_H_
#define CHAOS_IL2CPP_VECTOR_STUBS_H_

#include <chaos/native_types.h>

// Vector reduction stubs (SimpleForward for AOT codegen)
// Each stub decomposes a Vector<T> reduction into compare + any/all lanes check.

extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_any(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right, CHAOS_IL2CPP_INT32 element_size, CHAOS_IL2CPP_INT32 carrier_size);
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_all(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right, CHAOS_IL2CPP_INT32 element_size, CHAOS_IL2CPP_INT32 carrier_size);
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_any(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right, CHAOS_IL2CPP_INT32 element_size, CHAOS_IL2CPP_INT32 carrier_size);
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_all(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right, CHAOS_IL2CPP_INT32 element_size, CHAOS_IL2CPP_INT32 carrier_size);

#endif
