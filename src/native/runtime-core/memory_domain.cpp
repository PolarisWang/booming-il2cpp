#include "memory_domain.h"
#include "runtime_core.h"

#include <chaos/trace.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <vector>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <cstdlib>
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
        return heap_ != nullptr ? ::HeapAlloc(heap_, 0u, size) : nullptr;
    }

    void* Reallocate(void* ptr, CHAOS_IL2CPP_SIZE new_size) override {
        return heap_ != nullptr ? ::HeapReAlloc(heap_, 0u, ptr, new_size) : nullptr;
    }

    void Free(void* ptr) override {
        if (heap_ != nullptr && ptr != nullptr) {
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

#else  // not _WIN32 — portable fallback

class SegregatedHeap final : public IDomainHeap {
public:
    void* Allocate(CHAOS_IL2CPP_SIZE size) override {
        return CHAOS_IL2CPP_MALLOC(size);
    }

    void* Reallocate(void* ptr, CHAOS_IL2CPP_SIZE new_size) override {
        return CHAOS_IL2CPP_REALLOC(ptr, new_size);
    }

    void Free(void* ptr) override {
        CHAOS_IL2CPP_FREE(ptr);
    }

    void Destroy() override {
        // On platforms without heap handles we cannot do a bulk free,
        // so Destroy is semantically a no-op here.  Users who need
        // O(1) destroy should provide a custom heap factory that uses
        // mmap region or jemalloc arena.
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

}  // namespace

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
