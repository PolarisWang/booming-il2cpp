// hc_cpu_feedback_test.cpp — Phase 1 TDD: HC CPU real measurement verification
//
// Verifies that the HillClimbingController's CPU utilization tracking
// uses real (non-heuristic) measurements after the T1-HC-CPU fix.
//
// These tests are standalone — they test CPU utilization computation
// without requiring a running thread pool.

#include <gtest/gtest.h>

#include <thread_pool.h>

#include <chrono>
#include <cstdint>
#include <thread>

namespace threading = chaos::il2cpp::runtime_core::threading;

// ── CPU utilization tracking tests ─────────────────────────────────────

TEST(HC_CpuFeedback, CpuUtilizationStartsAtZero)
{
    threading::HillClimbingController controller;
    controller.Reset();

    // After Reset, utilization should be 0 (no samples yet).
    EXPECT_EQ(controller.GetCpuUtilization(), 0.0f);
}

TEST(HC_CpuFeedback, CpuUtilizationAfterFirstTick)
{
    threading::HillClimbingController controller;
    controller.Reset();

    // First OnGateTick call initializes timestamps but has no delta to compare.
    // Utilization should remain 0 (or near 0) for the first call.
    int32_t threads = controller.OnGateTick(/*completed=*/10, /*current_threads=*/2);
    EXPECT_GE(controller.GetCpuUtilization(), 0.0f);
    EXPECT_LE(controller.GetCpuUtilization(), 1.0f);
    EXPECT_GE(threads, 1);
}

TEST(HC_CpuFeedback, CpuUtilizationReasonableRange)
{
    // After multiple ticks with real work, CPU utilization should be in
    // a reasonable range (0.0–1.0) and non-zero if there was meaningful
    // computation between ticks.
    threading::HillClimbingController controller;
    controller.Reset();

    // Prime the controller with an initial tick (sets wall/cpu baseline).
    controller.OnGateTick(/*completed=*/10, /*current_threads=*/4);

    // Small delay to accumulate measurable CPU time.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Second tick should compute real CPU utilization from the delta.
    // Even if the measured value is very small (process was sleeping),
    // the computation path is exercised.
    int32_t threads = controller.OnGateTick(/*completed=*/10, /*current_threads=*/4);

    float util = controller.GetCpuUtilization();
    EXPECT_GE(util, 0.0f);
    EXPECT_LE(util, 1.0f);
    EXPECT_GE(threads, 1);
}

TEST(HC_CpuFeedback, CpuUtilizationWithBusyWork)
{
    // Perform actual computation between ticks to generate measurable
    // process CPU time, verifying the measurement path works end-to-end.
    threading::HillClimbingController controller;
    controller.Reset();

    // Baseline tick.
    controller.OnGateTick(/*completed=*/10, /*current_threads=*/2);

    // Busy-loop to burn CPU time.
    volatile uint64_t sum = 0;
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds(50)) {
        for (int i = 0; i < 1000; i++) {
            sum += static_cast<uint64_t>(i);
        }
    }
    (void)sum;

    // Tick after busy work — utilization should be measurably > 0.
    int32_t threads = controller.OnGateTick(/*completed=*/20, /*current_threads=*/2);
    float util = controller.GetCpuUtilization();
    EXPECT_GT(util, 0.0f) << "CPU utilization should be > 0 after busy work";
    EXPECT_LE(util, 1.0f);
    EXPECT_GE(threads, 1);
}

TEST(HC_CpuFeedback, CpuUtilizationAfterMultipleTicks)
{
    // Over many ticks, CPU utilization should remain bounded.
    threading::HillClimbingController controller;
    controller.Reset();

    int32_t threads = 4;
    for (int tick = 0; tick < 20; tick++) {
        threads = controller.OnGateTick(/*completed=*/15, threads);
        float util = controller.GetCpuUtilization();
        EXPECT_GE(util, 0.0f);
        EXPECT_LE(util, 1.0f);
    }
}

TEST(HC_CpuFeedback, CpuUtilizationResetClears)
{
    threading::HillClimbingController controller;
    controller.Reset();

    // Prime with a tick.
    controller.OnGateTick(/*completed=*/10, /*current_threads=*/2);

    // Reset should clear CPU utilization to 0.
    controller.Reset();
    EXPECT_EQ(controller.GetCpuUtilization(), 0.0f);

    // After reset, a tick reinitializes (utilization 0 or near 0).
    controller.OnGateTick(/*completed=*/10, /*current_threads=*/2);
    EXPECT_GE(controller.GetCpuUtilization(), 0.0f);
    EXPECT_LE(controller.GetCpuUtilization(), 1.0f);
}

// ── State machine still works correctly with real CPU ──────────────────

TEST(HC_CpuFeedback, StateMachineTransitionsWithCpuFeedback)
{
    // Verify the state machine still transitions through expected states
    // after the real CPU measurement is wired in.
    threading::HillClimbingController controller;
    controller.Reset();

    // Initial state after first tick with work.
    controller.OnGateTick(/*completed=*/10, /*current_threads=*/2);
    EXPECT_EQ(controller.GetState(), threading::HillClimbState::Warmup);
}

TEST(HC_CpuFeedback, CpuUtilizationAndStateConsistency)
{
    // Multiple ticks with moderate work: CPU utilization and state should
    // remain consistent (util in [0,1], state valid enum).
    threading::HillClimbingController controller;
    controller.Reset();

    int32_t threads = 4;
    for (int tick = 0; tick < 50; tick++) {
        int32_t completed = (tick % 10 < 7) ? 30 : 0;
        threads = controller.OnGateTick(completed, threads);

        float util = controller.GetCpuUtilization();
        EXPECT_GE(util, 0.0f);
        EXPECT_LE(util, 1.0f);

        auto state = controller.GetState();
        // Valid state range: Warmup(0) through Starving(9)
        EXPECT_GE(static_cast<int>(state), 0);
        EXPECT_LE(static_cast<int>(state), 9);
    }
}
