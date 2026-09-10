/// gc_sos.cpp — CRAG GC WinDbg SOS Debugger Extension
///
/// WinDbg extension DLL that inspects CRAG GC state in a live or
/// crashed target process.  Commands are invoked as !gc.<command>.
///
/// Architecture:
///   - Loads into WinDbg as a classic DbgEng extension DLL
///   - Reads target process memory via IDebugDataSpaces::ReadVirtual
///   - Finds GC data via extern "C" debug contract symbols
///   - Interprets raw memory using struct layouts matching the GC runtime
///
/// Commands:
///   !gc.help       — Show all available commands
///   !gc.info       — GC subsystem state (generations, pause times)
///   !gc.stats      — All GC counters
///   !gc.events     — GC event ring buffer (last 64 events)
///   !gc.histogram  — Pause time distribution histogram
///   !gc.dumpall    — Dump all available GC state at once
///
/// Build: requires DbgEng SDK (Debugging Tools for Windows).
///   cmake --build --target chaos_gc_sos

#include <windows.h>
#include <dbgeng.h>

#include <cstdint>
#include <cstdio>

#include "sos_common.h"

// ══════════════════════════════════════════════════════════════════════════
// POD mirror of GcStats (gc_stats.h)
//
// std::atomic<uint64_t> in MSVC is layout-compatible with uint64_t.
// This struct mirrors the field order and sizes of GcStats so we can
// read raw target memory directly.
// ══════════════════════════════════════════════════════════════════════════

#pragma pack(push, 8)
struct GcStatsPod {
    uint64_t young_collections;
    uint64_t young_objects_promoted;
    uint64_t young_bytes_promoted;
    uint64_t young_bytes_reclaimed;
    uint64_t young_cards_scanned;
    uint64_t young_pause_ns;
    uint64_t full_collections;
    uint64_t full_pages_collected;
    uint64_t full_objects_marked;
    uint64_t full_bytes_reclaimed;
    uint64_t full_finalizers_run;
    uint64_t finalization_pending_count;
    uint64_t full_pause_ns;
    uint64_t gen1_collections;
    uint64_t gen1_objects_promoted;
    uint64_t gen1_bytes_promoted;
    uint64_t gen1_bytes_reclaimed;
    uint64_t gen1_pause_ns;
    int32_t last_compacted;
    int32_t last_concurrent;
    int32_t last_gc_generation;
    uint32_t _padding0;
    uint64_t gc_index;
    uint64_t alloc_total;
    uint64_t alloc_bytes;
    uint64_t alloc_oversized;
};
#pragma pack(pop)

// GcEventEntry — ring buffer entry layout (mirrors gc_stats.h).
struct GcEventEntryPod {
    uint8_t  is_full_gc;   // bool (1 byte) + 7 padding
    uint8_t  _pad[7];
    uint64_t pause_ns;
    uint64_t objects_processed;
    uint64_t bytes_reclaimed;
};

// ══════════════════════════════════════════════════════════════════════════
// Constants matching GC runtime
// ══════════════════════════════════════════════════════════════════════════

static constexpr int kGcRingSize     = 64;
static constexpr int kGcBucketCount  = 6;

// ══════════════════════════════════════════════════════════════════════════
// DbgEng interface pointers
// ══════════════════════════════════════════════════════════════════════════

IDebugClient*    g_sos_client   = nullptr;
IDebugControl*   g_sos_control  = nullptr;
IDebugSymbols*   g_sos_symbols  = nullptr;
IDebugDataSpaces* g_sos_data    = nullptr;

// ══════════════════════════════════════════════════════════════════════════
// Helpers
// ══════════════════════════════════════════════════════════════════════════

