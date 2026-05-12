#include "memory_domain.h"
#include "runtime_core.h"

#include <chaos/trace.h>
#include <chaos/log.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <cstdlib>
#include <sys/mman.h>
#endif

namespace chaos::il2cpp::memory_domain {

// ======================================================================
// SegregatedHeap — default heap strategy
//
// Each domain gets its own Win32 heap so that Destroy() releases all
// memory in O(1) with no individual free calls.  On non-Windows
// platforms we fall back to a simple wrapper around malloc/free (the
// full segregated-heap semantic requires mmap / jemalloc extensions).
// ======================================================================

#if defined(_WIN32)

class SegregatedHeap final : public IDomainHeap {
public:
    SegregatedHeap() : heap_(::HeapCreate(0u, 0u, 0u)) {}
    ~SegregatedHeap() override { Destroy(); }

    void* Allocate(CHAOS_IL2CPP_SIZE size) override {
        if (heap_ == nullptr) return nullptr;
        if (!TrackAlloc(size)) return nullptr;
        return ::HeapAlloc(heap_, 0u, size);
    }

    void* Reallocate(void* ptr, CHAOS_IL2CPP_SIZE new_size) override {
        if (heap_ == nullptr) return nullptr;
        // Untrack old size first.
        if (ptr != nullptr) {
            CHAOS_IL2CPP_SIZE old_size = ::HeapSize(heap_, 0u, ptr);
            if (old_size != static_cast<CHAOS_IL2CPP_SIZE>(-1)) {
                TrackFree(old_size);
            }
        }
        if (!TrackAlloc(new_size)) return nullptr;
        return ::HeapReAlloc(heap_, 0u, ptr, new_size);
    }

    void Free(void* ptr) override {
        if (heap_ != nullptr && ptr != nullptr) {
            CHAOS_IL2CPP_SIZE old_size = ::HeapSize(heap_, 0u, ptr);
            if (old_size != static_cast<CHAOS_IL2CPP_SIZE>(-1)) {
                TrackFree(old_size);
            }
            ::HeapFree(heap_, 0u, ptr);
        }
    }

    void Destroy() override {
        if (heap_ != nullptr) {
            ::HeapDestroy(heap_);
            heap_ = nullptr;
        }
    }

private:
    HANDLE heap_;
};

#else  // not _WIN32 — region-based allocator

/// Minimum region size for the mmap-based SegregatedHeap.
/// 64 KB = typical large page on many systems, balances fragmentation vs waste.
static constexpr CHAOS_IL2CPP_SIZE kMmapRegionSize = 64 * 1024;

class SegregatedHeap final : public IDomainHeap {
public:
    SegregatedHeap() {
        AllocateNewRegion();
    }

    ~SegregatedHeap() override { Destroy(); }

    void* Allocate(CHAOS_IL2CPP_SIZE size) override {
        size = AlignUp(size, sizeof(void*));
        if (!TrackAlloc(size)) return nullptr;

        std::lock_guard<std::mutex> lock(region_mutex_);

        // Try current region first.
        if (current_ != nullptr &&
            (current_pos_ + size) <= current_end_) {
            void* ptr = reinterpret_cast<void*>(current_pos_);
            current_pos_ += size;
            return ptr;
        }

        // Large allocation: get a dedicated mmap region just for this block.
        if (size > kMmapRegionSize / 2) {
            CHAOS_IL2CPP_SIZE alloc_size = AlignUp(size, kMmapRegionSize);
            void* ptr = MmapAlloc(alloc_size);
            if (ptr == nullptr) return nullptr;
            regions_.push_back({ptr, alloc_size});
            return ptr;
        }

        // Need a fresh region.
        if (!AllocateNewRegion()) return nullptr;

        // Retry with the new region.
        void* ptr = reinterpret_cast<void*>(current_pos_);
        current_pos_ += size;
        return ptr;
    }

    void* Reallocate(void* ptr, CHAOS_IL2CPP_SIZE new_size) override {
        if (!TrackAlloc(new_size)) return nullptr;

        std::lock_guard<std::mutex> lock(region_mutex_);

        // If ptr is the last allocation in the current region, just extend.
        if (ptr != nullptr && IsInCurrentRegion(ptr)) {
            CHAOS_IL2CPP_SIZE needed = AlignUp(new_size, sizeof(void*));
            if ((reinterpret_cast<char*>(ptr) + needed) <= current_end_) {
                current_pos_ = reinterpret_cast<CHAOS_IL2CPP_SIZE>(ptr) + needed;
                return ptr;
            }
        }

        // Generic fallback: alloc + copy + free.
        void* new_ptr = AllocateLocked(new_size);
        if (new_ptr == nullptr || ptr == nullptr) return new_ptr;
        std::memcpy(new_ptr, ptr, new_size);
        // Bump allocator: no individual free needed.
        return new_ptr;
    }

