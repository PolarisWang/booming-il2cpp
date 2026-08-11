// ── Codegen GC Slot Map unit tests ──────────────────────────────────────────
//
// Tests the GcSlotMap registration, lookup, unregister, precise frame scanning,
// and hybrid scanning paths in gc_root_scanner.cpp.
//
// These tests complement the integration-level GcSlotMapRegistration test in
// codegen_native_test.cpp by adding focused unit coverage for edge cases and
// scan callback verification.

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <vector>

#include <codegen_bridge.h>
#include <gc_root_scanner.h>

using chaos::il2cpp::runtime_core::GcRegisterSlotMap;
using chaos::il2cpp::runtime_core::GcUnregisterSlotMap;
using chaos::il2cpp::runtime_core::GcLookupSlotMap;
using chaos::il2cpp::runtime_core::GcRegisterSlotMapsFromSection;
using chaos::il2cpp::runtime_core::GcScanPreciseFrame;
using chaos::il2cpp::runtime_core::GcScanPreciseSafepoint;
using chaos::il2cpp::runtime_core::GcScanSafepointRegisterRoots;
using chaos::il2cpp::runtime_core::GcScanFrameHybrid;
using chaos::il2cpp::runtime_core::ManagedFrameInfo;
using chaos::il2cpp::runtime_core::GcRootCallback;
using chaos::il2cpp::runtime_core::GcConservativeRootCallback;

namespace {

// ── Test data ─────────────────────────────────────────────────────────────

// A single GcSlotMapV0 with 3 object slots.
alignas(4) static const uint8_t kTestSlotMapData[] = {
    // GcSlotMapV0 header
    0x40, 0x00, 0x00, 0x00,  // frame_size = 64
    0x03, 0x00, 0x00, 0x00,  // num_gc_slots = 3
    // slots[] (variable length)
    0x10, 0x00, 0x00, 0x00,  // slots[0]: offset=0x10, kind=OBJECT
    0x20, 0x00, 0x00, 0x00,  // slots[1]: offset=0x20, kind=OBJECT
    0x30, 0x00, 0x00, 0x00,  // slots[2]: offset=0x30, kind=OBJECT
};

static const GcSlotMapV0* kTestSlotMap =
    reinterpret_cast<const GcSlotMapV0*>(kTestSlotMapData);

// Same layout but with one interior slot at offset 0x30.
// Interior kind moved to bit 31 (T2.2-C1 widened offset encoding).
alignas(4) static const uint8_t kTestSlotMapInteriorData[] = {
    0x40, 0x00, 0x00, 0x00,  // frame_size = 64
    0x03, 0x00, 0x00, 0x00,  // num_gc_slots = 3
    0x10, 0x00, 0x00, 0x00,  // slots[0]: offset=0x10, kind=OBJECT
    0x20, 0x00, 0x00, 0x00,  // slots[1]: offset=0x20, kind=OBJECT
    0x30, 0x00, 0x00, 0x80,  // slots[2]: offset=0x30, kind=INTERIOR (bit31 = 0x80000030)
};

static const GcSlotMapV0* kTestSlotMapInterior =
    reinterpret_cast<const GcSlotMapV0*>(kTestSlotMapInteriorData);

// Callback counters for scan tests.
struct ScanCounters {
    int precise_calls = 0;
    int conservative_calls = 0;
    void* last_root_addr = nullptr;
    bool last_is_interior = false;
};

void PreciseCallback(void* root_addr, bool is_interior, void* user_data) noexcept {
    auto* c = static_cast<ScanCounters*>(user_data);
    c->precise_calls++;
    c->last_root_addr = root_addr;
    c->last_is_interior = is_interior;
}

void ConservativeCallback(void* candidate, void* user_data) noexcept {
    auto* c = static_cast<ScanCounters*>(user_data);
    c->conservative_calls++;
}

// ── Tests ─────────────────────────────────────────────────────────────────

TEST(CodegenGcSlotMap, RegisterAndLookup) {
    int stub = 0;
    GcRegisterSlotMap(&stub, kTestSlotMap);
    const auto* found = GcLookupSlotMap(&stub);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->frame_size, 64u);
    EXPECT_EQ(found->num_gc_slots, 3u);
    // Verify slot encodings.
    EXPECT_EQ(found->slots[0], 0x10u);
    EXPECT_EQ(found->slots[1], 0x20u);
    EXPECT_EQ(found->slots[2], 0x30u);
    GcUnregisterSlotMap(&stub);
}

