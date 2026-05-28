#include "gc_static_roots.h"

#include <mutex>
#include <vector>

#include <chaos/log.h>

namespace chaos::il2cpp::runtime_core {
namespace {

/// A registered static root range.
struct StaticRootRange {
    void*                start;       // Inclusive
    void*                end;         // Exclusive (start + size)
    CHAOS_IL2CPP_UINT32 domain_id;   // For bulk unload
};

/// Mutex-protected registry of all static root ranges.
std::mutex g_static_roots_mutex;
std::vector<StaticRootRange> g_static_roots;

}  // anonymous namespace

void GcRegisterStaticRootRange(void* start, CHAOS_IL2CPP_SIZE size,
                               CHAOS_IL2CPP_UINT32 domain_id) noexcept {
    if (start == nullptr || size == 0) return;

    auto end = static_cast<char*>(start) + size;
    std::lock_guard<std::mutex> lock(g_static_roots_mutex);

    g_static_roots.push_back({start, end, domain_id});
    CHAOS_IL2CPP_LOG_DEBUG_M("GC_STATIC", "register_range start=%p end=%p domain=%u size=%llu",
        start, end, domain_id, static_cast<unsigned long long>(size));
}

void GcUnregisterStaticRootRange(void* start) noexcept {
    if (start == nullptr) return;

    std::lock_guard<std::mutex> lock(g_static_roots_mutex);
    for (size_t i = 0; i < g_static_roots.size(); ++i) {
        if (g_static_roots[i].start == start) {
            CHAOS_IL2CPP_LOG_DEBUG_M("GC_STATIC", "unregister_range start=%p domain=%u",
                start, g_static_roots[i].domain_id);
            g_static_roots[i] = g_static_roots.back();
            g_static_roots.pop_back();
            return;
        }
    }
    CHAOS_IL2CPP_LOG_WARN_M("GC_STATIC", "unregister_range start=%p not found", start);
}

void GcUnregisterDomainStaticRoots(CHAOS_IL2CPP_UINT32 domain_id) noexcept {
    std::lock_guard<std::mutex> lock(g_static_roots_mutex);
    size_t count = 0;
    for (size_t i = 0; i < g_static_roots.size(); ) {
        if (g_static_roots[i].domain_id == domain_id) {
            g_static_roots[i] = g_static_roots.back();
            g_static_roots.pop_back();
            ++count;
        } else {
            ++i;
        }
    }
    if (count > 0) {
        CHAOS_IL2CPP_LOG_DEBUG_M("GC_STATIC", "unregister_domain domain=%u count=%zu",
            domain_id, count);
    }
}

void GcScanStaticRoots(void (*callback)(void* root_addr, bool is_interior, void* user_data),
                       void* user_data) noexcept {
    if (callback == nullptr) return;

    std::lock_guard<std::mutex> lock(g_static_roots_mutex);
    for (const auto& range : g_static_roots) {
        uintptr_t start_aligned = (reinterpret_cast<uintptr_t>(range.start) + sizeof(void*) - 1)
            & ~static_cast<uintptr_t>(sizeof(void*) - 1);
        uintptr_t end_aligned = reinterpret_cast<uintptr_t>(range.end)
            & ~static_cast<uintptr_t>(sizeof(void*) - 1);

        for (uintptr_t slot = start_aligned; slot < end_aligned; slot += sizeof(void*)) {
            callback(reinterpret_cast<void*>(slot), /*is_interior=*/false, user_data);
        }
    }
}

}  // namespace chaos::il2cpp::runtime_core
