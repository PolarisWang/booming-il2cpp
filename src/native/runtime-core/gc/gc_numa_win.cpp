#include "gc_numa.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>

#include <atomic>
#include <cstdlib>
#include <cstring>

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

    if (count == 1) {
        SYSTEM_INFO si;
        ::GetSystemInfo(&si);
        if (si.dwNumberOfProcessors > 1) {
            // Single NUMA node with multiple CPUs.
        }
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

    PROCESSOR_NUMBER pnum;
    ::GetCurrentProcessorNumberEx(&pnum);

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

    return 0;
}

int GcNumaNodeOfAddress(const void* addr) noexcept {
    if (addr == nullptr) return -1;

    int count = g_numa_node_count.load(std::memory_order_acquire);
    if (count <= 1) return 0;

    // Windows 10+: Use GetNumaNodeNumberFromNumaNodeInfo.
    using GetNumaNodeNumberFromNumaNodeInfo_t =
        BOOLEAN(WINAPI*)(USHORT, PVOID, PUSHORT);
    static auto get_node_fn = reinterpret_cast<GetNumaNodeNumberFromNumaNodeInfo_t>(
        ::GetProcAddress(::GetModuleHandleW(L"kernel32.dll"),
                         "GetNumaNodeNumberFromNumaNodeInfo"));
    if (get_node_fn != nullptr) {
        USHORT node = 0;
        if (get_node_fn(0, const_cast<void*>(addr), &node)) {
            return static_cast<int>(node) % count;
        }
    }

    // Fallback: VirtualQuery to get page base, then return 0.
    MEMORY_BASIC_INFORMATION mbi;
    if (::VirtualQuery(addr, &mbi, sizeof(mbi)) != 0) {
        PSAPI_WORKING_SET_EX_INFORMATION ws_info;
        ws_info.VirtualAddress = mbi.AllocationBase;
        if (::QueryWorkingSetEx(::GetCurrentProcess(), &ws_info, sizeof(ws_info))) {
            // Bit 62-56 of the entry contain the NUMA node on some SKUs.
            // For now, fall back to node 0.
        }
    }

    return 0;
}

void* GcNumaVirtualAlloc(CHAOS_IL2CPP_SIZE size, int node) noexcept {
    int count = g_numa_node_count.load(std::memory_order_acquire);
    if (count <= 1 || node < 0 || node >= count) {
        return ::VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    }

    HANDLE process = ::GetCurrentProcess();
    void* mem = ::VirtualAllocExNuma(process, nullptr, size,
                                     MEM_RESERVE | MEM_COMMIT,
                                     PAGE_READWRITE, static_cast<DWORD>(node));
    if (mem == nullptr) {
        mem = ::VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    }
    return mem;
}

void GcNumaVirtualFree(void* ptr, CHAOS_IL2CPP_SIZE size) noexcept {
    if (ptr == nullptr) return;
    (void)size;
    ::VirtualFree(ptr, 0, MEM_RELEASE);
}

}  // namespace chaos::il2cpp::runtime_core
