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

// Detect AddressSanitizer at compile time.
// MSVC defines __SANITIZE_ADDRESS__ when /fsanitize=address is active.
// Clang defines __has_feature(address_sanitizer) for the same purpose.
#if defined(__SANITIZE_ADDRESS__)
#  include <sanitizer/asan_interface.h>
#  define CHAOS_IL2CPP_ASAN_ENABLED 1
#  if defined(_MSC_VER)
#    define CHAOS_IL2CPP_NO_ASAN __declspec(no_sanitize_address)
#  elif defined(__clang__) || defined(__GNUC__)
#    define CHAOS_IL2CPP_NO_ASAN __attribute__((no_sanitize("address")))
#  else
#    define CHAOS_IL2CPP_NO_ASAN
#  endif
#elif defined(__has_feature)
#  if __has_feature(address_sanitizer)
#    include <sanitizer/asan_interface.h>
#    define CHAOS_IL2CPP_ASAN_ENABLED 1
#    if defined(_MSC_VER)
#      define CHAOS_IL2CPP_NO_ASAN __declspec(no_sanitize_address)
#    elif defined(__clang__) || defined(__GNUC__)
#      define CHAOS_IL2CPP_NO_ASAN __attribute__((no_sanitize("address")))
#    else
#      define CHAOS_IL2CPP_NO_ASAN
#    endif
#  endif
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
/// Used by conservative stack scanning to read from stack addresses
/// that ASan considers poisoned (stack redzones between frames).
/// The tiny function body ensures the no_sanitize_address attribute
/// scope is minimal — all surrounding code remains fully instrumented.
CHAOS_IL2CPP_NO_ASAN
inline void* AsanReadPtrNoCheck(void* addr) noexcept {
    return *static_cast<void**>(addr);
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
