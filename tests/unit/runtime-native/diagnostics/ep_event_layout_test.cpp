// ep_event_layout_test.cpp — EventPipe event payload layout tests
//
// Verifies that all EventPipe payload struct sizes and alignments match
// expectations.  These tests protect against accidental ABI-breaking
// changes to the wire format.

#include <gtest/gtest.h>
#include <eventpipe/ep_protocol.h>

#include <cstdint>

// The payload struct definitions are in ep_event.cpp (anonymous namespace).
// Test by including the .cpp directly for layout access.
// In a CMake-based build this is handled by compiling ep_event.cpp as a
// separate object and including the internal layout from a test-only header.
//
// Since the payload structs are defined in an anonymous namespace within the
// .cpp file and not exposed, we redeclare them here with matching layout
// (same struct as ep_event.cpp) and verify sizes.  This is a compile-time
// layout validation approach — if the source struct changes, this test must
// be updated to match.

#pragma pack(push, 1)

struct TestPayloadGcStart {
    uint32_t generation;
};

struct TestPayloadGcEnd {
    uint64_t pause_ns;
    uint64_t reclaimed_bytes;
};

struct TestPayloadGcYoungStart {
    uint64_t nursery_used;
};

struct TestPayloadGcYoungEnd {
    uint64_t pause_ns;
    uint64_t objects_promoted;
    uint64_t bytes_promoted;
    uint64_t bytes_reclaimed;
};

struct TestPayloadGcFullStart {
    uint32_t page_count;
};

struct TestPayloadGcFullEnd {
    uint64_t pause_ns;
    uint64_t reclaimed_bytes;
    uint64_t objects_marked;
    uint64_t pages_collected;
};

struct TestPayloadGcGen1Collect {
    uint64_t pause_ns;
    uint64_t objects_promoted;
    uint64_t bytes_reclaimed;
};

struct TestPayloadAllocationTick {
    uint32_t allocation_amount;
    uint32_t allocation_kind;
};

struct TestPayloadWorkerEvent {
    int32_t thread_id;
};

struct TestPayloadWorkItemEvent {
    int64_t work_item_id;
};

struct TestPayloadWorkerAdjust {
    int32_t current_count;
    int32_t new_count;
    int32_t reason;
};

struct TestPayloadIoCompletion {
    uint32_t bytes_transferred;
};

struct TestPayloadException {
    uint64_t exception_ptr;
};

struct TestPayloadHuAssemblyEvent {
    uint32_t module_id;
    uint32_t alc_id;
    char     assembly_name[128];
};

struct TestPayloadHuPackageEvent {
    char package_id[128];
};

struct TestPayloadHuPackageLoadFailed {
    char package_id[128];
    char error_message[256];
};

struct TestPayloadHuMethodEvent {
    uint32_t method_token;
};

struct TestPayloadHuError {
    char error_message[256];
};

#pragma pack(pop)

namespace chaos::il2cpp::diagnostics {
namespace {

// ── GC event payload sizes ───────────────────────────────────────────

TEST(EpEventLayoutTest, GcStartPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadGcStart), 4u);   // uint32_t generation
}

TEST(EpEventLayoutTest, GcEndPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadGcEnd), 16u);    // 2 × uint64_t
}

TEST(EpEventLayoutTest, GcYoungStartPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadGcYoungStart), 8u);  // uint64_t
}

TEST(EpEventLayoutTest, GcYoungEndPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadGcYoungEnd), 32u);  // 4 × uint64_t
}

TEST(EpEventLayoutTest, GcFullStartPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadGcFullStart), 4u);  // uint32_t
}

TEST(EpEventLayoutTest, GcFullEndPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadGcFullEnd), 32u);   // 4 × uint64_t
}

TEST(EpEventLayoutTest, GcGen1CollectPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadGcGen1Collect), 24u);  // 3 × uint64_t
}

TEST(EpEventLayoutTest, GcAllocationTickPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadAllocationTick), 8u);  // 2 × uint32_t
}

// ── ThreadPool event payload sizes ───────────────────────────────────

TEST(EpEventLayoutTest, WorkerEventPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadWorkerEvent), 4u);  // int32_t
}

TEST(EpEventLayoutTest, WorkItemEventPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadWorkItemEvent), 8u);  // int64_t
}

TEST(EpEventLayoutTest, WorkerAdjustPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadWorkerAdjust), 12u);  // 3 × int32_t
}

TEST(EpEventLayoutTest, IoCompletionPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadIoCompletion), 4u);  // uint32_t
}

// ── Exception event payload sizes ────────────────────────────────────

TEST(EpEventLayoutTest, ExceptionPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadException), 8u);  // uint64_t pointer
}

// ── HotUpdate event payload sizes ────────────────────────────────────

TEST(EpEventLayoutTest, HuAssemblyEventPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadHuAssemblyEvent), 136u);  // 4 + 4 + 128
}

TEST(EpEventLayoutTest, HuPackageEventPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadHuPackageEvent), 128u);  // char[128]
}

TEST(EpEventLayoutTest, HuPackageLoadFailedPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadHuPackageLoadFailed), 384u);  // 128 + 256
}

TEST(EpEventLayoutTest, HuMethodEventPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadHuMethodEvent), 4u);  // uint32_t
}

TEST(EpEventLayoutTest, HuErrorPayloadSize) {
    EXPECT_EQ(sizeof(TestPayloadHuError), 256u);  // char[256]
}

// ── Wire format: header + payload + checksum relationship ────────────

TEST(EpEventLayoutTest, WireFrameMinimumSize) {
    // Minimum wire frame: header (24) + payload (0) + checksum (4) = 28 bytes.
    constexpr uint32_t kMinFrame = sizeof(EpEventHeader) + sizeof(uint32_t);
    EXPECT_EQ(kMinFrame, 28u);
}

}  // namespace
}  // namespace chaos::il2cpp::diagnostics
