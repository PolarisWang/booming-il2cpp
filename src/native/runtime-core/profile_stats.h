#ifndef CHAOS_IL2CPP_PROFILE_STATS_H_
#define CHAOS_IL2CPP_PROFILE_STATS_H_

#include <cstdint>
#include <cstdio>
#include <cinttypes>

namespace chaos { namespace il2cpp { namespace runtime_core {

// ── Per-method profile data collected during --profile mode ──────────
struct GcPauseProfile {
    int64_t total_pause_ns = 0;
    int64_t max_pause_ns = 0;
    int32_t pause_count = 0;
};

struct AllocVolumeProfile {
    int64_t nursery_bytes = 0;
    int64_t old_gen_bytes = 0;
    int64_t large_object_bytes = 0;
    int32_t alloc_count = 0;
};

struct ProfileData {
    GcPauseProfile gc_pause;
    AllocVolumeProfile alloc_volume;
    int64_t heap_before = 0;
    int64_t heap_after = 0;
    int32_t fast_path_count = 0;   // bump cache hits
    int32_t slow_path_count = 0;   // bump cache misses → NurseryAllocateSlow
};

// ── Circular buffer storing per-method profile snapshots ────────────
struct ProfileSnapshot {
    int method_index;
    ProfileData data;
};

constexpr int kMaxProfileMethods = 65536;

// ── Thread-local profile API ────────────────────────────────────────
// Each dispatch thread accumulates its own ProfileData.
// Before dispatching a method, the caller snapshots heap_before.
// After dispatching, the caller reads heap_after + accumulated GC stats
// and calls FlushThreadProfileData() to commit the snapshot.

ProfileData& GetThreadProfileData() noexcept;
void ResetThreadProfileData() noexcept;
void FlushThreadProfileData(int method_index) noexcept;

// ── Profile accumulation counters (updated by GC/allocation paths) ──
// These are called from within GC code paths to record per-dispatch
// GC pause events and allocation stats.  They modify the calling
// thread's ProfileData.

void ProfileRecordGcPause(int64_t pause_ns) noexcept;
void ProfileRecordNurseryAlloc(int64_t size) noexcept;
void ProfileRecordOldGenAlloc(int64_t size) noexcept;
void ProfileRecordLargeObjAlloc(int64_t size) noexcept;
void ProfileRecordAllocCount(int32_t count = 1) noexcept;
void ProfileRecordFastPath() noexcept;
void ProfileRecordSlowPath() noexcept;

// ── Process-wide profile store ──────────────────────────────────────
// After all methods have been dispatched, the main thread calls
// ProfileStoreInit() / ProfileStoreAdd() / ProfileStoreFinalize()
// to collect snapshots from all thread-local buffers into a single
// process-wide array, then emits JSON.

void ProfileStoreInit(int capacity) noexcept;
bool ProfileStoreAdd(const ProfileSnapshot& snap) noexcept;
int ProfileStoreFinalize() noexcept;  // returns total snapshot count

// Access snapshot by index (for JSON serialization)
const ProfileSnapshot* ProfileStoreGet(int index) noexcept;
int ProfileStoreCount() noexcept;

// ── JSON serialization ──────────────────────────────────────────────
// Emits per-method profile data as JSON to stdout.
// Call after ProfileStoreFinalize().

void ProfileEmitJson() noexcept;

}}} // namespace chaos::il2cpp::runtime_core

#endif // CHAOS_IL2CPP_PROFILE_STATS_H_
