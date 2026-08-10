#ifndef CHAOS_IL2CPP_COMMON_NATIVE_TYPES_H_
#define CHAOS_IL2CPP_COMMON_NATIVE_TYPES_H_

// Native type abstraction layer.
// All generated C++ code and common library public APIs MUST use
// these macros instead of raw std:: types.  Backend implementations
// are swapped by changing the definitions below.

#include <chaos/config.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

// ── Integer types ──────────────────────────────────────────
#define CHAOS_IL2CPP_INT8     std::int8_t
#define CHAOS_IL2CPP_INT16    std::int16_t
#define CHAOS_IL2CPP_INT32    std::int32_t
#define CHAOS_IL2CPP_INT64    std::int64_t
#define CHAOS_IL2CPP_UINT8    std::uint8_t
#define CHAOS_IL2CPP_UINT16   std::uint16_t
#define CHAOS_IL2CPP_UINT32   std::uint32_t
#define CHAOS_IL2CPP_UINT64   std::uint64_t
#define CHAOS_IL2CPP_UINT64_C(x)  UINT64_C(x)
#define CHAOS_IL2CPP_INTPTR   std::intptr_t
#define CHAOS_IL2CPP_UINTPTR  std::uintptr_t
#define CHAOS_IL2CPP_SIZE     std::size_t

// ── Floating-point types ──────────────────────────────────────
#define CHAOS_IL2CPP_FLOAT32  float
#define CHAOS_IL2CPP_FLOAT64  double

// ── Math functions (abstracted for platform portability) ────────
#define CHAOS_IL2CPP_SIN           std::sin
#define CHAOS_IL2CPP_COS           std::cos
#define CHAOS_IL2CPP_COPYSIGN      std::copysign
#define CHAOS_IL2CPP_ABS           std::abs
#define CHAOS_IL2CPP_REMAINDER     std::remainder
#define CHAOS_IL2CPP_ILOGB         std::ilogb
#define CHAOS_IL2CPP_SCALBN        std::scalbn
#define CHAOS_IL2CPP_LOG           std::log
#define CHAOS_IL2CPP_ROUND         std::round
#define CHAOS_IL2CPP_NEARBYINT     std::nearbyint
#define CHAOS_IL2CPP_NEXTAFTER     std::nextafter
#define CHAOS_IL2CPP_SINCOS(x,s,c) do { (s) = std::sin(x); (c) = std::cos(x); } while(false)
#define CHAOS_IL2CPP_MAXMAGNITUDE  std::fmaxmag
#define CHAOS_IL2CPP_MINMAGNITUDE  std::fminmag

// ── Containers ──────────────────────────────────────────────
#define CHAOS_IL2CPP_VECTOR(T)          std::vector<T>
/// std::array alias for codegen-generated fixed-size stack allocations (no heap).
#define CHAOS_IL2CPP_ARRAY(T, N)        std::array<T, N>
#define CHAOS_IL2CPP_STRING             std::string
#define CHAOS_IL2CPP_STRING_VIEW        std::string_view
#define CHAOS_IL2CPP_UNORDERED_MAP(K,V) std::unordered_map<K, V>
#define CHAOS_IL2CPP_PAIR(K,V)          std::pair<K, V>

// ── Smart pointers ──────────────────────────────────────────
#define CHAOS_IL2CPP_UNIQUE_PTR(T)  std::unique_ptr<T>
#define CHAOS_IL2CPP_MAKE_UNIQUE(T) std::make_unique<T>
#define CHAOS_IL2CPP_SHARED_PTR(T)  std::shared_ptr<T>
#define CHAOS_IL2CPP_MAKE_SHARED(T) std::make_shared<T>

