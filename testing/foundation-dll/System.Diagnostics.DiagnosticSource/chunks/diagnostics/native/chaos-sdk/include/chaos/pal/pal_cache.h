#ifndef CHAOS_IL2CPP_PAL_CACHE_H_
#define CHAOS_IL2CPP_PAL_CACHE_H_

// ── Instruction cache coherency ───────────────────────────────────────
// ARM64 requires explicit I-cache invalidation after writing executable
// memory (JIT code generation, trampoline patching).  x86 has hardware
// I-cache consistency and needs no software flush.
// ============================================================================

#include <chaos/pal/pal_types.h>

#include <cstddef>

namespace chaos::il2cpp::pal {

/// Flush the instruction cache for a range of memory.
/// Required after writing executable code (JIT, precode trampolines) on
/// ARM64.  No-op on x86 (hardware I-cache coherency).
/// @param addr  Start address of the range (does not need to be aligned).
/// @param size  Size of the range in bytes.
void PalFlushInstructionCache(void* addr, size_t size) noexcept;

}  // namespace chaos::il2cpp::pal

#endif  // CHAOS_IL2CPP_PAL_CACHE_H_
