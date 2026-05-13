#ifndef CHAOS_IL2CPP_MEMORY_DOMAIN_H_
#define CHAOS_IL2CPP_MEMORY_DOMAIN_H_

#include <runtime_abi.h>

#include <chaos/native_types.h>
#include <chaos/log.h>

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::memory_domain {

/// Opaque identifier for a memory domain.
/// Zero is reserved as the invalid/unset sentinel.
using DomainId = CHAOS_IL2CPP_UINT32;

constexpr DomainId kDomainIdInvalid = 0u;

struct MemoryDomain;

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
    /// If usage_limit is set on the owning domain and would be exceeded,
    /// returns nullptr without allocating.
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

    /// Set the owning MemoryDomain (for usage tracking).
    /// Called by RegisterMemoryDomain after heap construction.
    void SetOwner(MemoryDomain* domain) { owner_ = domain; }

    /// Get the owning MemoryDomain.
    MemoryDomain* GetOwner() const { return owner_; }

    /// Get the domain id of the owning MemoryDomain.
    DomainId GetDomainId() const;

protected:
    MemoryDomain* owner_ = nullptr;

    /// Track an allocation of @a size bytes.  Returns false if the
    /// domain's usage_limit would be exceeded (caller should fail).
    bool TrackAlloc(CHAOS_IL2CPP_SIZE size) noexcept;

    /// Track a free of @a size bytes.
    void TrackFree(CHAOS_IL2CPP_SIZE size) noexcept;
};

// ── MemoryDomain (full definition) ───────────────────────────────────
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

// ── IDomainHeap inline implementations (after MemoryDomain is complete) ──

inline DomainId IDomainHeap::GetDomainId() const {
    return owner_ ? owner_->domain_id : kDomainIdInvalid;
}

inline bool IDomainHeap::TrackAlloc(CHAOS_IL2CPP_SIZE size) noexcept {
    if (owner_ == nullptr) return true;
    CHAOS_IL2CPP_INT64 new_usage = owner_->current_usage + static_cast<CHAOS_IL2CPP_INT64>(size);
    if (owner_->usage_limit > 0 && new_usage > owner_->usage_limit) {
        CHAOS_IL2CPP_LOG_WARN_M("MemoryDomain", "allocation of {0} bytes would exceed limit {1}",
                              size, owner_->usage_limit);
        return false;
    }
    owner_->current_usage = new_usage;
    if (new_usage > owner_->peak_usage) {
        owner_->peak_usage = new_usage;
    }
    return true;
}

inline void IDomainHeap::TrackFree(CHAOS_IL2CPP_SIZE size) noexcept {
    if (owner_ == nullptr) return;
    owner_->current_usage -= static_cast<CHAOS_IL2CPP_INT64>(size);
}

// -----------------------------------------------------------------------
// Domain-tagged allocation — cross-domain safe free routing.
//
// Each domain allocation prepends an AllocationHeader containing the
// originating heap pointer + a magic tag (bit 0).  DomainFreeTagged reads
// this header to route the free() call to the correct heap — no dependency
// on thread-local domain state, no hash lookup, no race condition.
//
// The magic tag (bit 0 set on the heap pointer) provides defense-in-depth:
// untagged pointers (GC domain, Raw domain, wild pointer) are detected and
// rejected with a CHAOS_IL2CPP_LOG_WARN before they can cause heap
// corruption.
//
// Layout:  [ IDomainHeap* (tagged) | user data ... ]
//            ^- header              ^- returned pointer
// Overhead: 8 bytes per allocation (same as before, magic is zero-cost).

/// Allocate @a size bytes through @a domain's heap and prepend a routing
/// header.  Returns a pointer to the user data (header + sizeof(void*)).
/// The caller must pair this with DomainFreeTagged().
void* DomainAllocateTagged(MemoryDomain* domain, CHAOS_IL2CPP_SIZE size);

/// Like DomainAllocateTagged but uses CurrentDomain() for the domain.
/// Falls back to std::malloc tagged with nullptr when no domain is active.
void* DomainCurrentAllocateTagged(CHAOS_IL2CPP_SIZE size);

/// Free a pointer previously returned by DomainAllocateTagged() or
/// DomainCurrentAllocateTagged().  Reads the header to route to the
/// originating heap.  Safe to call even after the domain has been
/// unloaded (no-ops when heap is gone).
void DomainFreeTagged(void* ptr);

/// Reallocate a tagged pointer, preserving the routing header.
/// Reads the AllocationHeader from @a ptr, calls the originating heap's
/// Reallocate (or std::realloc for null-heap allocations), then writes
/// the header on the new block.  Returns the new user-data pointer, or
/// nullptr on failure (original block remains valid).
void* DomainCurrentReallocateTagged(void* ptr, CHAOS_IL2CPP_SIZE new_size);
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