TEST(CodegenGcSlotMap, UnregisterRemovesEntry) {
    int stub = 0;
    GcRegisterSlotMap(&stub, kTestSlotMap);
    ASSERT_NE(GcLookupSlotMap(&stub), nullptr);
    GcUnregisterSlotMap(&stub);
    EXPECT_EQ(GcLookupSlotMap(&stub), nullptr);
}

TEST(CodegenGcSlotMap, LookupNullAddressReturnsNull) {
    EXPECT_EQ(GcLookupSlotMap(nullptr), nullptr);
}

TEST(CodegenGcSlotMap, UnregisterNullAddressNoCrash) {
    GcUnregisterSlotMap(nullptr);  // Should not crash.
}

TEST(CodegenGcSlotMap, UnregisterUnknownAddressNoCrash) {
    int stub = 0;
    GcUnregisterSlotMap(&stub);  // Was never registered — should not crash.
}

TEST(CodegenGcSlotMap, DuplicateRegistrationDoesNotCorrupt) {
    int stub = 0;
    GcRegisterSlotMap(&stub, kTestSlotMap);
    GcRegisterSlotMap(&stub, kTestSlotMapInterior);
    // After duplicate registration with a different map, the registry should
    // still return a valid pointer (may be first or last registered).
    const auto* found = GcLookupSlotMap(&stub);
    ASSERT_NE(found, nullptr);
    // Both registrations have frame_size=64.
    EXPECT_EQ(found->frame_size, 64u);
    GcUnregisterSlotMap(&stub);
}

TEST(CodegenGcSlotMap, ScanPreciseFrameCallbacks) {
    // Register a slot map, then scan a frame and verify callbacks fire.
    int code = 0;
    GcRegisterSlotMap(&code, kTestSlotMap);

    // Build a fake frame: 64 bytes of stack with pointers at offsets 0x10, 0x20, 0x30.
    alignas(8) uint8_t frame_memory[64] = {};
    void* const kFakeObj = reinterpret_cast<void*>(0x12345678ULL);
    void* const kFakeObj2 = reinterpret_cast<void*>(0x9ABCDEF0ULL);
    std::memcpy(frame_memory + 0x10, &kFakeObj, sizeof(void*));
    std::memcpy(frame_memory + 0x20, &kFakeObj2, sizeof(void*));
    // Leave offset 0x30 as nullptr.

    ManagedFrameInfo frame;
    frame.frame_ptr = frame_memory;
    frame.frame_size = 64;
    frame.return_address = &code;

    ScanCounters counters;
    GcScanPreciseFrame(frame, *kTestSlotMap, PreciseCallback, &counters);

    // Expect 3 precise callbacks (one per slot).
    EXPECT_EQ(counters.precise_calls, 3);
    // The interior slot flag should be false (all OBJECT slots).
    EXPECT_FALSE(counters.last_is_interior);

    GcUnregisterSlotMap(&code);
}