    void Free(void* ptr) override {
        (void)ptr;
        // Bump allocator: individual free is a no-op.
        // All memory released in Destroy() via bulk munmap.
    }

    void Destroy() override {
        std::lock_guard<std::mutex> lock(region_mutex_);

        CHAOS_IL2CPP_INT64 total = 0;
        for (auto& r : regions_) {
            if (r.ptr != nullptr) {
                total += static_cast<CHAOS_IL2CPP_INT64>(r.size);
                ::munmap(r.ptr, r.size);
            }
        }
        // Subtract all tracked region sizes from usage (since we can't
        // track individual bump-allocated blocks, use region total as
        // a conservative estimate).
        if (owner_ != nullptr) {
            owner_->current_usage -= total;
            if (owner_->current_usage < 0) owner_->current_usage = 0;
        }
        regions_.clear();
        current_ = nullptr;
        current_pos_ = 0;
        current_end_ = 0;
    }

private:
    struct Region {
        void* ptr;
        CHAOS_IL2CPP_SIZE size;
    };

    std::mutex            region_mutex_;   ///< Serializes region metadata and bump-pointer access.
    std::vector<Region>   regions_;

    void*  current_     = nullptr;
    CHAOS_IL2CPP_SIZE current_pos_  = 0;
    CHAOS_IL2CPP_SIZE current_end_  = 0;

    static CHAOS_IL2CPP_SIZE AlignUp(CHAOS_IL2CPP_SIZE size, CHAOS_IL2CPP_SIZE align) {
        return (size + align - 1) & ~(align - 1);
    }

    static void* MmapAlloc(CHAOS_IL2CPP_SIZE size) {
        void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        return (ptr == MAP_FAILED) ? nullptr : ptr;
    }

    void SetupCurrent(void* region, CHAOS_IL2CPP_SIZE size) {
        current_ = region;
        current_pos_ = reinterpret_cast<CHAOS_IL2CPP_SIZE>(current_);
        current_end_ = current_pos_ + size;
    }

    bool AllocateNewRegion() {
        // Must be called with region_mutex_ held.
        CHAOS_IL2CPP_SIZE size = kMmapRegionSize;
        void* ptr = MmapAlloc(size);
        if (ptr == nullptr) return false;
        SetupCurrent(ptr, size);
        regions_.push_back({ptr, size});
        return true;
    }

    bool IsInCurrentRegion(const void* ptr) const {
        // Must be called with region_mutex_ held.
        if (current_ == nullptr) return false;
        CHAOS_IL2CPP_SIZE p = reinterpret_cast<CHAOS_IL2CPP_SIZE>(ptr);
        CHAOS_IL2CPP_SIZE start = reinterpret_cast<CHAOS_IL2CPP_SIZE>(current_);
        return p >= start && p < current_end_;
    }

