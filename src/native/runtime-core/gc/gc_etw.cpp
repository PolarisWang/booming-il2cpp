// gc_etw.cpp — ETW event provider for CRAG GC
//
// Implements the GC ETW provider using the chaos PAL abstraction.
// On non-Windows platforms all functions are no-ops (the provider
// handle is always nullptr).
//
// The CHAOS_IL2CPP_GC_EVENTS compile flag controls whether GC ETW
// events are compiled at all (independent of platform).

#include "gc_etw.h"
#include "gc_features.h"

#include <chaos/pal/pal_etw.h>

#include <cstdint>
#include <atomic>

namespace chaos::il2cpp::runtime_core {

// ── Provider GUID and event descriptors ──────────────────────────────
// {3A5C4B6D-8E9F-4A2B-9C1D-7E8F0A3B6C5D}
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
namespace {

static const uint8_t kGcEtwProviderGuid[16] = {
    0x3A, 0x5C, 0x4B, 0x6D, 0x8E, 0x9F, 0x4A, 0x2B,
    0x9C, 0x1D, 0x7E, 0x8F, 0x0A, 0x3B, 0x6C, 0x5D
};

static constexpr uint8_t kEtwLevelInfo = 4;
static constexpr uint64_t kKeywordGcLifecycle = 0x0000000000000001ull;
static constexpr uint64_t kKeywordGcAllocation = 0x0000000000000002ull;

// Event IDs
static constexpr uint8_t kEventGcStart          = 0x01;
static constexpr uint8_t kEventGcEnd            = 0x02;
static constexpr uint8_t kEventGcYoungStart     = 0x03;
static constexpr uint8_t kEventGcYoungEnd       = 0x04;
static constexpr uint8_t kEventGcFullStart      = 0x05;
static constexpr uint8_t kEventGcFullEnd        = 0x06;
static constexpr uint8_t kEventGcOom            = 0x07;
static constexpr uint8_t kEventGcGen1Collect    = 0x08;
static constexpr uint8_t kEventGcAllocationTick = 0x09;

chaos::il2cpp::pal::PalEtwProvider* g_gc_etw_provider = nullptr;
std::atomic<int32_t> g_gc_etw_registered{0};

// ── Helper: write a payload as a single contiguous struct ────────────
template <typename T>
static void WriteEtwEvent(uint8_t event_id, const T& payload) noexcept {
    if (g_gc_etw_registered.load(std::memory_order_acquire) == 0) return;
    chaos::il2cpp::pal::PalEtwWrite(g_gc_etw_provider,
        event_id, 0, 0, kEtwLevelInfo, kKeywordGcLifecycle,
        &payload, sizeof(payload));
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
static constexpr uint32_t kAllocationTickThreshold = 1024 * 100;  // 100 KB
thread_local uint32_t tls_alloc_tick_counter = 0;

}  // anonymous namespace
#endif  // CHAOS_IL2CPP_GC_EVENTS

// ── Public API ───────────────────────────────────────────────────────

void GcEtwInitialize() noexcept {
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    if (g_gc_etw_registered.load(std::memory_order_relaxed) != 0) return;

    g_gc_etw_provider = chaos::il2cpp::pal::PalEtwRegister(kGcEtwProviderGuid);
    if (g_gc_etw_provider != nullptr) {
        g_gc_etw_registered.store(1, std::memory_order_release);
    }
#endif
}

void GcEtwShutdown() noexcept {
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    if (g_gc_etw_registered.load(std::memory_order_acquire) == 0) return;

    chaos::il2cpp::pal::PalEtwUnregister(g_gc_etw_provider);
    g_gc_etw_provider = nullptr;
    g_gc_etw_registered.store(0, std::memory_order_release);
#endif
}

void GcEtwFireGcStart(uint32_t generation) noexcept {
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();  // lazy init on first use
    GcEtwPayloadGcStart payload{ generation };
    WriteEtwEvent(kEventGcStart, payload);
#endif
}

void GcEtwFireGcEnd(uint64_t pause_ns, uint64_t reclaimed_bytes) noexcept {
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadGcEnd payload{ pause_ns, reclaimed_bytes };
    WriteEtwEvent(kEventGcEnd, payload);
#endif
}

void GcEtwFireGcYoungStart(uint64_t nursery_used) noexcept {
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadYoungStart payload{ nursery_used };
    WriteEtwEvent(kEventGcYoungStart, payload);
#endif
}

void GcEtwFireGcYoungEnd(
    uint64_t pause_ns,
    uint64_t objects_promoted,
    uint64_t bytes_promoted,
    uint64_t bytes_reclaimed) noexcept
{
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadYoungEnd payload{ pause_ns, objects_promoted, bytes_promoted, bytes_reclaimed };
    WriteEtwEvent(kEventGcYoungEnd, payload);
#endif
}

void GcEtwFireGcFullStart(uint32_t page_count) noexcept {
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadFullStart payload{ page_count };
    WriteEtwEvent(kEventGcFullStart, payload);
#endif
}

void GcEtwFireGcFullEnd(
    uint64_t pause_ns,
    uint64_t reclaimed_bytes,
    uint64_t objects_marked,
    uint64_t pages_collected) noexcept
{
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadFullEnd payload{ pause_ns, reclaimed_bytes, objects_marked, pages_collected };
    WriteEtwEvent(kEventGcFullEnd, payload);
#endif
}

void GcEtwFireGcOom() noexcept {
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    // No payload — OOM is a bare signal.
    chaos::il2cpp::pal::PalEtwWrite(g_gc_etw_provider,
        kEventGcOom, 0, 0, kEtwLevelInfo, kKeywordGcLifecycle,
        nullptr, 0);
#endif
}

void GcEtwFireGcGen1Collect(
    uint64_t pause_ns,
    uint64_t objects_promoted,
    uint64_t bytes_reclaimed) noexcept
{
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    GcEtwInitialize();
    GcEtwPayloadGen1Collect payload{ pause_ns, objects_promoted, bytes_reclaimed };
    WriteEtwEvent(kEventGcGen1Collect, payload);
#endif
}

void GcEtwFireAllocationTick(uint32_t bytes, uint32_t is_large_object) noexcept {
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
    if (g_gc_etw_registered.load(std::memory_order_acquire) == 0) return;
    GcEtwPayloadAllocationTick payload{ bytes, is_large_object };
    chaos::il2cpp::pal::PalEtwWrite(g_gc_etw_provider,
        kEventGcAllocationTick, 0, 0, kEtwLevelInfo, kKeywordGcAllocation,
        &payload, sizeof(payload));
#endif
}

void GcEtwRecordAlloc(CHAOS_IL2CPP_SIZE bytes) noexcept {
#if defined(CHAOS_IL2CPP_GC_EVENTS) && CHAOS_IL2CPP_GC_EVENTS == 1
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
