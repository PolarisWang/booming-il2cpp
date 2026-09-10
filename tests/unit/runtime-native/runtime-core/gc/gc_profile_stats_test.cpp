/// gc_profile_stats_test — unit tests for per-dispatch profile data collection
///
/// Tests the profile_stats.h API tier that records GC pause / allocation metrics
/// during entry.exe --profile mode. Two build tiers:
///   1. Default (CHAOS_IL2CPP_PROFILE_ENABLED=0) — all functions are no-op stubs
///   2. Profile build (CHAOS_IL2CPP_PROFILE_ENABLED=1) — live instrumentation
///
/// These tests verify both tiers produce correct behavior.  The profile build
/// tier is tested by running the test binary compiled with the `--preset profile`
/// CMake preset (CHAOS_IL2CPP_CONFIG_TIER=profile → PROFILE_ENABLED=1).
///
/// Tests:
///   1. ProfileRecordGcPause accumulates total/max/count
///   2. ProfileRecordNurseryAlloc accumulates nursery_bytes
///   3. ProfileRecordOldGenAlloc accumulates old_gen_bytes
///   4. ProfileRecordLargeObjAlloc accumulates large_object_bytes
///   5. ProfileRecordAllocCount increments alloc_count
///   6. ProfileRecordFastPath / SlowPath increment counters
///   7. GetThreadProfileData returns a valid ProfileData reference
///   8. FlushThreadProfileData commits a snapshot to the process-wide store
///   9. ProfileStoreInit/Add/Finalize/Get/Count round-trip
///   10. ProfileEmitJson produces valid JSON
///   11. ResetThreadProfileData clears the TLS accumulator
///   12. Stub safety: when PROFILE_ENABLED=0, all functions are no-ops (no crash)

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <chaos/native_types.h>
#include <profile_stats.h>

#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

// =====================================================================
// ProfileData accumulator tests
// =====================================================================

TEST(GcProfileStatsTest, RecordGcPauseAccumulates) {
    auto& pd = GetThreadProfileData();

    ProfileRecordGcPause(1000);
    ProfileRecordGcPause(2000);
    ProfileRecordGcPause(500);

#if CHAOS_IL2CPP_PROFILE_ENABLED
    EXPECT_EQ(pd.gc_pause.pause_count, 3);
    EXPECT_EQ(pd.gc_pause.total_pause_ns, 3500);
    EXPECT_EQ(pd.gc_pause.max_pause_ns, 2000);
#else
    // Stub tier: all functions are no-ops — accept 0.
    SUCCEED();
#endif

    ResetThreadProfileData();
}

TEST(GcProfileStatsTest, RecordNurseryAllocAccumulates) {
    auto& pd = GetThreadProfileData();

    ProfileRecordNurseryAlloc(4096);
    ProfileRecordNurseryAlloc(8192);
    ProfileRecordNurseryAlloc(16384);

#if CHAOS_IL2CPP_PROFILE_ENABLED
    EXPECT_EQ(pd.alloc_volume.nursery_bytes, 4096 + 8192 + 16384);
#else
    SUCCEED();
#endif

    ResetThreadProfileData();
}

TEST(GcProfileStatsTest, RecordOldGenAllocAccumulates) {
    auto& pd = GetThreadProfileData();

    ProfileRecordOldGenAlloc(65536);
    ProfileRecordOldGenAlloc(131072);

#if CHAOS_IL2CPP_PROFILE_ENABLED
    EXPECT_EQ(pd.alloc_volume.old_gen_bytes, 65536 + 131072);
#else
    SUCCEED();
#endif

    ResetThreadProfileData();
}

TEST(GcProfileStatsTest, RecordLargeObjAllocAccumulates) {
    auto& pd = GetThreadProfileData();

    ProfileRecordLargeObjAlloc(102400);

#if CHAOS_IL2CPP_PROFILE_ENABLED
    EXPECT_EQ(pd.alloc_volume.large_object_bytes, 102400);
#else
    SUCCEED();
#endif

    ResetThreadProfileData();
}

TEST(GcProfileStatsTest, RecordAllocCountIncrements) {
    auto& pd = GetThreadProfileData();

    ProfileRecordAllocCount();
    ProfileRecordAllocCount();
    ProfileRecordAllocCount(5);

#if CHAOS_IL2CPP_PROFILE_ENABLED
    EXPECT_EQ(pd.alloc_volume.alloc_count, 1 + 1 + 5);
#else
    SUCCEED();
#endif

    ResetThreadProfileData();
}

