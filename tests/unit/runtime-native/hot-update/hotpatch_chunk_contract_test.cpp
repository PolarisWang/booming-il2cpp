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
#include <cstddef>
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

// ── End-to-end: registration + name lookup over a chunked module ─────────
//
// The accessors above are only mechanism. What actually has to hold is that
// the REGISTRY answers identically for a chunked module and a flat one — that
// is the contract callers (bootstrap, patch resolution) depend on, and the one
// a boundary bug would break silently by returning a different method's token.

TEST(HotpatchChunks, RegistryLookup_ChunkedMatchesFlat) {
    TableData d;

    // Flat reference registry.
    chaos::il2cpp::runtime_core::HotpatchNameRegistry flatReg;
    const auto flatMod = MakeFlat(d);
    flatReg.RegisterModule(&flatMod);

    // Chunked registry: methods cut 2|3, so a lookup for the second type's
    // first method crosses the boundary.
    ChaosAbiChunkV0 tc[1] = {ChaosAbiChunkV0{&d.types[0], kTypeCount, sizeof(HotpatchTypeEntryV0)}};
    ChaosAbiChunkV0 mc[2] = {
        ChaosAbiChunkV0{&d.methods[0], 2u, sizeof(HotpatchMethodEntryV0)},
        ChaosAbiChunkV0{&d.methods[2], 3u, sizeof(HotpatchMethodEntryV0)},
    };
    const uint32_t c[1] = {kTypeCount};
    const uint32_t mcc[2] = {2u, 3u};
    auto chunkedMod = MakeChunked(d, c, 1, mcc, 2, tc, mc);

    chaos::il2cpp::runtime_core::HotpatchNameRegistry chunkedReg;
    chunkedReg.RegisterModule(&chunkedMod);

    ASSERT_EQ(flatReg.ModuleCount(), 1u);
    ASSERT_EQ(chunkedReg.ModuleCount(), 1u)
        << "a chunked module must still register — the old `type_entries == nullptr` "
           "guard would have skipped it entirely";

    // Every (type, method) pair must resolve to the same composite key.
    for (uint32_t ti = 0; ti < kTypeCount; ++ti) {
        for (uint32_t mi = 0; mi < d.types[ti].method_count; ++mi) {
            const auto* me = &d.methods[d.types[ti].first_method_index + mi];
            uint64_t flatKey = flatReg.LookupMethod(
                d.types[ti].namespace_name, d.types[ti].type_name, me->method_name);
            uint64_t chunkedKey = chunkedReg.LookupMethod(
                d.types[ti].namespace_name, d.types[ti].type_name, me->method_name);

            EXPECT_NE(flatKey, 0ull) << "flat lookup failed for " << me->method_name;
            EXPECT_EQ(chunkedKey, flatKey)
                << "chunked lookup disagreed for " << d.types[ti].type_name
                << "::" << me->method_name;
        }
    }
}

// An unknown name must miss in both layouts — a chunk walk that ran off the end
// would otherwise match whatever entry it landed on.
TEST(HotpatchChunks, RegistryLookup_UnknownNameMissesInBothLayouts) {
    TableData d;
    chaos::il2cpp::runtime_core::HotpatchNameRegistry flatReg;
    const auto flatMod = MakeFlat(d);
    flatReg.RegisterModule(&flatMod);

    ChaosAbiChunkV0 tc[1] = {ChaosAbiChunkV0{&d.types[0], kTypeCount, sizeof(HotpatchTypeEntryV0)}};
    ChaosAbiChunkV0 mc[1] = {ChaosAbiChunkV0{&d.methods[0], kMethodCount, sizeof(HotpatchMethodEntryV0)}};
    const uint32_t c[1] = {kTypeCount};
    const uint32_t mcc[1] = {kMethodCount};
    auto chunkedMod = MakeChunked(d, c, 1, mcc, 1, tc, mc);
    chaos::il2cpp::runtime_core::HotpatchNameRegistry chunkedReg;
    chunkedReg.RegisterModule(&chunkedMod);

    EXPECT_EQ(flatReg.LookupMethod("NS", "Alpha", "nope"), 0ull);
    EXPECT_EQ(chunkedReg.LookupMethod("NS", "Alpha", "nope"), 0ull);
    EXPECT_EQ(chunkedReg.LookupMethod("NS", "NoSuchType", "a0"), 0ull);
}

