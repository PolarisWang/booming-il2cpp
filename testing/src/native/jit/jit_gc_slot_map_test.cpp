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
alignas(4) static const uint8_t kTestSlotMapInteriorData[] = {
    0x40, 0x00, 0x00, 0x00,  // frame_size = 64
    0x03, 0x00, 0x00, 0x00,  // num_gc_slots = 3
    0x10, 0x00, 0x00, 0x00,  // slots[0]: offset=0x10, kind=OBJECT
    0x20, 0x00, 0x00, 0x00,  // slots[1]: offset=0x20, kind=OBJECT
    0x30, 0x10, 0x00, 0x00,  // slots[2]: offset=0x30, kind=INTERIOR
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

}  // namespace
