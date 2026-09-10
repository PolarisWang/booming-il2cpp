/// Unit tests for CRAG root scanner (C1.1-C1.4) — GoogleTest.

#include <chaos/native_types.h>

#include "gc_root_scanner.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// GcSlotMap struct layout and encoding macros
// ════════════════════════════════════════════════════════════════════════════

TEST(GcRootScanner, SlotMapLayout) {
    EXPECT_EQ(sizeof(GcSlotMapV0), 8u) << "header: frame_size(4) + num_gc_slots(4)";

    uint32_t obj_slot = CHAOS_GC_SLOT_ENCODE(16, CHAOS_GC_SLOT_KIND_OBJECT);
    EXPECT_EQ(obj_slot & CHAOS_GC_SLOT_OFFSET_MASK, 16u);
    EXPECT_EQ(obj_slot & CHAOS_GC_SLOT_KIND_MASK,
              static_cast<uint32_t>(CHAOS_GC_SLOT_KIND_OBJECT));

    uint32_t interior_slot = CHAOS_GC_SLOT_ENCODE(32, CHAOS_GC_SLOT_KIND_INTERIOR);
    EXPECT_EQ(interior_slot & CHAOS_GC_SLOT_OFFSET_MASK, 32u);
    EXPECT_EQ(interior_slot & CHAOS_GC_SLOT_KIND_MASK,
              static_cast<uint32_t>(CHAOS_GC_SLOT_KIND_INTERIOR));

    uint32_t max_offset = CHAOS_GC_SLOT_ENCODE(4095, CHAOS_GC_SLOT_KIND_OBJECT);
    EXPECT_EQ(max_offset & CHAOS_GC_SLOT_OFFSET_MASK, 4095u);
}

// ════════════════════════════════════════════════════════════════════════════
// GcSlotMap registry
// ════════════════════════════════════════════════════════════════════════════

TEST(GcRootScanner, Registry) {
    EXPECT_EQ(GcLookupSlotMap(reinterpret_cast<const void*>(0x1234)), nullptr);

    uint32_t slots_a[] = { CHAOS_GC_SLOT_ENCODE(8, CHAOS_GC_SLOT_KIND_OBJECT) };
    alignas(GcSlotMapV0) uint8_t map_a_buf[sizeof(GcSlotMapV0) + sizeof(uint32_t)] = {};
    auto& map_a_stored = *reinterpret_cast<GcSlotMapV0*>(map_a_buf);
    map_a_stored.frame_size = 32;
    map_a_stored.num_gc_slots = 1;
    std::memcpy(map_a_buf + sizeof(GcSlotMapV0), slots_a, sizeof(uint32_t));

    void* addr_a = reinterpret_cast<void*>(0x1000);
    void* addr_b = reinterpret_cast<void*>(0x2000);

    GcRegisterSlotMap(addr_a, &map_a_stored);
    GcRegisterSlotMap(addr_b, nullptr);

    const GcSlotMapV0* found = GcLookupSlotMap(addr_a);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->frame_size, 32u);
    EXPECT_EQ(found->num_gc_slots, 1u);

    EXPECT_EQ(GcLookupSlotMap(addr_b), nullptr);
    EXPECT_EQ(GcLookupSlotMap(reinterpret_cast<void*>(0x3000)), nullptr);
    EXPECT_EQ(GcLookupSlotMap(nullptr), nullptr);
}

// ════════════════════════════════════════════════════════════════════════════
// Precise frame scanning
// ════════════════════════════════════════════════════════════════════════════

TEST(GcRootScanner, PreciseScan) {
    alignas(void*) uint8_t frame_memory[64] = {};
    std::memset(frame_memory, 0, 64);

    void* fake_obj = reinterpret_cast<void*>(0xBEEF);
    std::memcpy(frame_memory + 8, &fake_obj, sizeof(void*));

    uint32_t slots[] = { CHAOS_GC_SLOT_ENCODE(8, CHAOS_GC_SLOT_KIND_OBJECT) };

    struct SlotMapWithSlots {
        uint32_t frame_size;
        uint32_t num_gc_slots;
        uint32_t slots[1];
    };
    static_assert(sizeof(SlotMapWithSlots) == sizeof(GcSlotMapV0) + sizeof(uint32_t),
                  "layout must match GcSlotMapV0 with 1 slot");
    SlotMapWithSlots sm = {};
    sm.frame_size = 64;
    sm.num_gc_slots = 1;
    sm.slots[0] = slots[0];

    ManagedFrameInfo frame = {};
    frame.frame_ptr = frame_memory;
    frame.frame_size = 64;
    frame.return_address = reinterpret_cast<void*>(0x1000);

    struct ScanResult { void* root_addr = nullptr; bool is_interior = false; bool called = false; };
    ScanResult result;

    auto callback = [](void* root_addr, bool is_interior, void* user_data) {
        auto* r = static_cast<ScanResult*>(user_data);
        r->root_addr = root_addr;
        r->is_interior = is_interior;
        r->called = true;
    };

    GcScanPreciseFrame(frame, reinterpret_cast<const GcSlotMapV0&>(sm), callback, &result);

    EXPECT_TRUE(result.called) << "callback not invoked";
    EXPECT_EQ(result.root_addr, &frame_memory[8]) << "wrong root address";
    EXPECT_FALSE(result.is_interior) << "should not be interior";
}

