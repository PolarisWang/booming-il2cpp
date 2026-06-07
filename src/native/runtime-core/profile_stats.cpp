// profile_stats.cpp — Thread-local profile data collection for --profile mode
//
// This file implements the per-dispatch profiling infrastructure used by
// entry.exe --profile.  Each dispatch thread maintains a thread-local
// ProfileData accumulator.  Before each method dispatch the caller records
// heap_before; during dispatch, GC/allocation paths update the accumulator
// via ProfileRecord*(); after dispatch, the caller records heap_after and
// calls FlushThreadProfileData() to commit a ProfileSnapshot to the
// process-wide store.
//
// Only compiled when CHAOS_IL2CPP_PROFILE_ENABLED=1 (profile build).
// In check/ship builds this entire file compiles to empty stubs or is
// excluded at the cmake level.

#include "profile_stats.h"

#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>

// ── Thread-local accumulator ────────────────────────────────────────
// Each thread gets its own ProfileData so no locking is needed during
// the hot dispatch loop.  The main thread commits snapshots serially.

#if CHAOS_IL2CPP_PROFILE_ENABLED

namespace chaos { namespace il2cpp { namespace runtime_core {

static thread_local ProfileData tls_profile_data{};
static thread_local bool tls_profile_initialized = false;

ProfileData& GetThreadProfileData() noexcept {
    tls_profile_initialized = true;
    return tls_profile_data;
}

void ResetThreadProfileData() noexcept {
    if (tls_profile_initialized) {
        tls_profile_data = ProfileData{};
    }
}

// ── Process-wide snapshot store ─────────────────────────────────────
// Fixed-size ring buffer.  The main thread calls ProfileStoreAdd() after
// each method dispatch to commit the snapshot.  Since dispatch is single-
// threaded in the benchmark host, no locking is needed.

static ProfileSnapshot s_profile_store[kMaxProfileMethods];
static int s_profile_count = 0;
static int s_profile_capacity = 0;

void ProfileStoreInit(int capacity) noexcept {
    if (capacity <= 0 || capacity > kMaxProfileMethods) {
        capacity = kMaxProfileMethods;
    }
    s_profile_capacity = capacity;
    s_profile_count = 0;
}

bool ProfileStoreAdd(const ProfileSnapshot& snap) noexcept {
    if (s_profile_count >= s_profile_capacity) {
        return false;
    }
    s_profile_store[s_profile_count] = snap;
    s_profile_count++;
    return true;
}

int ProfileStoreFinalize() noexcept {
    return s_profile_count;
}

const ProfileSnapshot* ProfileStoreGet(int index) noexcept {
    if (index < 0 || index >= s_profile_count) return nullptr;
    return &s_profile_store[index];
}

int ProfileStoreCount() noexcept {
    return s_profile_count;
}

// ── GC/allocation recording helpers ─────────────────────────────────

void ProfileRecordGcPause(int64_t pause_ns) noexcept {
    if (!tls_profile_initialized) return;
    tls_profile_data.gc_pause.total_pause_ns += pause_ns;
    if (pause_ns > tls_profile_data.gc_pause.max_pause_ns) {
        tls_profile_data.gc_pause.max_pause_ns = pause_ns;
    }
    tls_profile_data.gc_pause.pause_count++;
}

void ProfileRecordNurseryAlloc(int64_t size) noexcept {
    if (!tls_profile_initialized) return;
    tls_profile_data.alloc_volume.nursery_bytes += size;
}

void ProfileRecordOldGenAlloc(int64_t size) noexcept {
    if (!tls_profile_initialized) return;
    tls_profile_data.alloc_volume.old_gen_bytes += size;
}

void ProfileRecordLargeObjAlloc(int64_t size) noexcept {
    if (!tls_profile_initialized) return;
    tls_profile_data.alloc_volume.large_object_bytes += size;
}

void ProfileRecordAllocCount(int32_t count) noexcept {
    if (!tls_profile_initialized) return;
    tls_profile_data.alloc_volume.alloc_count += count;
}

void ProfileRecordFastPath() noexcept {
    if (!tls_profile_initialized) return;
    tls_profile_data.fast_path_count++;
}

void ProfileRecordSlowPath() noexcept {
    if (!tls_profile_initialized) return;
    tls_profile_data.slow_path_count++;
}

// ── Flush: commit thread-local data to the process-wide store ───────

void FlushThreadProfileData(int method_index) noexcept {
    if (!tls_profile_initialized) return;

    // Snapshot the current state
    ProfileSnapshot snap;
    snap.method_index = method_index;
    snap.data = tls_profile_data;

    // Commit to process-wide store
    ProfileStoreAdd(snap);

    // Reset thread-local data for next method
    // (heap_before will be set by the caller before the next dispatch)
    tls_profile_data = ProfileData{};
}

// ── JSON emission ───────────────────────────────────────────────────

void ProfileEmitJson() noexcept {
    int count = ProfileStoreCount();
    printf("{\"profile\":[");
    for (int i = 0; i < count; i++) {
        const auto* snap = ProfileStoreGet(i);
        if (!snap) continue;
        if (i > 0) printf(",");
        printf("{"
               "\"methodIndex\":%d,"
               "\"gcPauseNs\":%" PRId64 ","
               "\"maxGcPauseNs\":%" PRId64 ","
               "\"gcPauseCount\":%d,"
               "\"nurseryAllocBytes\":%" PRId64 ","
               "\"oldGenAllocBytes\":%" PRId64 ","
               "\"largeObjBytes\":%" PRId64 ","
               "\"allocCount\":%d,"
               "\"heapBefore\":%" PRId64 ","
               "\"heapAfter\":%" PRId64 ","
               "\"fastPathCount\":%d,"
               "\"slowPathCount\":%d"
               "}",
               snap->method_index,
               snap->data.gc_pause.total_pause_ns,
               snap->data.gc_pause.max_pause_ns,
               snap->data.gc_pause.pause_count,
               snap->data.alloc_volume.nursery_bytes,
               snap->data.alloc_volume.old_gen_bytes,
               snap->data.alloc_volume.large_object_bytes,
               snap->data.alloc_volume.alloc_count,
               snap->data.heap_before,
               snap->data.heap_after,
               snap->data.fast_path_count,
               snap->data.slow_path_count);
    }
    printf("]}\n");
    fflush(stdout);
}

}}} // namespace chaos::il2cpp::runtime_core

