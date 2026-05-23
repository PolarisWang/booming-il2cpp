// ep_negative_test.cpp — EventPipe negative/edge-case protocol tests
//
// Tests invalid inputs, boundary conditions, and robustness of EventPipe
// protocol functions (P3-G: negative tests).  Uses only ep_protocol.h APIs.

#include <gtest/gtest.h>
#include <eventpipe/ep_protocol.h>

#include <cstring>
#include <climits>

namespace chaos::il2cpp::diagnostics {
namespace {

// ── EpComputeChecksum edge cases ───────────────────────────────────────

TEST(EpNegativeTest, ChecksumNullPayloadZeroSize) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.payload_size = 0;

    uint32_t cksum = EpComputeChecksum(header, nullptr);
    uint32_t cksum2 = EpComputeChecksum(header, nullptr);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpNegativeTest, ChecksumZeroedHeader) {
    EpEventHeader header{};
    // All fields zero — not a valid header, but must not crash.
    header.payload_size = 0;

    uint32_t cksum = EpComputeChecksum(header, nullptr);
    uint32_t cksum2 = EpComputeChecksum(header, nullptr);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpNegativeTest, ChecksumGarbageMagic) {
    EpEventHeader header{};
    header.magic = 0xFFFFFFFFu;
    header.version = kEpProtocolVersion;
    header.payload_size = 0;

    uint32_t cksum = EpComputeChecksum(header, nullptr);
    uint32_t cksum2 = EpComputeChecksum(header, nullptr);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpNegativeTest, ChecksumNullWithNonZeroSize) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.payload_size = 8;  // non-zero size but null payload

    uint32_t cksum = EpComputeChecksum(header, nullptr);
    uint32_t cksum2 = EpComputeChecksum(header, nullptr);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpNegativeTest, ChecksumExtremePayloadSize) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.payload_size = 0x7FFFFFFFu;

    uint32_t cksum = EpComputeChecksum(header, nullptr);
    uint32_t cksum2 = EpComputeChecksum(header, nullptr);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpNegativeTest, ChecksumAllZeroPayload) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.payload_size = 8;

    uint64_t payload = 0;
    uint32_t cksum = EpComputeChecksum(header, &payload);
    uint32_t cksum2 = EpComputeChecksum(header, &payload);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpNegativeTest, ChecksumPayloadWithAllBitsSet) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.payload_size = 8;

    uint64_t payload = 0xFFFFFFFFFFFFFFFFull;
    uint32_t cksum = EpComputeChecksum(header, &payload);
    uint32_t cksum2 = EpComputeChecksum(header, &payload);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpNegativeTest, ChecksumMaxPayloadSizeOneByte) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.payload_size = 1;

    uint8_t payload = 0xFF;
    uint32_t cksum = EpComputeChecksum(header, &payload);
    uint32_t cksum2 = EpComputeChecksum(header, &payload);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpNegativeTest, ChecksumLargePayload32Bytes) {
    EpEventHeader header{};
    header.magic = kEpMagic;
    header.version = kEpProtocolVersion;
    header.payload_size = 32;

    uint8_t payload[32];
    std::memset(payload, 0xAA, sizeof(payload));
    uint32_t cksum = EpComputeChecksum(header, &payload);
    uint32_t cksum2 = EpComputeChecksum(header, &payload);
    EXPECT_EQ(cksum, cksum2);
}

TEST(EpNegativeTest, ChecksumPayloadChangesWithMagic) {
    EpEventHeader header_a{}, header_b{};
    header_a.version = header_b.version = kEpProtocolVersion;
    header_a.payload_size = header_b.payload_size = 4;
    header_a.magic = 0x12345678u;
    header_b.magic = 0x87654321u;

    uint32_t val = 0xDEADBEEFu;
    uint32_t cksum_a = EpComputeChecksum(header_a, &val);
    uint32_t cksum_b = EpComputeChecksum(header_b, &val);
    EXPECT_NE(cksum_a, cksum_b);
}

TEST(EpNegativeTest, HeaderSizeConstant) {
    // Verify header size is as expected.
    EXPECT_EQ(sizeof(EpEventHeader), 24u);
}

TEST(EpNegativeTest, HeaderFieldOrdering) {
    // Verify field offsets within the header.
    EpEventHeader h{};
    const uint8_t* base = reinterpret_cast<const uint8_t*>(&h);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.magic) - base, 0u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.version) - base, 4u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.event_type) - base, 8u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.payload_size) - base, 12u);
    EXPECT_EQ(reinterpret_cast<const uint8_t*>(&h.timestamp) - base, 16u);
}

}  // namespace
}  // namespace chaos::il2cpp::diagnostics