// ── C standard library ─────────────────────────────────────
#define CHAOS_IL2CPP_ABORT()       std::abort()
// CHAOS_IL2CPP_FAIL: non-fatal failure indicator, distinct from ABORT.
// Codegen emits FAIL for null-checks / expected failures so verification
// mode can override FAIL without disabling ABORT for real crashes.
// Verification mode (runtime-entry.cpp) sets g_chaos_fail_hook to throw
// chaos_managed_exception{}, caught by try/catch around each method call.
// Forward declaration of g_chaos_fail_hook (defined in common.h) for
// direct-inclusion compilation units.
namespace chaos { namespace il2cpp { namespace common {
    extern void (*g_chaos_fail_hook)();
}}}
#define CHAOS_IL2CPP_FAIL(...) \
    do { \
        if (::chaos::il2cpp::common::g_chaos_fail_hook) \
            ::chaos::il2cpp::common::g_chaos_fail_hook(); \
        else \
            CHAOS_IL2CPP_ABORT(); \
    } while(0)

// CHAOS_IL2CPP_FAIL_FAST: direct trap for bounds checks, no indirect jump.
// Uses __fastfail (noreturn intrinsic) in PROFILE/SHIP configs, enabling the
// C++ compiler to hoist bounds checks out of loops — the optimizer knows the
// trap never returns, so checked variables are provably invariant after check.
// Falls back to CHAOS_IL2CPP_FAIL() in CHECK config for verification mode
// compatibility (where g_chaos_fail_hook routes failures through SEH).
#if defined(CHAOS_IL2CPP_CONFIG_CHECK)
#define CHAOS_IL2CPP_FAIL_FAST() CHAOS_IL2CPP_FAIL()
#elif defined(_MSC_VER)
#define CHAOS_IL2CPP_FAIL_FAST() __fastfail(7)  // FAST_FAIL_RANGE_CHECK
#else
#define CHAOS_IL2CPP_FAIL_FAST() __builtin_trap()
#endif
// HRESULT-style failure check: true when the high bit is set (negative).
// Defined here for cross-platform use in native-aot codegen output.
#define CHAOS_IL2CPP_FAILED(hr)    ((hr) < 0)
#define CHAOS_IL2CPP_SUCCEEDED(hr) ((hr) >= 0)
#define CHAOS_IL2CPP_MEMCPY(d,s,n) std::memcpy(d, s, n)
#define CHAOS_IL2CPP_MEMCMP(a,b,n) std::memcmp(a, b, n)
#define CHAOS_IL2CPP_MEMSET(d,c,n) std::memset(d, c, n)
#define CHAOS_IL2CPP_MEMMOVE(d,s,n) std::memmove(d, s, n)
#define CHAOS_IL2CPP_STRLEN(s)     std::strlen(s)
#define CHAOS_IL2CPP_STRCMP(a,b)   std::strcmp(a, b)
#define CHAOS_IL2CPP_MALLOC(s)     std::malloc(s)
#define CHAOS_IL2CPP_FREE(p)       std::free(p)
#define CHAOS_IL2CPP_REALLOC(p,s)  std::realloc(p, s)

// ── I/O (reference proof / test code) ──────────────────────
// NOTE: std::printf / std::fflush are forbidden project-wide.
// Use unified logging API:
//   CHAOS_IL2CPP_LOG_INFO("cat", "msg")          — human readable
//   CHAOS_IL2CPP_LOG_INFO_M("cat", "fmt{0}", a)  — formatted
//   CHAOS_IL2CPP_LOG_WRITE_RAW("json_line\n")    — machine protocol
//   CHAOS_IL2CPP_LOG_FLUSH_STDOUT()              — raw stdout flush
#define CHAOS_IL2CPP_FWRITE(buf, s, c, f) std::fwrite(buf, s, c, f)
#define CHAOS_IL2CPP_FPUTC(c, f)      std::fputc(c, f)

// ── Utility ─────────────────────────────────────────────────
#define CHAOS_IL2CPP_TO_STRING(v)     std::to_string(v)

// CPU pause hint (spinlock hint, yields pipeline slot).
#if defined(_MSC_VER)
    #if defined(_M_ARM64)
        #define CHAOS_IL2CPP_PAUSE_HINT()  __yield()
    #else
        #define CHAOS_IL2CPP_PAUSE_HINT()  _mm_pause()
    #endif
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define CHAOS_IL2CPP_PAUSE_HINT()  __asm__ __volatile__("yield" ::: "memory")
#else
    #define CHAOS_IL2CPP_PAUSE_HINT()  __builtin_ia32_pause()
