#include "memory_domain.h"
#include "runtime_core.h"

#include <chaos/trace.h>
#include <chaos/log.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <vector>

#include <chaos/pal/pal_heap.h>
#include <chaos/pal/pal_mem.h>

namespace chaos::il2cpp::memory_domain {

using chaos::il2cpp::pal::PalHeapCreate;
using chaos::il2cpp::pal::PalHeapAlloc;
using chaos::il2cpp::pal::PalHeapRealloc;
using chaos::il2cpp::pal::PalHeapFree;
using chaos::il2cpp::pal::PalHeapSize;
using chaos::il2cpp::pal::PalHeapDestroy;

// ======================================================================
// SegregatedHeap — default heap strategy
//
// Uses PalHeapCreate for portable heap allocation (Win32 Heap API on
// Windows, malloc/free on POSIX).  The Destroy() releases all memory
// at once with no individual free calls.
// ======================================================================

class SegregatedHeap final : public IDomainHeap {
public:
    SegregatedHeap() : heap_(PalHeapCreate()) {}
    ~SegregatedHeap() override { Destroy(); }

    void* Allocate(CHAOS_IL2CPP_SIZE size) override {
        if (!heap_) return nullptr;
        if (!TrackAlloc(size)) return nullptr;
        return PalHeapAlloc(heap_, size);
    }

    void* Reallocate(void* ptr, CHAOS_IL2CPP_SIZE new_size) override {
        if (!heap_) return nullptr;
        if (ptr != nullptr) {
            CHAOS_IL2CPP_SIZE old_size = PalHeapSize(heap_, ptr);
            if (old_size > 0) TrackFree(old_size);
        }
        if (!TrackAlloc(new_size)) return nullptr;
        return PalHeapRealloc(heap_, ptr, new_size);
    }

    void Free(void* ptr) override {
        if (heap_ != nullptr && ptr != nullptr) {
            CHAOS_IL2CPP_SIZE old_size = PalHeapSize(heap_, ptr);
            if (old_size > 0) TrackFree(old_size);
            PalHeapFree(heap_, ptr);
        }
    }

    void Destroy() override {
        if (heap_ != nullptr) {
            PalHeapDestroy(heap_);
            heap_ = nullptr;
        }
    }

private:
    void* heap_;
};

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
        CHAOS_IL2CPP_LOG_WARN_M("MemoryDomain",
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
        CHAOS_IL2CPP_LOG_WARN_M("MemoryDomain",
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
    domain->current_usage.store(0, std::memory_order_relaxed);
    domain->peak_usage.store(0, std::memory_order_relaxed);
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

    // Fire domain-registered event after successful registration.
    {
        MemoryDomainEventData ev_data{};
        ev_data.domain_id = domain->domain_id;
        ev_data.module_name = domain->module_name;
        ev_data.module_kind = domain->module_kind;
        MemoryDomainFireEvent(MemoryDomainEvent::DOMAIN_REGISTERED, ev_data);
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
    for (size_t i = 0; i < g_domains.size(); ++i) {
        auto* domain = g_domains[i];
        if (domain != nullptr && domain->domain_id == domain_id) {
            // Fire domain-unloaded event before destroying the domain.
            {
                MemoryDomainEventData ev_data{};
                ev_data.domain_id = domain->domain_id;
                MemoryDomainFireEvent(MemoryDomainEvent::DOMAIN_UNLOADED, ev_data);
            }

            if (domain->heap != nullptr) {
                domain->heap->Destroy();
                delete domain->heap;
                domain->heap = nullptr;
            }

            // Tombstone marking: set is_unloaded = true instead of erasing,
            // avoiding O(n) compaction on every unregister. FindDomainById
            // and FindDomainByName already skip is_unloaded domains.
            domain->is_unloaded = true;
            delete domain;
            g_domains[i] = nullptr;

            // Lazy compaction: when tombstone ratio exceeds 50% and vector
            // is larger than 1024 entries, compact by removing all nullptrs.
            if (g_domains.size() > 1024) {
                size_t tombstone_count = 0;
                for (auto* d : g_domains) {
                    if (d == nullptr) ++tombstone_count;
                }
                if (tombstone_count > g_domains.size() / 2) {
                    size_t write = 0;
                    for (size_t r = 0; r < g_domains.size(); ++r) {
                        if (g_domains[r] != nullptr) {
                            g_domains[write++] = g_domains[r];
                        }
                    }
                    g_domains.resize(write);
                }
            }

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
        return stack.top;
    } else {
        CHAOS_IL2CPP_LOG_ERROR("MemoryDomain", "PushDomain: thread-local stack overflow (max 64), domain push ignored");
        return 64;  // sentinel: no domain was pushed, keeps PopDomain balance
    }
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
