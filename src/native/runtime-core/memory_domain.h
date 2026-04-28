#ifndef CHAOS_IL2CPP_MEMORY_DOMAIN_H_
#define CHAOS_IL2CPP_MEMORY_DOMAIN_H_

#include "runtime_abi.h"

#include <chaos/native_types.h>

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::memory_domain {

/// Opaque identifier for a memory domain.
/// Zero is reserved as the invalid/unset sentinel.
using DomainId = CHAOS_IL2CPP_UINT32;

constexpr DomainId kDomainIdInvalid = 0u;

// -----------------------------------------------------------------------
// IDomainHeap — pluggable heap strategy for each domain.
//
// Different strategies can be swapped in without changing the domain model:
//   - SegregatedHeap:   per-domain Win32 HeapCreate / HeapAlloc
//   - ArenaHeap:        bump-allocated arena, O(1) destroy
//   - PassthroughHeap:  delegates to process-level malloc/free
// -----------------------------------------------------------------------
class IDomainHeap {
public:
    virtual ~IDomainHeap() = default;

    /// Allocate @a size bytes through this heap.
    /// Returns nullptr on allocation failure.
    virtual void* Allocate(CHAOS_IL2CPP_SIZE size) = 0;

    /// Reallocate a previously allocated block to @a new_size.
    /// Returns nullptr on failure; the original block remains valid.
    virtual void* Reallocate(void* ptr, CHAOS_IL2CPP_SIZE new_size) = 0;

    /// Free a block previously allocated through this heap.
    virtual void Free(void* ptr) = 0;

    /// Destroy the entire heap, releasing all memory without calling
    /// individual Free() for each allocation.  After this call the heap
    /// object is no longer usable and should be destroyed.
    virtual void Destroy() = 0;
};

// -----------------------------------------------------------------------
// MemoryDomain — per-module allocation domain.
//
// Each loaded module (AOT assembly, hot-update DLL) gets its own domain
// with an independent heap.  Marshal allocations are tagged with the
// domain id so that cross-module free() can correctly attribute the
// deallocation back to the originating domain.
// -----------------------------------------------------------------------
struct MemoryDomain {
    DomainId      domain_id;
    const char*   module_name;       ///< e.g. "System.Private.CoreLib"
    CHAOS_IL2CPP_UINT32 module_kind;       ///< 0 = AOT, 1 = HotUpdate

    IDomainHeap*  heap;              ///< Owned by this domain (destroyed on unregister)

    /// Tracks total live allocation bytes (used for quotas / diagnostics).
    CHAOS_IL2CPP_INT64  current_usage;
    CHAOS_IL2CPP_INT64  peak_usage;
    CHAOS_IL2CPP_INT64  usage_limit;       ///< 0 = unlimited

    bool          is_unloaded;       ///< Set to true when UnregisterMemoryDomain is called.
};

// -----------------------------------------------------------------------
// Heap factory — users can override these to provide custom heap strategies.
// -----------------------------------------------------------------------
using HeapFactoryFn = IDomainHeap* (*)(const MemoryDomain* domain, void* user_data);
using HeapFactoryUserData = void*;

/// Sets a process-wide heap factory.  When null (the default), the built-in
/// SegregatedHeap is used.  Call before any RegisterMemoryDomain call.
void SetHeapFactory(HeapFactoryFn factory, HeapFactoryUserData user_data);
HeapFactoryFn GetHeapFactory(HeapFactoryUserData* out_user_data);

// -----------------------------------------------------------------------
// Domain lifecycle
// -----------------------------------------------------------------------

/// Parameters for RegisterMemoryDomain.
struct DomainInit {
    const char*   module_name;        ///< Must outlive the domain (typically a string literal).
    CHAOS_IL2CPP_UINT32 module_kind;
    CHAOS_IL2CPP_INT64  usage_limit;        ///< 0 = unlimited
    const RuntimeConfig* runtime_config;  ///< Optional — provides default allocator
};

/// Register a new memory domain.  Returns the domain id on success,
/// kDomainIdInvalid on failure.
DomainId RegisterMemoryDomain(const DomainInit& init);

/// Look up a domain by id.  Returns nullptr if the domain does not exist
/// or has been unloaded.
MemoryDomain* FindDomainById(DomainId domain_id);

/// Look up a domain by name.  Returns nullptr if not found.
MemoryDomain* FindDomainByName(const char* module_name);

/// Unregister and destroy a domain.  Calls heap->Destroy() to release
/// all memory allocated through this domain.  All subsequent lookups by
/// id will return nullptr.
/// Returns true on success.
bool UnregisterMemoryDomain(DomainId domain_id);

// -----------------------------------------------------------------------
// Thread-local domain stack (for tracking "who is allocating").
// -----------------------------------------------------------------------

/// Push @a domain onto the calling thread's domain stack.
/// @returns the stack depth after push (0-based).  The sentinel null is
///          silently accepted (it pushes a no-op marker so the pop balance
///          works correctly).
int  PushDomain(MemoryDomain* domain);

/// Pop the most recently pushed domain from the calling thread's stack.
void PopDomain(int depth_before_push);

/// Returns the current (top-of-stack) domain for the calling thread.
/// May be nullptr when no domain has been pushed.
MemoryDomain* CurrentDomain();

// -----------------------------------------------------------------------
// Convenience: RAII scope guard for PushDomain / PopDomain.
// -----------------------------------------------------------------------
class DomainScope {
public:
    explicit DomainScope(MemoryDomain* domain)
        : depth_(PushDomain(domain)) {}

    ~DomainScope() { PopDomain(depth_); }

    DomainScope(const DomainScope&) = delete;
    DomainScope& operator=(const DomainScope&) = delete;

private:
    int depth_;
};

}  // namespace chaos::il2cpp::memory_domain

#endif  // CHAOS_IL2CPP_MEMORY_DOMAIN_H_
