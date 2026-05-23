// ── Exception negative test suite ──────────────────────────────────────────
//
// P3-F of the EH industrialization roadmap.  Verifies that the EH
// infrastructure handles error / boundary conditions correctly.
//
// These tests are gated on CHAOS_IL2CPP_EH_CPP_THROW because C++ native
// exceptions are the most portable test mode.  On other EH modes the test
// bodies call GTEST_SKIP().
//
// Test coverage:
//   1. Deeply nested try/catch — nesting depth equivalent to SETJMP's
//      kMaxNestedTry (16) works correctly in CPP_THROW mode.
//   2. Exception propagation through non-EH frames — verify exceptions
//      correctly propagate past intermediate frames that have no try/catch.
//   3. Finally guarantee — finally-like guards execute on both normal
//      and exceptional exit from try blocks.
//   4. Null / sentinel edge cases — boundary values around 0, -1, -2, -3.
//   5. Multiple throws from same try block — first catch, second rethrow;
//      verify state is clean between cycles.

#include <gtest/gtest.h>

// Sentinel constants (kManagedExceptionThreadAbort, etc.) and
// chaos_managed_exception type.
#include <generated_code_compat.h>

// chaos_raise_exception for CPP_THROW mode.
#include <exception_jmp.h>

// make_finally_guard for finally tests.
#include <chaos/finally.h>

#include <string>
#include <vector>

// =========================================================================
// All tests are gated on CHAOS_IL2CPP_EH_CPP_THROW.
// =========================================================================

#if defined(CHAOS_IL2CPP_EH_CPP_THROW)

using chaos::il2cpp::common::make_finally_guard;

// ═════════════════════════════════════════════════════════════════════════
// Group 1: Deeply nested try/catch
// ═════════════════════════════════════════════════════════════════════════

// SETJMP mode has a hard limit of kMaxNestedTry=16.  In CPP_THROW mode
// there is no such limit, but we still test the equivalent depth to verify
// the infrastructure handles it correctly.

static constexpr int kTestNestingDepth = 16;  // matches SETJMP kMaxNestedTry

// Helper: recursively nest N levels of try/catch blocks and verify that
// an exception thrown at the innermost level propagates to the outermost.
// Returns the caught exception value.
static CHAOS_IL2CPP_INTPTR deep_nest_throw(int remaining,
                                           CHAOS_IL2CPP_INTPTR value) {
    if (remaining <= 0) {
        throw chaos_managed_exception{value};
    }

    CHAOS_IL2CPP_INTPTR caught = 0;
    try {
        caught = deep_nest_throw(remaining - 1, value);
    } catch (const chaos_managed_exception& e) {
        caught = e.object_value;
    }
    return caught;
}

// Helper: throw at a specific depth (counted from outermost), propagate
// upward, and verify the catcher at the target depth receives the value.
// Returns true if the exception was caught at exactly the expected depth.
static bool deep_nest_catch_at_depth(int total_depth, int throw_depth,
                                     CHAOS_IL2CPP_INTPTR value,
                                     CHAOS_IL2CPP_INTPTR* out_caught) {
    if (total_depth <= 0) {
        // Should not reach here — exception should have been caught earlier.
        return false;
    }

    if (total_depth == throw_depth) {
        // This is the frame that should throw.
        throw chaos_managed_exception{value};
    }

    try {
        return deep_nest_catch_at_depth(total_depth - 1, throw_depth,
                                        value, out_caught);
    } catch (const chaos_managed_exception& e) {
        // If throw_depth was deeper (higher number) than current,
        // the exception propagates up to the correct depth.
        *out_caught = e.object_value;
        return (total_depth <= throw_depth);
    }
}

TEST(ExceptionNegative, DeepNestingAtFullDepth) {
    // 16 levels of nesting (matching SETJMP kMaxNestedTry).
    // The exception thrown at the innermost level propagates through
    // all intermediate handlers and is caught by the outermost.
    CHAOS_IL2CPP_INTPTR caught =
        deep_nest_throw(kTestNestingDepth, 42);
    EXPECT_EQ(caught, 42);
}

TEST(ExceptionNegative, DeepNestingCatchAtExactDepth) {
    // Throw at depth 8 of 16 deep nesting.  Verify the exception is
    // caught at the correct handler (level 8) and does not leak to
    // a different level.
    CHAOS_IL2CPP_INTPTR caught = 0;
    bool ok = deep_nest_catch_at_depth(
        kTestNestingDepth, /*throw_depth=*/8, 77, &caught);
    EXPECT_TRUE(ok);
    EXPECT_EQ(caught, 77);
}