#endif

// ── Threading ───────────────────────────────────────────────
#define CHAOS_IL2CPP_ONCE_FLAG                std::once_flag
#define CHAOS_IL2CPP_CALL_ONCE(f, init)       std::call_once(f, init)
#define CHAOS_IL2CPP_MUTEX                    std::mutex
#define CHAOS_IL2CPP_SHARED_MUTEX             std::shared_mutex
#define CHAOS_IL2CPP_RECURSIVE_MUTEX          std::recursive_timed_mutex
#define CHAOS_IL2CPP_RECURSIVE_LOCK_MUTEX     std::recursive_mutex
#define CHAOS_IL2CPP_LOCK_GUARD(M)            std::lock_guard<M>
#define CHAOS_IL2CPP_UNIQUE_LOCK(M)           std::unique_lock<M>
#define CHAOS_IL2CPP_SHARED_LOCK(M)           std::shared_lock<M>
#define CHAOS_IL2CPP_CONDITION_VARIABLE       std::condition_variable
#define CHAOS_IL2CPP_THREAD                   std::thread
#define CHAOS_IL2CPP_THIS_THREAD_SLEEP_FOR(t) std::this_thread::sleep_for(t)
#define CHAOS_IL2CPP_THIS_THREAD_GET_ID()     std::this_thread::get_id()

// ── Exceptions ──────────────────────────────────────────────
#define CHAOS_IL2CPP_RUNTIME_ERROR(msg)  std::runtime_error(msg)
#define CHAOS_IL2CPP_OUT_OF_RANGE(msg)   std::out_of_range(msg)
#define CHAOS_IL2CPP_INVALID_ARGUMENT(msg) std::invalid_argument(msg)

// ── Type traits ─────────────────────────────────────────────
#define CHAOS_IL2CPP_MAKE_UNSIGNED(T)    std::make_unsigned_t<T>
#define CHAOS_IL2CPP_IS_FLOATING_POINT(T) std::is_floating_point_v<T>
#define CHAOS_IL2CPP_IS_SIGNED(T)         std::is_signed_v<T>
#define CHAOS_IL2CPP_IS_UNSIGNED(T)       std::is_unsigned_v<T>
#define CHAOS_IL2CPP_IS_INTEGRAL(T)       std::is_integral_v<T>
#define CHAOS_IL2CPP_IS_SAME(T, U)        std::is_same_v<T, U>
#define CHAOS_IL2CPP_MOVE(expr)           std::move(expr)

// ── Atomic operations ───────────────────────────────────────
#define CHAOS_IL2CPP_ATOMIC(T)            std::atomic<T>
#define CHAOS_IL2CPP_MEMORY_ORDER_ACQUIRE std::memory_order_acquire
#define CHAOS_IL2CPP_MEMORY_ORDER_RELEASE std::memory_order_release
#define CHAOS_IL2CPP_MEMORY_ORDER_ACQ_REL std::memory_order_acq_rel
#define CHAOS_IL2CPP_MEMORY_ORDER_RELAXED std::memory_order_relaxed

