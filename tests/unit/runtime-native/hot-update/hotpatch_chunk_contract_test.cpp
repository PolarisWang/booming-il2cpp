/// Contract tests for the chunked hotpatch name table (C3-2).
///
/// A module's type/method arrays may be emitted across several translation
/// units, in which case it carries a chunk list instead of one flat block.
/// The invariant that makes this safe is that INDICES DO NOT CHANGE: the
/// logical array is the concatenation of the chunks in order, so
/// `first_method_index`, `type_entry_count` and every other index keep their
/// meaning. These tests pin that by building the same logical table twice —
/// once flat, once chunked — and requiring identical lookups.
///
/// The failure this guards against is silent: a chunk boundary resolved to the
/// wrong offset returns a *different but well-formed* entry, so the runtime
/// answers with the wrong method rather than crashing.

#include "hotpatch_table.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <vector>

// The `*V0` descriptors are C structs from codegen_bridge.h at global scope
// (they are the ABI), so they are used unqualified.  Only the accessors are
// C++ and live in runtime_core.
using chaos::il2cpp::runtime_core::HotpatchMethodEntryAt;
using chaos::il2cpp::runtime_core::HotpatchModuleHasMethodEntries;
using chaos::il2cpp::runtime_core::HotpatchModuleHasTypeEntries;
using chaos::il2cpp::runtime_core::HotpatchTypeEntryAt;

namespace {

// The logical table used by every test below. Two types; the first owns three
// methods (so `first_method_index` for the second type is non-zero and a
// mis-resolved chunk boundary is observable), the second owns two.
constexpr uint32_t kTypeCount = 2;
constexpr uint32_t kMethodCount = 5;

struct TableData {
    HotpatchTypeEntryV0 types[kTypeCount];
    HotpatchMethodEntryV0 methods[kMethodCount];
    HotpatchSlotEntryV0 slots[kMethodCount];

    TableData() {
        types[0] = HotpatchTypeEntryV0{"Alpha", "NS", 0u, 3u};
        types[1] = HotpatchTypeEntryV0{"Beta",  "NS", 3u, 2u};

        methods[0] = HotpatchMethodEntryV0{"a0", 0x06000001u, 0u};
        methods[1] = HotpatchMethodEntryV0{"a1", 0x06000002u, 1u};
        methods[2] = HotpatchMethodEntryV0{"a2", 0x06000003u, 2u};
        methods[3] = HotpatchMethodEntryV0{"b0", 0x06000004u, 0u};
        methods[4] = HotpatchMethodEntryV0{"b1", 0x06000005u, 1u};

        for (uint32_t i = 0; i < kMethodCount; ++i)
            slots[i] = HotpatchSlotEntryV0{methods[i].method_token, i};
    }
};

// Flat module: the pre-chunking layout, and the reference every chunked
// expectation is compared against.
HotpatchModuleV0 MakeFlat(const TableData& d) {
    HotpatchModuleV0 m{};
    m.struct_size = sizeof(HotpatchModuleV0);
    m.module_name = "test";
    m.type_entries = d.types;
    m.type_entry_count = kTypeCount;
    m.method_entries = d.methods;
    m.method_entry_count = kMethodCount;
    m.token_slot_entries = d.slots;
    m.token_slot_entry_count = kMethodCount;
    m.entry_table = nullptr;
    m.entry_table_size = 0u;
    return m;
}

// Chunked module. `split` picks where the logical arrays are cut, so the tests
// can place a boundary inside a type, between types, and at the very end.
HotpatchModuleV0 MakeChunked(const TableData& d,
                             const uint32_t* type_counts, uint32_t type_chunk_count,
                             const uint32_t* method_counts, uint32_t method_chunk_count,
                             const ChaosAbiChunkV0* type_chunks,
                             const ChaosAbiChunkV0* method_chunks) {
    HotpatchModuleV0 m{};
    m.struct_size = sizeof(HotpatchModuleV0);
    m.module_name = "test";
    // Deliberately null: a chunked module has no flat block, which is what the
    // presence checks must tolerate.
    m.type_entries = nullptr;
    m.type_entry_count = 0u;
    for (uint32_t i = 0; i < type_chunk_count; ++i) m.type_entry_count += type_counts[i];
    m.method_entries = nullptr;
    m.method_entry_count = 0u;
    for (uint32_t i = 0; i < method_chunk_count; ++i) m.method_entry_count += method_counts[i];
    m.token_slot_entries = d.slots;
    m.token_slot_entry_count = kMethodCount;
    m.type_chunks = type_chunks;
    m.type_chunk_count = type_chunk_count;
    m.method_chunks = method_chunks;
    m.method_chunk_count = method_chunk_count;
    (void)d;
    return m;
}

}  // namespace

