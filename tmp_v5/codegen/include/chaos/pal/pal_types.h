#ifndef CHAOS_IL2CPP_PAL_TYPES_H_
#define CHAOS_IL2CPP_PAL_TYPES_H_

// ── PAL common type definitions ───────────────────────────────────────
// Shared by all PAL subsystem headers.  No platform-specific macros here.
// ============================================================================

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::pal {

// Opaque handle types — consumers never inspect the contents.
// Actual types differ per-platform (e.g. HANDLE vs pthread_cond_t*),
// but callers always go through the PAL API.
using PalLibraryPtr    = void*;
using PalProcAddress   = void*;
using PalThreadHandle  = void*;   // HANDLE on Win32, pthread_t* on POSIX
using PalEventHandle   = void*;   // HANDLE on Win32, struct* on POSIX

// ── Memory protection flags ──────────────────────────────────────────
enum PalMemoryProtection : uint32_t {
    kPalMemNone          = 0,
    kPalMemRead          = 1,
    kPalMemWrite         = 2,
    kPalMemExec          = 4,
    kPalMemReadWrite     = kPalMemRead | kPalMemWrite,
    kPalMemReadExec      = kPalMemRead | kPalMemExec,
    kPalMemReadWriteExec = kPalMemRead | kPalMemWrite | kPalMemExec,
};

// ── Timeout sentinel (infinite wait) ──────────────────────────────────
static constexpr int32_t kPalWaitInfinite = -1;

}  // namespace chaos::il2cpp::pal

#endif  // CHAOS_IL2CPP_PAL_TYPES_H_