// Atomic intrinsic helpers for Interlocked/Volatile codegen inline shapes.
// These are used by generated C++ code at the call site to emit platform
// intrinsic instructions directly, eliminating extern "C" ABI stub overhead.
#if defined(_MSC_VER)
  #include <intrin.h>
  // _Interlocked* intrinsics return the OLD value; arithmetic ops need adjustment.
  #pragma intrinsic(_InterlockedIncrement, _InterlockedDecrement, _InterlockedExchange,\
                    _InterlockedCompareExchange, _InterlockedExchangeAdd, _InterlockedAnd,\
                    _InterlockedOr, _InterlockedIncrement64, _InterlockedDecrement64,\
                    _InterlockedExchange64, _InterlockedCompareExchange64, _InterlockedExchangeAdd64,\
                    _ReadWriteBarrier, _mm_mfence)
  // Increment/Decrement/Add return the NEW value
  #define CHAOS_IL2CPP_ATOMIC_INC(ptr)       _InterlockedIncrement(reinterpret_cast<volatile long*>(ptr))
  #define CHAOS_IL2CPP_ATOMIC_DEC(ptr)       _InterlockedDecrement(reinterpret_cast<volatile long*>(ptr))
  #define CHAOS_IL2CPP_ATOMIC_INC64(ptr)     _InterlockedIncrement64(reinterpret_cast<volatile __int64*>(ptr))
  #define CHAOS_IL2CPP_ATOMIC_DEC64(ptr)     _InterlockedDecrement64(reinterpret_cast<volatile __int64*>(ptr))
  // Exchange returns the OLD value (correct for Interlocked.Exchange return semantics)
  #define CHAOS_IL2CPP_ATOMIC_XCHG(ptr, val)  _InterlockedExchange(reinterpret_cast<volatile long*>(ptr), static_cast<long>(val))
  #define CHAOS_IL2CPP_ATOMIC_XCHG64(ptr, val) _InterlockedExchange64(reinterpret_cast<volatile __int64*>(ptr), static_cast<__int64>(val))
  #define CHAOS_IL2CPP_ATOMIC_XCHGPTR(ptr, val) _InterlockedExchangePointer(reinterpret_cast<void* volatile*>(ptr), reinterpret_cast<void*>(val))
  // CompareExchange returns the OLD value (correct for Interlocked.CompareExchange)
  #define CHAOS_IL2CPP_ATOMIC_CAS(ptr, cmp, val)  _InterlockedCompareExchange(reinterpret_cast<volatile long*>(ptr), static_cast<long>(val), static_cast<long>(cmp))
  #define CHAOS_IL2CPP_ATOMIC_CAS64(ptr, cmp, val) _InterlockedCompareExchange64(reinterpret_cast<volatile __int64*>(ptr), static_cast<__int64>(val), static_cast<__int64>(cmp))
  #define CHAOS_IL2CPP_ATOMIC_CASPTR(ptr, cmp, val) _InterlockedCompareExchangePointer(reinterpret_cast<void* volatile*>(ptr), reinterpret_cast<void*>(val), reinterpret_cast<void*>(cmp))
  // Add returns the NEW value (Interlocked.Add returns new value)
  #define CHAOS_IL2CPP_ATOMIC_ADD(ptr, val)   (_InterlockedExchangeAdd(reinterpret_cast<volatile long*>(ptr), static_cast<long>(val)) + (val))
  #define CHAOS_IL2CPP_ATOMIC_ADD64(ptr, val) (_InterlockedExchangeAdd64(reinterpret_cast<volatile __int64*>(ptr), static_cast<__int64>(val)) + (val))
  // Memory barrier
  #define CHAOS_IL2CPP_ATOMIC_FENCE()         (_ReadWriteBarrier(), _mm_mfence())
  // Volatile load/store using acquire/release semantics
  #define CHAOS_IL2CPP_VOLATILE_LOAD(ptr)     (*(const volatile CHAOS_IL2CPP_INT32*)(ptr))
  #define CHAOS_IL2CPP_VOLATILE_STORE(ptr, v) (*(volatile CHAOS_IL2CPP_INT32*)(ptr) = (v))
  #define CHAOS_IL2CPP_VOLATILE_LOAD64(ptr)    (*(const volatile CHAOS_IL2CPP_INT64*)(ptr))
  #define CHAOS_IL2CPP_VOLATILE_STORE64(ptr, v) (*(volatile CHAOS_IL2CPP_INT64*)(ptr) = (v))
