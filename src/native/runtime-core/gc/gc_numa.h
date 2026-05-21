#ifndef CHAOS_IL2CPP_GC_NUMA_H_
#define CHAOS_IL2CPP_GC_NUMA_H_

/// @file gc_numa.h
/// NUMA-aware memory allocation for the GC.
///
/// Provides platform-abstraction APIs for allocating virtual memory on
/// a specific NUMA node and querying NUMA topology.
///
/// == Platform support ==
/// - Windows: VirtualAllocExNuma (requires PRO access)
/// - Linux: libnuma mbind / get_mempolicy
/// - Other: stub (single-node fallback)
///
/// == Performance ==
/// These functions are called during page allocation (not on the hot
/// allocation path), so the overhead of node-local allocation is
/// amortized over many object allocations within a page.

#include <chaos/native_types.h>

namespace chaos::il2cpp::runtime_core {

/// Initialize NUMA subsystem.  Safe to call multiple times.
void GcNumaInit() noexcept;

/// Get the number of NUMA nodes in the system.
/// Returns 1 on non-NUMA systems or when NUMA is not supported.
int GcNumaNodeCount() noexcept;

/// Get the NUMA node for the current thread.
/// Returns 0 on single-node systems.
int GcNumaCurrentNode() noexcept;

/// Get the NUMA node for a given virtual address.
/// Returns 0 on single-node systems or when the node cannot be determined.
int GcNumaNodeOfAddress(const void* addr) noexcept;

/// Allocate virtual memory on a specific NUMA node.
/// @param size  Number of bytes to allocate.
/// @param node  Target NUMA node (0 to GcNumaNodeCount()-1).
/// @returns  Allocated pointer, or nullptr on failure.
void* GcNumaVirtualAlloc(CHAOS_IL2CPP_SIZE size, int node) noexcept;

/// Free memory allocated by GcNumaVirtualAlloc.
/// @param ptr   Pointer to free (must be from GcNumaVirtualAlloc).
/// @param size  Size passed to GcNumaVirtualAlloc (for region tracking).
void GcNumaVirtualFree(void* ptr, CHAOS_IL2CPP_SIZE size) noexcept;

/// Bind the calling thread to a specific NUMA node.
/// On Windows: uses SetThreadGroupAffinity.
/// On Linux: uses pthread_setaffinity_np.
/// On single-node systems or unsupported platforms: no-op.
/// @param node  Target NUMA node (0 to GcNumaNodeCount()-1).
void GcNumaBindThread(int node) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_NUMA_H_
