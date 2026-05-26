// threadpool_events.cpp — ThreadPool ETW event provider
//
// Provides Event Tracing for Windows (ETW) instrumentation for the
// ThreadPool subsystem.  On non-Windows platforms all functions are
// no-ops (the provider compiles away to zero runtime cost).
//
// The provider emits structured events for:
//   - Worker thread creation and destruction
//   - Work item queue and dequeue operations
//   - Hill-climbing worker count adjustments
//   - IOCP completion notifications
//
// These events can be consumed by Xperf, WPR, PerfView, or any ETW
// trace consumer for diagnostics and performance analysis.

#include <cstdint>
#include <atomic>
#include "threadpool_events.h"

#if defined(_WIN32) || defined(_WIN64)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0601  // Windows 7+
    #endif
    #include <windows.h>
    #include <evntprov.h>
#endif

namespace chaos::il2cpp::runtime_core::threading {

// ── Provider GUID ──────────────────────────────────────────────────────
// {C9A3E1F0-6C7A-4DF8-9D5C-5E1B8C3F7A2D}
// Unique to Chaos IL2CPP ThreadPool diagnostics.
#if defined(_WIN32) || defined(_WIN64)
namespace {
const GUID kThreadPoolProviderGuid = {
    0xc9a3e1f0, 0x6c7a, 0x4df8,
    { 0x9d, 0x5c, 0x5e, 0x1b, 0x8c, 0x3f, 0x7a, 0x2d }
};

// Event descriptors (one per event type).
// The keyword and level fields follow Windows ETW conventions:
//   keyword: 0x1 = ThreadPool worker lifecycle
//            0x2 = ThreadPool work item operations
//            0x4 = ThreadPool hill-climbing adjustments
//            0x8 = ThreadPool I/O operations
//   level:   4 (Informational)
//
// Event IDs:
//   1 = WorkerCreate
//   2 = WorkerDestroy
//   3 = WorkItemQueue
//   4 = WorkItemDequeue
//   5 = WorkerAdjust
//   6 = IOCompletion

REGHANDLE g_threadpool_provider = 0;
std::atomic<int32_t> g_provider_registered{0};

const EVENT_DESCRIPTOR kEventWorkerCreate    = { 0x01, 0x00, 0x00, 0x04, 0x00, 0x01, 0x0000000000000001ull };
const EVENT_DESCRIPTOR kEventWorkerDestroy   = { 0x02, 0x00, 0x00, 0x04, 0x00, 0x01, 0x0000000000000001ull };
const EVENT_DESCRIPTOR kEventWorkItemQueue   = { 0x03, 0x00, 0x00, 0x04, 0x00, 0x02, 0x0000000000000002ull };
const EVENT_DESCRIPTOR kEventWorkItemDequeue = { 0x04, 0x00, 0x00, 0x04, 0x00, 0x02, 0x0000000000000002ull };
const EVENT_DESCRIPTOR kEventWorkerAdjust    = { 0x05, 0x00, 0x00, 0x04, 0x00, 0x04, 0x0000000000000004ull };
const EVENT_DESCRIPTOR kEventIOCompletion    = { 0x06, 0x00, 0x00, 0x04, 0x00, 0x08, 0x0000000000000008ull };

}  // anonymous namespace
#endif

// ── Public API ─────────────────────────────────────────────────────────

void ThreadPoolEventProviderInitialize() noexcept {
#if defined(_WIN32) || defined(_WIN64)
    if (g_provider_registered.load(std::memory_order_relaxed) != 0) return;

    ULONG status = EventRegister(
        &kThreadPoolProviderGuid,
        nullptr,   // EnableCallback (null = no callback)
        nullptr,   // CallbackContext
        &g_threadpool_provider
    );

    if (status == ERROR_SUCCESS) {
        g_provider_registered.store(1, std::memory_order_release);
    }
#endif
}

void ThreadPoolEventProviderShutdown() noexcept {
#if defined(_WIN32) || defined(_WIN64)
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;

    EventUnregister(g_threadpool_provider);
    g_threadpool_provider = 0;
    g_provider_registered.store(0, std::memory_order_release);
#endif
}

// ── Helper to write a simple integer-only event ────────────────────────
#if defined(_WIN32) || defined(_WIN64)
namespace {
static void WriteIntEvent(const EVENT_DESCRIPTOR& desc, int32_t value) noexcept {
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;

    EVENT_DATA_DESCRIPTOR dataDesc;
    EventDataDescCreate(&dataDesc, &value, sizeof(value));
    EventWrite(g_threadpool_provider, &desc, 1, &dataDesc);
}

static void WriteIntPairEvent(const EVENT_DESCRIPTOR& desc, int32_t v1, int32_t v2, int32_t v3) noexcept {
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;

    // Pack three int32 values into 12 bytes.
    struct { int32_t a; int32_t b; int32_t c; } payload = { v1, v2, v3 };
    EVENT_DATA_DESCRIPTOR dataDesc;
    EventDataDescCreate(&dataDesc, &payload, sizeof(payload));
    EventWrite(g_threadpool_provider, &desc, 1, &dataDesc);
}

static void WriteUint32Event(const EVENT_DESCRIPTOR& desc, uint32_t value) noexcept {
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;

    EVENT_DATA_DESCRIPTOR dataDesc;
    EventDataDescCreate(&dataDesc, &value, sizeof(value));
    EventWrite(g_threadpool_provider, &desc, 1, &dataDesc);
}
}  // anonymous namespace
#endif

void ThreadPoolEventEmitWorkerCreate(int32_t thread_id) noexcept {
#if defined(_WIN32) || defined(_WIN64)
    WriteIntEvent(kEventWorkerCreate, thread_id);
#endif
}

void ThreadPoolEventEmitWorkerDestroy(int32_t thread_id) noexcept {
#if defined(_WIN32) || defined(_WIN64)
    WriteIntEvent(kEventWorkerDestroy, thread_id);
#endif
}

void ThreadPoolEventEmitWorkItemQueue(int64_t work_item_id) noexcept {
#if defined(_WIN32) || defined(_WIN64)
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;

    EVENT_DATA_DESCRIPTOR dataDesc;
    EventDataDescCreate(&dataDesc, &work_item_id, sizeof(work_item_id));
    EventWrite(g_threadpool_provider, &kEventWorkItemQueue, 1, &dataDesc);
#endif
}

void ThreadPoolEventEmitWorkItemDequeue(int64_t work_item_id) noexcept {
#if defined(_WIN32) || defined(_WIN64)
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;

    EVENT_DATA_DESCRIPTOR dataDesc;
    EventDataDescCreate(&dataDesc, &work_item_id, sizeof(work_item_id));
    EventWrite(g_threadpool_provider, &kEventWorkItemDequeue, 1, &dataDesc);
#endif
}

void ThreadPoolEventEmitWorkerAdjust(int32_t current_count, int32_t new_count, int32_t reason) noexcept {
#if defined(_WIN32) || defined(_WIN64)
    WriteIntPairEvent(kEventWorkerAdjust, current_count, new_count, reason);
#endif
}

void ThreadPoolEventEmitIOCompletion(uint32_t bytes_transferred) noexcept {
#if defined(_WIN32) || defined(_WIN64)
    WriteUint32Event(kEventIOCompletion, bytes_transferred);
#endif
}

}  // namespace chaos::il2cpp::runtime_core::threading