TEST(CodegenGcSlotMap, ScanPreciseFrameInteriorSlot) {
    int code = 0;
    GcRegisterSlotMap(&code, kTestSlotMapInterior);

    alignas(8) uint8_t frame_memory[64] = {};
    void* const kFakeObj = reinterpret_cast<void*>(0x12345678ULL);
    std::memcpy(frame_memory + 0x10, &kFakeObj, sizeof(void*));
    std::memcpy(frame_memory + 0x20, &kFakeObj, sizeof(void*));
    std::memcpy(frame_memory + 0x30, &kFakeObj, sizeof(void*));

    ManagedFrameInfo frame;
    frame.frame_ptr = frame_memory;
    frame.frame_size = 64;
    frame.return_address = &code;

    ScanCounters counters;
    GcScanPreciseFrame(frame, *kTestSlotMapInterior, PreciseCallback, &counters);

    EXPECT_EQ(counters.precise_calls, 3);
    // Last slot (offset 0x30) is INTERIOR — the callback should report it.
    EXPECT_TRUE(counters.last_is_interior);

    GcUnregisterSlotMap(&code);
}

TEST(CodegenGcSlotMap, ScanFrameHybridPrecisePath) {
    // Register the slot map → hybrid should take the precise path.
    int code = 0;
    GcRegisterSlotMap(&code, kTestSlotMap);

    alignas(8) uint8_t frame_memory[64] = {};
    ManagedFrameInfo frame;
    frame.frame_ptr = frame_memory;
    frame.frame_size = 64;
    frame.return_address = &code;

    ScanCounters counters;
    GcScanFrameHybrid(frame, PreciseCallback, ConservativeCallback, &counters);

    // With a registered slot map, precise path is taken → only precise callbacks.
    EXPECT_EQ(counters.precise_calls, 3);
    EXPECT_EQ(counters.conservative_calls, 0);

    GcUnregisterSlotMap(&code);
}

TEST(CodegenGcSlotMap, ScanFrameHybridConservativeFallback) {
    // Do NOT register a slot map → hybrid should fall back to conservative.
    int code = 0;

    alignas(8) uint8_t frame_memory[64] = {};
    // Place a non-null value in the frame so the conservative scanner
    // finds a candidate root (it only reports non-null slots).
    void* const kFakeObj = reinterpret_cast<void*>(0x12345678ULL);
    std::memcpy(frame_memory + 0x10, &kFakeObj, sizeof(void*));

    ManagedFrameInfo frame;
    frame.frame_ptr = frame_memory;
    frame.frame_size = 64;
    frame.return_address = &code;

    ScanCounters counters;
    GcScanFrameHybrid(frame, PreciseCallback, ConservativeCallback, &counters);

    // No slot map registered → conservative fallback scans all pointer-aligned slots.
    EXPECT_EQ(counters.precise_calls, 0);
    // At least the slot with the non-null object should be reported.
    EXPECT_GT(counters.conservative_calls, 0);
}

TEST(CodegenGcSlotMap, RegisterFromSection) {
    // Build a synthetic .gc_slot_maps section with one entry.
    // Section entry layout (packed, no alignment padding):
    //   [entry_total_size:4][code_address:8][frame_size:4][num_gc_slots:4][slots:N*4]
    alignas(8) uint8_t section_data[256] = {};
    uint32_t offset = 0;

    // Entry total size (includes itself): 4 + 8 + 4 + 4 + 2*4 = 28
    uint32_t entry_total = 28;
    std::memcpy(section_data + offset, &entry_total, 4); offset += 4;

    // Fake code address.
    int fake_code = 0;
    void* fake_code_ptr = &fake_code;
    std::memcpy(section_data + offset, &fake_code_ptr, sizeof(void*));
    offset += static_cast<uint32_t>(sizeof(void*));

    // GcSlotMapV0 body: frame_size + num_gc_slots + slots[]
    uint32_t frame_size = 48;
    uint32_t num_slots = 2;
    std::memcpy(section_data + offset, &frame_size, 4); offset += 4;
    std::memcpy(section_data + offset, &num_slots, 4);  offset += 4;

    // Slot entries
    uint32_t slot0 = CHAOS_GC_SLOT_ENCODE(0x08, CHAOS_GC_SLOT_KIND_OBJECT);
    uint32_t slot1 = CHAOS_GC_SLOT_ENCODE(0x18, CHAOS_GC_SLOT_KIND_OBJECT);
    std::memcpy(section_data + offset, &slot0, 4); offset += 4;
    std::memcpy(section_data + offset, &slot1, 4); offset += 4;

    GcRegisterSlotMapsFromSection(section_data, section_data + offset);

    // Verify the slot map was registered.
    const auto* found = GcLookupSlotMap(&fake_code);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->frame_size, 48u);
    EXPECT_EQ(found->num_gc_slots, 2u);

    // Cleanup.
    GcUnregisterSlotMap(&fake_code);
}