TEST(ExceptionNegative, DeepNestingCatchAtOutermost) {
    // Throw at the outermost level (depth 1) — should be caught by the
    // very first handler.
    CHAOS_IL2CPP_INTPTR caught = 0;
    bool ok = deep_nest_catch_at_depth(
        kTestNestingDepth, /*throw_depth=*/16, -5, &caught);
    EXPECT_TRUE(ok);
    EXPECT_EQ(caught, -5);
}

TEST(ExceptionNegative, DeepNestingEmptyCatchDoesNotInterfere) {
    // Nest 5 levels with all catches doing nothing but "handling" the
    // exception (i.e., swallowing it at level 3).  Verify the exception
    // does not escape to the outermost handler when swallowed early.
    CHAOS_IL2CPP_INTPTR outer_caught = 0;
    bool outer_reached = false;

    try {
        // Level 5
        try {
            // Level 4
            try {
                // Level 3 — throw here
                throw chaos_managed_exception{100};
            } catch (const chaos_managed_exception&) {
                // Level 3 handler: swallow the exception.
                // Execution continues normally after this block.
            }
            // Level 4 — should NOT be reached (exception was swallowed).
        } catch (const chaos_managed_exception& e) {
            outer_caught = e.object_value;
        }
        // Level 5 — should NOT be reached either.
    } catch (const chaos_managed_exception& e) {
        outer_reached = true;
        outer_caught = e.object_value;
    }

    EXPECT_FALSE(outer_reached);
    EXPECT_EQ(outer_caught, 0);
}

// ═════════════════════════════════════════════════════════════════════════
// Group 2: Exception propagation through non-EH frames
// ═════════════════════════════════════════════════════════════════════════

// Helper: a plain function with no try/catch at all.
[[maybe_unused]] static void plain_thrower(CHAOS_IL2CPP_INTPTR val) {
    throw chaos_managed_exception{val};
}

// Helper: intermediate frame that just calls the thrower — no EH.
[[maybe_unused]] static void intermediate_frame_no_eh(CHAOS_IL2CPP_INTPTR val) {
    plain_thrower(val);
}

// Helper: chain of 3 intermediate frames, none with EH.
[[maybe_unused]] static void chain_level_3(CHAOS_IL2CPP_INTPTR val) {
    intermediate_frame_no_eh(val);
}

[[maybe_unused]] static void chain_level_2(CHAOS_IL2CPP_INTPTR val) {
    chain_level_3(val);
}

[[maybe_unused]] static void chain_level_1(CHAOS_IL2CPP_INTPTR val) {
    chain_level_2(val);
}

TEST(ExceptionNegative, PropagationThroughSingleNonEHFrame) {
    // Exception thrown in plain_thrower propagates through
    // intermediate_frame_no_eh (no try/catch) and is caught by the
    // outer handler.
    bool caught = false;
    CHAOS_IL2CPP_INTPTR value = 0;

    try {
        intermediate_frame_no_eh(123);
    } catch (const chaos_managed_exception& e) {
        caught = true;
        value = e.object_value;
    }

    EXPECT_TRUE(caught);
    EXPECT_EQ(value, 123);
}

TEST(ExceptionNegative, PropagationThroughMultipleNonEHFrames) {
    // Exception propagates through 3 levels of non-EH intermediate
    // frames and is caught at the outermost level.
    bool caught = false;
    CHAOS_IL2CPP_INTPTR value = 0;

    try {
        chain_level_1(456);
    } catch (const chaos_managed_exception& e) {
        caught = true;
        value = e.object_value;
    }

    EXPECT_TRUE(caught);
    EXPECT_EQ(value, 456);
}

TEST(ExceptionNegative, PropagationMixedEHAndNonEH) {
    // chain: EH-frame → non-EH → non-EH (throws) → caught at outermost EH.
    // Intermediate non-EH frames must not absorb or alter the exception.
    bool caught = false;
    CHAOS_IL2CPP_INTPTR value = 0;

    try {
        // Non-EH chain that eventually calls plain_thrower.
        chain_level_1(789);
    } catch (const chaos_managed_exception& e) {
        caught = true;
        value = e.object_value;
    }

    EXPECT_TRUE(caught);
    EXPECT_EQ(value, 789);
}

// ═════════════════════════════════════════════════════════════════════════
// Group 3: Finally guarantee
// ═════════════════════════════════════════════════════════════════════════

TEST(ExceptionNegative, FinallyExecutesOnNormalExit) {
    // The finally-like guard MUST execute when the try block exits
    // normally (via return/ leaving scope).
    bool finally_ran = false;

    {
        auto guard = make_finally_guard([&finally_ran]() {
            finally_ran = true;
        });
        // Normal scope exit — guard destructor runs.
    }

    EXPECT_TRUE(finally_ran);
}

