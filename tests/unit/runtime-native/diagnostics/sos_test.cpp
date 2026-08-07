// sos_test.cpp — CRAG GC SOS extension unit tests
//
// Verifies struct layout compatibility between SOS POD mirrors and
// the actual GC runtime structs (gc_stats.h).  Also tests constants
// and bucket names.
//
// Unity-build: includes gc_sos.cpp directly so the struct definitions
// and file-scope constants are visible to the test code.

// Include gc_sos.cpp in global namespace (its types are at global scope).
#include "gc_sos.cpp"

#include <gtest/gtest.h>
#include <cstdint>
#include <gc/gc_stats.h>

// ── GcStatsPod ↔ GcStats layout compatibility ────────────────────────

TEST(SosTest, GcStatsPodSize) {
    // 22 uint64 + 3 int32 + 1 uint32 padding = 176 + 16 = 192
    EXPECT_EQ(sizeof(GcStatsPod), 192u);
}

TEST(SosTest, GcStatsPodFieldOffsets) {
    GcStatsPod pod{};
    const uint8_t* base = reinterpret_cast<const uint8_t*>(&pod);

    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.young_collections) - base, 0u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.young_objects_promoted) - base, 8u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.young_bytes_promoted) - base, 16u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.young_bytes_reclaimed) - base, 24u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.young_cards_scanned) - base, 32u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.young_pause_ns) - base, 40u);

    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.full_collections) - base, 48u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.full_pages_collected) - base, 56u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.full_objects_marked) - base, 64u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.full_bytes_reclaimed) - base, 72u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.full_finalizers_run) - base, 80u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.finalization_pending_count) - base, 88u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.full_pause_ns) - base, 96u);

    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.gen1_collections) - base, 104u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.gen1_objects_promoted) - base, 112u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.gen1_bytes_promoted) - base, 120u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.gen1_bytes_reclaimed) - base, 128u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.gen1_pause_ns) - base, 136u);

    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.last_compacted) - base, 144u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.last_concurrent) - base, 148u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.last_gc_generation) - base, 152u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod._padding0) - base, 156u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.gc_index) - base, 160u);

    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.alloc_total) - base, 168u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.alloc_bytes) - base, 176u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.alloc_oversized) - base, 184u);
}

// ── GcEventEntryPod ↔ GcEventEntry layout compatibility ─────────────

TEST(SosTest, GcEventEntryPodSize) {
    EXPECT_EQ(sizeof(GcEventEntryPod), 32u);
}

TEST(SosTest, GcEventEntryPodFieldOffsets) {
    GcEventEntryPod pod{};
    const uint8_t* base = reinterpret_cast<const uint8_t*>(&pod);

    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.is_full_gc) - base, 0u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.pause_ns) - base, 8u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.objects_processed) - base, 16u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&pod.bytes_reclaimed) - base, 24u);
}

// ── Constants ────────────────────────────────────────────────────────

TEST(SosTest, RingSize) {
    EXPECT_EQ(kGcRingSize, 64);
    EXPECT_EQ(kGcRingSize, chaos::il2cpp::runtime_core::kGcEventRingSize);
}

TEST(SosTest, BucketCount) {
    EXPECT_EQ(kGcBucketCount, 6);
    EXPECT_EQ(kGcBucketCount, chaos::il2cpp::runtime_core::kGcPauseBucketCount);
}

// ── Bucket names ─────────────────────────────────────────────────────

TEST(SosTest, BucketNamesCorrect) {
    EXPECT_STREQ(kBucketNames[0], "0-1 ms");
    EXPECT_STREQ(kBucketNames[1], "1-5 ms");
    EXPECT_STREQ(kBucketNames[2], "5-10 ms");
    EXPECT_STREQ(kBucketNames[3], "10-50 ms");
    EXPECT_STREQ(kBucketNames[4], "50-100 ms");
    EXPECT_STREQ(kBucketNames[5], "100+ ms");
}

TEST(SosTest, BucketNamesCount) {
    int count = sizeof(kBucketNames) / sizeof(kBucketNames[0]);
    EXPECT_EQ(count, 6);
}
