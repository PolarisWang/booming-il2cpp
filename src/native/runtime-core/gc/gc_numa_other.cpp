#include "gc_numa.h"

#include <atomic>
#include <cstdlib>

#include <chaos/log.h>

namespace chaos::il2cpp::runtime_core {
namespace {

std::atomic<int> g_numa_node_count{1};
std::atomic<bool> g_numa_initialized{false};

}  // anonymous namespace

void GcNumaInit() noexcept {
    if (g_numa_initialized.exchange(true, std::memory_order_acq_rel)) {
        return;
    }

    // Single-node fallback — NUMA not supported on this platform.
    g_numa_node_count.store(1, std::memory_order_release);
    CHAOS_IL2CPP_LOG_INFO("NUMA", "numa_init count=1 (fallback)");
}

int GcNumaNodeCount() noexcept {
    return g_numa_node_count.load(std::memory_order_acquire);
}

int GcNumaCurrentNode() noexcept {
    return 0;
}

int GcNumaNodeOfAddress(const void* addr) noexcept {
    if (addr == nullptr) return -1;
    return 0;
}

void* GcNumaVirtualAlloc(CHAOS_IL2CPP_SIZE size, int node) noexcept {
    (void)node;
    return nullptr;  // No NUMA support — caller must handle.
}

void GcNumaVirtualFree(void* ptr, CHAOS_IL2CPP_SIZE size) noexcept {
    (void)ptr;
    (void)size;
    // No-op — platform does not use NUMA-aware virtual memory.
}

}  // namespace chaos::il2cpp::runtime_core
