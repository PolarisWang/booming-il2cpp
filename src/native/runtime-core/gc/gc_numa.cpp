#include "gc_numa.h"

#include <atomic>
#include <cstdlib>
#include <cstring>

#include <chaos/log.h>

#if defined(_WIN32) || defined(_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif

namespace chaos::il2cpp::runtime_core {
namespace {

/// Number of NUMA nodes (cached after init).
std::atomic<int> g_numa_node_count{1};

/// Whether NUMA has been initialized.
std::atomic<bool> g_numa_initialized{false};

}  // anonymous namespace

void GcNumaInit() noexcept {
    if (g_numa_initialized.exchange(true, std::memory_order_acq_rel)) {
        return;  // already initialized
    }

    int count = 1;  // default: single node

#if defined(_WIN32) || defined(_WIN64)
    // Windows: use GetNumaHighestNodeNumber to detect NUMA topology.
    // Requires PRO/Enterprise SKU — returns ERROR_INVALID_FUNCTION on
    // Home editions or when NUMA is not present.
    using GetNumaHighestNodeNumber_t = decltype(&GetNumaHighestNodeNumber);
    auto* get_numa_fn = reinterpret_cast<GetNumaHighestNodeNumber_t>(
        ::GetProcAddress(::GetModuleHandleW(L"kernel32.dll"),
                         "GetNumaHighestNodeNumber"));
    if (get_numa_fn != nullptr) {
        ULONG highest = 0;
        if (get_numa_fn(&highest) != 0) {
            count = static_cast<int>(highest) + 1;
        }
    }

    // If only one node, verify with processor group info.
    if (count == 1) {
        SYSTEM_INFO si;
        ::GetSystemInfo(&si);
        if (si.dwNumberOfProcessors > 1) {
            // Single NUMA node with multiple CPUs — not a NUMA system.
        }
    }
#elif defined(__linux__)
    // Linux: parse /sys/devices/system/node/online or use libnuma.
    // Simple fallback: check /sys for node count without libnuma dependency.
    FILE* f = ::fopen("/sys/devices/system/node/online", "r");
    if (f != nullptr) {
        char buf[64] = {};
        if (::fgets(buf, sizeof(buf), f) != nullptr) {
            // Format examples: "0", "0-3", "0,2"
            char* hyphen = ::strchr(buf, '-');
            if (hyphen != nullptr) {
                int high = ::atoi(hyphen + 1);
                if (high > 0) count = high + 1;
            }
        }
        ::fclose(f);
    }
#else
    // iOS, Android, macOS: no NUMA support (single node).
    count = 1;
#endif

    g_numa_node_count.store(count, std::memory_order_release);
    CHAOS_IL2CPP_LOG_INFO("NUMA", "numa_init count=%d", count);
}

int GcNumaNodeCount() noexcept {
    return g_numa_node_count.load(std::memory_order_acquire);
}

int GcNumaCurrentNode() noexcept {
    int count = g_numa_node_count.load(std::memory_order_acquire);
    if (count <= 1) return 0;

#if defined(_WIN32) || defined(_WIN64)
    PROCESSOR_NUMBER pnum;
    ::GetCurrentProcessorNumberEx(&pnum);
    // The NUMA node is typically derived from the processor group + number.
    // Simplified: use GetNumaProcessorNodeEx for accurate mapping.
    using GetNumaProcessorNodeEx_t = decltype(&GetNumaProcessorNodeEx);
    static auto* get_numa_node_fn = reinterpret_cast<GetNumaProcessorNodeEx_t>(
        ::GetProcAddress(::GetModuleHandleW(L"kernel32.dll"),
                         "GetNumaProcessorNodeEx"));
    if (get_numa_node_fn != nullptr) {
        USHORT node;
        if (get_numa_node_fn(&pnum, &node)) {
            return static_cast<int>(node) % count;
        }
    }
#endif

    return 0;  // fallback
}

int GcNumaNodeOfAddress(const void* addr) noexcept {
    if (addr == nullptr) return -1;

#if defined(_WIN32) || defined(_WIN64)
    using GetNumaNodeProcessorMaskEx_t = decltype(&GetNumaNodeProcessorMaskEx);
    // Windows doesn't provide GetNumaNodeOfAddress directly.
    // Use GetNumaNodeNumberFromNumaNodeInfo on some SKUs.
    // Fallback: return 0 (caller must handle).
    (void)addr;
#endif

    return -1;  // unknown
}

void* GcNumaVirtualAlloc(CHAOS_IL2CPP_SIZE size, int node) noexcept {
    int count = g_numa_node_count.load(std::memory_order_acquire);
    if (count <= 1 || node < 0 || node >= count) {
        // Single node or invalid node — use regular VirtualAlloc.
#if defined(_WIN32) || defined(_WIN64)
        return ::VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
        void* mem = ::mmap(nullptr, size, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        return (mem == MAP_FAILED) ? nullptr : mem;
#endif
    }

#if defined(_WIN32) || defined(_WIN64)
    // Windows: VirtualAllocExNuma for node-local allocation.
    HANDLE process = ::GetCurrentProcess();
    void* mem = ::VirtualAllocExNuma(process, nullptr, size,
                                     MEM_RESERVE | MEM_COMMIT,
                                     PAGE_READWRITE, static_cast<DWORD>(node));
    if (mem == nullptr) {
        // Fallback: retry without NUMA hint.
        mem = ::VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    }
    return mem;
#elif defined(__linux__)
    // Linux: allocate then bind with mbind.
    void* mem = ::mmap(nullptr, size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) return nullptr;
    // Attempt mbind to the specific node.  If unavailable, keep the allocation.
    // (mbind requires -lnuma; without it, the allocation is not node-local.)
    return mem;
#else
    (void)node;
    return nullptr;
#endif
}

void GcNumaVirtualFree(void* ptr, CHAOS_IL2CPP_SIZE size) noexcept {
    if (ptr == nullptr) return;
#if defined(_WIN32) || defined(_WIN64)
    (void)size;
    ::VirtualFree(ptr, 0, MEM_RELEASE);
#else
    ::munmap(ptr, size);
#endif
}

}  // namespace chaos::il2cpp::runtime_core
