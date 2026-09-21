#ifndef CHAOS_IL2CPP_COMMON_COMPILER_HINTS_H_
#define CHAOS_IL2CPP_COMMON_COMPILER_HINTS_H_

// ── Compiler hint macros ──────────────────────────────────────────────
// Platform-agnostic wrappers for force-inline, branch prediction, and
// pointer aliasing hints.  These let the C++ compiler generate better
// code without #ifdef chains at every call site.
//
// Usage:
//   CHAOS_IL2CPP_FORCEINLINE int HotPathFn(int x) { ... }
//   if (CHAOS_IL2CPP_UNLIKELY(ptr == nullptr)) { ... }
//   void* CHAOS_IL2CPP_RESTRICT out = ...;

// ── CHAOS_IL2CPP_FORCEINLINE ──────────────────────────────────────────
// Always-inline annotation, even under /Ob1 or -fno-inline-functions.
// Expands to __forceinline on MSVC, __attribute__((always_inline)) on
// GCC/Clang.
#if defined(_MSC_VER)
#  define CHAOS_IL2CPP_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define CHAOS_IL2CPP_FORCEINLINE inline __attribute__((always_inline))
#else
#  define CHAOS_IL2CPP_FORCEINLINE inline
#endif

// ── CHAOS_IL2CPP_LIKELY / CHAOS_IL2CPP_UNLIKELY ──────────────────────
// Branch prediction hints.  Tell the compiler which path is statistically
// more likely so it can arrange the branch layout accordingly.
//   if (CHAOS_IL2CPP_LIKELY(ptr != nullptr))     // fast path
//   if (CHAOS_IL2CPP_UNLIKELY(ptr == nullptr))    // slow/error path
#if defined(__GNUC__) || defined(__clang__)
#  define CHAOS_IL2CPP_LIKELY(x)     __builtin_expect(!!(x), 1)
#  define CHAOS_IL2CPP_UNLIKELY(x)   __builtin_expect(!!(x), 0)
#else
#  define CHAOS_IL2CPP_LIKELY(x)     (x)
#  define CHAOS_IL2CPP_UNLIKELY(x)   (x)
#endif

// ── CHAOS_IL2CPP_RESTRICT ─────────────────────────────────────────────
// Non-aliased pointer qualifier.  Promises the compiler that the pointer
// is the only way to access the pointed-to object within its scope,
// enabling load/store reordering and elimination.
//   void CopyBuf(void* CHAOS_IL2CPP_RESTRICT dst, const void* CHAOS_IL2CPP_RESTRICT src, size_t n);
#if defined(_MSC_VER)
#  define CHAOS_IL2CPP_RESTRICT __restrict
#elif defined(__GNUC__) || defined(__clang__)
#  define CHAOS_IL2CPP_RESTRICT __restrict__
#else
#  define CHAOS_IL2CPP_RESTRICT
#endif

#endif  // CHAOS_IL2CPP_COMMON_COMPILER_HINTS_H_