TEST(ExceptionNegative, FinallyExecutesOnExceptionExit) {
    // The finally-like guard MUST execute when the try block exits
    // via an exception.  After the guard runs, the exception
    // continues to propagate.
    bool finally_ran = false;

    try {
        auto guard = make_finally_guard([&finally_ran]() {
            finally_ran = true;
        });
        throw chaos_managed_exception{kManagedExceptionNormal};
    } catch (const chaos_managed_exception&) {
        // Exception caught — finally must have already executed.
    }

    EXPECT_TRUE(finally_ran);
}

TEST(ExceptionNegative, FinallyExecutesInReverseOrder) {
    // Multiple finally-like guards must execute in LIFO (reverse of
    // construction) order, both on normal and exceptional exit.
    std::vector<int> order;

    try {
        auto guard1 = make_finally_guard([&order]() {
            order.push_back(1);
        });
        auto guard2 = make_finally_guard([&order]() {
            order.push_back(2);
        });
        auto guard3 = make_finally_guard([&order]() {
            order.push_back(3);
        });
        throw chaos_managed_exception{0};
    } catch (const chaos_managed_exception&) {
        // Expected.
    }

    ASSERT_EQ(order.size(), 3u);
    EXPECT_EQ(order[0], 3);  // guard3 destructor first
    EXPECT_EQ(order[1], 2);  // guard2 second
    EXPECT_EQ(order[2], 1);  // guard1 last
}

// ═════════════════════════════════════════════════════════════════════════
// Group 4: Null / sentinel edge cases
// ═════════════════════════════════════════════════════════════════════════

TEST(ExceptionNegative, NullObjectValuePropagation) {
    // Throwing chaos_managed_exception with object_value=0 should be
    // catchable and the value preserved.
    bool caught = false;
    CHAOS_IL2CPP_INTPTR value = 1;  // initially non-zero

    try {
        throw chaos_managed_exception{kManagedExceptionNormal};  // 0
    } catch (const chaos_managed_exception& e) {
        caught = true;
        value = e.object_value;
    }

    EXPECT_TRUE(caught);
    EXPECT_EQ(value, 0);
}

TEST(ExceptionNegative, ThreadAbortSentinelPropagation) {
    // Sentinel -1 must propagate past a typed catch via the sentinel
    // guard check (object_value < 0).
    bool inner_typed_catch = false;
    bool outer_caught = false;
    CHAOS_IL2CPP_INTPTR outer_value = 0;

    try {
        try {
            throw chaos_managed_exception{kManagedExceptionThreadAbort};
        } catch (const chaos_managed_exception& e) {
            if (e.object_value < 0) {
                throw;  // sentinel guard: re-raise
            }
            inner_typed_catch = true;
        }
    } catch (const chaos_managed_exception& e) {
        outer_caught = true;
        outer_value = e.object_value;
    }

    EXPECT_FALSE(inner_typed_catch);
    EXPECT_TRUE(outer_caught);
    EXPECT_EQ(outer_value, kManagedExceptionThreadAbort);
}

TEST(ExceptionNegative, ThreadInterruptSentinelPropagation) {
    // Sentinel -2 must propagate past a typed catch (same guard logic as -1).
    bool inner_typed_catch = false;
    bool outer_caught = false;
    CHAOS_IL2CPP_INTPTR outer_value = 0;

    try {
        try {
            throw chaos_managed_exception{kManagedExceptionThreadInterrupt};
        } catch (const chaos_managed_exception& e) {
            if (e.object_value < 0) {
                throw;  // sentinel guard: re-raise
            }
            inner_typed_catch = true;
        }
    } catch (const chaos_managed_exception& e) {
        outer_caught = true;
        outer_value = e.object_value;
    }

    EXPECT_FALSE(inner_typed_catch);
    EXPECT_TRUE(outer_caught);
    EXPECT_EQ(outer_value, kManagedExceptionThreadInterrupt);
}

TEST(ExceptionNegative, ComFailureSentinelPropagation) {
    // Sentinel -3 must propagate past a typed catch (same guard logic).
    bool inner_typed_catch = false;
    bool outer_caught = false;
    CHAOS_IL2CPP_INTPTR outer_value = 0;

    try {
        try {
            throw chaos_managed_exception{kManagedExceptionComFailure};
        } catch (const chaos_managed_exception& e) {
            if (e.object_value < 0) {
                throw;  // sentinel guard: re-raise
            }
            inner_typed_catch = true;
        }
    } catch (const chaos_managed_exception& e) {
        outer_caught = true;
        outer_value = e.object_value;
    }

    EXPECT_FALSE(inner_typed_catch);
    EXPECT_TRUE(outer_caught);
    EXPECT_EQ(outer_value, kManagedExceptionComFailure);
}

