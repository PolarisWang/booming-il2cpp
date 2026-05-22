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

# execution_context_smoke_test — ExecutionContext SuppressFlow/RestoreFlow
# Compiles execution_context.cpp directly to avoid stale prebuilt lib issues.
add_chaos_test(test_execution_context_smoke
    execution_context_smoke_test.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/execution_context.cpp
    LIBS chaos_common chaos_fmt
    LABELS "unit;threading"
)

# rwlock_upgrade_test — ReaderWriterLockSlim upgradeable read / upgrade / downgrade
# Compiles synchronization.cpp directly for new APIs, links chaos_runtime_core for
# GC_TRANSITION, SafepointPoll, and thread state symbols.
add_chaos_test(test_rwlock_upgrade
    rwlock_upgrade_test.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/synchronization.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "unit;threading"
)
