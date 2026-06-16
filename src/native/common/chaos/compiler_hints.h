#ifndef CHAOS_IL2CPP_COMMON_COMPILER_HINTS_H_
#define CHAOS_IL2CPP_COMMON_COMPILER_HINTS_H_

#if defined(_MSC_VER)
#  define CHAOS_IL2CPP_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define CHAOS_IL2CPP_FORCEINLINE inline __attribute__((always_inline))
#else
#  define CHAOS_IL2CPP_FORCEINLINE inline
#endif

#if defined(__GNUC__) || defined(__clang__)
#  define CHAOS_IL2CPP_LIKELY(x)     __builtin_expect(!!(x), 1)
#  define CHAOS_IL2CPP_UNLIKELY(x)   __builtin_expect(!!(x), 0)
#else
#  define CHAOS_IL2CPP_LIKELY(x)     (x)
#  define CHAOS_IL2CPP_UNLIKELY(x)   (x)
#endif

#if defined(_MSC_VER)
#  define CHAOS_IL2CPP_RESTRICT __restrict
#elif defined(__GNUC__) || defined(__clang__)
#  define CHAOS_IL2CPP_RESTRICT __restrict__
#else
#  define CHAOS_IL2CPP_RESTRICT
#endif

#if defined(_MSC_VER)
#  define CHAOS_IL2CPP_ASSUME(expr) __assume(expr)
#elif defined(__GNUC__) || defined(__clang__)
#  define CHAOS_IL2CPP_ASSUME(expr) do { if (!(expr)) __builtin_unreachable(); } while(0)
#else
#  define CHAOS_IL2CPP_ASSUME(expr) ((void)0)
#endif

#if defined(__GNUC__) || defined(__clang__)
#  define CHAOS_IL2CPP_COMPUTED_GOTO 1
#else
#  define CHAOS_IL2CPP_COMPUTED_GOTO 0
#endif

#endif