// ── Flat layout must be untouched ────────────────────────────────────────
//
// Every module emitted before chunking takes this path, so it has to behave
// exactly as the pre-C3 indexing did — including the bounds check, which the
// raw `arr[i]` never had and which callers now rely on.

TEST(HotpatchChunks, FlatLayout_ResolvesEveryIndex) {
    TableData d;
    const auto m = MakeFlat(d);

    for (uint32_t i = 0; i < kTypeCount; ++i) {
        const auto* t = HotpatchTypeEntryAt(&m, i);
        ASSERT_NE(t, nullptr) << "type index " << i;
        EXPECT_EQ(t, &d.types[i]);
        EXPECT_EQ(std::strcmp(t->type_name, d.types[i].type_name), 0) << "type " << i;
    }
    for (uint32_t i = 0; i < kMethodCount; ++i) {
        const auto* me = HotpatchMethodEntryAt(&m, i);
        ASSERT_NE(me, nullptr) << "method index " << i;
        EXPECT_EQ(me, &d.methods[i]);
        EXPECT_EQ(std::strcmp(me->method_name, d.methods[i].method_name), 0) << "method " << i;
    }
}

TEST(HotpatchChunks, FlatLayout_OutOfRange_ReturnsNull) {
    TableData d;
    const auto m = MakeFlat(d);
    EXPECT_TRUE(HotpatchTypeEntryAt(&m, kTypeCount) == nullptr);
    EXPECT_TRUE(HotpatchMethodEntryAt(&m, kMethodCount) == nullptr);
}

// ── Chunked layout equals flat ───────────────────────────────────────────
//
// The central C3 invariant. If a chunk boundary is resolved to the wrong
// offset the result is a different, well-formed entry — so the assertion is
// pointer identity against the same logical element, not merely "not null".

TEST(HotpatchChunks, ChunkedLayout_ResolvesEveryIndexIdentically) {
    TableData d;

    // Types cut 1|1, methods cut 2|3 — the second cut lands strictly INSIDE
    // the first type's method range, so a boundary bug shifts lookups.
    ChaosAbiChunkV0 type_chunks[2] = {
        ChaosAbiChunkV0{&d.types[0], 1u, sizeof(HotpatchTypeEntryV0)},
        ChaosAbiChunkV0{&d.types[1], 1u, sizeof(HotpatchTypeEntryV0)},
    };
    ChaosAbiChunkV0 method_chunks[2] = {
        ChaosAbiChunkV0{&d.methods[0], 2u, sizeof(HotpatchMethodEntryV0)},
        ChaosAbiChunkV0{&d.methods[2], 3u, sizeof(HotpatchMethodEntryV0)},
    };
    const uint32_t tc[2] = {1u, 1u};
    const uint32_t mc[2] = {2u, 3u};

    auto m = MakeChunked(d, tc, 2, mc, 2, type_chunks, method_chunks);

    const auto flat = MakeFlat(d);
    for (uint32_t i = 0; i < kTypeCount; ++i) {
        const auto* got = HotpatchTypeEntryAt(&m, i);
        ASSERT_NE(got, nullptr) << "chunked type index " << i;
        EXPECT_EQ(got, HotpatchTypeEntryAt(&flat, i))
            << "chunked type " << i << " disagrees with the flat layout";
    }
    for (uint32_t i = 0; i < kMethodCount; ++i) {
        const auto* got = HotpatchMethodEntryAt(&m, i);
        ASSERT_NE(got, nullptr) << "chunked method index " << i;
        EXPECT_EQ(got, HotpatchMethodEntryAt(&flat, i))
            << "chunked method " << i << " disagrees with the flat layout";
    }
}

// A single chunk is the degenerate case: every index must still resolve, and
// the count must come from the chunk list rather than the (null) flat field.
TEST(HotpatchChunks, SingleChunk_ResolvesEveryIndex) {
    TableData d;
    ChaosAbiChunkV0 tc[1] = {ChaosAbiChunkV0{&d.types[0], kTypeCount, sizeof(HotpatchTypeEntryV0)}};
    ChaosAbiChunkV0 mc[1] = {ChaosAbiChunkV0{&d.methods[0], kMethodCount, sizeof(HotpatchMethodEntryV0)}};
    const uint32_t c[1] = {kTypeCount};
    const uint32_t mc_count[1] = {kMethodCount};

    auto m = MakeChunked(d, c, 1, mc_count, 1, tc, mc);
    EXPECT_EQ(HotpatchTypeEntryAt(&m, 1), &d.types[1]);
    EXPECT_EQ(HotpatchMethodEntryAt(&m, 4), &d.methods[4]);
}