TEST(ExceptionNegative, AllSentinelBoundaryValuesRoundTrip) {
    // All sentinel boundary values (-3, -2, -1, 0) plus a positive
    // value must round-trip correctly through throw/catch.
    auto test_round_trip = [](CHAOS_IL2CPP_INTPTR val) -> CHAOS_IL2CPP_INTPTR {
        try {
            throw chaos_managed_exception{val};
        } catch (const chaos_managed_exception& e) {
            return e.object_value;
        }
        return -999;
    };

    EXPECT_EQ(test_round_trip(kManagedExceptionComFailure),      -3);
    EXPECT_EQ(test_round_trip(kManagedExceptionThreadInterrupt), -2);
    EXPECT_EQ(test_round_trip(kManagedExceptionThreadAbort),     -1);
    EXPECT_EQ(test_round_trip(kManagedExceptionNormal),           0);
    EXPECT_EQ(test_round_trip(42),                               42);
}

// ═════════════════════════════════════════════════════════════════════════
// Group 5: Multiple throws / rethrow from same try block
// ═════════════════════════════════════════════════════════════════════════

TEST(ExceptionNegative, MultipleThrowsFromSameTryCatchRethrow) {
    // Throw once, catch in inner handler, then throw a DIFFERENT
    // exception from the same inner handler — captured by the outer.
    bool first_catch_ok = false;
    bool second_catch_ok = false;
    CHAOS_IL2CPP_INTPTR first_val = 0;
    CHAOS_IL2CPP_INTPTR second_val = 0;

    try {
        try {
            throw chaos_managed_exception{111};
        } catch (const chaos_managed_exception& e) {
            first_catch_ok = true;
            first_val = e.object_value;
            throw chaos_managed_exception{222};  // new throw from catch
        }
    } catch (const chaos_managed_exception& e) {
        second_catch_ok = true;
        second_val = e.object_value;
    }

    EXPECT_TRUE(first_catch_ok);
    EXPECT_EQ(first_val, 111);
    EXPECT_TRUE(second_catch_ok);
    EXPECT_EQ(second_val, 222);
}

TEST(ExceptionNegative, StateIsCleanAfterFirstCatch) {
    // After catching an exception inside a try block, the state must be
    // clean: subsequent throws from outside the try block should be caught
    // normally.
    bool first_caught = false;
    bool second_caught = false;
    CHAOS_IL2CPP_INTPTR first_val = 0;
    CHAOS_IL2CPP_INTPTR second_val = 0;

    // First throw/catch
    try {
        throw chaos_managed_exception{100};
    } catch (const chaos_managed_exception& e) {
        first_caught = true;
        first_val = e.object_value;
    }

    EXPECT_TRUE(first_caught);
    EXPECT_EQ(first_val, 100);

    // Second throw/catch — independent, should work identically
    try {
        throw chaos_managed_exception{200};
    } catch (const chaos_managed_exception& e) {
        second_caught = true;
        second_val = e.object_value;
    }

    EXPECT_TRUE(second_caught);
    EXPECT_EQ(second_val, 200);
}

TEST(ExceptionNegative, ChaosaiseExceptionRoundTrip) {
    // chaos_raise_exception must throw a chaos_managed_exception that
    // carries the correct object_value.
    bool caught = false;
    CHAOS_IL2CPP_INTPTR value = 0;

    try {
        chaos::il2cpp::runtime_core::chaos_raise_exception(99);
    } catch (const chaos_managed_exception& e) {
        caught = true;
        value = e.object_value;
    }

    EXPECT_TRUE(caught);
    EXPECT_EQ(value, 99);
}

#else  // !CHAOS_IL2CPP_EH_CPP_THROW

// ── Stub bodies for non-CPP-THROW builds ─────────────────────────────────

TEST(ExceptionNegative, DeepNestingAtFullDepth) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, DeepNestingCatchAtExactDepth) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, DeepNestingCatchAtOutermost) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, DeepNestingEmptyCatchDoesNotInterfere) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, PropagationThroughSingleNonEHFrame) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, PropagationThroughMultipleNonEHFrames) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, PropagationMixedEHAndNonEH) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, FinallyExecutesOnNormalExit) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, FinallyExecutesOnExceptionExit) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, FinallyExecutesInReverseOrder) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, NullObjectValuePropagation) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, ThreadAbortSentinelPropagation) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, ThreadInterruptSentinelPropagation) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, ComFailureSentinelPropagation) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, AllSentinelBoundaryValuesRoundTrip) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, MultipleThrowsFromSameTryCatchRethrow) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, StateIsCleanAfterFirstCatch) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}
TEST(ExceptionNegative, ChaosaiseExceptionRoundTrip) {
    GTEST_SKIP() << "Requires CHAOS_IL2CPP_EH_CPP_THROW";
}

#endif  // CHAOS_IL2CPP_EH_CPP_THROW
