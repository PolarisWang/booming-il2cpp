// ep_protocol_test.cpp — EventPipe wire protocol unit tests
//
// Tests protocol constants, checksum computation, and event type enum values.
// All tests are pure C++ with no platform dependency.

#include <gtest/gtest.h>
#include <eventpipe/ep_protocol.h>

namespace chaos::il2cpp::diagnostics {
namespace {

// ── Protocol constants ────────────────────────────────────────────────

TEST(EpProtocolTest, MagicNumber) {
    EXPECT_EQ(kEpMagic, 0xACDB12CEu);
}

TEST(EpProtocolTest, ProtocolVersion) {
    EXPECT_EQ(kEpProtocolVersion, 1u);
}

// ── Header layout ────────────────────────────────────────────────────

TEST(EpProtocolTest, HeaderSize) {
    EXPECT_EQ(sizeof(EpEventHeader), 24u);
}

TEST(EpProtocolTest, HeaderFieldOffsets) {
    EpEventHeader h{};
    const uint8_t* base = reinterpret_cast<const uint8_t*>(&h);

    // magic      (uint32_t) at offset 0
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.magic) - base, 0u);
    // version    (uint32_t) at offset 4
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.version) - base, 4u);
    // event_type (uint32_t) at offset 8
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.event_type) - base, 8u);
    // payload_size (uint32_t) at offset 12
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.payload_size) - base, 12u);
    // timestamp  (uint64_t) at offset 16
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.timestamp) - base, 16u);
}

// ── Checksum computation ─────────────────────────────────────────────

TEST(EpProtocolTest, ChecksumEmptyPayload) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.payload_size = 0;

    // With zeroed header + empty payload, checksum = XOR of all header bytes.
    uint32_t cksum = EpComputeChecksum(header, nullptr);
    // Just verify it's deterministic (same call produces same result).
    uint32_t cksum2 = EpComputeChecksum(header, nullptr);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpProtocolTest, ChecksumWithPayload) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.payload_size = 8;

    uint64_t payload = 0xDEADBEEFCAFEBABEull;

    uint32_t cksum = EpComputeChecksum(header, &payload);
    uint32_t cksum2 = EpComputeChecksum(header, &payload);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpProtocolTest, ChecksumChangesWithPayload) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.payload_size = 4;

    uint32_t payload_a = 0x12345678;
    uint32_t payload_b = 0x87654321;

    uint32_t cksum_a = EpComputeChecksum(header, &payload_a);
    uint32_t cksum_b = EpComputeChecksum(header, &payload_b);
    EXPECT_NE(cksum_a, cksum_b);
}

TEST(EpProtocolTest, ChecksumNullPayloadWithZeroSize) {
    // Zero payload_size with nullptr should be treated consistently.
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.payload_size = 0;

    // Should not crash, should be deterministic.
    uint32_t cksum = EpComputeChecksum(header, nullptr);
    uint32_t cksum2 = EpComputeChecksum(header, nullptr);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpProtocolTest, ChecksumNonZeroPayloadChangesWhenHeaderChanges) {
    uint64_t payload = 0x1111222233334444ull;

    EpEventHeader header_a{};
    header_a.magic = kEpMagic;
    header_a.version = kEpProtocolVersion;
    header_a.payload_size = 8;
    header_a.event_type = EpEventType::GcStart;

    EpEventHeader header_b{};
    header_b.magic = kEpMagic;
    header_b.version = kEpProtocolVersion;
    header_b.payload_size = 8;
    header_b.event_type = EpEventType::GcEnd;  // different event type

    uint32_t cksum_a = EpComputeChecksum(header_a, &payload);
    uint32_t cksum_b = EpComputeChecksum(header_b, &payload);
    EXPECT_NE(cksum_a, cksum_b);
}

// ── Event type enum ──────────────────────────────────────────────────

TEST(EpProtocolTest, GcEventTypeRanges) {
    // All GC event types must be in the 0x0100-0x01FF range.
    EXPECT_GE(static_cast<uint32_t>(EpEventType::GcStart), 0x0100u);
    EXPECT_LT(static_cast<uint32_t>(EpEventType::GcStart), 0x0200u);
    EXPECT_GE(static_cast<uint32_t>(EpEventType::GcAllocationTick), 0x0100u);
    EXPECT_LT(static_cast<uint32_t>(EpEventType::GcAllocationTick), 0x0200u);
}

