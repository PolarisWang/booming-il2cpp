# Build smoke test + GC core tests + interpreter tests + threading tests
add_subdirectory(gc)
add_subdirectory(interpreter)
add_subdirectory(threading)

# Smoke test for build verification
add_chaos_test(smoke_test smoke_test.cpp
    LABELS "smoke;core"
)

# N1 — Non-GC runtime-core tests
set(CHAOS_COMMON_TEST_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_codegen
    chaos_hot_update
    chaos_common
    chaos_support
    chaos_fmt
)

add_chaos_test(capacity_stress_test capacity_stress_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "stress;core"
)
add_chaos_test(exception_sentinel_test exception_sentinel_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)
add_chaos_test(method_aot_map_test method_aot_map_test.cpp gc/gc_test_stubs.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)

# B1 — Blittable benchmark (labelled "benchmark", excluded from default run)
add_chaos_test(blittable_benchmark blittable_benchmark_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "benchmark;core"
)