// One method per chunk: every index is its own chunk, so the prefix-sum walk
// is exercised at every position.
TEST(HotpatchChunks, ManyChunks_ResolveEveryIndex) {
    TableData d;
    std::vector<ChaosAbiChunkV0> tc;
    std::vector<ChaosAbiChunkV0> mc;
    std::vector<uint32_t> tc_counts, mc_counts;
    for (uint32_t i = 0; i < kTypeCount; ++i) {
        tc.push_back(ChaosAbiChunkV0{&d.types[i], 1u, sizeof(HotpatchTypeEntryV0)});
        tc_counts.push_back(1u);
    }
    for (uint32_t i = 0; i < kMethodCount; ++i) {
        mc.push_back(ChaosAbiChunkV0{&d.methods[i], 1u, sizeof(HotpatchMethodEntryV0)});
        mc_counts.push_back(1u);
    }

    auto m = MakeChunked(d, tc_counts.data(), kTypeCount, mc_counts.data(), kMethodCount,
                         tc.data(), mc.data());
    for (uint32_t i = 0; i < kTypeCount; ++i)
        EXPECT_EQ(HotpatchTypeEntryAt(&m, i), &d.types[i]) << "type " << i;
    for (uint32_t i = 0; i < kMethodCount; ++i)
        EXPECT_EQ(HotpatchMethodEntryAt(&m, i), &d.methods[i]) << "method " << i;
}

TEST(HotpatchChunks, ChunkedLayout_OutOfRange_ReturnsNull) {
    TableData d;
    ChaosAbiChunkV0 tc[1] = {ChaosAbiChunkV0{&d.types[0], kTypeCount, sizeof(HotpatchTypeEntryV0)}};
    ChaosAbiChunkV0 mc[1] = {ChaosAbiChunkV0{&d.methods[0], kMethodCount, sizeof(HotpatchMethodEntryV0)}};
    const uint32_t c[1] = {kTypeCount};
    const uint32_t mcc[1] = {kMethodCount};

    auto m = MakeChunked(d, c, 1, mcc, 1, tc, mc);
    EXPECT_TRUE(HotpatchTypeEntryAt(&m, kTypeCount) == nullptr);
    EXPECT_TRUE(HotpatchMethodEntryAt(&m, kMethodCount) == nullptr);
}

// A descriptor whose element size does not match the type is a codegen/contract
// mismatch. Returning it would reinterpret memory as the wrong struct, so the
// accessor must decline rather than hand back a mis-typed pointer.
TEST(HotpatchChunks, ElementSizeMismatch_ReturnsNull) {
    TableData d;
    ChaosAbiChunkV0 tc[1] = {ChaosAbiChunkV0{&d.types[0], kTypeCount, 4u /* wrong */}};
    const uint32_t c[1] = {kTypeCount};

    auto m = MakeChunked(d, c, 1, nullptr, 0, tc, nullptr);
    EXPECT_TRUE(HotpatchTypeEntryAt(&m, 0) == nullptr);
}

// ── Presence checks must tolerate the chunked layout ─────────────────────
//
// The pre-C3 guard was `type_entries == nullptr`, which a chunked module
// satisfies — it would skip registration entirely and silently lose every
// name in the module.

TEST(HotpatchChunks, PresenceCheck_ChunkedModuleIsPresent) {
    TableData d;
    ChaosAbiChunkV0 tc[1] = {ChaosAbiChunkV0{&d.types[0], kTypeCount, sizeof(HotpatchTypeEntryV0)}};
    ChaosAbiChunkV0 mc[1] = {ChaosAbiChunkV0{&d.methods[0], kMethodCount, sizeof(HotpatchMethodEntryV0)}};
    const uint32_t c[1] = {kTypeCount};
    const uint32_t mcc[1] = {kMethodCount};

    auto m = MakeChunked(d, c, 1, mcc, 1, tc, mc);
    ASSERT_EQ(m.type_entries, nullptr) << "precondition: no flat block";
    ASSERT_EQ(m.method_entries, nullptr) << "precondition: no flat block";
    EXPECT_TRUE(HotpatchModuleHasTypeEntries(&m));
    EXPECT_TRUE(HotpatchModuleHasMethodEntries(&m));
}

TEST(HotpatchChunks, PresenceCheck_EmptyModuleIsAbsent) {
    HotpatchModuleV0 m{};
    m.struct_size = sizeof(HotpatchModuleV0);
    EXPECT_FALSE(HotpatchModuleHasTypeEntries(&m));
    EXPECT_FALSE(HotpatchModuleHasMethodEntries(&m));
    EXPECT_FALSE(HotpatchModuleHasTypeEntries(nullptr));
}