TEST(CodegenGcSlotMap, RegisterFromSectionEmptyRange) {
    // Empty section range should not crash.
    GcRegisterSlotMapsFromSection(nullptr, nullptr);
}

// Т2.2-A: per-safepoint precise scanning.  Build a GcPointMapV0 with two
// safepoints at different native offsets, each with DIFFERENT live stack
// slots.  Scanning at a return address in the first safepoint's range must
// report ONLY that safepoint's slots — proving per-safepoint precision (vs the
// whole-method union map).
TEST(CodegenGcSlotMap, ScanPreciseSafepointPerOffset) {
    // Layout: GcPointMapV0 { code_size=0x100, num_safepoints=2, num_gprs=16 }
    //   safepoints blob:
    //     sp[0]: GcSafepointV0{ offset=0x10, num_gc_slots=2, num_live_regs=0 }
    //            slots[0]=ENC(0x08,OBJ), slots[1]=ENC(0x18,OBJ)
    //     sp[1]: GcSafepointV0{ offset=0x40, num_gc_slots=1, num_live_regs=0 }
    //            slots[0]=ENC(0x50,OBJ)
    alignas(8) uint8_t pm_data[256] = {};
    uint32_t off = 0;
    uint32_t code_size = 0x100, num_sp = 2, num_gprs = 16;
    std::memcpy(pm_data + off, &code_size, 4); off += 4;
    std::memcpy(pm_data + off, &num_sp, 4);    off += 4;
    std::memcpy(pm_data + off, &num_gprs, 4);  off += 4;

    // sp[0]
    uint32_t off0 = 0x10, ns0 = 2, nr0 = 0;
    std::memcpy(pm_data + off, &off0, 4); off += 4;
    std::memcpy(pm_data + off, &ns0, 4);  off += 4;
    std::memcpy(pm_data + off, &nr0, 4);  off += 4;
    uint32_t s = CHAOS_GC_SLOT_ENCODE(0x08, CHAOS_GC_SLOT_KIND_OBJECT);
    std::memcpy(pm_data + off, &s, 4); off += 4;
    s = CHAOS_GC_SLOT_ENCODE(0x18, CHAOS_GC_SLOT_KIND_OBJECT);
    std::memcpy(pm_data + off, &s, 4); off += 4;

    // sp[1]
    uint32_t off1 = 0x40, ns1 = 1, nr1 = 0;
    std::memcpy(pm_data + off, &off1, 4); off += 4;
    std::memcpy(pm_data + off, &ns1, 4);  off += 4;
    std::memcpy(pm_data + off, &nr1, 4);  off += 4;
    s = CHAOS_GC_SLOT_ENCODE(0x50, CHAOS_GC_SLOT_KIND_OBJECT);
    std::memcpy(pm_data + off, &s, 4); off += 4;

    auto* pm = reinterpret_cast<const GcPointMapV0*>(pm_data);

    // Frame base at a known address; code "start" artificial.
    alignas(8) uint8_t frame_storage[256] = {};
    void* code_start = pm_data;  // arbitrary, only used for offset math

    ManagedFrameInfo frame;
    frame.frame_ptr = frame_storage;
    frame.frame_size = sizeof(frame_storage);
    frame.return_address = nullptr;
    ScanCounters c1, c2;

    // Return within sp[0] range (offset 0x12 → after 0x10 safepoint).
    frame.return_address = static_cast<uint8_t*>(code_start) + 0x12;
    GcScanPreciseSafepoint(frame, *pm, code_start, nullptr, 0, PreciseCallback, &c1);
    // sp[0] has 2 live slots.
    EXPECT_EQ(c1.precise_calls, 2);
    EXPECT_EQ(c1.last_root_addr, static_cast<void*>(static_cast<uint8_t*>(frame.frame_ptr) + 0x18));

    // Return within sp[1] range (offset 0x42 → after 0x40 safepoint).
    frame.return_address = static_cast<uint8_t*>(code_start) + 0x42;
    GcScanPreciseSafepoint(frame, *pm, code_start, nullptr, 0, PreciseCallback, &c2);
    // sp[1] has 1 live slot — NOT the union of both.
    EXPECT_EQ(c2.precise_calls, 1);
    EXPECT_EQ(c2.last_root_addr, static_cast<void*>(static_cast<uint8_t*>(frame.frame_ptr) + 0x50));

    // Before any safepoint (offset 0x00) → nothing live (frame not yet
    // interruptible).
    ScanCounters c0;
    frame.return_address = code_start;
    GcScanPreciseSafepoint(frame, *pm, code_start, nullptr, 0, PreciseCallback, &c0);
    EXPECT_EQ(c0.precise_calls, 0);
}

