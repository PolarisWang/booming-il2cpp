# Threading tests
#
# Each test registers as a ctest target via add_chaos_test.
# Naming convention: test_<subsystem>_<name>
# Benchmark tests are labeled "benchmark" (excluded from default run).

set(CHAOS_THREADING_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_jit
    chaos_hot_update
    chaos_common
    chaos_support
    chaos_fmt
)

# threading_smoke_test — Thread identity, registration, pool, P2 capabilities
# test_codegen_subsystem_stubs.cpp: standalone defs for codegen-only extern symbols
# (kChaosExternalRuntime*/hotpatch_module) that the runtime libs reference; see file header.
add_chaos_test(test_threading_smoke
    threading_smoke_test.cpp
    test_codegen_subsystem_stubs.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "unit;threading"
)

# synchronization_smoke_test — Semaphore, RWLock, WaitHandle, TimerQueue
add_chaos_test(test_synchronization_smoke
    synchronization_smoke_test.cpp
    test_codegen_subsystem_stubs.cpp
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
    test_codegen_subsystem_stubs.cpp
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

# queue_backpressure_test — Global queue backpressure verification (T2-QUEUE)
add_chaos_test(test_queue_backpressure
    queue_backpressure_test.cpp
    test_codegen_subsystem_stubs.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "unit;threading"
)

# threading_stress_test — Multi-threaded stress tests (T2-STRESS)
add_chaos_test(test_threading_stress
    threading_stress_test.cpp
    test_codegen_subsystem_stubs.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "unit;threading"
)

# hc_cpu_feedback_test — HC CPU utilization real measurement verification (T1-HC-CPU)
add_chaos_test(test_hc_cpu_feedback
    hc_cpu_feedback_test.cpp
    test_codegen_subsystem_stubs.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "unit;threading"
)

# rwlock_upgrade_test — ReaderWriterLockSlim upgradeable read / upgrade / downgrade
# Compiles synchronization.cpp directly for new APIs, links chaos_runtime_core for
# GC_TRANSITION, SafepointPoll, and thread state symbols.
add_chaos_test(test_rwlock_upgrade
    rwlock_upgrade_test.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/synchronization.cpp
    test_codegen_subsystem_stubs.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "unit;threading"
)

# phase3_industrialization_test — Phase 3 TDD: timer precision, shutdown race, injection rate
add_chaos_test(test_phase3_industrialization
    phase3_industrialization_test.cpp
    test_codegen_subsystem_stubs.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "unit;threading"
)

# object_header_stress_test — Type System Phase 1 stress tests (P4.3)
# ThinLockTable contention, reentrancy, multi-object, TryEnter, ping-pong, mixed
add_chaos_test(test_object_header_stress
    object_header_stress_test.cpp
    test_codegen_subsystem_stubs.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "stress;threading;typesystem"
)

# object_header_benchmark_test — Type System Phase 1 benchmarks (P4.2/P4.4)
# ThinLock uncontended latency + header allocation latency
add_chaos_test(test_object_header_benchmark
    object_header_benchmark_test.cpp
    test_codegen_subsystem_stubs.cpp
    LIBS ${CHAOS_THREADING_LIBS}
    LABELS "benchmark;threading;typesystem"
)
