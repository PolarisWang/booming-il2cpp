# Threading tests
#
# Each test registers as a ctest target via add_chaos_test.
# Naming convention: test_<subsystem>_<name>
# Benchmark tests are labeled "benchmark" (excluded from default run).

set(CHAOS_THREADING_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_codegen
    chaos_hot_update
    chaos_common
    chaos_support
    chaos_fmt
)

# threading_smoke_test — Thread identity, registration, pool, P2 capabilities
add_chaos_test(test_threading_smoke
    threading_smoke_test.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "unit;threading"
)

# synchronization_smoke_test — Semaphore, RWLock, WaitHandle, TimerQueue
add_chaos_test(test_synchronization_smoke
    synchronization_smoke_test.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "unit;threading"
)

# threading_benchmark_test — Threading benchmarks
add_chaos_test(test_threading_benchmark
    threading_benchmark_test.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "benchmark;threading"
)

# async_integration_smoke_test — Task.Run integration with ThreadPool
add_chaos_test(test_async_integration_smoke
    async_integration_smoke_test.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "unit;threading"
)