#else
  // GCC/Clang __atomic_* builtins — cross-platform, work on raw pointers.
  // Increment/Decrement/Add return the NEW value
  #define CHAOS_IL2CPP_ATOMIC_INC(ptr)        __atomic_add_fetch((ptr), 1, __ATOMIC_SEQ_CST)
  #define CHAOS_IL2CPP_ATOMIC_DEC(ptr)        __atomic_sub_fetch((ptr), 1, __ATOMIC_SEQ_CST)
  #define CHAOS_IL2CPP_ATOMIC_INC64(ptr)      __atomic_add_fetch((ptr), 1ll, __ATOMIC_SEQ_CST)
  #define CHAOS_IL2CPP_ATOMIC_DEC64(ptr)      __atomic_sub_fetch((ptr), 1ll, __ATOMIC_SEQ_CST)
  // Exchange returns the OLD value
  #define CHAOS_IL2CPP_ATOMIC_XCHG(ptr, val)   __atomic_exchange_n((ptr), (val), __ATOMIC_SEQ_CST)
  #define CHAOS_IL2CPP_ATOMIC_XCHG64(ptr, val) __atomic_exchange_n((ptr), (val), __ATOMIC_SEQ_CST)
  #define CHAOS_IL2CPP_ATOMIC_XCHGPTR(ptr, val) __atomic_exchange_n((ptr), (val), __ATOMIC_SEQ_CST)
  // CompareExchange: __sync version returns old value directly
  #define CHAOS_IL2CPP_ATOMIC_CAS(ptr, cmp, val)    __sync_val_compare_and_swap((ptr), (cmp), (val))
  #define CHAOS_IL2CPP_ATOMIC_CAS64(ptr, cmp, val)  __sync_val_compare_and_swap((ptr), (cmp), (val))
  #define CHAOS_IL2CPP_ATOMIC_CASPTR(ptr, cmp, val) __sync_val_compare_and_swap((ptr), (cmp), (val))
  // Add with __sync_fetch_and_add returns OLD value, add val to get NEW
  #define CHAOS_IL2CPP_ATOMIC_ADD(ptr, val)   __atomic_add_fetch((ptr), (val), __ATOMIC_SEQ_CST)
  #define CHAOS_IL2CPP_ATOMIC_ADD64(ptr, val) __atomic_add_fetch((ptr), (val), __ATOMIC_SEQ_CST)
  // Memory barrier
  #define CHAOS_IL2CPP_ATOMIC_FENCE()         __atomic_thread_fence(__ATOMIC_SEQ_CST)
  // Volatile load/store using acquire/release semantics
  #define CHAOS_IL2CPP_VOLATILE_LOAD(ptr)     __atomic_load_n((ptr), __ATOMIC_ACQUIRE)
  #define CHAOS_IL2CPP_VOLATILE_STORE(ptr, v) __atomic_store_n((ptr), (v), __ATOMIC_RELEASE)
  #define CHAOS_IL2CPP_VOLATILE_LOAD64(ptr)    __atomic_load_n((ptr), __ATOMIC_ACQUIRE)
  #define CHAOS_IL2CPP_VOLATILE_STORE64(ptr, v) __atomic_store_n((ptr), (v), __ATOMIC_RELEASE)
#endif

// ── Chrono ──────────────────────────────────────────────────
#define CHAOS_IL2CPP_CHRONO_STEADY_CLOCK            std::chrono::steady_clock
#define CHAOS_IL2CPP_CHRONO_DURATION(T, P)          std::chrono::duration<T, P>
#define CHAOS_IL2CPP_CHRONO_NANOSECONDS(v)          std::chrono::nanoseconds(v)
#define CHAOS_IL2CPP_CHRONO_DURATION_CAST(T, expr)  std::chrono::duration_cast<T>(expr)
#define CHAOS_IL2CPP_CHRONO_MILLISECONDS(v)         std::chrono::milliseconds(v)
#define CHAOS_IL2CPP_CHRONO_MILLI                   std::milli

