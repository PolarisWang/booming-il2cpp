// hill_climbing_smoke_test.cpp — HillClimbing V2 tests
//
// Goertzel filter unit tests (frequency-domain energy detection)
// and HillClimbingController state machine transition tests.
//
// These tests are standalone — they test the algorithms directly without
// requiring a running thread pool.

#include <gtest/gtest.h>

#include <thread_pool.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <random>
#include <vector>

namespace threading = chaos::il2cpp::runtime_core::threading;

// ── Goertzel filter unit tests ──────────────────────────────────────────

TEST(HillClimbingTest, GoertzelInitZeroState)
{
    threading::GoertzelFilter filter;
    filter.Init(0.125f);  // normalized freq = 1/8 (one cycle over 8 samples)

    EXPECT_EQ(filter.s1, 0.0f);
    EXPECT_EQ(filter.s2, 0.0f);
    EXPECT_EQ(filter.sample_count, 0u);

    // coeff = 2*cos(2*pi*0.125) ≈ 1.4142
    EXPECT_NEAR(filter.coeff, 1.41421356f, 0.001f);
}

TEST(HillClimbingTest, GoertzelPowerBelowTwoSamples)
{
    threading::GoertzelFilter filter;
    filter.Init(0.125f);

    // 0 samples → power = 0
    EXPECT_EQ(filter.Power(), 0.0f);

    // 1 sample → power = 0
    filter.Feed(1.0f);
    EXPECT_EQ(filter.Power(), 0.0f);
}

TEST(HillClimbingTest, GoertzelPowerWithConstantDC)
{
    // DC input (all same value) should produce near-zero power at non-DC frequency.
    threading::GoertzelFilter filter;
    filter.Init(0.125f);

    for (int i = 0; i < 8; i++) {
        filter.Feed(1.0f);
    }

    float power = filter.Power();
    EXPECT_LT(power, 0.1f);
}

TEST(HillClimbingTest, GoertzelPowerAtTargetFrequency)
{
    // Feed a sine wave at exactly the target frequency (1 cycle over 8 samples).
    // Power should be significantly higher than for DC or off-frequency input.
    threading::GoertzelFilter filter;
    filter.Init(0.125f);  // target freq = 1/8

    for (int i = 0; i < 8; i++) {
        float sample = sinf(2.0f * 3.14159265f * 0.125f * static_cast<float>(i));
        filter.Feed(sample);
    }

    float power = filter.Power();
    EXPECT_GT(power, 0.5f);  // Strong response at target frequency
}

TEST(HillClimbingTest, GoertzelPowerOffFrequency)
{
    // Feed a sine wave at a different frequency (2 cycles over 8 samples).
    // Power at the target frequency should be lower.
    threading::GoertzelFilter filter;
    filter.Init(0.125f);  // target freq = 1/8

    for (int i = 0; i < 8; i++) {
        // 2 cycles over 8 samples = normalized freq 0.25
        float sample = sinf(2.0f * 3.14159265f * 0.25f * static_cast<float>(i));
        filter.Feed(sample);
    }

    float power = filter.Power();
    EXPECT_LT(power, 0.5f);  // Weaker response at non-target frequency
}

TEST(HillClimbingTest, GoertzelTargetStrongerThanOff)
{
    // The power at the target frequency should be higher than at off-frequency.
    threading::GoertzelFilter target_filter;
    threading::GoertzelFilter off_filter;
    target_filter.Init(0.125f);
    off_filter.Init(0.125f);

    for (int i = 0; i < 8; i++) {
        float t = 2.0f * 3.14159265f * 0.125f * static_cast<float>(i);
        float o = 2.0f * 3.14159265f * 0.25f * static_cast<float>(i);
        target_filter.Feed(sinf(t));
        off_filter.Feed(sinf(o));
    }

    EXPECT_GT(target_filter.Power(), off_filter.Power());
}

TEST(HillClimbingTest, GoertzelResetClearsState)
{
    threading::GoertzelFilter filter;
    filter.Init(0.125f);

    // Feed sine wave at target frequency to build non-zero power.
    for (int i = 0; i < 8; i++) {
        float sample = sinf(2.0f * 3.14159265f * 0.125f * static_cast<float>(i));
        filter.Feed(sample);
    }
    ASSERT_GT(filter.Power(), 0.1f);  // Should have measurable energy now.

    filter.Reset();

    EXPECT_EQ(filter.s1, 0.0f);
    EXPECT_EQ(filter.s2, 0.0f);
    EXPECT_EQ(filter.sample_count, 0u);
    EXPECT_EQ(filter.Power(), 0.0f);
}

TEST(HillClimbingTest, GoertzelMultipleWindows)
{
    // Running the filter across multiple windows should still detect the
    // target frequency. Test with 3 full windows (24 samples).
    threading::GoertzelFilter filter;
    filter.Init(0.125f);

    for (int i = 0; i < 24; i++) {
        float sample = sinf(2.0f * 3.14159265f * 0.125f * static_cast<float>(i));
        filter.Feed(sample);
    }

    float power = filter.Power();
    EXPECT_GT(power, 0.5f);
}

