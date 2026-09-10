/// hotpatch_api_version_roundtrip_test.cpp — v4 backward-compatibility test
///
/// Verifies:
///   1. A v3 .patchdata blob (header_size=132) is still accepted (backward compat).
///   2. A v4 blob with min_host_revision=0 is accepted regardless of host revision.
///   3. A v4 blob with min_host_revision > host revision → CHAOS_PATCH_ERR_VERSION_MISMATCH.
///   4. A v4 blob with min_host_revision <= host revision → accepted (no version error).
///   5. A mismarked blob (version=4 but header_size=132, no trailing fields) handled
///      gracefully (not a version mismatch, not a crash).
///
/// These construct raw byte buffers to test ONLY the C++ loader/API layer, not the
/// C# PatchDataExtractor producer.
///
/// NOTE: All header-only blobs (no method defs, no body data) go through the full
/// loader path and return NO_METHODS (-3) — the loader successfully creates a context
/// with 0 patched methods, then ChaosApplyPatch calls Unpatch(ctx) and returns -3.
/// The acceptance assertions therefore pin the expected NO_METHODS code, not just
/// "not a version/format error".  A header-only blob that returns VERSION_MISMATCH
/// or INVALID_FORMAT is a regression.

#include "hotpatch_api.h"

#include <gtest/gtest.h>
#include <cstdint>
#include <cstring>

// Byte-layout mirrors of the patch data header (all fields uint32_t, no padding).
//
// v3 = 33 × uint32 = 132 bytes.  We only need to set the first 3 fields
// (magic/version/header_size); the other 30 zero-initialized fields are unused
// for these pure-header tests.
struct Hdr33 {                       // base v3 (33 fields)
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t rest[30];               // remaining v3 fields, zero
};
static_assert(sizeof(Hdr33) == 132, "v3 = 132 bytes");

struct Hdr35 {                       // v4 = 35 fields
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t rest[30];               // fields 4..33 (dependencies etc.)
    uint32_t min_host_revision;      // v4 trailing field 34
    uint32_t patch_revision;         // v4 trailing field 35
};
static_assert(sizeof(Hdr35) == 140, "v4 = 140 bytes");

// ── Helpers ────────────────────────────────────────────────────────────────

static std::vector<uint8_t> MakeV3() {
    std::vector<uint8_t> buf(sizeof(Hdr33), 0);
    auto* h = reinterpret_cast<Hdr33*>(buf.data());
    h->magic = 0x50415854u;   // "PADT"
    h->version = 3;
    h->header_size = 132;
    return buf;
}

static std::vector<uint8_t> MakeV4(uint32_t min_host_rev, uint32_t patch_rev) {
    std::vector<uint8_t> buf(sizeof(Hdr35), 0);
    auto* h = reinterpret_cast<Hdr35*>(buf.data());
    h->magic = 0x50415854u;
    h->version = 4;
    h->header_size = 140;
    h->min_host_revision = min_host_rev;
    h->patch_revision = patch_rev;
    return buf;
}

static std::vector<uint8_t> MakeV4_ShortHeader() {
    // version=4 but header_size=132 → no trailing fields present.
    std::vector<uint8_t> buf(132, 0);
    auto* h = reinterpret_cast<Hdr33*>(buf.data());
    h->magic = 0x50415854u;
    h->version = 4;
    h->header_size = 132;
    return buf;
}

#include <vector>

// ── Tests ──────────────────────────────────────────────────────────────────

TEST(HotPatchApiV4, V3Blob_StillAccepted_BackwardCompat)
{
    auto blob = MakeV3();
    auto rc = ChaosApplyPatch(blob.data(), blob.size());
    // A valid v3 header with no methods must not be a format error or version
    // mismatch.  It parses successfully and the loader creates a 0-method context,
    // so the expected outcome is NO_METHODS (-3) — the patch is accepted but
    // applies nothing.
    EXPECT_EQ(rc, CHAOS_PATCH_ERR_NO_METHODS);
}

TEST(HotPatchApiV4, V4Blob_ZeroRevision_Accepted)
{
    ChaosSetHostRevision(0);       // accept any
    auto blob = MakeV4(0, 0);      // min_host_revision = 0 → compatible with any
    auto rc = ChaosApplyPatch(blob.data(), blob.size());
    // No version constraint, no methods → NO_METHODS, not VERSION_MISMATCH.
    EXPECT_EQ(rc, CHAOS_PATCH_ERR_NO_METHODS);
}

TEST(HotPatchApiV4, V4Blob_HostBelowMin_Rejected)
{
    ChaosSetHostRevision(5);
    auto blob = MakeV4(10, 1);     // patch requires host >= 10, host = 5
    auto rc = ChaosApplyPatch(blob.data(), blob.size());
    EXPECT_EQ(rc, CHAOS_PATCH_ERR_VERSION_MISMATCH);
}

TEST(HotPatchApiV4, V4Blob_HostMetMin_Accepted)
{
    ChaosSetHostRevision(10);
    auto blob = MakeV4(10, 2);     // requires >= 10, host = 10
    auto rc = ChaosApplyPatch(blob.data(), blob.size());
    // Host revision meets the minimum → version gate passes, no methods → NO_METHODS.
    EXPECT_EQ(rc, CHAOS_PATCH_ERR_NO_METHODS);
}

TEST(HotPatchApiV4, V4Blob_HostAboveMin_Accepted)
{
    ChaosSetHostRevision(20);
    auto blob = MakeV4(10, 3);     // requires >= 10, host = 20
    auto rc = ChaosApplyPatch(blob.data(), blob.size());
    // Host revision exceeds the minimum → version gate passes, no methods → NO_METHODS.
    EXPECT_EQ(rc, CHAOS_PATCH_ERR_NO_METHODS);
}

TEST(HotPatchApiV4, V4ShortHeader_NoVersionMismatch_NoCrash)
{
    ChaosSetHostRevision(5);
    auto blob = MakeV4_ShortHeader();  // only 132 bytes, no trailing fields
    auto rc = ChaosApplyPatch(blob.data(), blob.size());
    // No trailing fields to check → must not report a version mismatch.
    // The loader detects header_size < min_header(140) for version=4 and
    // returns nullptr, which ChaosApplyPatch surfaces as PARTIAL_ROLLBACK (-4).
    // This is the expected outcome for a structurally incomplete blob.
    EXPECT_NE(rc, CHAOS_PATCH_ERR_VERSION_MISMATCH);
    EXPECT_NE(rc, CHAOS_PATCH_ERR_INVALID_FORMAT);
    EXPECT_EQ(rc, CHAOS_PATCH_ERR_PARTIAL_ROLLBACK);
}

TEST(HotPatchApiV4, EmptyInput_InvalidFormat)
{
    EXPECT_EQ(ChaosApplyPatch("", 0), CHAOS_PATCH_ERR_INVALID_FORMAT);
}