// ── Math ────────────────────────────────────────────────────
#define CHAOS_IL2CPP_SQRT(x)       std::sqrt(x)
#define CHAOS_IL2CPP_CEIL(x)       std::ceil(x)
#define CHAOS_IL2CPP_FLOOR(x)      std::floor(x)
#define CHAOS_IL2CPP_POW(x, y)     std::pow(x, y)
#define CHAOS_IL2CPP_TAN(x)        std::tan(x)
#define CHAOS_IL2CPP_EXP(x)        std::exp(x)
#define CHAOS_IL2CPP_FABS(x)       std::fabs(x)
#define CHAOS_IL2CPP_ISNAN(x)      std::isnan(x)
#define CHAOS_IL2CPP_ISFINITE(x)   std::isfinite(x)
#define CHAOS_IL2CPP_ISINF(x)      std::isinf(x)
#define CHAOS_IL2CPP_FMOD(x, y)    std::fmod(x, y)
#define CHAOS_IL2CPP_TRUNC(x)      std::trunc(x)
#define CHAOS_IL2CPP_LDEXP(x, e)   std::ldexp(x, e)
#define CHAOS_IL2CPP_FREXP(x, e)   std::frexp(x, e)
#define CHAOS_IL2CPP_FMIN(x,y)     std::fmin(x, y)
#define CHAOS_IL2CPP_FMAX(x,y)     std::fmax(x, y)

// ── Algorithm ──────────────────────────────────────────────
#define CHAOS_IL2CPP_MIN(a,b)          (std::min)(a, b)
#define CHAOS_IL2CPP_FIND(b,e,v)       std::find(b, e, v)
#define CHAOS_IL2CPP_LOWER_BOUND(...) std::lower_bound(__VA_ARGS__)
#define CHAOS_IL2CPP_IFSTREAM                 std::ifstream
#define CHAOS_IL2CPP_ATOI(s)                   std::atoi(s)
#define CHAOS_IL2CPP_STRNCMP(a, b, n)          std::strncmp(a, b, n)
#define CHAOS_IL2CPP_ISTREAMBUF_ITERATOR(T)    std::istreambuf_iterator<T>
#define CHAOS_IL2CPP_IOS_BINARY                std::ios::binary
#define CHAOS_IL2CPP_NOTROW                    std::nothrow

// ── Memory allocation ──────────────────────────────────────
//
// == Three allocation domains ==
//
// GC domain   — managed objects/arrays/strings (GcAllocate placement-new)
// Domain      — per-module metadata / runtime internals (IDomainHeap)
// Raw         — temp buffers, vtable arrays, non-GC structures (malloc/free)
//
// == Critical constraints ==
//
// 1. NO global operator new/delete overrides — generated code is imported
//    into game engine source code, global overrides would pollute the
//    engine's allocation behavior.
// 2. Codegen output MUST use CHAOS_IL2CPP_NEW_GC / CHAOS_IL2CPP_NEW_GC_ARRAY
//    instead of raw "new T{}" / "new T[N]".
// 3. No cross-domain free — each domain's memory must be released by its
//    own deallocation mechanism.
//
// ========== GC domain — managed object allocation ==========
// CHAOS_IL2CPP_NEW_GC* macros → GcAllocateFast / GcAllocateAtomicFast
// (__forceinline fast path in gc_alloc_stubs.h, no PROFILE_SCOPE / global atoms)
#define CHAOS_IL2CPP_NEW_GC(T, ...) \
    ::new (chaos::il2cpp::runtime_core::GcAllocateFast(sizeof(T))) T{__VA_ARGS__}

// GC non-scanned allocation (pointer-free data, e.g. string UTF-8 bytes)
#define CHAOS_IL2CPP_NEW_GC_ATOMIC(T, ...) \
    ::new (chaos::il2cpp::runtime_core::GcAllocateAtomicFast(sizeof(T))) T{__VA_ARGS__}

// Zero-initialized POD array in GC heap (memory already zeroed by GC_MALLOC)
#define CHAOS_IL2CPP_NEW_GC_ARRAY(T, count) \
    static_cast<T*>(chaos::il2cpp::runtime_core::GcAllocateFast(sizeof(T) * (count)))

// Raw GC allocation for contiguous arrays (single allocation: header + element data).
// Returns untyped void* — caller must placement-initialize the header fields.
// Uses NoZero variant — the caller immediately writes all header fields, so the
// GC allocator's std::memset(0) would be wasted work.
#define CHAOS_IL2CPP_MALLOC_GC(size) \
    static_cast<void*>(chaos::il2cpp::runtime_core::GcAllocateFastNoZero(size))

