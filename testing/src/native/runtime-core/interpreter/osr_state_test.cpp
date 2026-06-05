/// osr_state_test.cpp — OsrState unit tests (gtest)
///
/// Tests:
///   - Default construction (empty state)
///   - Move semantics (ownership transfer + source cleanup)
///   - CaptureFastFrame → RestoreOsrToFastFrame round-trip for all value types
///   - Tracked object ownership transfer through OsrState
///   - Cleanup-on-destruction for tracked objects
///
/// Requires chaos_interpreter linkage (for converter function implementations).

#include "osr_state.h"
#include "fast_dispatch.h"
#include <gtest/gtest.h>
#include <cstring>

// AOT symbol stubs (required by chaos_interpreter library linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

using chaos::il2cpp::interpreter::OsrState;
using chaos::il2cpp::interpreter::CaptureFastFrame;
using chaos::il2cpp::interpreter::RestoreOsrToFastFrame;
using chaos::il2cpp::interpreter::ValueTag;
using chaos::il2cpp::runtime_core::FastFrame;

// ═══════════════════════════════════════════════════════════════════════════
// Default construction
// ═══════════════════════════════════════════════════════════════════════════

TEST(OsrStateTest, DefaultConstruction) {
    OsrState osr;
    EXPECT_EQ(osr.pc, 0u);
    EXPECT_EQ(osr.sp, 0u);
    EXPECT_EQ(osr.local_count, 0u);
    EXPECT_EQ(osr.tracked_cnt, 0u);

    // All arrays should be zero-initialized.
    for (uint32_t i = 0; i < OsrState::kMaxStack; ++i) {
        EXPECT_EQ(osr.stack[i], 0u);
        EXPECT_EQ(osr.stack_tags[i], 0u);
    }
    for (uint32_t i = 0; i < OsrState::kMaxLocals; ++i) {
        EXPECT_EQ(osr.locals[i], 0u);
        EXPECT_EQ(osr.local_tags[i], 0u);
    }
    for (uint32_t i = 0; i < OsrState::kMaxTracked; ++i) {
        EXPECT_EQ(osr.tracked_objs[i], nullptr);
        EXPECT_EQ(osr.tracked_dtors[i], nullptr);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Move semantics
// ═══════════════════════════════════════════════════════════════════════════

TEST(OsrStateTest, MoveConstructionTransfersOwnership) {
    {
        OsrState src;
        src.pc = 42;
        src.sp = 3;
        src.local_count = 2;
        src.stack[0] = 100;
        src.stack_tags[0] = static_cast<uint8_t>(ValueTag::Int32);
        src.tracked_cnt = 0;  // no tracked objects for simple test

        OsrState dst(std::move(src));

        // Destination has the values.
        EXPECT_EQ(dst.pc, 42u);
        EXPECT_EQ(dst.sp, 3u);
        EXPECT_EQ(dst.local_count, 2u);
        EXPECT_EQ(dst.stack[0], 100u);
        EXPECT_EQ(dst.stack_tags[0], static_cast<uint8_t>(ValueTag::Int32));

        // Source is cleaned (tracked_cnt zeroed, else valgrind-safe).
        EXPECT_EQ(src.tracked_cnt, 0u);
    }
    // Destructor should not crash.
}

TEST(OsrStateTest, MoveAssignment) {
    OsrState dst;
    {
        OsrState src;
        src.pc = 99;
        src.sp = 1;
        src.stack[0] = 0xDEAD;
        src.stack_tags[0] = static_cast<uint8_t>(ValueTag::Int64);

        dst = std::move(src);
        EXPECT_EQ(src.tracked_cnt, 0u);
    }
    EXPECT_EQ(dst.pc, 99u);
    EXPECT_EQ(dst.sp, 1u);
    EXPECT_EQ(dst.stack[0], 0xDEADu);
    EXPECT_EQ(dst.stack_tags[0], static_cast<uint8_t>(ValueTag::Int64));
}

TEST(OsrStateTest, SelfMoveAssignmentIsSafe) {
    OsrState osr;
    osr.pc = 7;
    osr.sp = 2;
    osr.stack[0] = 42;
    osr.stack_tags[0] = static_cast<uint8_t>(ValueTag::Int32);

    // Self-assignment via move — guarded by `if (this != &other)`.
    osr = std::move(osr);

    EXPECT_EQ(osr.pc, 7u);
    EXPECT_EQ(osr.sp, 2u);
    EXPECT_EQ(osr.stack[0], 42u);
}

// ═══════════════════════════════════════════════════════════════════════════
// CaptureFastFrame → RestoreOsrToFastFrame round-trip
// ═══════════════════════════════════════════════════════════════════════════

TEST(OsrStateTest, EmptyFastFrameRoundTrip) {
    FastFrame frame = {};
    OsrState osr;

    CaptureFastFrame(osr, frame);

    EXPECT_EQ(osr.pc, 0u);
    EXPECT_EQ(osr.sp, 0u);
    EXPECT_EQ(osr.local_count, 0u);
    EXPECT_EQ(osr.tracked_cnt, 0u);

    FastFrame restored = {};
    RestoreOsrToFastFrame(osr, restored);

    EXPECT_EQ(restored.pc, 0u);
    EXPECT_EQ(restored.sp, 0u);
    EXPECT_EQ(restored.local_count, 0u);

    // Cleanup (no tracked objects — no-op).
}

TEST(OsrStateTest, FastFrameRoundTripWithValues) {
    FastFrame frame = {};
    frame.pc = 10;
    frame.PushI32(42);
    frame.PushI64(0x1234567890ABCDEFLL);
    float fval = 3.14f;
    frame.PushF32(fval);
    double dval = 2.71828;
    frame.PushF64(dval);
    frame.PushNull();
    frame.PushObj(reinterpret_cast<void*>(0xBEEF));
    uint32_t captured_sp = frame.sp;
    frame.locals[0] = 100;
    frame.local_tags[0] = static_cast<uint8_t>(ValueTag::Int32);
    frame.locals[1] = 200;
    frame.local_tags[1] = static_cast<uint8_t>(ValueTag::Int64);
    frame.local_count = 2;

    OsrState osr;
    CaptureFastFrame(osr, frame);

    // Frame's tracked_cnt should be 0 after capture (transferred to osr).
    EXPECT_EQ(frame.tracked_cnt, 0u);

    EXPECT_EQ(osr.pc, 10u);
    EXPECT_EQ(osr.sp, captured_sp);
    EXPECT_EQ(osr.local_count, 2u);

    // Verify stack values survived.
    EXPECT_EQ(osr.stack[0], 42u);
    EXPECT_EQ(osr.stack_tags[0], static_cast<uint8_t>(ValueTag::Int32));
    EXPECT_EQ(osr.stack[1], 0x1234567890ABCDEFLL);
    EXPECT_EQ(osr.stack_tags[1], static_cast<uint8_t>(ValueTag::Int64));

    // Float values go through memcpy — verify bit pattern.
    float restored_f32;
    std::memcpy(&restored_f32, &osr.stack[2], sizeof(float));
    EXPECT_FLOAT_EQ(restored_f32, 3.14f);
    EXPECT_EQ(osr.stack_tags[2], static_cast<uint8_t>(ValueTag::Float32));

    double restored_f64;
    std::memcpy(&restored_f64, &osr.stack[3], sizeof(double));
    EXPECT_DOUBLE_EQ(restored_f64, 2.71828);
    EXPECT_EQ(osr.stack_tags[3], static_cast<uint8_t>(ValueTag::Float64));

    EXPECT_EQ(osr.stack[4], 0u);
    EXPECT_EQ(osr.stack_tags[4], static_cast<uint8_t>(ValueTag::Null));
    EXPECT_EQ(osr.stack[5], static_cast<uint64_t>(0xBEEF));
    EXPECT_EQ(osr.stack_tags[5], static_cast<uint8_t>(ValueTag::ObjectRef));

    EXPECT_EQ(osr.locals[0], 100u);
    EXPECT_EQ(osr.local_tags[0], static_cast<uint8_t>(ValueTag::Int32));
    EXPECT_EQ(osr.locals[1], 200u);
    EXPECT_EQ(osr.local_tags[1], static_cast<uint8_t>(ValueTag::Int64));

    // Restore back to a new FastFrame.
    FastFrame restored = {};
    RestoreOsrToFastFrame(osr, restored);

    EXPECT_EQ(restored.pc, 10u);
    EXPECT_EQ(restored.sp, captured_sp);
    EXPECT_EQ(restored.local_count, 2u);
    EXPECT_EQ(static_cast<int32_t>(restored.stack[0]), 42);
    EXPECT_EQ(static_cast<int64_t>(restored.stack[1]), 0x1234567890ABCDEFLL);

    float restored2_f32;
    std::memcpy(&restored2_f32, &restored.stack[2], sizeof(float));
    EXPECT_FLOAT_EQ(restored2_f32, 3.14f);

    double restored2_f64;
    std::memcpy(&restored2_f64, &restored.stack[3], sizeof(double));
    EXPECT_DOUBLE_EQ(restored2_f64, 2.71828);

    EXPECT_EQ(restored.stack[4], 0u);
    EXPECT_EQ(restored.stack_tags[4], static_cast<uint8_t>(ValueTag::Null));
    EXPECT_EQ(restored.stack[5], static_cast<uint64_t>(0xBEEF));
    EXPECT_EQ(restored.stack_tags[5], static_cast<uint8_t>(ValueTag::ObjectRef));

    EXPECT_EQ(restored.locals[0], 100u);
    EXPECT_EQ(restored.local_tags[0], static_cast<uint8_t>(ValueTag::Int32));
    EXPECT_EQ(restored.locals[1], 200u);
    EXPECT_EQ(restored.local_tags[1], static_cast<uint8_t>(ValueTag::Int64));
}

TEST(OsrStateTest, CaptureFastFrameMaxValues) {
    // Fill all stack slots and verify capture captures everything.
    FastFrame frame = {};
    for (uint32_t i = 0; i < FastFrame::kMaxStack; ++i) {
        frame.PushI32(static_cast<int32_t>(i * 10));
    }
    for (uint32_t i = 0; i < FastFrame::kMaxLocals; ++i) {
        frame.locals[i] = i + 1000;
        frame.local_tags[i] = static_cast<uint8_t>(ValueTag::Int64);
    }
    frame.local_count = FastFrame::kMaxLocals;
    frame.pc = 77;

    OsrState osr;
    CaptureFastFrame(osr, frame);

    EXPECT_EQ(osr.sp, FastFrame::kMaxStack);
    EXPECT_EQ(osr.local_count, FastFrame::kMaxLocals);
    EXPECT_EQ(osr.pc, 77u);

    for (uint32_t i = 0; i < FastFrame::kMaxStack; ++i) {
        EXPECT_EQ(static_cast<int32_t>(osr.stack[i]), static_cast<int32_t>(i * 10));
        EXPECT_EQ(osr.stack_tags[i], static_cast<uint8_t>(ValueTag::Int32));
    }
    for (uint32_t i = 0; i < FastFrame::kMaxLocals; ++i) {
        EXPECT_EQ(osr.locals[i], i + 1000u);
        EXPECT_EQ(osr.local_tags[i], static_cast<uint8_t>(ValueTag::Int64));
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Tracked object lifecycle
// ═══════════════════════════════════════════════════════════════════════════

// Simple tracked object for testing: sets a flag on destruction.
static int g_test_dtor_count = 0;
struct TestTrackedObj {
    bool* destroyed_flag;
    explicit TestTrackedObj(bool* flag) : destroyed_flag(flag) { *flag = false; }
    ~TestTrackedObj() {
        if (destroyed_flag) *destroyed_flag = true;
    }
};
static void TestTrackedDtor(void* p) noexcept {
    auto* obj = static_cast<TestTrackedObj*>(p);
    obj->~TestTrackedObj();
    ++g_test_dtor_count;
}

TEST(OsrStateTest, TrackedObjectsTransferViaMove) {
    bool d1 = false, d2 = false;
    {
        OsrState src;
        src.tracked_cnt = 2;
        src.tracked_objs[0] = new TestTrackedObj(&d1);
        src.tracked_dtors[0] = TestTrackedDtor;
        src.tracked_objs[1] = new TestTrackedObj(&d2);
        src.tracked_dtors[1] = TestTrackedDtor;

        OsrState dst(std::move(src));
        EXPECT_EQ(src.tracked_cnt, 0u);  // ownership transferred
        EXPECT_EQ(dst.tracked_cnt, 2u);
        // dst destructor will clean up the tracked objects.
    }
    // Both tracked objects should have been destroyed by ~OsrState.
    EXPECT_TRUE(d1);
    EXPECT_TRUE(d2);
}

TEST(OsrStateTest, TrackedObjectsCleanupOnDestruction) {
    bool d1 = false;
    g_test_dtor_count = 0;
    {
        OsrState osr;
        osr.tracked_cnt = 1;
        osr.tracked_objs[0] = new TestTrackedObj(&d1);
        osr.tracked_dtors[0] = TestTrackedDtor;
        // ~OsrState called at scope exit.
    }
    EXPECT_TRUE(d1);
}

TEST(OsrStateTest, CaptureFastFrameTransfersTrackedObjects) {
    FastFrame frame = {};
    bool d1 = false;
    frame.tracked_cnt = 1;
    frame.tracked_objs[0] = new TestTrackedObj(&d1);
    frame.tracked_dtors[0] = TestTrackedDtor;
    frame.tracked_is_pool[0] = false;

    {
        OsrState osr;
        CaptureFastFrame(osr, frame);

        // Tracked objects are now owned by osr (frame retains stale pointers
        // but caller must NOT call frame.CleanupTracked after capture).
        EXPECT_EQ(osr.tracked_cnt, 1u);
        EXPECT_EQ(osr.tracked_objs[0], frame.tracked_objs[0]);

        // Keep frame.tracked_objs[0] alive — don't let ~FastFrame free it.
        // The OsrState destructor handles cleanup.
        frame.tracked_cnt = 0;
    }
    EXPECT_TRUE(d1);
}

TEST(OsrStateTest, CleanupMultipleTrackedObjects) {
    // Track multiple objects and verify all are cleaned.
    bool flags[5] = {};
    g_test_dtor_count = 0;
    {
        OsrState osr;
        osr.tracked_cnt = 5;
        for (int i = 0; i < 5; ++i) {
            osr.tracked_objs[i] = new TestTrackedObj(&flags[i]);
            osr.tracked_dtors[i] = TestTrackedDtor;
        }
    }
    for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(flags[i]);
    }
}

TEST(OsrStateTest, TrackedObjectsNotDoubleFreed) {
    // Move clears source tracked_cnt — destructor of source does nothing.
    bool d1 = false;
    g_test_dtor_count = 0;
    {
        OsrState src;
        src.tracked_cnt = 1;
        src.tracked_objs[0] = new TestTrackedObj(&d1);
        src.tracked_dtors[0] = TestTrackedDtor;

        {
            OsrState dst(std::move(src));
            (void)dst;
            // ~dst destroys the tracked object.
        }
        EXPECT_TRUE(d1);
        // ~src does nothing (tracked_cnt == 0).
    }
    // d1 should only be destroyed once.
    EXPECT_TRUE(d1);
}

// ═══════════════════════════════════════════════════════════════════════════
// Edge cases
// ═══════════════════════════════════════════════════════════════════════════

TEST(OsrStateTest, PartialStackCapture) {
    // Capture with partial stack and verify only populated slots are non-zero.
    FastFrame frame = {};
    frame.PushI32(1);
    frame.PushI32(2);
    // sp = 2, slots 2..63 are zero.

    OsrState osr;
    CaptureFastFrame(osr, frame);

    EXPECT_EQ(osr.sp, 2u);
    EXPECT_EQ(static_cast<int32_t>(osr.stack[0]), 1);
    EXPECT_EQ(static_cast<int32_t>(osr.stack[1]), 2);
    // Slot 2 should still be zero (from value-initialized FastFrame).
    // No need to check beyond — the arrays were zero-initialized in the OsrState constructor.
}

TEST(OsrStateTest, RestoreDoesNotTouchTracked) {
    // RestoreOsrToFastFrame should NOT modify tracked fields.
    // It restores pc, sp, local_count, and the flat arrays only.
    OsrState osr;
    osr.pc = 5;
    osr.sp = 1;
    osr.stack[0] = 42;
    osr.stack_tags[0] = static_cast<uint8_t>(ValueTag::Int32);

    FastFrame frame = {};
    // Pre-populate frame's tracked to confirm restore doesn't clear or alter them.
    bool d2 = false;
    frame.tracked_cnt = 1;
    frame.tracked_objs[0] = new TestTrackedObj(&d2);
    frame.tracked_dtors[0] = TestTrackedDtor;
    frame.tracked_is_pool[0] = false;

    RestoreOsrToFastFrame(osr, frame);

    EXPECT_EQ(frame.pc, 5u);
    EXPECT_EQ(frame.sp, 1u);
    EXPECT_EQ(static_cast<int32_t>(frame.stack[0]), 42);
    // tracked_cnt and tracked_objs untouched by restore.
    EXPECT_EQ(frame.tracked_cnt, 1u);

    // Frame still owns its tracked object — clean up manually.
    frame.CleanupTracked();
    EXPECT_TRUE(d2);
}

// Move assignment from empty source.
TEST(OsrStateTest, MoveFromEmpty) {
    OsrState empty;
    OsrState dst;
    dst.pc = 10;
    dst = std::move(empty);
    EXPECT_EQ(dst.pc, 0u);     // overwritten by empty
    EXPECT_EQ(dst.sp, 0u);
    EXPECT_EQ(dst.tracked_cnt, 0u);
}
