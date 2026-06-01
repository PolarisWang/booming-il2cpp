#ifndef CHAOS_IL2CPP_PAL_MEM_H_
#define CHAOS_IL2CPP_PAL_MEM_H_

// ── Virtual memory management ────────────────────────────────────────
// Platform-independent wrappers for VirtualAlloc/mmap, VirtualFree/munmap,
// and VirtualProtect/mprotect.  Large page support included.
// ============================================================================

#include <chaos/pal/pal_types.h>

#include <cstddef>

namespace chaos::il2cpp::pal {

/// Allocate a region of read-write virtual memory.
/// Returns nullptr on failure.
void* PalVirtualAlloc(size_t size) noexcept;

/// Free a region previously allocated with PalVirtualAlloc.
/// @param ptr  Base address of the region.
/// @param size  Size passed to PalVirtualAlloc (used on POSIX for munmap).
bool PalVirtualFree(void* ptr, size_t size) noexcept;

/// Change the protection of a previously allocated region.
bool PalVirtualProtect(void* ptr, size_t size, PalMemoryProtection prot) noexcept;

/// Commit a previously reserved region (Windows: MEM_COMMIT; POSIX: no-op).
/// On POSIX, this is a no-op since mmap already commits.  Returns true.
bool PalVirtualCommit(void* ptr, size_t size) noexcept;

/// Decommit a previously committed region (Windows: MEM_DECOMMIT; POSIX: no-op).
/// On POSIX this is a hint that the pages may be reused.
bool PalVirtualDecommit(void* ptr, size_t size) noexcept;

/// Return the system page size (e.g. 4096 on most systems).
size_t PalGetPageSize() noexcept;

/// Return the minimum large page size, or 0 if large pages are not supported.
/// On Windows: GetLargePageMinimum().  On Linux: 2MB (if MAP_HUGETLB works).
size_t PalGetLargePageSize() noexcept;

/// Allocate a region using large pages (2MB on Linux, platform-dependent on Windows).
/// Caller must check PalGetLargePageSize() > 0 first.
void* PalVirtualAllocLarge(size_t size) noexcept;

// ── Memory status ────────────────────────────────────────────────────

/// Platform-agnostic memory status data.
struct PalMemoryStatus {
    int64_t total_phys;   ///< Total physical memory in bytes
    int64_t avail_phys;   ///< Available physical memory in bytes
};

/// Query the OS for current physical memory status.
/// On Windows: calls GlobalMemoryStatusEx.
/// On Linux: reads /proc/meminfo.
/// Returns zeros on failure or unsupported platform.
void PalGetMemoryStatus(PalMemoryStatus& out) noexcept;

}  // namespace chaos::il2cpp::pal

#endif  // CHAOS_IL2CPP_PAL_MEM_H_
