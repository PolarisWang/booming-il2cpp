// dbg_stepping_test.cpp — Stepping controller unit tests
//
// Tests DbgBeginStepping / DbgEndStepping / DbgIsStepping / DbgGetStepMode /
// DbgGetStepTargetDepth / DbgShouldStopAtCurrentPosition for all three
// stepping modes (StepInto, StepOver, StepOut).
// Links chaos_debugger for the stepping implementation.

#include <gtest/gtest.h>
#include <debugger/dbg_stepping.h>

namespace chaos::il2cpp::diagnostics {
namespace {

class SteppingTest : public ::testing::Test {
protected:
    void SetUp() override {
        DbgSteppingInitialize();
    }
    void TearDown() override {
        DbgSteppingShutdown();
    }
};

TEST_F(SteppingTest, InitiallyNotStepping) {
    EXPECT_FALSE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::None);
}

TEST_F(SteppingTest, StepInto) {
    DbgBeginStepping(1, StepMode::StepInto, 0);
    EXPECT_TRUE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepInto);
}

TEST_F(SteppingTest, StepOver) {
    DbgBeginStepping(1, StepMode::StepOver, 5);
    EXPECT_TRUE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOver);
    EXPECT_EQ(DbgGetStepTargetDepth(), 5);
}

TEST_F(SteppingTest, StepOut) {
    DbgBeginStepping(1, StepMode::StepOut, 3);
    EXPECT_TRUE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOut);
    EXPECT_EQ(DbgGetStepTargetDepth(), 3);
}

TEST_F(SteppingTest, EndStepping) {
    DbgBeginStepping(1, StepMode::StepInto, 0);
    EXPECT_TRUE(DbgIsStepping());

    DbgEndStepping();
    EXPECT_FALSE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::None);
}

TEST_F(SteppingTest, StepIntoAlwaysStops) {
    DbgBeginStepping(1, StepMode::StepInto, 0);
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(0));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(1));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(10));
}

TEST_F(SteppingTest, StepOverStopAtOrAboveTarget) {
    DbgBeginStepping(1, StepMode::StepOver, 5);

    // Below target depth: don't stop.
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(6));
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(10));

    // At or above target depth: stop.
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(5));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(4));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(0));
}

TEST_F(SteppingTest, StepOutStopAboveTarget) {
    DbgBeginStepping(1, StepMode::StepOut, 5);

    // At or below target depth: don't stop.
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(6));
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(5));

    // Above target depth (returned to caller): stop.
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(4));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(0));
}

TEST_F(SteppingTest, NotSteppingNeverStops) {
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(0));
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(5));
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(100));
}

TEST_F(SteppingTest, ThreadIdIsolation) {
    DbgBeginStepping(42, StepMode::StepInto, 0);
    EXPECT_TRUE(DbgIsStepping());

    DbgEndStepping();
    EXPECT_FALSE(DbgIsStepping());

    // Re-begin with different thread.
    DbgBeginStepping(99, StepMode::StepOver, 3);
    EXPECT_EQ(DbgGetStepTargetDepth(), 3);
}

TEST_F(SteppingTest, DoubleShutdownIsSafe) {
    DbgSteppingShutdown();  // second call in same test
    DbgSteppingInitialize();
    DbgBeginStepping(1, StepMode::StepInto, 0);
    EXPECT_TRUE(DbgIsStepping());
}

TEST_F(SteppingTest, StepOverAtBoundaryValues) {
    // At depth 0 with target 0.
    DbgBeginStepping(1, StepMode::StepOver, 0);
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(0));    // 0 <= 0 → stop
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(1));   // 1 <= 0 → false (deeper than target)
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(2));   // 2 <= 0 → false
}

TEST_F(SteppingTest, StepOutFromDepth0) {
    // Step out from depth 0 — stop when strictly above target (depth < 0).
    // At depth 0: not strictly above, so don't stop.
    // At negative depths: strictly above, so stop.
    DbgBeginStepping(1, StepMode::StepOut, 0);
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(0));    // 0 < 0 → false (not yet returned)
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(-1));    // -1 < 0 → true (returned to caller)
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(1));    // 1 < 0 → false (deeper)
}

TEST_F(SteppingTest, StepIntoAfterEnd) {
    DbgBeginStepping(1, StepMode::StepInto, 0);
    DbgEndStepping();
    EXPECT_FALSE(DbgIsStepping());

    // Re-begin stepping.
    DbgBeginStepping(2, StepMode::StepOver, 3);
    EXPECT_TRUE(DbgIsStepping());
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOver);
    EXPECT_EQ(DbgGetStepTargetDepth(), 3);
}

TEST_F(SteppingTest, StepIntoAlwaysStopsEvenAtNegativeDepth) {
    DbgBeginStepping(1, StepMode::StepInto, 0);
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(-1));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(-100));
}

TEST_F(SteppingTest, StepOverDeepStack) {
    // Step over from frame depth 100.
    DbgBeginStepping(1, StepMode::StepOver, 100);
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(101));
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(200));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(100));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(99));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(0));
}

TEST_F(SteppingTest, StepOutDeepStack) {
    // Step out from frame depth 100.
    DbgBeginStepping(1, StepMode::StepOut, 100);
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(101));
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(100));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(99));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(0));
}

TEST_F(SteppingTest, StepModeTransitions) {
    // Simulate: stepIn -> stepOver -> stepOut -> end
    DbgBeginStepping(1, StepMode::StepInto, 0);
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepInto);

    // Transition to stepOver (like changing mind mid-step).
    DbgBeginStepping(1, StepMode::StepOver, 5);
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOver);
    EXPECT_EQ(DbgGetStepTargetDepth(), 5);

    // Transition to stepOut.
    DbgBeginStepping(1, StepMode::StepOut, 5);
    EXPECT_EQ(DbgGetStepMode(), StepMode::StepOut);

    DbgEndStepping();
    EXPECT_EQ(DbgGetStepMode(), StepMode::None);
}

TEST_F(SteppingTest, EndSteppingIdempotent) {
    DbgEndStepping();
    DbgEndStepping();
    DbgEndStepping();
    EXPECT_FALSE(DbgIsStepping());
}

TEST_F(SteppingTest, NegativeFrameDepthStepOver) {
    DbgBeginStepping(1, StepMode::StepOver, 3);
    // Negative frame depth is "above" target 3 (returned to a caller further up).
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(-5));    // -5 <= 3 → stop
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(-1));    // -1 <= 3 → stop
    // At zero — also above target, should stop.
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(0));     // 0 <= 3 → stop
}

TEST_F(SteppingTest, StepOverNegativeTarget) {
    DbgBeginStepping(1, StepMode::StepOver, -5);
    // target depth is -5 — positive depths are deeper, not above.
    // 0 <= -5 is false, so should NOT stop.
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(0));
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(10));
    EXPECT_FALSE(DbgShouldStopAtCurrentPosition(100));
    // Only depths at or below -5 should stop.
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(-5));
    EXPECT_TRUE(DbgShouldStopAtCurrentPosition(-10));
}


}  // namespace
}  // namespace chaos::il2cpp::diagnostics