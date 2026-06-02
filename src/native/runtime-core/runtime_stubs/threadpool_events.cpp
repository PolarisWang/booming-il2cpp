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
//   - IO completion notifications

#include <chaos/pal/pal_etw.h>

#include <cstdint>
#include <atomic>
#include "threadpool_events.h"

namespace chaos::il2cpp::runtime_core::threading {

// ── Provider GUID ──────────────────────────────────────────────────────
// {C9A3E1F0-6C7A-4DF8-9D5C-5E1B8C3F7A2D}
// Unique to Chaos IL2CPP ThreadPool diagnostics.
static const uint8_t kThreadPoolProviderGuid[16] = {
    0xC9, 0xA3, 0xE1, 0xF0, 0x6C, 0x7A, 0x4D, 0xF8,
    0x9D, 0x5C, 0x5E, 0x1B, 0x8C, 0x3F, 0x7A, 0x2D
};

// Event keywords (matching the original ETW convention):
//   0x1 = ThreadPool worker lifecycle
//   0x2 = ThreadPool work item operations
//   0x4 = ThreadPool hill-climbing adjustments
//   0x8 = ThreadPool I/O operations

// Level: 4 = Informational
static constexpr uint8_t kEtwLevelInfo = 4;

chaos::il2cpp::pal::PalEtwProvider* g_threadpool_provider = nullptr;
std::atomic<int32_t> g_provider_registered{0};

// ── Public API ─────────────────────────────────────────────────────────

void ThreadPoolEventProviderInitialize() noexcept {
    if (g_provider_registered.load(std::memory_order_relaxed) != 0) return;

    g_threadpool_provider = chaos::il2cpp::pal::PalEtwRegister(kThreadPoolProviderGuid);
    if (g_threadpool_provider != nullptr) {
        g_provider_registered.store(1, std::memory_order_release);
    }
}

void ThreadPoolEventProviderShutdown() noexcept {
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;

    chaos::il2cpp::pal::PalEtwUnregister(g_threadpool_provider);
    g_threadpool_provider = nullptr;
    g_provider_registered.store(0, std::memory_order_release);
}

// ── Helper to write a simple integer-only event ────────────────────────
static void WriteIntEvent(uint8_t event_id, uint64_t keyword, int32_t value) noexcept {
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;
    chaos::il2cpp::pal::PalEtwWrite(g_threadpool_provider,
        event_id, 0, 0, kEtwLevelInfo, keyword,
        &value, sizeof(value));
}

static void WriteIntTripleEvent(uint8_t event_id, uint64_t keyword,
                                int32_t v1, int32_t v2, int32_t v3) noexcept
{
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;
    struct { int32_t a; int32_t b; int32_t c; } payload = { v1, v2, v3 };
    chaos::il2cpp::pal::PalEtwWrite(g_threadpool_provider,
        event_id, 0, 0, kEtwLevelInfo, keyword,
        &payload, sizeof(payload));
}

static void WriteUint32Event(uint8_t event_id, uint64_t keyword, uint32_t value) noexcept {
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;
    chaos::il2cpp::pal::PalEtwWrite(g_threadpool_provider,
        event_id, 0, 0, kEtwLevelInfo, keyword,
        &value, sizeof(value));
}

void ThreadPoolEventEmitWorkerCreate(int32_t thread_id) noexcept {
    WriteIntEvent(0x01, 0x0000000000000001ull, thread_id);
}

void ThreadPoolEventEmitWorkerDestroy(int32_t thread_id) noexcept {
    WriteIntEvent(0x02, 0x0000000000000001ull, thread_id);
}

void ThreadPoolEventEmitWorkItemQueue(int64_t work_item_id) noexcept {
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;
    chaos::il2cpp::pal::PalEtwWrite(g_threadpool_provider,
        0x03, 0, 0, kEtwLevelInfo, 0x0000000000000002ull,
        &work_item_id, sizeof(work_item_id));
}

void ThreadPoolEventEmitWorkItemDequeue(int64_t work_item_id) noexcept {
    if (g_provider_registered.load(std::memory_order_acquire) == 0) return;
    chaos::il2cpp::pal::PalEtwWrite(g_threadpool_provider,
        0x04, 0, 0, kEtwLevelInfo, 0x0000000000000002ull,
        &work_item_id, sizeof(work_item_id));
}

void ThreadPoolEventEmitWorkerAdjust(int32_t current_count, int32_t new_count, int32_t reason) noexcept {
    WriteIntTripleEvent(0x05, 0x0000000000000004ull, current_count, new_count, reason);
}

void ThreadPoolEventEmitIOCompletion(uint32_t bytes_transferred) noexcept {
    WriteUint32Event(0x06, 0x0000000000000008ull, bytes_transferred);
}

}  // namespace chaos::il2cpp::runtime_core::threading