// ── ABI compatibility: a module from an OLDER codegen ────────────────────
//
// The chunk fields sit at the END of HotpatchModuleV0, after `struct_size`.
// A module emitted before chunking is physically SHORTER, so reading
// `.type_chunks` off it would read past its end — undefined behaviour that a
// release build would happily perform.
//
// `struct_size` is what makes the read legal, and it must be consulted BEFORE
// any chunk field is touched. These tests build such a module by reporting a
// size that predates the chunk fields, which is exactly what an older codegen
// stamps.

namespace {

// The size a pre-chunking codegen stamped: everything up to (and including)
// entry_table_size, without the four chunk fields. Derived from the real
// layout rather than hard-coded, so adding a field to the struct does not
// silently invalidate the test.
constexpr uint32_t kPreChunkStructSize =
    static_cast<uint32_t>(offsetof(HotpatchModuleV0, type_chunks));

}  // namespace

TEST(HotpatchChunks, OldCodegenModule_UsesFlatLayoutAndRegisters) {
    TableData d;
    auto m = MakeFlat(d);
    m.struct_size = kPreChunkStructSize;  // an older codegen's stamp

    // The flat fields are authoritative for this module.
    EXPECT_EQ(HotpatchTypeEntryAt(&m, 0), &d.types[0]);
    EXPECT_EQ(HotpatchMethodEntryAt(&m, 4), &d.methods[4]);
    EXPECT_TRUE(HotpatchModuleHasTypeEntries(&m));
    EXPECT_TRUE(HotpatchModuleHasMethodEntries(&m));

    // And it must actually register — a module that silently fails to
    // register loses every name in it.
    chaos::il2cpp::runtime_core::HotpatchNameRegistry reg;
    reg.RegisterModule(&m);
    EXPECT_EQ(reg.ModuleCount(), 1u);
    EXPECT_NE(reg.LookupMethod("NS", "Alpha", "a0"), 0ull);
}

// The dangerous case: an old module whose chunk fields LOOK populated, because
// the memory past its end happens to contain something non-null. The size gate
// must ignore them entirely — falling back to the flat fields, which the old
// module does have.
TEST(HotpatchChunks, OldCodegenModule_IgnoresChunkFieldsEvenWhenPopulated) {
    TableData d;
    auto m = MakeFlat(d);
    // Simulate "memory past the struct end looks like a chunk list": populate
    // the fields and then declare the module old. A size-blind implementation
    // would take the chunked path and resolve every index wrongly.
    ChaosAbiChunkV0 bogus_tc[1] = {ChaosAbiChunkV0{&d.types[0], 1u, sizeof(HotpatchTypeEntryV0)}};
    ChaosAbiChunkV0 bogus_mc[1] = {ChaosAbiChunkV0{&d.methods[3], 2u, sizeof(HotpatchMethodEntryV0)}};
    m.type_chunks = bogus_tc;
    m.type_chunk_count = 1u;
    m.method_chunks = bogus_mc;
    m.method_chunk_count = 1u;
    m.struct_size = kPreChunkStructSize;  // ...but the module is OLD

    // Must resolve via the flat arrays, NOT the (bogus) chunk list.
    EXPECT_EQ(HotpatchMethodEntryAt(&m, 0), &d.methods[0])
        << "an old module must not be addressed through chunk fields it does not have";
    EXPECT_EQ(HotpatchMethodEntryAt(&m, 4), &d.methods[4]);
    EXPECT_EQ(HotpatchTypeEntryAt(&m, 1), &d.types[1]);
}

// A producer that stamps no size at all is treated as pre-chunking: the flat
// fields exist in every layout, so they are the safe interpretation.
TEST(HotpatchChunks, UnstampedSize_TreatedAsFlat) {
    TableData d;
    auto m = MakeFlat(d);
    m.struct_size = 0u;
    EXPECT_EQ(HotpatchTypeEntryAt(&m, 0), &d.types[0]);
    EXPECT_TRUE(HotpatchModuleHasTypeEntries(&m));
}

// ── Negative control, automated ──────────────────────────────────────────
//
// The equivalence tests above (ChunkedLayout_ResolvesEveryIndexIdentically,
// RegistryLookup_ChunkedMatchesFlat) only mean something if a WRONG chunk
// layout would actually make them fail. That was verified by hand during
// development; this test makes it permanent, so a future change that
// accidentally weakens the chunk walk is caught rather than quietly accepted.
//
// 🔴 The failure mode being guarded is SILENT: a mis-resolved chunk boundary
// returns a DIFFERENT but well-formed entry, so the caller gets the wrong
// method's token — no crash, no diagnostic. That is why the assertion below
// is "these two layouts DISAGREE", not "this lookup returns null".