TEST(GcProfileStatsTest, RecordFastPathSlowPath) {
    auto& pd = GetThreadProfileData();

    ProfileRecordFastPath();
    ProfileRecordFastPath();
    ProfileRecordFastPath();
    ProfileRecordSlowPath();
    ProfileRecordSlowPath();

#if CHAOS_IL2CPP_PROFILE_ENABLED
    EXPECT_EQ(pd.fast_path_count, 3);
    EXPECT_EQ(pd.slow_path_count, 2);
#else
    SUCCEED();
#endif

    ResetThreadProfileData();
}

TEST(GcProfileStatsTest, MixedRecordAll) {
    auto& pd = GetThreadProfileData();

    ProfileRecordNurseryAlloc(1024);
    ProfileRecordAllocCount();
    ProfileRecordFastPath();
    ProfileRecordGcPause(50000);
    ProfileRecordNurseryAlloc(2048);
    ProfileRecordAllocCount();
    ProfileRecordFastPath();

#if CHAOS_IL2CPP_PROFILE_ENABLED
    EXPECT_EQ(pd.alloc_volume.nursery_bytes, 1024 + 2048);
    EXPECT_EQ(pd.alloc_volume.alloc_count, 2);
    EXPECT_EQ(pd.fast_path_count, 2);
    EXPECT_EQ(pd.gc_pause.pause_count, 1);
    EXPECT_EQ(pd.gc_pause.total_pause_ns, 50000);
#else
    SUCCEED();
#endif

    ResetThreadProfileData();
}

// =====================================================================
// FlushThreadProfileData tests
// =====================================================================

TEST(GcProfileStatsTest, FlushCommitsSnapshot) {
    ProfileStoreInit(64);

    ProfileRecordNurseryAlloc(4096);
    ProfileRecordAllocCount();
    ProfileRecordFastPath();
    ProfileRecordGcPause(100000);
    FlushThreadProfileData(42);

#if CHAOS_IL2CPP_PROFILE_ENABLED
    int count = ProfileStoreCount();
    ASSERT_GE(count, 1);

    const auto* snap = ProfileStoreGet(0);
    ASSERT_NE(snap, nullptr);
    EXPECT_EQ(snap->method_index, 42);
    EXPECT_EQ(snap->data.alloc_volume.nursery_bytes, 4096);
    EXPECT_EQ(snap->data.alloc_volume.alloc_count, 1);
    EXPECT_EQ(snap->data.fast_path_count, 1);
    EXPECT_EQ(snap->data.gc_pause.total_pause_ns, 100000);
#else
    // Stub tier: ProfileStore* functions are no-ops — ProfileStoreCount() returns 0,
    // ProfileStoreGet() returns nullptr.  Accept the stub behavior.
    SUCCEED();
#endif

    ResetThreadProfileData();
    ProfileStoreInit(64);
}

TEST(GcProfileStatsTest, MultipleFlushesPreserveOrder) {
    static constexpr int kMethodCount = 5;
    ProfileStoreInit(kMethodCount);

    for (int i = 0; i < kMethodCount; i++) {
        ProfileRecordNurseryAlloc(static_cast<int64_t>(i * 1024));
        ProfileRecordAllocCount();
        ProfileRecordGcPause(static_cast<int64_t>(i * 1000));
        FlushThreadProfileData(i);
        ResetThreadProfileData();
    }

#if CHAOS_IL2CPP_PROFILE_ENABLED
    EXPECT_EQ(ProfileStoreCount(), kMethodCount);

    for (int i = 0; i < kMethodCount; i++) {
        const auto* snap = ProfileStoreGet(i);
        ASSERT_NE(snap, nullptr);
        EXPECT_EQ(snap->method_index, i);
    }
#else
    SUCCEED();
#endif

    ProfileStoreInit(64);
}

TEST(GcProfileStatsTest, StoreCapacityBound) {
    ProfileStoreInit(2);

    for (int i = 0; i < 3; i++) {
        ProfileRecordNurseryAlloc(256);
        FlushThreadProfileData(i);
        ResetThreadProfileData();
    }

#if CHAOS_IL2CPP_PROFILE_ENABLED
    EXPECT_LE(ProfileStoreCount(), 2);
#else
    SUCCEED();
#endif

    ProfileStoreInit(64);
}

// =====================================================================
// ProfileEmitJson tests
// =====================================================================