// Т2.2-C1: offset encoding widened past the old 12-bit (4096) limit.  A stack
// slot at offset 5000 (> 4095, the previous CHAOS_GC_SLOT_OFFSET_MASK cap) must
// now be encoded and scanned correctly.  Uses the union GcSlotMapV0 scan path.
TEST(CodegenGcSlotMap, ScanPreciseLargeOffsetBeyond4096) {
    // GcSlotMapV0 with one slot at offset 5000.
    alignas(8) uint8_t sm_data[64] = {};
    uint32_t off = 0;
    uint32_t frame_size = 6000, num_slots = 1;
    std::memcpy(sm_data + off, &frame_size, 4); off += 4;
    std::memcpy(sm_data + off, &num_slots, 4);  off += 4;
    uint32_t enc = CHAOS_GC_SLOT_ENCODE(5000, CHAOS_GC_SLOT_KIND_OBJECT);
    std::memcpy(sm_data + off, &enc, 4); off += 4;

    auto* sm = reinterpret_cast<const GcSlotMapV0*>(sm_data);
    EXPECT_EQ(sm->slots[0] & CHAOS_GC_SLOT_OFFSET_MASK, 5000u);
    EXPECT_EQ((sm->slots[0] & CHAOS_GC_SLOT_KIND_MASK), CHAOS_GC_SLOT_KIND_OBJECT);

    // Frame base sufficiently large that frame_base + 5000 is valid.
    alignas(8) uint8_t frame_storage[12000] = {};
    ManagedFrameInfo frame;
    frame.frame_ptr = frame_storage;
    frame.frame_size = sizeof(frame_storage);
    frame.return_address = nullptr;

    ScanCounters c;
    GcScanPreciseFrame(frame, *sm, PreciseCallback, &c);
    EXPECT_EQ(c.precise_calls, 1);
    EXPECT_EQ(c.last_root_addr, static_cast<void*>(static_cast<uint8_t*>(frame.frame_ptr) + 5000));
}

