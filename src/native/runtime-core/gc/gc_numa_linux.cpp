#include "gc_numa.h"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <sys/mman.h>
#include <unistd.h>
#include <numa.h>
#include <numaif.h>

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

    int count = 1;

    // Parse /sys/devices/system/node/online for node topology.
    FILE* f = ::fopen("/sys/devices/system/node/online", "r");
    if (f != nullptr) {
        char buf[64] = {};
        if (::fgets(buf, sizeof(buf), f) != nullptr) {
            char* hyphen = ::strchr(buf, '-');
            if (hyphen != nullptr) {
                int high = ::atoi(hyphen + 1);
                if (high > 0) count = high + 1;
            }
        }
        ::fclose(f);
    }

    g_numa_node_count.store(count, std::memory_order_release);
    CHAOS_IL2CPP_LOG_INFO("NUMA", "numa_init count=%d", count);
}

int GcNumaNodeCount() noexcept {
    return g_numa_node_count.load(std::memory_order_acquire);
}

int GcNumaCurrentNode() noexcept {
    int count = g_numa_node_count.load(std::memory_order_acquire);
    if (count <= 1) return 0;

    // Use getcpu() to find current CPU, then map to NUMA node.
    // If numa_available() fails, assume node 0.
    if (::numa_available() < 0) return 0;

    int cpu = ::sched_getcpu();
    if (cpu < 0) return 0;

    int node = ::numa_node_of_cpu(cpu);
    return (node >= 0) ? (node % count) : 0;
}

int GcNumaNodeOfAddress(const void* addr) noexcept {
    if (addr == nullptr) return -1;

    int count = g_numa_node_count.load(std::memory_order_acquire);
    if (count <= 1) return 0;

    // Use move_pages() to query the node of a single page.
    void* pages = const_cast<void*>(addr);
    int status = -1;
    long ret = ::move_pages(/*pid=*/0, /*count=*/1, &pages,
                            /*nodes=*/nullptr, &status, /*flags=*/0);
    if (ret == 0 && status >= 0) {
        return status % count;
    }

    // Fallback: parse /proc/self/numa_maps for this address.
    FILE* f = ::fopen("/proc/self/numa_maps", "r");
    if (f != nullptr) {
        char line[256];
        while (::fgets(line, sizeof(line), f) != nullptr) {
            unsigned long addr_val = reinterpret_cast<uintptr_t>(addr);
            unsigned long line_addr;
            if (::sscanf(line, "%lx", &line_addr) == 1) {
                unsigned long page = addr_val & ~(4096ULL - 1);
                if (line_addr == page) {
                    const char* p = line;
                    while (*p) {
                        if (*p >= '0' && *p <= '9' && *(p + 1) == '=') {
                            int node = 0;
                            if (::sscanf(p, "%d=", &node) == 1) {
                                ::fclose(f);
                                return node % count;
                            }
                        }
                        p++;
                    }
                }
            }
        }
        ::fclose(f);
    }

    return 0;
}

void* GcNumaVirtualAlloc(CHAOS_IL2CPP_SIZE size, int node) noexcept {
    int count = g_numa_node_count.load(std::memory_order_acquire);
    if (count <= 1 || node < 0 || node >= count) {
        void* mem = ::mmap(nullptr, size, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        return (mem == MAP_FAILED) ? nullptr : mem;
    }

    void* mem = ::mmap(nullptr, size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) return nullptr;

    // Bind to the target NUMA node via mbind.
    unsigned long mask = 1UL << static_cast<unsigned long>(node);
    int mbind_ret = ::mbind(mem, size, MPOL_BIND, &mask,
                            sizeof(mask) * 8, MPOL_MF_STRICT | MPOL_MF_MOVE);
    if (mbind_ret != 0) {
        CHAOS_IL2CPP_LOG_DEBUG("NUMA", "mbind(node=%d, size=%zu) failed: errno=%d",
                                node, size, errno);
    }
    return mem;
}

void GcNumaVirtualFree(void* ptr, CHAOS_IL2CPP_SIZE size) noexcept {
    if (ptr == nullptr) return;
    ::munmap(ptr, size);
}

void GcNumaBindThread(int node) noexcept {
    int count = g_numa_node_count.load(std::memory_order_acquire);
    if (count <= 1 || node < 0 || node >= count) return;

    cpu_set_t cpus;
    CPU_ZERO(&cpus);

    // Approximate: bind to CPUs on the target node.
    // A precise implementation would enumerate CPUs per node via
    // get_mempolicy or /sys/devices/system/node/nodeN/cpumap.
    int cpu = node * 2;
    int ncpus = sysconf(_SC_NPROCESSORS_CONF);
    if (cpu < ncpus) {
        CPU_SET(cpu, &cpus);
        if (cpu + 1 < ncpus) {
            CPU_SET(cpu + 1, &cpus);
        }
    }

    pthread_setaffinity_np(pthread_self(), sizeof(cpus), &cpus);
}

}  // namespace chaos::il2cpp::runtime_core
