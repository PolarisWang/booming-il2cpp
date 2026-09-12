# Threading tests
#
# Each test registers as a ctest target via add_chaos_test (thin wrapper over the
# shared chaos_native_add_test factory). The factory provides the runtime lib set,
# /utf-8 + /FORCE:MULTIPLE flags, include dirs, and the codegen-symbol stub.

# threading_smoke_test — Thread identity, registration, pool, P2 capabilities
chaos_native_add_test(test_threading_smoke
    threading_smoke_test.cpp
    LABELS "unit;threading"
)

# synchronization_smoke_test — Semaphore, RWLock, WaitHandle, TimerQueue
chaos_native_add_test(test_synchronization_smoke
    synchronization_smoke_test.cpp
    LABELS "unit;threading"
)

# threading_benchmark_test — Threading benchmarks
chaos_native_add_test(test_threading_benchmark
    threading_benchmark_test.cpp
    LABELS "benchmark;threading"
)

# async_integration_smoke_test — Task.Run integration with ThreadPool
chaos_native_add_test(test_async_integration_smoke
    async_integration_smoke_test.cpp
    LABELS "unit;threading"
)

# execution_context_smoke_test — ExecutionContext SuppressFlow/RestoreFlow
# Compiles execution_context.cpp directly to avoid stale prebuilt lib issues.
chaos_native_add_test(test_execution_context_smoke
    execution_context_smoke_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/execution_context.cpp
    LABELS "unit;threading"
)

# async_task_state_test — ASYNC-P1-4: three-state distinction (resolved/faulted/
# cancelled).  Compiles async_stubs.cpp directly so the test exercises the real
# ChaosAsyncTask* entry points rather than a stale prebuilt copy.
chaos_native_add_test(test_async_task_state
    async_task_state_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    LABELS "unit;threading"
)

# async_task_exception_test — ASYNC-P1-3: exception propagation (Phase 1 exit
# gate).  Links async_stubs.cpp directly so the real ChaosAsyncTask* entry
# points are under test.
chaos_native_add_test(test_async_task_exception
    async_task_exception_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    LABELS "unit;threading"
)

# async_task_run_e2e_test — ASYNC-P1-1: Task.Run end-to-end against the
# PRODUCTION task_runner.cpp (registered by runtime_init).  Every other Task.Run
# test substitutes a test-local stand-in, so the shipping delegate-invocation
# path was untested.  Compiles task_runner.cpp + thread_pool.cpp directly.
chaos_native_add_test(test_async_task_run_e2e
    async_task_run_e2e_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/task_runner.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/thread_pool.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/execution_context.cpp
    LABELS "unit;threading"
)

# async_task_blocking_test — ASYNC-P1-2: Wait must really block (≥ task
# duration) and must PARK rather than spin a core while blocked.
chaos_native_add_test(test_async_task_blocking
    async_task_blocking_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/task_runner.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/thread_pool.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/execution_context.cpp
    LABELS "unit;threading"
)

# async_when_test — ASYNC-P2-1: WhenAll / WhenAny combinators.  The aggregate
# must reflect the children's real outcomes (fault propagation, winner index),
# not merely report "completed".  Compiles async_stubs.cpp directly so the real
# chaos_task_when_* entry points are under test.
chaos_native_add_test(test_async_when
    async_when_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    LABELS "unit;threading"
)

# async_when_async_test — ASYNC-P2-1 (cont.): WhenAll/WhenAny where the children
# complete ASYNCHRONOUSLY.  async_when_test.cpp covers only already-completed
# children, which fire the continuation inline and finish in 0 ms; these exercise
# the finish_async_task delivery path, which is what real composition uses.
chaos_native_add_test(test_async_when_async
    async_when_async_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    LABELS "unit;threading"
)

# async_continue_with_test — ASYNC-P2-2: Task.ContinueWith.  The continuation
# must receive the antecedent (and its result), run for BOTH completion orders,
# run on a faulted/cancelled antecedent, and resolve its own task with the
# continuation's return value.  Links delegate_helpers.cpp so the continuation
# delegate is really invoked rather than assumed to be.
chaos_native_add_test(test_async_continue_with
    async_continue_with_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/core/delegate_helpers.cpp
    LABELS "unit;threading"
)

# async_when_array_test — ASYNC-P2-3: the managed-array WhenAll/WhenAny overload
# that codegen actually calls.  Asserts the result SET [1,2,3] (not just
# "completed") and that the aggregate owns its copy of the child handles, so a
# child completing later is still observed.
chaos_native_add_test(test_async_when_array
    async_when_array_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    LABELS "unit;threading"
)