// Atomic (non-scanned) variant of CHAOS_IL2CPP_MALLOC_GC for pointer-free element data.
#define CHAOS_IL2CPP_MALLOC_ATOMIC_GC(size) \
    static_cast<void*>(chaos::il2cpp::runtime_core::GcAllocateAtomicFastNoZero(size))

// ========== Domain domain — per-module metadata ==========
// Allocate through the current TLS domain heap.  Each allocation is tagged
// with the originating heap pointer so that Free() routes to the correct
// heap without depending on thread-local domain state — cross-domain safe
// for 200+ DLL hotupdate scenarios.
// Overhead: one pointer (8 bytes) per allocation.
//
// Normal lifecycle: allocated once, never individually freed (heap->Destroy()
// handles bulk release on module unload).  The tag only matters on error
// paths where individual Free() is needed.
//
// Falls back to tagged std::malloc when no domain is active (AOT root).

// Tagged allocation through current domain (default).  Returns pointer
// to user data after the routing header.
#define CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(size)                             \
    ::chaos::il2cpp::memory_domain::DomainCurrentAllocateTagged(size)

// Free a tagged allocation.  Reads the header to route to the originating
// heap — safe from any thread, any domain context.
#define CHAOS_IL2CPP_DOMAIN_CURRENT_FREE(ptr)                                  \
    ::chaos::il2cpp::memory_domain::DomainFreeTagged(const_cast<void*>(static_cast<const void*>(ptr)))

// Reallocate through the current TLS domain heap, preserving the routing
// tag so that DomainFreeTagged() can find the correct heap regardless of
// the calling thread's domain context.
#define CHAOS_IL2CPP_DOMAIN_CURRENT_REALLOC(ptr, new_size)                       \
    ::chaos::il2cpp::memory_domain::DomainCurrentReallocateTagged(               \
        const_cast<void*>(static_cast<const void*>(ptr)), new_size)

// String duplicate through current domain (uses tagged allocation internally).
#define CHAOS_IL2CPP_DOMAIN_CURRENT_STRDUP(src)                                \
    ::chaos::il2cpp::runtime_core::DomainStrDup(src)

// Placement-new through current domain (uses ALLOCATE internally).
#define CHAOS_IL2CPP_DOMAIN_CURRENT_NEW(T, ...)                                \
    ::new (CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(sizeof(T))) T{__VA_ARGS__}

// ========== Raw domain — temp / non-GC structures ==========
// Existing macros: NEW/NEW_ARRAY/CALLOC only.  MALLOC/FREE/REALLOC are defined
// once above (unified allocation section) — DO NOT redefine here (a silent
// re-definition of identical replacement was removed; keep a single source of
// truth for the raw allocator macros).
#define CHAOS_IL2CPP_NEW(T)          new T
#define CHAOS_IL2CPP_NEW_ARRAY(T, N) new T[N]
#define CHAOS_IL2CPP_CALLOC(n,s)   std::calloc(n, s)