    /// Allocate under the assumption that region_mutex_ is already held.
    void* AllocateLocked(CHAOS_IL2CPP_SIZE size) {
        size = AlignUp(size, sizeof(void*));
        // Try current region first.
        if (current_ != nullptr &&
            (current_pos_ + size) <= current_end_) {
            void* ptr = reinterpret_cast<void*>(current_pos_);
            current_pos_ += size;
            return ptr;
        }
        // Large allocation.
        if (size > kMmapRegionSize / 2) {
            CHAOS_IL2CPP_SIZE alloc_size = AlignUp(size, kMmapRegionSize);
            void* ptr = MmapAlloc(alloc_size);
            if (ptr == nullptr) return nullptr;
            regions_.push_back({ptr, alloc_size});
            return ptr;
        }
        // Fresh region.
        if (!AllocateNewRegion()) return nullptr;
        void* ptr = reinterpret_cast<void*>(current_pos_);
        current_pos_ += size;
        return ptr;
    }
};

#endif

// ======================================================================
// Process-wide heap factory
// ======================================================================

namespace {

HeapFactoryFn     g_heap_factory = nullptr;
HeapFactoryUserData g_heap_factory_user_data = nullptr;

CHAOS_IL2CPP_MUTEX        g_registry_mutex;
CHAOS_IL2CPP_VECTOR(MemoryDomain*) g_domains;
CHAOS_IL2CPP_ATOMIC(DomainId) g_next_domain_id{1};

// Thread-local domain stack.
// A depth of nullptr entries is allowed so PushDomain(nullptr) is valid.
thread_local struct DomainStack {
    MemoryDomain* domains[64];
    int           top;
} g_tls_domain_stack = {{nullptr}, -1};

/// Built-in default heap factory.
IDomainHeap* DefaultHeapFactory(const MemoryDomain* /*domain*/, void* /*user_data*/) {
    return new SegregatedHeap();
}

// ======================================================================
// Tagged allocation — cross-domain safe free routing
//
// Each allocation prepends an AllocationHeader containing the originating
// heap pointer OR-tagged with a validity bit (bit 0).  DomainFreeTagged
// reads this header to route the free() call to the correct heap — no
// dependency on thread-local domain state, no hash lookup, no race.
//
// Magic tag: bit 0 is forced to 1 on all domain allocations.  IDomainHeap
// pointers are at least 4-byte aligned (vtable), so bit 0 on a legitimate
// heap pointer is naturally 0.  GC domain and Raw domain pointers likewise
// have bit 0 = 0.  The tag detects cross-domain pointer misrouting with
// zero additional memory overhead.
//
// Layout:
//   [ uintptr_t (tagged heap ptr) | user data ... ]
//     ^- header                    ^- returned pointer
//
// Overhead: 8 bytes (one pointer) per domain allocation (unchanged).
// ======================================================================

}  // namespace (anonymous)

// ── Tagged allocation header ──────────────────────────────────────────
//
// Uses bit 0 of the stored value as a validity tag.  IDomainHeap pointers
// are at least 4-byte aligned (vtable requirement), so bit 0 is always 0
// in a legitimate heap pointer.  Setting it to 1 marks the allocation as
// domain-tagged, providing zero-cost defense against cross-domain free:
// GC domain and Raw domain pointers (both naturally even-aligned) will
// have bit 0 = 0 and be rejected with a diagnostic.

static constexpr uintptr_t kDomainAllocTag = 1;

/// Tag a heap pointer by setting bit 0 (caller ensures pointer is aligned).
static uintptr_t TagHeapPtr(IDomainHeap* heap) noexcept {
    return reinterpret_cast<uintptr_t>(heap) | kDomainAllocTag;
}

/// Strip the tag and return the original heap pointer (nullptr → raw malloc).
static IDomainHeap* UntagHeapPtr(uintptr_t tagged) noexcept {
    return reinterpret_cast<IDomainHeap*>(tagged & ~kDomainAllocTag);
}

/// Returns true when @a tagged has the validity bit set.
static bool IsTaggedAllocation(uintptr_t tagged) noexcept {
    return (tagged & kDomainAllocTag) != 0;
}

struct AllocationHeader {
    uintptr_t heap_or_tagged;  // tagged heap pointer, bit 0 = validity marker
};

void* DomainAllocateTagged(MemoryDomain* domain, CHAOS_IL2CPP_SIZE size) {
    const CHAOS_IL2CPP_SIZE total = size + sizeof(AllocationHeader);
    void* raw;
    if (domain != nullptr && domain->heap != nullptr) {
        raw = domain->heap->Allocate(total);
    } else {
        raw = std::malloc(total);
    }
    if (raw == nullptr) return nullptr;

    auto* hdr = static_cast<AllocationHeader*>(raw);
    IDomainHeap* heap = (domain != nullptr && domain->heap != nullptr) ? domain->heap : nullptr;
    hdr->heap_or_tagged = TagHeapPtr(heap);
    return static_cast<void*>(hdr + 1);
}

void* DomainCurrentAllocateTagged(CHAOS_IL2CPP_SIZE size) {
    return DomainAllocateTagged(CurrentDomain(), size);
}

void DomainFreeTagged(void* ptr) {
    if (ptr == nullptr) return;
    auto* hdr = static_cast<AllocationHeader*>(static_cast<void*>(ptr)) - 1;

    // Validate: must have the magic tag bit.  GC domain and Raw domain
    // pointers (even-aligned, bit 0 = 0) are caught here.
    if (!IsTaggedAllocation(hdr->heap_or_tagged)) {
        CHAOS_IL2CPP_LOG_WARN("MemoryDomain",
            "DomainFreeTagged: pointer %p lacks domain magic tag (bit 0 = 0) — "
            "likely a GC or Raw domain pointer.  Skipping free to prevent heap corruption.",
            ptr);
        return;
    }

    IDomainHeap* heap = UntagHeapPtr(hdr->heap_or_tagged);
    if (heap != nullptr) {
        heap->Free(static_cast<void*>(hdr));
    } else {
        // Tagged with nullptr → raw malloc allocation.
        std::free(static_cast<void*>(hdr));
    }
}

void* DomainCurrentReallocateTagged(void* ptr, CHAOS_IL2CPP_SIZE new_size) {
    if (ptr == nullptr) {
        return DomainCurrentAllocateTagged(new_size);
    }

    auto* old_hdr = static_cast<AllocationHeader*>(static_cast<void*>(ptr)) - 1;

    // Validate tag before processing.
    if (!IsTaggedAllocation(old_hdr->heap_or_tagged)) {
        CHAOS_IL2CPP_LOG_WARN("MemoryDomain",
            "DomainCurrentReallocateTagged: pointer %p lacks domain magic tag — "
            "cannot determine originating heap.  Returning nullptr (original block untouched).",
            ptr);
        return nullptr;
    }

    IDomainHeap* heap = UntagHeapPtr(old_hdr->heap_or_tagged);
    const CHAOS_IL2CPP_SIZE total = new_size + sizeof(AllocationHeader);

    void* new_raw;
    if (heap != nullptr) {
        new_raw = heap->Reallocate(static_cast<void*>(old_hdr), total);
    } else {
        new_raw = std::realloc(static_cast<void*>(old_hdr), total);
    }
    if (new_raw == nullptr) return nullptr;

    auto* new_hdr = static_cast<AllocationHeader*>(new_raw);
    new_hdr->heap_or_tagged = TagHeapPtr(heap);
    return static_cast<void*>(new_hdr + 1);
}

// ======================================================================
// Heap factory accessors
// ======================================================================

void SetHeapFactory(HeapFactoryFn factory, HeapFactoryUserData user_data) {
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_registry_mutex);
    g_heap_factory = factory;
    g_heap_factory_user_data = user_data;
}