TEST(HillClimbingTest, GoertzelRandomNoiseLowPower)
{
    // Random noise should produce low power at any specific frequency.
    threading::GoertzelFilter filter;
    filter.Init(0.125f);

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int i = 0; i < 8; i++) {
        filter.Feed(dist(rng));
    }

    float power = filter.Power();
    // Random noise power should be well below a clean sine at target freq.
    EXPECT_LT(power, 0.5f);
}

// ── Sigmoid gain tests ─────────────────────────────────────────────────

TEST(HillClimbingTest, SigmoidGainNearCPUCount)
{
    // The HillClimbingController uses SigmoidGain to reduce gain near CPU count.
    threading::HillClimbingController controller;

    // Set cpu_count_ directly to simulate an 8-core machine.
    // (All fields are public for testability.)
    controller.cpu_count_ = 8;

    // Far below CPU count: gain ≈ 1.0  (x = (1-8)/2 = -3.5, gain = 1/(1+exp(-3.5)) ≈ 0.97)
    float gain_far = controller.SigmoidGain(1);

    // At CPU count: gain ≈ 0.5  (x = (8-8)/2 = 0, gain = 0.5)
    float gain_at = controller.SigmoidGain(8);

    // Above CPU count: gain < 0.5  (x = (12-8)/2 = 2, gain = 1/(1+exp(2)) ≈ 0.12)
    float gain_above = controller.SigmoidGain(12);

    EXPECT_GT(gain_far, 0.8f);
    EXPECT_NEAR(gain_at, 0.5f, 0.15f);
    EXPECT_LT(gain_above, 0.5f);
}

// ── HillClimbingController state machine tests ─────────────────────────

TEST(HillClimbingTest, ControllerWarmupRampsUp)
{
    // When current_threads < min+2 and there's work, Warmup should add threads.
    threading::HillClimbingController controller;
    controller.Reset();

    int32_t suggestion = controller.OnGateTick(/*completed=*/10, /*threads=*/2);
    // With min_workers=2 and current_threads=2, warmed up, should remain or ramp.
    // The exact suggestion depends on internal state, but should not decrease.
    EXPECT_GE(suggestion, 2);
}

TEST(HillClimbingTest, ControllerIdleShrinks)
{
    // When there's no work and threads > min, should eventually shrink.
    threading::HillClimbingController controller;
    controller.Reset();

    // Multiple ticks with work to get past Warmup and build throughput history.
    for (int i = 0; i < 20; i++) {
        controller.OnGateTick(/*completed=*/10, /*threads=*/4);
    }

    // Then sustained idle — controller should eventually suggest reducing.
    int32_t suggestion = 4;
    bool seen_shrink = false;
    for (int i = 0; i < 50; i++) {
        suggestion = controller.OnGateTick(/*completed=*/0, suggestion);
        if (suggestion < 4) {
            seen_shrink = true;
            break;
        }
    }

    EXPECT_TRUE(seen_shrink) << "Controller never suggested shrinking after 50 idle ticks";
}

TEST(HillClimbingTest, ControllerResetClearsState)
{
    threading::HillClimbingController controller;
    controller.Reset();

    // After reset, state should be Warmup.
    controller.OnGateTick(10, 2);

    // Reset again.
    controller.Reset();

    // State should be back to initial.
    int32_t suggestion = controller.OnGateTick(10, 2);
    EXPECT_GE(suggestion, 2);
}

TEST(HillClimbingTest, ControllerStateTransitions)
{
    // This test exercises the state machine through multiple ticks to
    // verify it doesn't crash or return invalid values.
    // The controller may suggest values below the pool minimum (caller enforces
    // the floor), so we accept >= 0 as valid suggestions.
    threading::HillClimbingController controller;
    controller.Reset();

    int32_t threads = 2;
    for (int tick = 0; tick < 100; tick++) {
        // Vary completed count to exercise different states.
        int32_t completed = (tick % 20 < 10) ? 20 + tick : 0;
        threads = controller.OnGateTick(completed, threads);

        // Suggestion must be non-negative.
        EXPECT_GE(threads, 0);
        EXPECT_LE(threads, 64);

        // Cap threads for next tick (controller doesn't create threads,
        // it only suggests — we simulate the pool following suggestions).
        if (threads > 8) threads = 8;
        if (threads < 2) threads = 2;
    }
}

TEST(HillClimbingTest, ControllerGoertzelIntegration)
{
    // With oscillating throughput, the Goertzel filter should detect the
    // oscillation frequency and the controller should respond.
    threading::HillClimbingController controller;
    controller.Reset();

    // Feed a pattern with a clear oscillation: 5 ticks high, 5 ticks low.
    int32_t threads = 4;
    for (int cycle = 0; cycle < 10; cycle++) {
        for (int tick = 0; tick < 5; tick++) {
            threads = controller.OnGateTick(/*completed=*/50, threads);
        }
        for (int tick = 0; tick < 5; tick++) {
            threads = controller.OnGateTick(/*completed=*/5, threads);
        }
    }

    // Should not crash, threads should be within bounds.
    EXPECT_GE(threads, 2);
    EXPECT_LE(threads, 8);
}

