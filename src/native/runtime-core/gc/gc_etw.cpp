// gc_etw.cpp — ETW event provider for CRAG GC
//
// Implements the GC ETW provider using the classic EventRegister/EventWrite
// API.  On non-Windows platforms all functions are no-ops.
//
// Pattern: follows threadpool_events.cpp convention exactly:
//   - EVENT_DESCRIPTOR constants per event type
//   - EventDataDescCreate + EventWrite for emission
//   - Lazy registration via atomic g_provider_registered flag
//   - Platform-guarded with #if defined(_WIN32) || defined(_WIN64)

#include "gc_etw.h"
#include "gc_features.h"

#include <cstdint>
#include <atomic>

#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0601  // Windows 7+
    #endif
    #include <windows.h>
    #include <evntprov.h>
#endif

namespace chaos::il2cpp::runtime_core {

// ── Provider GUID and event descriptors ──────────────────────────────
// {3A5C4B6D-8E9F-4A2B-9C1D-7E8F0A3B6C5D}
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
namespace {

const GUID kGcEtwProviderGuid = {
    0x3a5c4b6d, 0x8e9f, 0x4a2b,
    { 0x9c, 0x1d, 0x7e, 0x8f, 0x0a, 0x3b, 0x6c, 0x5d }
};

// Event descriptors: (id, version, channel, level, opcode, task, keyword)
//   id:      0x01-0x08  (unique event ID)
//   version: 0x00
//   channel: 0x00 (admin)
//   level:   0x04 (Informational)
//   opcode:  0x00
//   task:    0x01-0x08  (matches event ID)
//   keyword: 0x0000000000000001 (GC lifecycle)
const EVENT_DESCRIPTOR kEtwGcStart       = { 0x01, 0x00, 0x00, 0x04, 0x00, 0x01, 0x0000000000000001ull };
const EVENT_DESCRIPTOR kEtwGcEnd         = { 0x02, 0x00, 0x00, 0x04, 0x00, 0x02, 0x0000000000000001ull };
const EVENT_DESCRIPTOR kEtwGcYoungStart  = { 0x03, 0x00, 0x00, 0x04, 0x00, 0x03, 0x0000000000000001ull };
const EVENT_DESCRIPTOR kEtwGcYoungEnd    = { 0x04, 0x00, 0x00, 0x04, 0x00, 0x04, 0x0000000000000001ull };
const EVENT_DESCRIPTOR kEtwGcFullStart   = { 0x05, 0x00, 0x00, 0x04, 0x00, 0x05, 0x0000000000000001ull };
const EVENT_DESCRIPTOR kEtwGcFullEnd     = { 0x06, 0x00, 0x00, 0x04, 0x00, 0x06, 0x0000000000000001ull };
const EVENT_DESCRIPTOR kEtwGcOom         = { 0x07, 0x00, 0x00, 0x04, 0x00, 0x07, 0x0000000000000001ull };
const EVENT_DESCRIPTOR kEtwGcGen1Collect = { 0x08, 0x00, 0x00, 0x04, 0x00, 0x08, 0x0000000000000001ull };
const EVENT_DESCRIPTOR kEtwGcAllocationTick = { 0x09, 0x00, 0x00, 0x04, 0x00, 0x09, 0x0000000000000002ull };

REGHANDLE g_gc_etw_provider = 0;
std::atomic<int32_t> g_gc_etw_registered{0};

// ── Helper: write a payload as a single contiguous struct ────────────
template <typename T>
static void WriteEtwEvent(const EVENT_DESCRIPTOR& desc, const T& payload) noexcept {
    if (g_gc_etw_registered.load(std::memory_order_acquire) == 0) return;

    EVENT_DATA_DESCRIPTOR dataDesc;
    EventDataDescCreate(&dataDesc, &payload, sizeof(payload));
    EventWrite(g_gc_etw_provider, &desc, 1, &dataDesc);
}

// ── Payload structs (packed, no padding between fields) ──────────────
#pragma pack(push, 1)
struct GcEtwPayloadGcStart {
    uint32_t generation;
};
struct GcEtwPayloadGcEnd {
    uint64_t pause_ns;
    uint64_t reclaimed_bytes;
};
struct GcEtwPayloadYoungStart {
    uint64_t nursery_used;
};
struct GcEtwPayloadYoungEnd {
    uint64_t pause_ns;
    uint64_t objects_promoted;
    uint64_t bytes_promoted;
    uint64_t bytes_reclaimed;
};
struct GcEtwPayloadFullStart {
    uint32_t page_count;
};
struct GcEtwPayloadFullEnd {
    uint64_t pause_ns;
    uint64_t reclaimed_bytes;
    uint64_t objects_marked;
    uint64_t pages_collected;
};
struct GcEtwPayloadGen1Collect {
    uint64_t pause_ns;
    uint64_t objects_promoted;
    uint64_t bytes_reclaimed;
};
struct GcEtwPayloadAllocationTick {
    uint32_t allocation_amount;   // bytes allocated since last tick
    uint32_t allocation_kind;     // 0=small object, 1=large object (LOH)
};
#pragma pack(pop)

// ── Per-thread allocation tick tracking ──────────────────────────────
// Fires GCAllocationTick every ~kAllocationTickThreshold bytes of managed
// allocation.  Using a per-thread counter so multi-threaded workloads each
// get independent tick coverage.  Matches CoreCLR behavior.
static constexpr uint32_t kAllocationTickThreshold = 1024 * 100;  // 100 KB
thread_local uint32_t tls_alloc_tick_counter = 0;

}  // anonymous namespace
#endif  // _WIN32 && CHAOS_IL2CPP_GC_EVENTS

// ── Public API ───────────────────────────────────────────────────────

void GcEtwInitialize() noexcept {
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    if (g_gc_etw_registered.load(std::memory_order_relaxed) != 0) return;

    ULONG status = EventRegister(
        &kGcEtwProviderGuid,
        nullptr,   // EnableCallback (null = no callback)
        nullptr,   // CallbackContext
        &g_gc_etw_provider
    );

    if (status == ERROR_SUCCESS) {
        g_gc_etw_registered.store(1, std::memory_order_release);
    }
#endif
}

void GcEtwShutdown() noexcept {
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    if (g_gc_etw_registered.load(std::memory_order_acquire) == 0) return;

    EventUnregister(g_gc_etw_provider);
    g_gc_etw_provider = 0;
    g_gc_etw_registered.store(0, std::memory_order_release);
#endif
}

void GcEtwFireGcStart(uint32_t generation) noexcept {
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();  // lazy init on first use
    GcEtwPayloadGcStart payload{ generation };
    WriteEtwEvent(kEtwGcStart, payload);
#endif
}

void GcEtwFireGcEnd(uint64_t pause_ns, uint64_t reclaimed_bytes) noexcept {
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadGcEnd payload{ pause_ns, reclaimed_bytes };
    WriteEtwEvent(kEtwGcEnd, payload);
#endif
}

void GcEtwFireGcYoungStart(uint64_t nursery_used) noexcept {
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadYoungStart payload{ nursery_used };
    WriteEtwEvent(kEtwGcYoungStart, payload);
#endif
}

void GcEtwFireGcYoungEnd(
    uint64_t pause_ns,
    uint64_t objects_promoted,
    uint64_t bytes_promoted,
    uint64_t bytes_reclaimed) noexcept
{
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadYoungEnd payload{ pause_ns, objects_promoted, bytes_promoted, bytes_reclaimed };
    WriteEtwEvent(kEtwGcYoungEnd, payload);
#endif
}

void GcEtwFireGcFullStart(uint32_t page_count) noexcept {
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadFullStart payload{ page_count };
    WriteEtwEvent(kEtwGcFullStart, payload);
#endif
}

void GcEtwFireGcFullEnd(
    uint64_t pause_ns,
    uint64_t reclaimed_bytes,
    uint64_t objects_marked,
    uint64_t pages_collected) noexcept
{
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadFullEnd payload{ pause_ns, reclaimed_bytes, objects_marked, pages_collected };
    WriteEtwEvent(kEtwGcFullEnd, payload);
#endif
}

void GcEtwFireGcOom() noexcept {
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    // No payload — OOM is a bare signal.
    EVENT_DATA_DESCRIPTOR dataDesc;
    EventDataDescCreate(&dataDesc, nullptr, 0);
    EventWrite(g_gc_etw_provider, &kEtwGcOom, 0, nullptr);
#endif
}

void GcEtwFireGcGen1Collect(
    uint64_t pause_ns,
    uint64_t objects_promoted,
    uint64_t bytes_reclaimed) noexcept
{
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadGen1Collect payload{ pause_ns, objects_promoted, bytes_reclaimed };
    WriteEtwEvent(kEtwGcGen1Collect, payload);
#endif
}

void GcEtwFireAllocationTick(uint32_t bytes, uint32_t is_large_object) noexcept {
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    if (g_gc_etw_registered.load(std::memory_order_acquire) == 0) return;
    GcEtwPayloadAllocationTick payload{ bytes, is_large_object };
    WriteEtwEvent(kEtwGcAllocationTick, payload);
#endif
}

/// Record an allocation for tick tracking.  Called from GcRecordAlloc on every
/// managed allocation.  Accumulates a per-thread counter and fires the ETW
/// GCAllocationTick event when the threshold (100 KB) is crossed.
void GcEtwRecordAlloc(CHAOS_IL2CPP_SIZE bytes) noexcept {
#if (defined(_WIN32) || defined(_WIN64)) && \
    defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    if (g_gc_etw_registered.load(std::memory_order_acquire) == 0) return;

    // Accumulate.  Use saturation-at-max to avoid overflow on pathological
    // single-call sizes (e.g. a 4GB array allocation).
    uint32_t prev = tls_alloc_tick_counter;
    uint32_t add = bytes > UINT32_MAX ? UINT32_MAX : static_cast<uint32_t>(bytes);
    uint32_t sum = prev + add;
    if (sum < prev) sum = UINT32_MAX;  // saturation on overflow
    tls_alloc_tick_counter = sum;

    if (sum >= kAllocationTickThreshold) {
        GcEtwFireAllocationTick(sum, bytes > 85000 ? 1 : 0);
        tls_alloc_tick_counter = 0;
    }
#endif
}

}  // namespace chaos::il2cpp::runtime_core
