# Interpreter subsystem tests
# Each test links chaos_interpreter + supporting chaos libraries.
# Note: with STATIC IMPORTED targets, ALL transitive library dependencies must
# be listed explicitly (CMake does not auto-propagate IMPORTED target deps).
# The canonical order is: specific libs first, common support libs last.

# Common set of libraries needed by any test that uses chaos_interpreter or
# chaos_runtime_core (includes transitive deps for fmt, method_replacement, etc.)
# Tests that don't use chaos_bootstrap directly (but runtime_core references it)
add_chaos_test(test_interpreter_smoke
    interpreter_smoke_test.cpp
    LABELS "unit;interpreter"
)

add_chaos_test(test_interpreter_mini_smoke
    interpreter_mini_smoke_test.cpp
    LABELS "unit;interpreter"
)

add_chaos_test(test_interpreter_smoke_isolated
    interpreter_smoke_isolated_test.cpp
    LABELS "unit;interpreter"
)

add_chaos_test(test_interpreter_integration
    interpreter_integration_test.cpp
    LABELS "unit;interpreter"
)

add_chaos_test(test_interpreter_bridge
    interpreter_bridge_test.cpp
    LABELS "unit;interpreter"
)

add_chaos_test(test_interpreter_entry
    interpreter_entry_test.cpp
    LABELS "unit;interpreter"
)

add_chaos_test(test_interpreter_new_features
    interpreter_new_features_test.cpp
    LABELS "unit;interpreter"
)

add_chaos_test(test_interpreter_profile
    interpreter_profile_test.cpp
    LABELS "unit;interpreter"
)

# Tiering benchmark (non-gtest, standalone performance measurement)
add_chaos_test(test_tiering_benchmark
    tiering_benchmark.cpp
    LABELS "benchmark;interpreter"
)

# Tiering benchmark with extended scenarios (arithmetic, register, array, etc.)
add_chaos_test(test_interpreter_tiering_benchmark
    interpreter_tiering_benchmark_test.cpp
    LABELS "benchmark;interpreter"
)

# Stress / soak tests for FastExecute — multi-threaded concurrency, loop backedge OSR,
# mixed opcode sequences. Each test runs 10k-50k iterations with result validation.
# TIMEOUT: 120 seconds per test (kDefaultStressTimeoutMs).
add_chaos_test(test_interpreter_stress
    interpreter_stress_test.cpp
    LABELS "stress;interpreter"
)

# ── T5: Unit tests for osr_state.cpp and ir_reg_alloc.cpp ──────────────────

add_chaos_test(test_osr_state
    osr_state_test.cpp
    LABELS "unit;interpreter"
)

add_chaos_test(test_ir_reg_alloc
    ir_reg_alloc_test.cpp
    LABELS "unit;interpreter"
)