#else // !CHAOS_IL2CPP_PROFILE_ENABLED

// Stubs for non-profile builds — compile to no-ops
namespace chaos { namespace il2cpp { namespace runtime_core {

ProfileData& GetThreadProfileData() noexcept {
    static ProfileData dummy{};
    return dummy;
}
void ResetThreadProfileData() noexcept {}
void FlushThreadProfileData(int) noexcept {}
void ProfileRecordGcPause(int64_t) noexcept {}
void ProfileRecordNurseryAlloc(int64_t) noexcept {}
void ProfileRecordOldGenAlloc(int64_t) noexcept {}
void ProfileRecordLargeObjAlloc(int64_t) noexcept {}
void ProfileRecordAllocCount(int32_t) noexcept {}
void ProfileRecordFastPath() noexcept {}
void ProfileRecordSlowPath() noexcept {}
void ProfileStoreInit(int) noexcept {}
bool ProfileStoreAdd(const ProfileSnapshot&) noexcept { return false; }
int ProfileStoreFinalize() noexcept { return 0; }
const ProfileSnapshot* ProfileStoreGet(int) noexcept { return nullptr; }
int ProfileStoreCount() noexcept { return 0; }
void ProfileEmitJson() noexcept { printf("{\"profile\":[]}\n"); fflush(stdout); }

}}} // namespace chaos::il2cpp::runtime_core

#endif // CHAOS_IL2CPP_PROFILE_ENABLED