// Т2.2-B: register-root decoding.  A safepoint with num_live_regs=1 whose
// slot encodes physical register 8 (R8) should report the value held in R8
// (an object pointer) from an explicit register-value file.
TEST(CodegenGcSlotMap, ScanSafepointRegisterRoots) {
    // GcSafepointV0: offset=0, num_gc_slots=0, num_live_regs=1, slots[] = reg8.
    alignas(8) uint8_t sp_data[32] = {};
    uint32_t off = 0, nslots = 0, nregs = 1;
    std::memcpy(sp_data + off, &off, 4);       off += 4;  // native_offset
    std::memcpy(sp_data + off, &nslots, 4);    off += 4;  // num_gc_slots
    std::memcpy(sp_data + off, &nregs, 4);     off += 4;  // num_live_regs
    uint32_t reg_enc = CHAOS_GC_REG_ENCODE(8, CHAOS_GC_SLOT_KIND_OBJECT);
    std::memcpy(sp_data + off, &reg_enc, 4);   off += 4;

    auto* sp = reinterpret_cast<const GcSafepointV0*>(sp_data);

    // Register-value file: 16 slots, R8 (index 8) holds a fake object pointer.
    alignas(8) uint64_t gpr_values[16] = {};
    int fake_obj = 0;
    gpr_values[8] = reinterpret_cast<uint64_t>(&fake_obj);

    ScanCounters c;
    GcScanSafepointRegisterRoots(*sp, reinterpret_cast<const void* const*>(gpr_values), 16,
                                 PreciseCallback, &c);
    EXPECT_EQ(c.precise_calls, 1);
    EXPECT_EQ(c.last_root_addr, &fake_obj);
    EXPECT_FALSE(c.last_is_interior);
}

// Т2.2-D: precision-vs-safety invariant.  A per-safepoint precise scan must
// never under-retain — the exact roots it reports at a safepoint must be a
// SUBSET of the slots the conservative union GcSlotMapV0 would report for the
// same frame.  Build a point map whose safepoint reports one slot (offset 0x08)
// and a union map covering offsets {0x08, 0x18}; scanning the same frame with
// each at an identical return offset must yield precise ⊆ union.
TEST(CodegenGcSlotMap, ScanPreciseSubsetOfUnion) {
    // Union map: slots at 0x08 and 0x18.
    alignas(8) uint8_t sm_data[64] = {};
    uint32_t off = 0, fs = 64, ns = 2;
    std::memcpy(sm_data + off, &fs, 4); off += 4;
    std::memcpy(sm_data + off, &ns, 4); off += 4;
    uint32_t e = CHAOS_GC_SLOT_ENCODE(0x08, CHAOS_GC_SLOT_KIND_OBJECT);
    std::memcpy(sm_data + off, &e, 4); off += 4;
    e = CHAOS_GC_SLOT_ENCODE(0x18, CHAOS_GC_SLOT_KIND_OBJECT);
    std::memcpy(sm_data + off, &e, 4); off += 4;
    auto* sm = reinterpret_cast<const GcSlotMapV0*>(sm_data);

    // Point map: GcPointMapV0 with a single safepoint at offset 0x10 reporting
    // only slot 0x08 (the live set — the over-retention reduction).
    alignas(8) uint8_t pm_data[128] = {};
    off = 0;
    uint32_t code_size = 0x60, num_sp = 1, num_gprs = 16;
    std::memcpy(pm_data + off, &code_size, 4); off += 4;
    std::memcpy(pm_data + off, &num_sp, 4);    off += 4;
    std::memcpy(pm_data + off, &num_gprs, 4);  off += 4;
    uint32_t sp_off = 0x10, nsl = 1, nrg = 0;
    std::memcpy(pm_data + off, &sp_off, 4); off += 4;
    std::memcpy(pm_data + off, &nsl, 4);     off += 4;
    std::memcpy(pm_data + off, &nrg, 4);     off += 4;
    e = CHAOS_GC_SLOT_ENCODE(0x08, CHAOS_GC_SLOT_KIND_OBJECT);
    std::memcpy(pm_data + off, &e, 4); off += 4;
    auto* pm = reinterpret_cast<const GcPointMapV0*>(pm_data);

    // Same frame for both scans.
    alignas(8) uint8_t frame_storage[256] = {};
    ManagedFrameInfo frame;
    frame.frame_ptr = frame_storage;
    frame.frame_size = sizeof(frame_storage);
    frame.return_address = static_cast<uint8_t*>(pm_data) + 0x12;  // offset 0x12 ≥ 0x10
    void* code_start = pm_data;

    ScanCounters union_c, precise_c;
    GcScanPreciseFrame(frame, *sm, PreciseCallback, &union_c);
    GcScanPreciseSafepoint(frame, *pm, code_start, nullptr, 0, PreciseCallback, &precise_c);

    // Precise must not report more roots than the conservative union.
    EXPECT_LE(precise_c.precise_calls, union_c.precise_calls);
    // And the precise live slot (0x08) is a true subset member.
    EXPECT_EQ(precise_c.precise_calls, 1);
    EXPECT_EQ(precise_c.last_root_addr, static_cast<void*>(static_cast<uint8_t*>(frame.frame_ptr) + 0x08));
}