TEST(EpProtocolTest, ThreadPoolEventTypeRanges) {
    EXPECT_GE(static_cast<uint32_t>(EpEventType::TpWorkerCreate), 0x0200u);
    EXPECT_LT(static_cast<uint32_t>(EpEventType::TpWorkerCreate), 0x0300u);
    EXPECT_GE(static_cast<uint32_t>(EpEventType::TpIoCompletion), 0x0200u);
    EXPECT_LT(static_cast<uint32_t>(EpEventType::TpIoCompletion), 0x0300u);
}

TEST(EpProtocolTest, ExceptionEventTypeRanges) {
    EXPECT_GE(static_cast<uint32_t>(EpEventType::ExceptionThrow), 0x0300u);
    EXPECT_LT(static_cast<uint32_t>(EpEventType::ExceptionThrow), 0x0400u);
    EXPECT_GE(static_cast<uint32_t>(EpEventType::ExceptionCatch), 0x0300u);
    EXPECT_LT(static_cast<uint32_t>(EpEventType::ExceptionCatch), 0x0400u);
}

TEST(EpProtocolTest, HotUpdateEventTypeRanges) {
    EXPECT_GE(static_cast<uint32_t>(EpEventType::HuAssemblyLoaded), 0x0400u);
    EXPECT_LT(static_cast<uint32_t>(EpEventType::HuAssemblyLoaded), 0x0500u);
    EXPECT_GE(static_cast<uint32_t>(EpEventType::HuError), 0x0400u);
    EXPECT_LT(static_cast<uint32_t>(EpEventType::HuError), 0x0500u);
}

TEST(EpProtocolTest, EventTypesAreUnique) {
    // Collect all event types into a set by value to verify uniqueness.
    uint32_t values[] = {
        static_cast<uint32_t>(EpEventType::GcStart),
        static_cast<uint32_t>(EpEventType::GcEnd),
        static_cast<uint32_t>(EpEventType::GcYoungStart),
        static_cast<uint32_t>(EpEventType::GcYoungEnd),
        static_cast<uint32_t>(EpEventType::GcFullStart),
        static_cast<uint32_t>(EpEventType::GcFullEnd),
        static_cast<uint32_t>(EpEventType::GcOom),
        static_cast<uint32_t>(EpEventType::GcGen1Collect),
        static_cast<uint32_t>(EpEventType::GcAllocationTick),
        static_cast<uint32_t>(EpEventType::TpWorkerCreate),
        static_cast<uint32_t>(EpEventType::TpWorkerDestroy),
        static_cast<uint32_t>(EpEventType::TpWorkItemQueue),
        static_cast<uint32_t>(EpEventType::TpWorkItemDequeue),
        static_cast<uint32_t>(EpEventType::TpWorkerAdjust),
        static_cast<uint32_t>(EpEventType::TpIoCompletion),
        static_cast<uint32_t>(EpEventType::ExceptionThrow),
        static_cast<uint32_t>(EpEventType::ExceptionRethrow),
        static_cast<uint32_t>(EpEventType::ExceptionCatch),
        static_cast<uint32_t>(EpEventType::HuAssemblyLoaded),
        static_cast<uint32_t>(EpEventType::HuAssemblyUnloaded),
        static_cast<uint32_t>(EpEventType::HuPackageLoaded),
        static_cast<uint32_t>(EpEventType::HuPackageLoadFailed),
        static_cast<uint32_t>(EpEventType::HuPackageUnloaded),
        static_cast<uint32_t>(EpEventType::HuMethodReplaced),
        static_cast<uint32_t>(EpEventType::HuMethodReverted),
        static_cast<uint32_t>(EpEventType::HuError),
    };

    constexpr int kCount = sizeof(values) / sizeof(values[0]);
    for (int i = 0; i < kCount; ++i) {
        for (int j = i + 1; j < kCount; ++j) {
            EXPECT_NE(values[i], values[j])
                << "Duplicate event type value at indices " << i << " and " << j;
        }
    }
}

}  // namespace
}  // namespace chaos::il2cpp::diagnostics