# async_when_each_test — ASYNC-P2-8: Task.WhenEach's completion-ORDERED stream.
# Distinct contract from the two tests above: WhenAll yields the aggregate once
# and WhenAny yields the first completed once, whereas WhenEach yields EVERY
# task as it completes.  The central test completes children in an order
# different from their array order and asserts the drain observes COMPLETION
# order — an array-order implementation would pass a count-only test and be a
# silently different program.
chaos_native_add_test(test_async_when_each
    async_when_each_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    LABELS "unit;threading"
)

# async_continue_when_test — TaskFactory.ContinueWhenAll/ContinueWhenAny.
# The composition's one plausible-but-wrong implementation registers the
# continuation on each CHILD rather than on the aggregate, firing it once per
# task.  The tests use three tasks and assert exactly ONE invocation — a
# single-element array (what a naive test would use) cannot tell them apart.
chaos_native_add_test(test_async_continue_when
    async_continue_when_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    LABELS "unit;threading"
)

# async_stress_test — Phase 5: 100k Task lifecycle + concurrent + deep chain.
# Links async_stubs.cpp for the task create/completion API surface.
chaos_native_add_test(test_async_stress
    async_stress_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    LABELS "unit;threading;stress"
)

# async_task_factory_test — ASYNC-P2-5: TaskFactory.get_Factory / StartNew.
# chaos_task_factory_start_new drops the (opaque) factory receiver and forwards
# the delegate to the production ThreadPool runner, so this links the same
# sources as task_run_e2e to exercise the real delegate-invocation path rather
# than a stand-in.
chaos_native_add_test(test_async_task_factory
    async_task_factory_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/task_runner.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/thread_pool.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/execution_context.cpp
    LABELS "unit;threading"
)

# async_iterator_test — ASYNC-P2-8 A2: the native iterator source pool +
# AsyncIteratorMethodBuilder ops.  A2's only new failure mode is POOL RECYCLING (a
# stale awaiter observing a recycled source's new value — silently wrong, not a
# crash), so the tests target the token/version boundary and the overflow path rather
# than the happy path.
#
# Compiles async_stubs.cpp directly: the second half of the file drives the extern "C"
# chaos_async_iterator_* entry points that codegen actually binds to, and a bug
# confined to that translation layer (int16↔int32 token, INTPTR↔fn-pointer) would be
# invisible if only the header classes were exercised.
chaos_native_add_test(test_async_iterator
    async_iterator_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/async_stubs.cpp
    LABELS "unit;threading"
)

# queue_backpressure_test — Global queue backpressure verification (T2-QUEUE)
chaos_native_add_test(test_queue_backpressure
    queue_backpressure_test.cpp
    LABELS "unit;threading"
)

# threading_stress_test — Multi-threaded stress tests (T2-STRESS)
chaos_native_add_test(test_threading_stress
    threading_stress_test.cpp
    RESOURCE_LOCK bgc
    LABELS "unit;threading"
)

# hc_cpu_feedback_test — HC CPU utilization real measurement verification (T1-HC-CPU)
chaos_native_add_test(test_hc_cpu_feedback
    hc_cpu_feedback_test.cpp
    LABELS "unit;threading"
)

# rwlock_upgrade_test — ReaderWriterLockSlim upgradeable read / upgrade / downgrade
# Compiles synchronization.cpp directly for new APIs, links chaos_runtime_core for
# GC_TRANSITION, SafepointPoll, and thread state symbols.
chaos_native_add_test(test_rwlock_upgrade
    rwlock_upgrade_test.cpp
    ${CHAOS_CMAKE_ROOT}/src/native/runtime-core/synchronization.cpp
    LABELS "unit;threading"
)

# phase3_industrialization_test — Phase 3 TDD: timer precision, shutdown race, injection rate
chaos_native_add_test(test_phase3_industrialization
    phase3_industrialization_test.cpp
    LABELS "unit;threading"
)

# object_header_stress_test — Type System Phase 1 stress tests (P4.3)
# ThinLockTable contention, reentrancy, multi-object, TryEnter, ping-pong, mixed
chaos_native_add_test(test_object_header_stress
    object_header_stress_test.cpp
    LABELS "stress;threading;typesystem"
)

# object_header_benchmark_test — Type System Phase 1 benchmarks (P4.2/P4.4)
# ThinLock uncontended latency + header allocation latency
chaos_native_add_test(test_object_header_benchmark
    object_header_benchmark_test.cpp
    LABELS "benchmark;threading;typesystem"
)