// Phase 2 (2a): windowed precise scan.  When a register-value file is passed,
// GcScanPreciseSafepoint must report BOTH the stack slots AND the live register
// roots at the safepoint — never losing the stack slots (no under-retain when a
// window is present), and adding the register root reported by
// GcScanSafepointRegisterRoots.
TEST(CodegenGcSlotMap, ScanPreciseSafepointWindowed) {
    // Point map: one safepoint at offset 0x10 with 1 stack slot (0x08) + 1
    // register root (physical R8).  num_gc_slots=1, num_live_regs=1.
    alignas(8) uint8_t pm_data[128] = {};
    uint32_t off = 0;
    uint32_t code_size = 0x60, num_sp = 1, num_gprs_hdr = 16;
    std::memcpy(pm_data + off, &code_size, 4); off += 4;
    std::memcpy(pm_data + off, &num_sp, 4);    off += 4;
    std::memcpy(pm_data + off, &num_gprs_hdr, 4); off += 4;
    uint32_t sp_off = 0x10, nsl = 1, nrg = 1;
    std::memcpy(pm_data + off, &sp_off, 4); off += 4;
    std::memcpy(pm_data + off, &nsl, 4);     off += 4;
    std::memcpy(pm_data + off, &nrg, 4);     off += 4;
    uint32_t e = CHAOS_GC_SLOT_ENCODE(0x08, CHAOS_GC_SLOT_KIND_OBJECT);
    std::memcpy(pm_data + off, &e, 4); off += 4;
    e = CHAOS_GC_REG_ENCODE(8, CHAOS_GC_SLOT_KIND_OBJECT);   // R8 holds a ref
    std::memcpy(pm_data + off, &e, 4); off += 4;
    auto* pm = reinterpret_cast<const GcPointMapV0*>(pm_data);

    alignas(8) uint8_t frame_storage[256] = {};
    ManagedFrameInfo frame;
    frame.frame_ptr = frame_storage;
    frame.frame_size = sizeof(frame_storage);
    frame.return_address = pm_data + 0x12;   // offset 0x12 ≥ 0x10 safepoint

    // A register window where R8 (index 8) holds a fake object pointer.
    alignas(8) uint64_t gpr_values[16] = {};
    int fake_obj = 0;
    gpr_values[8] = reinterpret_cast<uint64_t>(&fake_obj);

    // Windowed scan must report stack slot + register root.
    ScanCounters c1;
    GcScanPreciseSafepoint(frame, *pm, pm_data,
                           reinterpret_cast<const void* const*>(gpr_values), 16,
                           PreciseCallback, &c1);
    // 1 stack slot + 1 register root = 2 roots total.
    EXPECT_EQ(c1.precise_calls, 2);
    // Register roots are reported after stack slots, so the last root is BOTH
    // the register root (R8's fake object) — verifying the window is applied.
    EXPECT_EQ(c1.last_root_addr, &fake_obj);

    // No-window scan (nullptr, 0) must report ONLY the stack slot — the window
    // is strictly additive, never removing stack slots.
    ScanCounters c2;
    GcScanPreciseSafepoint(frame, *pm, pm_data, nullptr, 0, PreciseCallback, &c2);
    EXPECT_EQ(c2.precise_calls, 1);
    EXPECT_EQ(c2.last_root_addr, static_cast<void*>(static_cast<uint8_t*>(frame.frame_ptr) + 0x08));
}

}  // namespace