TEST(HotpatchChunks, NegativeControl_CorruptedChunkListDivergesFromFlat) {
    TableData d;

    // Correct chunking: methods cut 2|3.
    ChaosAbiChunkV0 good_mc[2] = {
        ChaosAbiChunkV0{&d.methods[0], 2u, sizeof(HotpatchMethodEntryV0)},
        ChaosAbiChunkV0{&d.methods[2], 3u, sizeof(HotpatchMethodEntryV0)},
    };
    // Corrupted: the second chunk claims to start one element early, so every
    // index landing in it resolves one element too far back.
    ChaosAbiChunkV0 bad_mc[2] = {
        ChaosAbiChunkV0{&d.methods[0], 2u, sizeof(HotpatchMethodEntryV0)},
        ChaosAbiChunkV0{&d.methods[1], 3u, sizeof(HotpatchMethodEntryV0)},
    };
    ChaosAbiChunkV0 tc[1] = {ChaosAbiChunkV0{&d.types[0], kTypeCount, sizeof(HotpatchTypeEntryV0)}};
    const uint32_t c[1] = {kTypeCount};
    const uint32_t good_counts[2] = {2u, 3u};
    const uint32_t bad_counts[2] = {2u, 3u};

    auto goodMod = MakeChunked(d, c, 1, good_counts, 2, tc, good_mc);
    auto badMod = MakeChunked(d, c, 1, bad_counts, 2, tc, bad_mc);

    const auto flat = MakeFlat(d);

    // Sanity: the CORRECT chunking agrees with flat at every index. Without
    // this the test could pass by both being wrong.
    for (uint32_t i = 0; i < kMethodCount; ++i) {
        ASSERT_EQ(HotpatchMethodEntryAt(&goodMod, i), HotpatchMethodEntryAt(&flat, i))
            << "precondition: correct chunking must match flat at index " << i;
    }

    // The corrupted list must DISAGREE somewhere — proving the comparison in
    // the equivalence tests is capable of detecting a bad boundary.
    int disagreements = 0;
    for (uint32_t i = 0; i < kMethodCount; ++i) {
        if (HotpatchMethodEntryAt(&badMod, i) != HotpatchMethodEntryAt(&flat, i))
            ++disagreements;
    }
    EXPECT_GT(disagreements, 0)
        << "a corrupted chunk list resolved identically to the flat layout at every "
           "index — the chunk walk is not being exercised, so the equivalence tests "
           "would pass even with a broken boundary";
}

// Same idea at the registry level: a corrupted chunk list must produce a
// different answer than the flat layout, or the lookup equivalence test is
// vacuous.
TEST(HotpatchChunks, NegativeControl_CorruptedChunkListChangesLookup) {
    TableData d;
    ChaosAbiChunkV0 tc[1] = {ChaosAbiChunkV0{&d.types[0], kTypeCount, sizeof(HotpatchTypeEntryV0)}};
    ChaosAbiChunkV0 bad_mc[1] = {
        ChaosAbiChunkV0{&d.methods[1], 4u, sizeof(HotpatchMethodEntryV0)},
    };
    const uint32_t c[1] = {kTypeCount};
    const uint32_t bad_counts[1] = {4u};
    auto badMod = MakeChunked(d, c, 1, bad_counts, 1, tc, bad_mc);

    chaos::il2cpp::runtime_core::HotpatchNameRegistry badReg;
    badReg.RegisterModule(&badMod);

    chaos::il2cpp::runtime_core::HotpatchNameRegistry flatReg;
    const auto flatMod = MakeFlat(d);
    flatReg.RegisterModule(&flatMod);

    // Look up the first type's first method: the flat layout finds it, and the
    // corrupted chunk list (which starts at methods[1]) finds a DIFFERENT
    // method for the same name — so the keys must differ.
    uint64_t flatKey = flatReg.LookupMethod("NS", "Alpha", "a0");
    uint64_t badKey  = badReg.LookupMethod("NS", "Alpha", "a0");
    EXPECT_NE(flatKey, 0ull) << "precondition: flat layout must resolve a0";
    EXPECT_NE(badKey, flatKey)
        << "a corrupted chunk list produced the SAME lookup result as the flat "
           "layout — the chunk walk is not being exercised";
}
