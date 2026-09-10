// asan_interface.h — AddressSanitizer integration layer
//
// Provides CHAOS_IL2CPP_ASAN_UNPOISON / CHAOS_IL2CPP_ASAN_POISON macros
// that compile to zero instructions when AddressSanitizer is disabled.
// When ASan is active, these forward to __asan_unpoison_memory_region
// and __asan_poison_memory_region respectively.
//
// Include this header instead of <sanitizer/asan_interface.h> directly
// to maintain clean conditional compilation in both ASan and non-ASan builds.

#ifndef CHAOS_IL2CPP_ASAN_INTERFACE_H_
#define CHAOS_IL2CPP_ASAN_INTERFACE_H_

#include <cstddef>

// Detect AddressSanitizer at compile time and define CHAOS_IL2CPP_NO_ASAN.
// ASan is enabled by MSVC's __SANITIZE_ADDRESS__ or Clang's
// __has_feature(address_sanitizer); when either fires we also pull in the
// ASan runtime header for __asan_* helpers.
#if defined(__SANITIZE_ADDRESS__)
#  include <sanitizer/asan_interface.h>
#  define CHAOS_IL2CPP_ASAN_ENABLED 1
#elif defined(__has_feature)
#  if __has_feature(address_sanitizer)
#    include <sanitizer/asan_interface.h>
#    define CHAOS_IL2CPP_ASAN_ENABLED 1
#  endif
#endif

// CHAOS_IL2CPP_NO_ASAN = the no_sanitize("address") attribute for ONE function.
// It is applied UNCONDITIONALLY on compilers that support it (independent of
// CHAOS_IL2CPP_ASAN_ENABLED): if some build enables ASan through a macro we
// don't recognize, the no-check helpers MUST still shed instrumentation, or the
// cross-thread stack-redzone false positives silently return with no way for
// the caller to detect the fix has regressed (review #6).  A compiler that has
// no ASan simply ignores the attribute token and the helpers behave as plain
// reads/writes.
#if defined(_MSC_VER)
#  define CHAOS_IL2CPP_NO_ASAN __declspec(no_sanitize_address)
#elif defined(__clang__) || defined(__GNUC__)
#  define CHAOS_IL2CPP_NO_ASAN __attribute__((no_sanitize("address")))
#else
#  define CHAOS_IL2CPP_NO_ASAN
#endif

// CHAOS_IL2CPP_NO_ASAN must always be defined (even when ASan is off).
#if !defined(CHAOS_IL2CPP_NO_ASAN)
#  define CHAOS_IL2CPP_NO_ASAN
#endif

namespace chaos {
namespace il2cpp {
namespace common {

/// Unpoison @a size bytes starting at @a addr (mark as accessible).
/// Call AFTER allocating GC heap memory so ASan knows the region is valid.
inline void AsanUnpoison(void* addr, std::size_t size) noexcept {
#if defined(CHAOS_IL2CPP_ASAN_ENABLED)
    __asan_unpoison_memory_region(addr, size);
#else
    (void)addr;
    (void)size;
#endif
}

/// Poison @a size bytes starting at @a addr (mark as inaccessible).
/// Call AFTER the GC has reclaimed memory (free/sweep) so ASan detects
/// any subsequent access as a use-after-free error.
inline void AsanPoison(void* addr, std::size_t size) noexcept {
#if defined(CHAOS_IL2CPP_ASAN_ENABLED)
    __asan_poison_memory_region(addr, size);
#else
    (void)addr;
    (void)size;
#endif
}

/// Read a pointer from @a addr without ASan instrumentation.
/// Use ONLY when @a addr is guaranteed to be ASan-poisoned (e.g. a stack-frame
/// redzone on a suspended thread) — never as the default read for all root
/// slots, or genuine heap OOB/UAF on a live slot is silently masked (review #2).
/// Prefer AsanReadPtrProbe(), which gates on __asan_address_is_poisoned and
/// keeps live slots instrumented.  The tiny function body keeps the
/// no_sanitize_address attribute scope minimal.
CHAOS_IL2CPP_NO_ASAN
inline void* AsanReadPtrNoCheck(void* addr) noexcept {
    return *static_cast<void**>(addr);
}

/// Write @a val to the pointer at @a addr without ASan instrumentation.
/// Use ONLY when @a addr is guaranteed to be ASan-poisoned (a suspended thread's
/// stack redzone).  Prefer AsanWritePtrProbe() so live root slots stay
/// instrumented and a real out-of-bounds write into a root slot is still caught.
/// The emitted store is a plain store; only ASan instrumentation is elided.
/// NOTE (review #9): this writes into ANOTHER thread's stack with no runtime
/// guard here — callers must hold a global STW safepoint so the target thread is
/// truly suspended and cannot concurrently read/write the slot.
CHAOS_IL2CPP_NO_ASAN
inline void AsanWritePtrNoCheck(void* addr, void* val) noexcept {
    *static_cast<void**>(addr) = val;
}

/// Read the pointer at @a addr, shedding ASan instrumentation ONLY when the slot
/// is genuinely poisoned (cross-thread stack redzone).  Ordinary live stack slots
/// are read with instrumentation intact, so a real OOB/UAF into a root slot is
/// still reported (review #2/#4).  Never instrument-free on the fast path.
inline void* AsanReadPtrProbe(void* addr) noexcept {
#if defined(CHAOS_IL2CPP_ASAN_ENABLED)
    if (__asan_address_is_poisoned(addr)) {
        return AsanReadPtrNoCheck(addr);
    }
#else
    (void)0;
#endif
    return *static_cast<void**>(addr);
}

/// Write @a val to the pointer at @a addr, shedding ASan instrumentation ONLY
/// for genuinely poisoned slots (suspended thread's stack redzone).  Live slots
/// are written with instrumentation intact (review #2/#4).
inline void AsanWritePtrProbe(void* addr, void* val) noexcept {
#if defined(CHAOS_IL2CPP_ASAN_ENABLED)
    if (__asan_address_is_poisoned(addr)) {
        AsanWritePtrNoCheck(addr, val);
        return;
    }
#else
    (void)0;
#endif
    *static_cast<void**>(addr) = val;
}

} // namespace common
} // namespace il2cpp
} // namespace chaos

// Preprocessor macros for header hot-path code where even the inline
// function call should be elided at the preprocessing level.
// These are intended for use in allocation fast paths (e.g. gc_region.h).
#if defined(CHAOS_IL2CPP_ASAN_ENABLED)
#  define CHAOS_IL2CPP_ASAN_UNPOISON(addr, size) \
       __asan_unpoison_memory_region((addr), (size))
#  define CHAOS_IL2CPP_ASAN_POISON(addr, size) \
       __asan_poison_memory_region((addr), (size))
#else
#  define CHAOS_IL2CPP_ASAN_UNPOISON(addr, size) ((void)0)
#  define CHAOS_IL2CPP_ASAN_POISON(addr, size)   ((void)0)
#endif

#endif // CHAOS_IL2CPP_ASAN_INTERFACE_H_