// ── Numeric limits ─────────────────────────────────────────
// Parentheses around min/max prevent collision with Windows macros (NOMINMAX not guaranteed).
#define CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(T) (std::numeric_limits<T>::min)()
#define CHAOS_IL2CPP_NUMERIC_LIMITS_MAX(T) (std::numeric_limits<T>::max)()
#define CHAOS_IL2CPP_NUMERIC_LIMITS_INFINITY(T) (std::numeric_limits<T>::infinity)()
#define CHAOS_IL2CPP_NUMERIC_LIMITS_QUIET_NAN(T) (std::numeric_limits<T>::quiet_NaN)()

	// ── Explicit *_[MIN|MAX] constants (for generated C++ overflow checks) ──
	#define CHAOS_IL2CPP_INT8_MIN     (std::numeric_limits<CHAOS_IL2CPP_INT8>::min)()
	#define CHAOS_IL2CPP_INT8_MAX     (std::numeric_limits<CHAOS_IL2CPP_INT8>::max)()
	#define CHAOS_IL2CPP_INT16_MIN    (std::numeric_limits<CHAOS_IL2CPP_INT16>::min)()
	#define CHAOS_IL2CPP_INT16_MAX    (std::numeric_limits<CHAOS_IL2CPP_INT16>::max)()
	#define CHAOS_IL2CPP_INT32_MIN    (std::numeric_limits<CHAOS_IL2CPP_INT32>::min)()
	#define CHAOS_IL2CPP_INT32_MAX    (std::numeric_limits<CHAOS_IL2CPP_INT32>::max)()
	#define CHAOS_IL2CPP_INT64_MIN    (std::numeric_limits<CHAOS_IL2CPP_INT64>::min)()
	#define CHAOS_IL2CPP_INT64_MAX    (std::numeric_limits<CHAOS_IL2CPP_INT64>::max)()
	#define CHAOS_IL2CPP_UINT8_MAX    (std::numeric_limits<CHAOS_IL2CPP_UINT8>::max)()
	#define CHAOS_IL2CPP_UINT16_MAX   (std::numeric_limits<CHAOS_IL2CPP_UINT16>::max)()
	#define CHAOS_IL2CPP_UINT32_MAX   (std::numeric_limits<CHAOS_IL2CPP_UINT32>::max)()
	#define CHAOS_IL2CPP_UINT64_MAX   (std::numeric_limits<CHAOS_IL2CPP_UINT64>::max)()
	#define CHAOS_IL2CPP_INTPTR_MIN   (std::numeric_limits<CHAOS_IL2CPP_INTPTR>::min)()
	#define CHAOS_IL2CPP_INTPTR_MAX   (std::numeric_limits<CHAOS_IL2CPP_INTPTR>::max)()
	#define CHAOS_IL2CPP_UINTPTR_MAX  (std::numeric_limits<CHAOS_IL2CPP_UINTPTR>::max)()

// ── Indirect load/store templates ────────────────────────────
#define CHAOS_IL2CPP_RAW_POINTER_TAG  (CHAOS_IL2CPP_UINTPTR(1) << 63)

// Resolve a native int slot pointer — used by chaos_load_indirect / chaos_store_indirect.
namespace chaos::il2cpp::common {

inline CHAOS_IL2CPP_INTPTR* chaos_resolve_native_int_slot(CHAOS_IL2CPP_INTPTR chaos_value)
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_value));
}

template<typename T>
inline T chaos_load_indirect(CHAOS_IL2CPP_INTPTR address) {
    if (address == 0) { CHAOS_IL2CPP_ABORT(); }
    auto addr = static_cast<CHAOS_IL2CPP_UINTPTR>(address);
    if ((addr & CHAOS_IL2CPP_RAW_POINTER_TAG) != 0) {
        return *reinterpret_cast<const T*>(addr & ~CHAOS_IL2CPP_RAW_POINTER_TAG);
    }
    return static_cast<T>(*chaos_resolve_native_int_slot(address));
}

template<typename T>
inline void chaos_store_indirect(CHAOS_IL2CPP_INTPTR address, T value) {
    if (address == 0) { CHAOS_IL2CPP_ABORT(); }
    auto addr = static_cast<CHAOS_IL2CPP_UINTPTR>(address);
    if ((addr & CHAOS_IL2CPP_RAW_POINTER_TAG) != 0) {
        *reinterpret_cast<T*>(addr & ~CHAOS_IL2CPP_RAW_POINTER_TAG) = value;
        return;
    }
    *chaos_resolve_native_int_slot(address) = static_cast<CHAOS_IL2CPP_INTPTR>(value);
}

}  // namespace chaos::il2cpp::common

// Expose at global scope so all code (including generated code in anonymous
// namespaces) can use these helpers without qualification.
using namespace chaos::il2cpp::common;

#endif  // CHAOS_IL2CPP_COMMON_NATIVE_TYPES_H_