void SosPrint(PCSTR fmt, ...) noexcept {
    if (!g_sos_control) return;
    char buf[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    g_sos_control->Output(DEBUG_OUTPUT_NORMAL, "%s", buf);
}

bool SosReadTarget(ULONG64 offset, void* buf, ULONG size) noexcept {
    if (!g_sos_data) return false;
    ULONG bytes_read = 0;
    return SUCCEEDED(g_sos_data->ReadVirtual(offset, buf, size, &bytes_read))
           && bytes_read == size;
}

/// Resolve an extern "C" pointer variable in the target:
/// 1. GetOffsetByName to find the variable's address
/// 2. ReadVirtual to get the pointer value stored in the variable
ULONG64 SosResolvePointer(PCSTR name) noexcept {
    ULONG64 var_addr = 0;
    if (!SUCCEEDED(g_sos_symbols->GetOffsetByName(name, &var_addr))) {
        return 0;
    }
    ULONG64 ptr_val = 0;
    if (!SosReadTarget(var_addr, &ptr_val, sizeof(ptr_val))) {
        return 0;
    }
    return ptr_val;
}

// ══════════════════════════════════════════════════════════════════════════
// Standard DbgEng Extension Exports
// ══════════════════════════════════════════════════════════════════════════

// Forward decl — DebugExtensionInitialize's FAILED-QI path releases acquired
// references via DebugExtensionUninitialize, which is defined below.
extern "C" void CALLBACK DebugExtensionUninitialize();

extern "C" HRESULT CALLBACK
DebugExtensionInitialize(PDEBUG_CLIENT pDebugClient, void*, void*) {
    if (!pDebugClient) return E_FAIL;
    pDebugClient->AddRef();
    g_sos_client = pDebugClient;

    HRESULT hr = g_sos_client->QueryInterface(
        __uuidof(IDebugControl),
        reinterpret_cast<void**>(&g_sos_control));
    if (FAILED(hr)) goto fail;

    hr = g_sos_client->QueryInterface(
        __uuidof(IDebugSymbols),
        reinterpret_cast<void**>(&g_sos_symbols));
    if (FAILED(hr)) goto fail;

    hr = g_sos_client->QueryInterface(
        __uuidof(IDebugDataSpaces),
        reinterpret_cast<void**>(&g_sos_data));
    if (FAILED(hr)) goto fail;

    SosPrint("CRAG GC SOS extension loaded. Use !gc.help for commands.\n");
    return S_OK;

fail:
    // Release whatever was AddRef'd so far (an early FAILED QI must not leak).
    DebugExtensionUninitialize();
    return hr;
}

extern "C" void CALLBACK
DebugExtensionNotify(ULONG, ULONG) {}

extern "C" void CALLBACK
DebugExtensionUninitialize() {
    if (g_sos_data)    { g_sos_data->Release();    g_sos_data    = nullptr; }
    if (g_sos_symbols) { g_sos_symbols->Release(); g_sos_symbols = nullptr; }
    if (g_sos_control) { g_sos_control->Release(); g_sos_control = nullptr; }
    if (g_sos_client)  { g_sos_client->Release();  g_sos_client  = nullptr; }
}

// ══════════════════════════════════════════════════════════════════════════
// Command: !gc.help
// ══════════════════════════════════════════════════════════════════════════

extern "C" HRESULT CALLBACK
help(PDEBUG_CLIENT, PCSTR) {
    SosPrint("\n"
             "CRAG GC SOS Debugger Extension\n"
             "═══════════════════════════════\n"
             "Commands:\n"
             "  !gc.help       - Show this help\n"
             "  !gc.info       - GC subsystem state (generations, modes)\n"
             "  !gc.stats      - All GC statistics from GcSnapshot\n"
             "  !gc.events     - GC event ring buffer (last 64 events)\n"
             "  !gc.histogram  - Pause time distribution histogram\n"
             "  !gc.dumpall    - Dump all available GC state at once\n"
             "\n");
    return S_OK;
}

// ══════════════════════════════════════════════════════════════════════════
// Command: !gc.info — GC subsystem state
// ══════════════════════════════════════════════════════════════════════════

extern "C" HRESULT CALLBACK
info(PDEBUG_CLIENT, PCSTR) {
    ULONG64 stats_addr = SosResolvePointer("g_chaos_gc_stats_ptr");
    if (stats_addr == 0) {
        SosPrint("ERROR: Cannot locate g_chaos_gc_stats_ptr. Ensure the\n"
                 "target was built with gc_debug_contract enabled.\n");
        return E_FAIL;
    }

    GcStatsPod stats;
    if (!SosReadTarget(stats_addr, &stats, sizeof(stats))) {
        SosPrint("ERROR: Cannot read GcStats from target at 0x%llx.\n",
                 stats_addr);
        return E_FAIL;
    }

    uint64_t young_avg = stats.young_collections > 0
        ? stats.young_pause_ns / stats.young_collections : 0;
    uint64_t full_avg = stats.full_collections > 0
        ? stats.full_pause_ns / stats.full_collections : 0;
    uint64_t gen1_avg = stats.gen1_collections > 0
        ? stats.gen1_pause_ns / stats.gen1_collections : 0;

    const char* last_gen = "?";
    if (stats.last_gc_generation == 0) last_gen = "Young";
    else if (stats.last_gc_generation == 1) last_gen = "Gen1";
    else if (stats.last_gc_generation == 2) last_gen = "Full";

    SosPrint("\n"
             "CRAG GC Subsystem State\n"
             "═══════════════════════\n"
             "  GC Index:         %llu\n"
             "  Last Generation:  %s\n"
             "  Young Collections:  %llu\n"
             "  Full Collections:   %llu\n"
             "  Gen1 Collections:   %llu\n"
             "\n"
             "  Young Pause (total):  %llu ns  (avg: %llu ns)\n"
             "  Full  Pause (total):  %llu ns  (avg: %llu ns)\n"
             "  Gen1  Pause (total):  %llu ns  (avg: %llu ns)\n"
             "\n",
             stats.gc_index, last_gen,
             stats.young_collections, stats.full_collections,
             stats.gen1_collections,
             stats.young_pause_ns, young_avg,
             stats.full_pause_ns,  full_avg,
             stats.gen1_pause_ns,  gen1_avg);

    return S_OK;
}

// ══════════════════════════════════════════════════════════════════════════
// Command: !gc.stats — Full GC statistics
// ══════════════════════════════════════════════════════════════════════════

extern "C" HRESULT CALLBACK
stats(PDEBUG_CLIENT, PCSTR) {
    ULONG64 stats_addr = SosResolvePointer("g_chaos_gc_stats_ptr");
    if (stats_addr == 0) {
        SosPrint("ERROR: Cannot locate g_chaos_gc_stats_ptr.\n");
        return E_FAIL;
    }

    GcStatsPod stats;
    if (!SosReadTarget(stats_addr, &stats, sizeof(stats))) {
        SosPrint("ERROR: Cannot read GcStats from target.\n");
        return E_FAIL;
    }

    uint64_t young_avg = stats.young_collections > 0
        ? stats.young_pause_ns / stats.young_collections : 0;
    uint64_t full_avg = stats.full_collections > 0
        ? stats.full_pause_ns / stats.full_collections : 0;
    uint64_t gen1_avg = stats.gen1_collections > 0
        ? stats.gen1_pause_ns / stats.gen1_collections : 0;

    SosPrint("\n"
             "CRAG GC Statistics\n"
             "══════════════════\n"
             "── Young Collection ──\n"
             "  Collections:      %llu\n"
             "  Objects Promoted: %llu\n"
             "  Bytes Promoted:   %llu\n"
             "  Bytes Reclaimed:  %llu\n"
             "  Cards Scanned:    %llu\n"
             "  Pause (total):    %llu ns\n"
             "  Pause (avg):      %llu ns\n"
             "\n"
             "── Full Collection ──\n"
             "  Collections:      %llu\n"
             "  Pages Collected:  %llu\n"
             "  Objects Marked:   %llu\n"
             "  Bytes Reclaimed:  %llu\n"
             "  Finalizers Run:   %llu\n"
             "  Pause (total):    %llu ns\n"
             "  Pause (avg):      %llu ns\n"
             "\n"
             "── Gen1 Collection ──\n"
             "  Collections:      %llu\n"
             "  Objects Promoted: %llu\n"
             "  Bytes Promoted:   %llu\n"
             "  Bytes Reclaimed:  %llu\n"
             "  Pause (total):    %llu ns\n"
             "  Pause (avg):      %llu ns\n"
             "\n"
             "── Allocation ──\n"
             "  Total Allocs:     %llu\n"
             "  Total Bytes:      %llu\n"
             "  Oversized Allocs: %llu\n"
             "  Finalization Pending: %llu\n"
             "\n",
             stats.young_collections, stats.young_objects_promoted,
             stats.young_bytes_promoted, stats.young_bytes_reclaimed,
             stats.young_cards_scanned,
             stats.young_pause_ns, young_avg,
             stats.full_collections, stats.full_pages_collected,
             stats.full_objects_marked, stats.full_bytes_reclaimed,
             stats.full_finalizers_run,
             stats.full_pause_ns, full_avg,
             stats.gen1_collections, stats.gen1_objects_promoted,
             stats.gen1_bytes_promoted, stats.gen1_bytes_reclaimed,
             stats.gen1_pause_ns, gen1_avg,
             stats.alloc_total, stats.alloc_bytes,
             stats.alloc_oversized, stats.finalization_pending_count);

    return S_OK;
}

// ══════════════════════════════════════════════════════════════════════════
// Command: !gc.events — Dump GC event ring buffer
// ══════════════════════════════════════════════════════════════════════════

extern "C" HRESULT CALLBACK
events(PDEBUG_CLIENT, PCSTR) {
    ULONG64 ring_addr   = SosResolvePointer("g_chaos_gc_event_ring_ptr");
    ULONG64 head_addr   = SosResolvePointer("g_chaos_gc_event_ring_head_ptr");

    if (ring_addr == 0 || head_addr == 0) {
        SosPrint("ERROR: Cannot locate GC event ring symbols.\n");
        return E_FAIL;
    }

    // Read ring size from target.
    ULONG64 size_var_addr = 0;
    int32_t ring_size = kGcRingSize;
    if (SUCCEEDED(g_sos_symbols->GetOffsetByName("g_chaos_gc_event_ring_size",
                                             &size_var_addr))) {
        SosReadTarget(size_var_addr, &ring_size, sizeof(ring_size));
    }

    // Read ring head index.
    int ring_head = 0;
    if (!SosReadTarget(head_addr, &ring_head, sizeof(ring_head))) {
        SosPrint("WARNING: Cannot read ring head, assuming 0.\n");
    }

    // Read ring entries.
    GcEventEntryPod ring_buf[64];
    ULONG entries_to_read = (ring_size > 64) ? 64
                          : (ring_size <= 0) ? 64 : ring_size;

    if (!SosReadTarget(ring_addr, ring_buf,
                    entries_to_read * sizeof(GcEventEntryPod))) {
        SosPrint("ERROR: Cannot read GC event ring buffer.\n");
        return E_FAIL;
    }

    SosPrint("\n"
             "GC Event Ring Buffer (cap=%d, head=%d)\n"
             "══════════════════════════════════════\n"
             "  #  | Type     | Pause (ns) | Processed | Reclaimed\n"
             "  ---+----------+------------+-----------+----------\n",
             entries_to_read, ring_head);

    int printed = 0;
    for (int i = 0; i < entries_to_read; i++) {
        int idx = (ring_head + i) % entries_to_read;
        const auto& e = ring_buf[idx];
        if (e.pause_ns == 0 && e.bytes_reclaimed == 0) {
            continue;
        }
        SosPrint("  %2d | %-8s | %10llu | %9llu | %llu\n",
                 i,
                 e.is_full_gc ? "Full" : "Young",
                 e.pause_ns,
                 e.objects_processed,
                 e.bytes_reclaimed);
        printed++;
    }
    if (printed == 0) {
        SosPrint("  (no events recorded yet)\n");
    }
    SosPrint("\n");

    return S_OK;
}

// ══════════════════════════════════════════════════════════════════════════
// Command: !gc.histogram — Pause time distribution
// ══════════════════════════════════════════════════════════════════════════

static const char* kBucketNames[] = {
    "0-1 ms", "1-5 ms", "5-10 ms", "10-50 ms",
    "50-100 ms", "100+ ms"
};

extern "C" HRESULT CALLBACK
histogram(PDEBUG_CLIENT, PCSTR) {
    ULONG64 hist_addr   = SosResolvePointer("g_chaos_gc_pause_histogram_ptr");
    ULONG64 bounds_addr = SosResolvePointer("g_chaos_gc_pause_bucket_bounds_ptr");

    if (hist_addr == 0) {
        SosPrint("ERROR: Cannot locate GC pause histogram.\n");
        return E_FAIL;
    }

    // Read bucket count from target.
    int32_t bucket_count = kGcBucketCount;
    ULONG64 count_var_addr = 0;
    if (SUCCEEDED(g_sos_symbols->GetOffsetByName("g_chaos_gc_pause_bucket_count",
                                             &count_var_addr))) {
        SosReadTarget(count_var_addr, &bucket_count, sizeof(bucket_count));
    }
    if (bucket_count > kGcBucketCount) bucket_count = kGcBucketCount;

    // Read histogram bucket values (atomic<uint64_t> = plain uint64_t layout).
    uint64_t hist_values[6] = {};
    if (!SosReadTarget(hist_addr, hist_values,
                    bucket_count * sizeof(uint64_t))) {
        SosPrint("ERROR: Cannot read pause histogram values.\n");
        return E_FAIL;
    }

    // Count total samples.
    uint64_t total = 0;
    for (int i = 0; i < bucket_count; i++) total += hist_values[i];

    SosPrint("\n"
             "GC Pause Time Histogram (total samples: %llu)\n"
             "══════════════════════════════════════════════\n",
             total);

    if (total == 0) {
        SosPrint("  (no GC pauses recorded yet)\n\n");
        return S_OK;
    }

    for (int i = 0; i < bucket_count && i < 6; i++) {
        double pct = 100.0 * hist_values[i] / total;
        SosPrint("  %-12s: %5llu  (%5.1f%%)\n",
                 kBucketNames[i], hist_values[i], pct);
    }
    SosPrint("  ────────────\n"
             "  Total       : %5llu  (100.0%%)\n"
             "\n",
             total);

    return S_OK;
}

// ══════════════════════════════════════════════════════════════════════════
// Command: !gc.dumpall — Dump all GC state at once
// ══════════════════════════════════════════════════════════════════════════

extern "C" HRESULT CALLBACK
dumpall(PDEBUG_CLIENT Client, PCSTR) {
    help(Client, "");
    info(Client, "");
    stats(Client, "");
    histogram(Client, "");
    events(Client, "");
    return S_OK;
}

// ══════════════════════════════════════════════════════════════════════════
// DllMain
// ══════════════════════════════════════════════════════════════════════════

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) {
    return TRUE;
}