HeapFactoryFn GetHeapFactory(HeapFactoryUserData* out_user_data) {
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_registry_mutex);
    if (out_user_data != nullptr) {
        *out_user_data = g_heap_factory_user_data;
    }
    return g_heap_factory;
}

// ======================================================================
// Domain lifecycle
// ======================================================================

DomainId RegisterMemoryDomain(const DomainInit& init) {
    if (init.module_name == nullptr) {
        return kDomainIdInvalid;
    }

    auto* domain = new MemoryDomain();

    domain->domain_id    = g_next_domain_id.fetch_add(1, CHAOS_IL2CPP_MEMORY_ORDER_RELAXED);
    domain->module_name  = init.module_name;
    domain->module_kind  = init.module_kind;
    domain->heap         = nullptr;
    domain->current_usage = 0;
    domain->peak_usage   = 0;
    domain->usage_limit  = init.usage_limit;

    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_registry_mutex);

        HeapFactoryFn factory = g_heap_factory;
        if (factory == nullptr) {
            factory = DefaultHeapFactory;
        }

        domain->heap = factory(domain, g_heap_factory_user_data);
        if (domain->heap == nullptr) {
            delete domain;
            return kDomainIdInvalid;
        }
        domain->heap->SetOwner(domain);

        g_domains.push_back(domain);
    }

    return domain->domain_id;
}

MemoryDomain* FindDomainById(DomainId domain_id) {
    if (domain_id == kDomainIdInvalid) {
        return nullptr;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_registry_mutex);
    for (auto* domain : g_domains) {
        if (domain != nullptr && domain->domain_id == domain_id && !domain->is_unloaded) {
            return domain;
        }
    }
    return nullptr;
}

MemoryDomain* FindDomainByName(const char* module_name) {
    if (module_name == nullptr) {
        return nullptr;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_registry_mutex);
    for (auto* domain : g_domains) {
        if (domain != nullptr && !domain->is_unloaded &&
            domain->module_name != nullptr &&
            CHAOS_IL2CPP_STRCMP(domain->module_name, module_name) == 0) {
            return domain;
        }
    }
    return nullptr;
}

bool UnregisterMemoryDomain(DomainId domain_id) {
    if (domain_id == kDomainIdInvalid) {
        return false;
    }

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_registry_mutex);
    for (auto& domain : g_domains) {
        if (domain != nullptr && domain->domain_id == domain_id) {
            if (domain->heap != nullptr) {
                domain->heap->Destroy();
                delete domain->heap;
                domain->heap = nullptr;
            }

            domain->is_unloaded = true;
            return true;
        }
    }
    return false;
}

// ======================================================================
// TLS domain stack
// ======================================================================

int PushDomain(MemoryDomain* domain) {
    auto& stack = g_tls_domain_stack;
    if (stack.top < 63) {
        stack.domains[++stack.top] = domain;
    } else {
        CHAOS_IL2CPP_LOG_ERROR("MemoryDomain", "PushDomain: thread-local stack overflow (max 64), domain push ignored");
    }
    return stack.top;
}

void PopDomain(int depth_before_push) {
    auto& stack = g_tls_domain_stack;
    if (stack.top >= depth_before_push) {
        stack.top = depth_before_push - 1;
    }
}

MemoryDomain* CurrentDomain() {
    auto& stack = g_tls_domain_stack;
    if (stack.top >= 0) {
        return stack.domains[stack.top];
    }
    return nullptr;
}

}  // namespace chaos::il2cpp::memory_domain