// ════════════════════════════════════════════════════════════════════════════
// Conservative frame scanning
// ════════════════════════════════════════════════════════════════════════════

TEST(GcRootScanner, ConservativeScan) {
    alignas(void*) uint8_t frame_memory[32] = {};
    std::memset(frame_memory, 0, 32);

    void* fake_ptr = reinterpret_cast<void*>(0x1000);
    std::memcpy(frame_memory, &fake_ptr, sizeof(void*));

    void* null_ptr = nullptr;
    std::memcpy(frame_memory + 8, &null_ptr, sizeof(void*));

    uintptr_t misaligned = 0x1001;
    std::memcpy(frame_memory + 16, &misaligned, sizeof(uintptr_t));

    ManagedFrameInfo frame = {};
    frame.frame_ptr = frame_memory;
    frame.frame_size = 32;
    frame.return_address = reinterpret_cast<void*>(0x2000);

    struct ConservativeResult { void* candidate = nullptr; int count = 0; };
    ConservativeResult result;

    auto callback = [](void* candidate, void* user_data) {
        auto* r = static_cast<ConservativeResult*>(user_data);
        r->candidate = candidate;
        r->count++;
    };

    GcScanConservativeFrame(frame, callback, &result);

    EXPECT_EQ(result.count, 1) << "expected 1 conservative root";
    EXPECT_EQ(result.candidate, fake_ptr) << "wrong candidate";
}

// ════════════════════════════════════════════════════════════════════════════
// Hybrid scanning
// ════════════════════════════════════════════════════════════════════════════

TEST(GcRootScanner, HybridScanPrecisePath) {
    uint32_t slots[] = { CHAOS_GC_SLOT_ENCODE(8, CHAOS_GC_SLOT_KIND_OBJECT) };
    struct SlotMapWithSlots {
        uint32_t frame_size;
        uint32_t num_gc_slots;
        uint32_t slots[1];
    };
    static SlotMapWithSlots s_sm = {};
    s_sm.frame_size = 64;
    s_sm.num_gc_slots = 1;
    s_sm.slots[0] = slots[0];
    GcRegisterSlotMap(reinterpret_cast<void*>(0x3000),
                       reinterpret_cast<const GcSlotMapV0*>(&s_sm));

    alignas(void*) uint8_t frame_memory[64] = {};
    std::memset(frame_memory, 0, 64);
    void* fake_obj = reinterpret_cast<void*>(0xBEE8);
    std::memcpy(frame_memory + 8, &fake_obj, sizeof(void*));

    ManagedFrameInfo frame = {};
    frame.frame_ptr = frame_memory;
    frame.frame_size = 64;
    frame.return_address = reinterpret_cast<void*>(0x3000);

    struct ScanResult { void* root_addr = nullptr; bool is_interior = false; bool called = false; };
    ScanResult precise_result;

    auto precise_cb = [](void* root_addr, bool is_interior, void* user_data) {
        auto* r = static_cast<ScanResult*>(user_data);
        r->root_addr = root_addr;
        r->is_interior = is_interior;
        r->called = true;
    };
    auto conservative_cb = [](void*, void*) {};

    GcScanFrameHybrid(frame, precise_cb, conservative_cb, &precise_result);

    EXPECT_TRUE(precise_result.called) << "precise callback not invoked";
    EXPECT_EQ(precise_result.root_addr, &frame_memory[8]) << "precise wrong addr";
}

TEST(GcRootScanner, HybridScanConservativeFallback) {
    uint32_t slots[] = { CHAOS_GC_SLOT_ENCODE(8, CHAOS_GC_SLOT_KIND_OBJECT) };
    struct SlotMapWithSlots {
        uint32_t frame_size;
        uint32_t num_gc_slots;
        uint32_t slots[1];
    };
    static SlotMapWithSlots s_sm = {};
    s_sm.frame_size = 64;
    s_sm.num_gc_slots = 1;
    s_sm.slots[0] = slots[0];
    GcRegisterSlotMap(reinterpret_cast<void*>(0x3000),
                       reinterpret_cast<const GcSlotMapV0*>(&s_sm));

    alignas(void*) uint8_t frame_memory[64] = {};
    std::memset(frame_memory, 0, 64);
    void* fake_obj = reinterpret_cast<void*>(0xBEE8);
    std::memcpy(frame_memory + 8, &fake_obj, sizeof(void*));

    ManagedFrameInfo frame = {};
    frame.frame_ptr = frame_memory;
    frame.frame_size = 64;
    frame.return_address = reinterpret_cast<void*>(0x4000);  // unknown address

    struct ConservativeResult { int count = 0; };
    ConservativeResult cons_result;

    auto precise_cb = [](void*, bool, void*) {};
    auto conservative_cb = [](void*, void* user_data) {
        static_cast<ConservativeResult*>(user_data)->count++;
    };

    GcScanFrameHybrid(frame, precise_cb, conservative_cb, &cons_result);
    EXPECT_GE(cons_result.count, 1) << "conservative should find at least 1 root";
}
