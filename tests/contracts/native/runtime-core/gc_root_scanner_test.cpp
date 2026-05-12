/// Unit tests for CRAG root scanner (C1.1-C1.4).
///
/// Exercises:
///   - GcSlotMapV0 struct layout and slot encoding macros
///   - GcRegisterSlotMap / GcLookupSlotMap
///   - GcScanPreciseFrame with known slot map
///   - GcScanConservativeFrame fallback
///   - GcScanFrameHybrid (preferred path + fallback)

#include <chaos/native_types.h>

#include "gc_root_scanner.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ════════════════════════════════════════════════════════════════════════════
// Test helpers
// ════════════════════════════════════════════════════════════════════════════

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

#define TEST(name)                                                      \
    do {                                                                \
        ++g_tests;                                                      \
        g_sub = 0;                                                      \
        printf("  TEST: %s ... ", name);                                \
    } while (0)

#define SUBTEST(name)                                                   \
    do {                                                                \
        ++g_sub;                                                        \
        printf("\n    SUB %d: %s ... ", g_sub, name);                   \
    } while (0)

#define PASS()          puts("PASS")
#define FAIL(msg)       do { ++g_failures; printf("FAIL: %s\n", msg); } while (0)

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// GcSlotMap struct layout and encoding macros
// ════════════════════════════════════════════════════════════════════════════

