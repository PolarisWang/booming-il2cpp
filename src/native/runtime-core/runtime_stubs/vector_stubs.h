#ifndef CHAOS_IL2CPP_VECTOR_STUBS_H_
#define CHAOS_IL2CPP_VECTOR_STUBS_H_

#include <chaos/native_types.h>

// Vector reduction stubs (SimpleForward for AOT codegen)
// Each stub decomposes a Vector<T> reduction into compare + any/all lanes check.
// Suffixes: i32/u32/i64/u64/f/d/i16/u16/u8/i8

// i32 (System.Int32) — the most common Vector<T> element type
#define CHAOS_VECTOR_REDUCTION_DECLS(suffix) \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_any_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right); \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_all_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right); \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_or_equal_any_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right); \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_or_equal_all_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right); \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_any_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right); \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_all_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right); \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_or_equal_any_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right); \
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_or_equal_all_##suffix(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);

CHAOS_VECTOR_REDUCTION_DECLS(i32)
CHAOS_VECTOR_REDUCTION_DECLS(u32)
CHAOS_VECTOR_REDUCTION_DECLS(i64)
CHAOS_VECTOR_REDUCTION_DECLS(u64)
CHAOS_VECTOR_REDUCTION_DECLS(f)
CHAOS_VECTOR_REDUCTION_DECLS(d)
CHAOS_VECTOR_REDUCTION_DECLS(i16)
CHAOS_VECTOR_REDUCTION_DECLS(u16)
CHAOS_VECTOR_REDUCTION_DECLS(u8)
CHAOS_VECTOR_REDUCTION_DECLS(i8)

#undef CHAOS_VECTOR_REDUCTION_DECLS

// Backward-compatible aliases (existing generated code uses non-suffixed names)
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_any(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_all(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_or_equal_any(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_greater_than_or_equal_all(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_any(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_all(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_or_equal_any(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);
extern "C" CHAOS_IL2CPP_INT32 chaos_vector_less_than_or_equal_all(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right);

#endif