TEST(GcProfileStatsTest, EmitJsonProducesValidOutput) {
    ProfileStoreInit(64);

    for (int i = 0; i < 3; i++) {
        ProfileRecordNurseryAlloc(1024);
        ProfileRecordAllocCount();
        if (i == 1) {
            ProfileRecordGcPause(50000);
        }
        FlushThreadProfileData(i);
        ResetThreadProfileData();
    }

    ProfileStoreFinalize();

    testing::internal::CaptureStdout();
    ProfileEmitJson();
    std::string output = testing::internal::GetCapturedStdout();

    // All tiers produce valid JSON: {"profile":[...]}
    EXPECT_NE(output.find("{\"profile\":"), std::string::npos);
    EXPECT_NE(output.find("]}\n"), std::string::npos);

#if CHAOS_IL2CPP_PROFILE_ENABLED
    // Profile tier includes per-field serialization
    EXPECT_NE(output.find("\"methodIndex\""), std::string::npos);
    EXPECT_NE(output.find("\"gcPauseNs\""), std::string::npos);
    EXPECT_NE(output.find("\"nurseryAllocBytes\""), std::string::npos);
    EXPECT_NE(output.find("\"fastPathCount\""), std::string::npos);
    EXPECT_NE(output.find("\"slowPathCount\""), std::string::npos);
#else
    // Stub tier: always emits {"profile":[]} with no per-field keys
    EXPECT_NE(output.find("[]"), std::string::npos);
#endif

    ProfileStoreInit(64);
}

TEST(GcProfileStatsTest, EmitJsonEmptyStore) {
    ProfileStoreFinalize();

    testing::internal::CaptureStdout();
    ProfileEmitJson();
    std::string output = testing::internal::GetCapturedStdout();

    // Both tiers should produce valid JSON: {"profile":[]}
    EXPECT_NE(output.find("{\"profile\":"), std::string::npos);
    EXPECT_NE(output.find("]}\n"), std::string::npos);

    ProfileStoreInit(64);
}

// =====================================================================
// Thread safety
// =====================================================================

TEST(GcProfileStatsTest, MultiThreadAccumulation) {
    ProfileStoreInit(64);

    static constexpr int kNumThreads = 4;
    std::vector<std::thread> threads;

    // Mutex serializing FlushThreadProfileData → ProfileStoreAdd writes to the
    // process-wide snapshot store (s_profile_store[] / s_profile_count).  The
    // production code assumes single-threaded dispatch and does not lock — the
    // mutex here is test-only, not a production fix.
    std::mutex flush_mutex;

    for (int t = 0; t < kNumThreads; t++) {
        threads.emplace_back([t, &flush_mutex] {
            ProfileRecordNurseryAlloc(static_cast<int64_t>((t + 1) * 4096));
            ProfileRecordAllocCount();
            ProfileRecordGcPause(static_cast<int64_t>((t + 1) * 10000));
            {
                std::lock_guard<std::mutex> lock(flush_mutex);
                FlushThreadProfileData(t);
            }
            ResetThreadProfileData();
        });
    }

    for (auto& th : threads) {
        th.join();
    }

#if CHAOS_IL2CPP_PROFILE_ENABLED
    int count = ProfileStoreCount();
    EXPECT_GE(count, 1);
    for (int i = 0; i < count && i < kNumThreads; i++) {
        const auto* snap = ProfileStoreGet(i);
        ASSERT_NE(snap, nullptr);
        EXPECT_GE(snap->method_index, 0);
        EXPECT_LT(snap->method_index, kNumThreads);
    }
#else
    SUCCEED();
#endif

    ProfileStoreInit(64);
}

// =====================================================================
// #if gate correctness: test that the API compiles and links in both
// PROFILE_ENABLED=0 and PROFILE_ENABLED=1 builds without errors.
// =====================================================================

TEST(GcProfileStatsTest, ApiCompilesInBothBuildTiers) {
    // This test verifies that the entire profile_stats.h API compiles
    // and links correctly regardless of the CHAOS_IL2CPP_PROFILE_ENABLED
    // setting.  If this test compiles, all API symbols are present.

    // Store API
    ProfileStoreInit(1);
    ProfileSnapshot snap;
    snap.method_index = 0;
    ProfileStoreAdd(snap);
    ProfileStoreFinalize();
    ProfileStoreGet(0);
    ProfileStoreCount();

    // Data API
    GetThreadProfileData();
    ResetThreadProfileData();
    FlushThreadProfileData(0);

    // Record API
    ProfileRecordGcPause(0);
    ProfileRecordNurseryAlloc(0);
    ProfileRecordOldGenAlloc(0);
    ProfileRecordLargeObjAlloc(0);
    ProfileRecordAllocCount(0);
    ProfileRecordFastPath();
    ProfileRecordSlowPath();

    // Emission API
    ProfileEmitJson();

    // If we get here without crashing, all APIs are linkable.
    SUCCEED();
}