static void test_slot_map_layout() {
    TEST("GcSlotMapV0 layout");

    SUBTEST("struct size sanity");
    // GcSlotMapV0: frame_size(4) + num_gc_slots(4) = 8 bytes header
    if (sizeof(GcSlotMapV0) != 8) {
        FAIL("unexpected header size");
        return;
    }
    PASS();

    SUBTEST("slot encoding macros");
    uint32_t obj_slot = CHAOS_GC_SLOT_ENCODE(16, CHAOS_GC_SLOT_KIND_OBJECT);
    if ((obj_slot & CHAOS_GC_SLOT_OFFSET_MASK) != 16) { FAIL("offset mismatch"); return; }
    if ((obj_slot & CHAOS_GC_SLOT_KIND_MASK) != CHAOS_GC_SLOT_KIND_OBJECT) { FAIL("kind mismatch"); return; }

    uint32_t interior_slot = CHAOS_GC_SLOT_ENCODE(32, CHAOS_GC_SLOT_KIND_INTERIOR);
    if ((interior_slot & CHAOS_GC_SLOT_OFFSET_MASK) != 32) { FAIL("interior offset mismatch"); return; }
    if ((interior_slot & CHAOS_GC_SLOT_KIND_MASK) != CHAOS_GC_SLOT_KIND_INTERIOR) { FAIL("interior kind mismatch"); return; }
    PASS();

    SUBTEST("max offset fits in 12 bits");
    uint32_t max_offset = CHAOS_GC_SLOT_ENCODE(4095, CHAOS_GC_SLOT_KIND_OBJECT);
    if ((max_offset & CHAOS_GC_SLOT_OFFSET_MASK) != 4095) { FAIL("max offset encode fail"); return; }
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// GcSlotMap registry
// ════════════════════════════════════════════════════════════════════════════

static void test_registry() {
    TEST("GcSlotMap registry");

    // If no maps registered, lookup returns nullptr.
    SUBTEST("lookup null when empty");
    if (GcLookupSlotMap(reinterpret_cast<const void*>(0x1234)) != nullptr) {
        FAIL("expected null for empty registry");
    }
    PASS();

    // Register two slot maps and verify lookup.
    SUBTEST("register and lookup");
    uint32_t slots_a[] = { CHAOS_GC_SLOT_ENCODE(8, CHAOS_GC_SLOT_KIND_OBJECT) };
    GcSlotMapV0 map_a = { .frame_size = 32, .num_gc_slots = 1 };
    // Can't copy slots[] directly in C++ initializer; use memcpy.
    GcSlotMapV0 map_a_stored;
    std::memcpy(&map_a_stored, &map_a, sizeof(map_a));

    uint32_t slots_b[] = {
        CHAOS_GC_SLOT_ENCODE(8, CHAOS_GC_SLOT_KIND_OBJECT),
        CHAOS_GC_SLOT_ENCODE(24, CHAOS_GC_SLOT_KIND_INTERIOR)
    };

    void* addr_a = reinterpret_cast<void*>(0x1000);
    void* addr_b = reinterpret_cast<void*>(0x2000);

    GcRegisterSlotMap(addr_a, &map_a_stored);
    GcRegisterSlotMap(addr_b, nullptr);  // should no-op

    const GcSlotMapV0* found = GcLookupSlotMap(addr_a);
    if (found == nullptr) { FAIL("map_a not found"); return; }
    if (found->frame_size != 32) { FAIL("map_a frame_size mismatch"); return; }
    if (found->num_gc_slots != 1) { FAIL("map_a num_gc_slots mismatch"); return; }

    found = GcLookupSlotMap(addr_b);
    if (found != nullptr) { FAIL("map_b should not be registered"); return; }

    // Lookup of unregistered address.
    found = GcLookupSlotMap(reinterpret_cast<void*>(0x3000));
    if (found != nullptr) { FAIL("unregistered addr should return null"); return; }

    // Null lookup.
    found = GcLookupSlotMap(nullptr);
    if (found != nullptr) { FAIL("null addr should return null"); return; }

    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Precise frame scanning
// ════════════════════════════════════════════════════════════════════════════

struct ScanResult {
    void*   root_addr;
    bool    is_interior;
    bool    called;
};

static void precise_callback(void* root_addr, bool is_interior, void* user_data) {
    auto* result = static_cast<ScanResult*>(user_data);
    result->root_addr = root_addr;
    result->is_interior = is_interior;
    result->called = true;
}

static void test_precise_scan() {
    TEST("GcScanPreciseFrame");

    // Build a fake stack frame with known GC slots.
    alignas(void*) uint8_t frame_memory[64] = {};
    std::memset(frame_memory, 0, 64);

    // Place a fake pointer at offset 8 (simulating a GC root).
    void* fake_obj = reinterpret_cast<void*>(0xBEEF);
    std::memcpy(frame_memory + 8, &fake_obj, sizeof(void*));

    // Create a slot map describing offset 8 as an object reference.
    uint32_t slots[] = { CHAOS_GC_SLOT_ENCODE(8, CHAOS_GC_SLOT_KIND_OBJECT) };
    GcSlotMapV0 map;
    // Using memcpy to work around C flexible array member limitation.
    // We allocate enough space for header + 1 slot.
    struct SlotMapWithSlots {
        GcSlotMapV0 header;
        uint32_t slots[1];
    };
    SlotMapWithSlots sm = {};
    sm.header.frame_size = 64;
    sm.header.num_gc_slots = 1;
    sm.slots[0] = slots[0];

    ManagedFrameInfo frame = {};
    frame.frame_ptr = frame_memory;
    frame.frame_size = 64;
    frame.return_address = reinterpret_cast<void*>(0x1000);

    ScanResult result = {};
    GcScanPreciseFrame(frame, sm.header, precise_callback, &result);

    if (!result.called) { FAIL("callback not invoked"); return; }
    if (result.root_addr != &frame_memory[8]) { FAIL("wrong root address"); return; }
    if (result.is_interior != false) { FAIL("should not be interior"); return; }

    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Conservative frame scanning
// ════════════════════════════════════════════════════════════════════════════

struct ConservativeResult {
    void* candidate;
    int   count;
};

static void conservative_callback(void* candidate, void* user_data) {
    auto* result = static_cast<ConservativeResult*>(user_data);
    result->candidate = candidate;
    result->count++;
}

static void test_conservative_scan() {
    TEST("GcScanConservativeFrame");

    alignas(void*) uint8_t frame_memory[32] = {};
    std::memset(frame_memory, 0, 32);

    // Place a valid-looking pointer at offset 0.
    void* fake_ptr = reinterpret_cast<void*>(0x1000);  // aligned, non-null
    std::memcpy(frame_memory, &fake_ptr, sizeof(void*));

    // Place a null at offset 8 (should be skipped).
    void* null_ptr = nullptr;
    std::memcpy(frame_memory + 8, &null_ptr, sizeof(void*));

    // Place a misaligned value at offset 16 (should be skipped).
    uintptr_t misaligned = 0x1001;
    std::memcpy(frame_memory + 16, &misaligned, sizeof(uintptr_t));

    ManagedFrameInfo frame = {};
    frame.frame_ptr = frame_memory;
    frame.frame_size = 32;
    frame.return_address = reinterpret_cast<void*>(0x2000);

    ConservativeResult result = {};
    GcScanConservativeFrame(frame, conservative_callback, &result);

    if (result.count != 1) { FAIL("expected 1 conservative root"); return; }
    if (result.candidate != fake_ptr) { FAIL("wrong candidate"); return; }

    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Hybrid scanning
// ════════════════════════════════════════════════════════════════════════════

static void test_hybrid_scan() {
    TEST("GcScanFrameHybrid");

    // Register a slot map at address 0x3000.
    uint32_t slots[] = { CHAOS_GC_SLOT_ENCODE(8, CHAOS_GC_SLOT_KIND_OBJECT) };
    struct SlotMapWithSlots {
        GcSlotMapV0 header;
        uint32_t slots[1];
    };
    static SlotMapWithSlots s_sm = {};
    s_sm.header.frame_size = 64;
    s_sm.header.num_gc_slots = 1;
    s_sm.slots[0] = slots[0];
    GcRegisterSlotMap(reinterpret_cast<void*>(0x3000), &s_sm.header);

    alignas(void*) uint8_t frame_memory[64] = {};
    std::memset(frame_memory, 0, 64);
    void* fake_obj = reinterpret_cast<void*>(0xBEEF);
    std::memcpy(frame_memory + 8, &fake_obj, sizeof(void*));

    SUBTEST("precise path for registered frame");
    ManagedFrameInfo frame = {};
    frame.frame_ptr = frame_memory;
    frame.frame_size = 64;
    frame.return_address = reinterpret_cast<void*>(0x3000);

    ScanResult precise_result = {};
    ConservativeResult cons_result = {};

    GcScanFrameHybrid(frame, precise_callback, conservative_callback, &precise_result);

    if (!precise_result.called) { FAIL("precise callback not invoked"); return; }
    if (precise_result.root_addr != &frame_memory[8]) { FAIL("precise wrong addr"); return; }
    PASS();

    SUBTEST("conservative fallback for unknown frame");
    frame.return_address = reinterpret_cast<void*>(0x4000);
    cons_result.count = 0;

    GcScanFrameHybrid(frame, precise_callback, conservative_callback, &cons_result);

    // Should find the fake_obj pointer at offset 0.
    if (cons_result.count < 1) { FAIL("conservative should find at least 1 root"); return; }
    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Main
// ════════════════════════════════════════════════════════════════════════════

int main() {
    puts("CRAG C1 tests (GcSlotMap + root scanner):");
    puts("══════════════════════════════════════════\n");

    test_slot_map_layout();
    test_registry();
    test_precise_scan();
    test_conservative_scan();
    test_hybrid_scan();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}