# GC subsystem tests
# Each test registers as a ctest target via add_chaos_test.
#
# Naming convention: test_gc_<subsystem>
# Test source files use GoogleTest (gtest) framework.
#
# Two sets of link libraries:
#   CHAOS_GC_LIBS    — full set (needed by tests that link chaos_runtime_core)
#   CHAOS_GC_STUBS   — AOT codegen symbol stubs (for tests without real codegen)
#   CHAOS_GC_BASE    — shared test fixture source (gc_test_base.cpp)

set(CHAOS_GC_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_codegen
    chaos_hot_update
    chaos_common
    chaos_support
    chaos_fmt
)

set(CHAOS_GC_STUBS gc_test_stubs.cpp)
set(CHAOS_GC_BASE  gc_test_base.cpp)

# ═══════════════════════════════════════════════════════════════════════════
# Batch A — Simple unit tests
# ═══════════════════════════════════════════════════════════════════════════

# gc_bit_utils_test — bit utility functions (ctz, popcount, foreach-set-bit)
add_chaos_test(test_gc_bit_utils
    gc_bit_utils_test.cpp
    LIBS chaos_fmt chaos_common
    LABELS "unit;gc"
)

# gc_bump_cache_test — GcBumpCache size-class allocator unit tests
add_chaos_test(test_gc_bump_cache
    gc_bump_cache_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_layout_test — GcLayoutRegistry unit tests
add_chaos_test(test_gc_layout
    gc_layout_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# ═══════════════════════════════════════════════════════════════════════════
# Batch B — Standard unit tests (need stubs)
# ═══════════════════════════════════════════════════════════════════════════

# gc_region_test — CRAG region framework + card table unit tests
add_chaos_test(test_gc_region
    gc_region_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)
target_link_options(test_gc_region PRIVATE /FORCE:MULTIPLE)

# gc_card_table_ext_test — Card table edge case tests
add_chaos_test(test_gc_card_table_ext
    gc_card_table_ext_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_events_test — GC events unit tests
add_chaos_test(test_gc_events
    gc_events_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_stats_test — GC stats unit tests
# NOTE: GcGetSnapshot() reads from g_gc_stats in chaos_runtime_core.lib
# (GNU ar archive), but MSVC link.exe creates a duplicate variable that
# isn't properly shared with inline functions. The alloc counters are
# verified via direct g_gc_stats access in the test instead.
add_chaos_test(test_gc_stats
    gc_stats_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_scheduler_test — GcScheduler unit tests
add_chaos_test(test_gc_scheduler
    gc_scheduler_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_memory_domain_test — MemoryDomain lifecycle, tagged allocation, usage limits
add_chaos_test(test_gc_memory_domain
    memory_domain_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# ═══════════════════════════════════════════════════════════════════════════
# Batch C — GC functional tests (need stubs + young gen init)
# ═══════════════════════════════════════════════════════════════════════════

# gc_root_scanner_test — GcSlotMap registry + precise/conservative/hybrid scanning
add_chaos_test(test_gc_root_scanner
    gc_root_scanner_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_handle_test — GCHandle strong/weak/pinned/dependent lifecycle
add_chaos_test(test_gc_handle
    gc_handle_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_poh_test — Pinned Object Heap allocation + handles
add_chaos_test(test_gc_poh
    gc_poh_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_loh_test — Large Object Heap allocation/sweep/compaction
add_chaos_test(test_gc_loh
    gc_loh_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_tlab_test — Thread-Local Allocation Buffer unit tests
add_chaos_test(test_gc_tlab
    gc_tlab_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# ═══════════════════════════════════════════════════════════════════════════
# Batch D — GC unit tests (need stubs + fixture setup)
# ═══════════════════════════════════════════════════════════════════════════

# gc_young_collector_test — CRAG young collector (forwarding, scavenge, card table)
# NOTE: gc_young_collector.cpp is compiled directly because the prebuilt
# chaos_runtime_core.lib uses GNU ar format which MSVC link.exe cannot
# search for COMDAT resolution. Include path is set via target_include.
add_chaos_test(test_gc_young_collector
    gc_young_collector_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_young_collector.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_scheduler.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_card_table.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_layout.cpp
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)
target_include_directories(test_gc_young_collector PRIVATE
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc)
# gc_young_collector.cpp symbols are also in chaos_runtime_core.lib (GNU ar
# format that MSVC link.exe cannot search for COMDAT).  /FORCE:MULTIPLE lets
# the directly-compiled symbols override the archive copies.
target_link_options(test_gc_young_collector PRIVATE /FORCE:MULTIPLE)

# gc_old_gen_unit_test — OldGen allocate/free/reallocate/mark
add_chaos_test(test_gc_old_gen
    gc_old_gen_unit_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_gen1_test — Gen1 collection (empty/single/mixed/fragmentation)
add_chaos_test(test_gc_gen1
    gc_gen1_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_finalizer_test — Finalizer registration + execution
add_chaos_test(test_gc_finalizer
    gc_finalizer_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_finalizer_integration_test — End-to-end finalizer via public ABI
# NOTE: gc_young_collector.cpp + gc_layout.cpp compiled directly
# (same /FORCE:MULTIPLE pattern).
add_chaos_test(test_gc_finalizer_integration
    gc_finalizer_integration_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_young_collector.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_scheduler.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_card_table.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_layout.cpp
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)
target_include_directories(test_gc_finalizer_integration PRIVATE
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc)
target_link_options(test_gc_finalizer_integration PRIVATE /FORCE:MULTIPLE)

# gc_max_promote_test — kMaxPromoteObjects guard verification
# NOTE: gc_young_collector.cpp + gc_layout.cpp compiled directly.
add_chaos_test(test_gc_max_promote
    gc_max_promote_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_young_collector.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_scheduler.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_card_table.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_layout.cpp
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)
target_include_directories(test_gc_max_promote PRIVATE
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc)
target_link_options(test_gc_max_promote PRIVATE /FORCE:MULTIPLE)

# gc_full_gc_test — Full GC collection channel tests
add_chaos_test(test_gc_full_gc
    gc_full_gc_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_managed_api_test — Managed GC API native verification tests
# NOTE: gc_api.cpp and its transitive dependencies are compiled directly because
# the prebuilt chaos_runtime_core.lib uses GNU ar format that MSVC link.exe
# cannot search for COMDAT resolution.
add_chaos_test(test_gc_managed_api
    gc_managed_api_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_api.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_young_collector.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_scheduler.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_card_table.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_stats.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_layout.cpp
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)
target_include_directories(test_gc_managed_api PRIVATE
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc)
target_link_options(test_gc_managed_api PRIVATE /FORCE:MULTIPLE)

# gc_safepoint_test — Safepoint protocol unit tests
add_chaos_test(test_gc_safepoint
    gc_safepoint_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_parallel_mark_test — ParallelMark work-stealing unit tests
add_chaos_test(test_gc_parallel_mark
    gc_parallel_mark_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_mark_bitmap_test — GcMarkBitmap unit tests
add_chaos_test(test_gc_mark_bitmap
    gc_mark_bitmap_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_sanity_test — POH, ThinLock, GCMemoryInfo sanity
add_chaos_test(test_gc_sanity
    gc_sanity_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_domain_unload_test — Domain unloader orchestration (C4)
add_chaos_test(test_gc_domain_unload
    domain_unload_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_demotion_test — Gen2→Gen1 demotion (ShouldDemote, CollectDemotionCandidates, DemotionRelocate)
add_chaos_test(test_gc_demotion
    gc_demotion_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_heap_limit_test — Hard/soft memory limit tests (scheduler API + old-gen integration)
add_chaos_test(test_gc_heap_limit
    gc_heap_limit_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# ═══════════════════════════════════════════════════════════════════════════
# Batch E — Stress tests (labelled "stress", excluded from default run)
# ═══════════════════════════════════════════════════════════════════════════

# gc_stress_scenarios — Multi-threaded GC stress tests (Scenario S)
add_chaos_test(test_gc_stress
    gc_stress_scenarios.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
set_tests_properties(test_gc_stress PROPERTIES LABELS "stress;gc")

# ═══════════════════════════════════════════════════════════════════════════
# Batch F — Benchmarks (labelled "benchmark", excluded from default run)
# ═══════════════════════════════════════════════════════════════════════════

# gc_gen1_benchmark — Gen1 collection pause time benchmarks
add_chaos_test(test_gc_gen1_benchmark
    gc_gen1_benchmark.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
target_link_options(test_gc_gen1_benchmark PRIVATE /FORCE:MULTIPLE)
set_tests_properties(test_gc_gen1_benchmark PROPERTIES LABELS "benchmark;gc")

# gc_throughput_benchmark — Native GC throughput (allocation + GC pause + BGC latency)
add_chaos_test(test_gc_throughput_benchmark
    gc_throughput_benchmark.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
target_link_options(test_gc_throughput_benchmark PRIVATE /FORCE:MULTIPLE)
set_tests_properties(test_gc_throughput_benchmark PROPERTIES LABELS "benchmark;gc")

# gc_regression_benchmark — GC regression benchmark suite (G-15)
# Measures allocation throughput, GC pause time, multi-threaded allocation.
add_chaos_test(test_gc_regression_benchmark
    gc_regression_benchmark.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
target_link_options(test_gc_regression_benchmark PRIVATE /FORCE:MULTIPLE)
set_tests_properties(test_gc_regression_benchmark PROPERTIES LABELS "benchmark;gc")

# ═══════════════════════════════════════════════════════════════════════════
# Batch G — Migrated from old contracts: stress/smoke tests (G1)
# ═══════════════════════════════════════════════════════════════════════════

# gc_worker_pool_test — GcWorkerPool smoke test (no stress label, quick)
add_chaos_test(test_gc_worker_pool
    gc_worker_pool_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
    LABELS "unit;gc"
)

# gc_mini_stress_test — Minimal multi-threaded nursery stress
add_chaos_test(test_gc_mini_stress
    gc_mini_stress_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
set_tests_properties(test_gc_mini_stress PROPERTIES LABELS "stress;gc")

# gc_monitor_stress_test — Monitor Enter/Exit/Wait/Pulse with GC interleave
# NOTE: gc_young_collector.cpp compiled directly (same pattern as
# test_gc_young_collector) because GcYoungCollection() is needed and
# the prebuilt chaos_runtime_core.lib uses GNU ar format that MSVC
# link.exe cannot search for COMDAT resolution.
add_chaos_test(test_gc_monitor_stress
    gc_monitor_stress_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_young_collector.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_scheduler.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_card_table.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_layout.cpp
    LIBS ${CHAOS_GC_LIBS}
)
target_include_directories(test_gc_monitor_stress PRIVATE
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc)
target_link_options(test_gc_monitor_stress PRIVATE /FORCE:MULTIPLE)
set_tests_properties(test_gc_monitor_stress PROPERTIES LABELS "stress;gc")

# ═══════════════════════════════════════════════════════════════════════════
# Batch H — Migrated from old contracts: medium stress tests (G2)
# ═══════════════════════════════════════════════════════════════════════════

# gc_finalizer_stress_test — Finalizer stress (F1-F5)
add_chaos_test(test_gc_finalizer_stress
    gc_finalizer_stress_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
set_tests_properties(test_gc_finalizer_stress PROPERTIES LABELS "stress;gc")

# loh_stress_test — Large Object Heap stress (L1-L5)
add_chaos_test(test_gc_loh_stress
    loh_stress_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
set_tests_properties(test_gc_loh_stress PROPERTIES LABELS "stress;gc")

# oversized_stress_test — Oversized allocation stress (O1-O4)
add_chaos_test(test_gc_oversized_stress
    oversized_stress_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
set_tests_properties(test_gc_oversized_stress PROPERTIES LABELS "stress;gc")

# ═══════════════════════════════════════════════════════════════════════════
# Batch I — BGC tests (G3)
# ═══════════════════════════════════════════════════════════════════════════

# gc_bgc_smoke_test — BGC cycle lifecycle + young-GC interop
add_chaos_test(test_gc_bgc_smoke
    gc_bgc_smoke_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_young_collector.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_scheduler.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_card_table.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_layout.cpp
    LIBS ${CHAOS_GC_LIBS}
)
target_include_directories(test_gc_bgc_smoke PRIVATE
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc)
target_link_options(test_gc_bgc_smoke PRIVATE /FORCE:MULTIPLE)
set_tests_properties(test_gc_bgc_smoke PROPERTIES LABELS "stress;gc")

# gc_bgc_root_scan_test — Young-to-old root scan during BGC
add_chaos_test(test_gc_bgc_root_scan
    gc_bgc_root_scan_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
target_link_options(test_gc_bgc_root_scan PRIVATE /FORCE:MULTIPLE)
set_tests_properties(test_gc_bgc_root_scan PROPERTIES LABELS "stress;gc")

# gc_bgc_unit_test — BGC subsystem unit tests (SATB, phase, worker deque)
# NOTE: gc_bgc.cpp compiled directly (same pattern as test_gc_bgc_smoke) because
# the prebuilt chaos_runtime_core.lib uses GNU ar format that MSVC link.exe
# cannot search for COMDAT resolution.
add_chaos_test(test_gc_bgc_unit
    gc_bgc_unit_test.cpp ${CHAOS_GC_STUBS}
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_bgc.cpp
    LIBS ${CHAOS_GC_LIBS}
    LABELS "stress;gc"
)
target_include_directories(test_gc_bgc_unit PRIVATE
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc)
target_link_options(test_gc_bgc_unit PRIVATE /FORCE:MULTIPLE)

# gc_bgc_stress_test — BGC concurrent stress, work-stealing, graph mutation
add_chaos_test(test_gc_bgc_stress
    gc_bgc_stress_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
target_link_options(test_gc_bgc_stress PRIVATE /FORCE:MULTIPLE)
set_tests_properties(test_gc_bgc_stress PROPERTIES LABELS "stress;gc")

# ═══════════════════════════════════════════════════════════════════════════
# Batch J — Large stress tests (G4)
# ═══════════════════════════════════════════════════════════════════════════

# gc_general_stress_test — Multi-threaded general GC stress (BaselineConcurrent,
# MixedSizeAllocation, AggressiveYoungGc, ExtendedGcPressure)
add_chaos_test(test_gc_general_stress
    gc_general_stress_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
set_tests_properties(test_gc_general_stress PROPERTIES LABELS "stress;gc" TIMEOUT 300)

# delegate_stress_test — Multi-threaded delegate stress (A1-E5)
add_chaos_test(test_gc_delegate_stress
    delegate_stress_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)
set_tests_properties(test_gc_delegate_stress PROPERTIES LABELS "stress;gc" TIMEOUT 300)

# ═══════════════════════════════════════════════════════════════════════════
# Original smoke test
# ═══════════════════════════════════════════════════════════════════════════

add_chaos_test(gc_test gc_smoke_test.cpp)

# ═══════════════════════════════════════════════════════════════════════════
# Batch K — Migrated from old contracts: remaining standalone tests
# These use custom main() (not GoogleTest), so they are added as raw
# add_executable targets rather than via add_chaos_test.
# NOTE: gc_test_macros.h unconditionally includes <gtest/gtest.h>, so
# these targets link gtest (for include paths only, not gtest_main) even
# though they use the custom CHECK/FAIL macros with their own main().
# ═══════════════════════════════════════════════════════════════════════════

# gc_atomic_alloc_test — NurseryAllocateAtomic unit tests (5 scenarios)
add_executable(test_gc_atomic_alloc
    gc_atomic_alloc_test.cpp ${CHAOS_GC_STUBS})
target_compile_features(test_gc_atomic_alloc PRIVATE cxx_std_17)
target_compile_definitions(test_gc_atomic_alloc PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(test_gc_atomic_alloc PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_include_directories(test_gc_atomic_alloc PRIVATE ${CHAOS_INCLUDE_DIRS})
target_link_libraries(test_gc_atomic_alloc PRIVATE gtest ${CHAOS_GC_LIBS})
target_link_options(test_gc_atomic_alloc PRIVATE /FORCE:MULTIPLE)
add_test(NAME test_gc_atomic_alloc COMMAND test_gc_atomic_alloc)
set_tests_properties(test_gc_atomic_alloc PROPERTIES LABELS "unit;gc")

# gc_debug_contract_test — GC debug contract symbol validation (4 tests)
# NOTE: gc_debug_contract.cpp compiled directly to provide the debug contract
# symbols (g_chaos_gc_stats_ptr, etc.) that are in the prebuilt
# chaos_runtime_core.lib which uses GNU ar format that MSVC link.exe cannot
# search for COMDAT resolution.
add_executable(test_gc_debug_contract
    gc_debug_contract_test.cpp
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_debug_contract.cpp)
target_compile_features(test_gc_debug_contract PRIVATE cxx_std_17)
target_compile_definitions(test_gc_debug_contract PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(test_gc_debug_contract PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_include_directories(test_gc_debug_contract PRIVATE ${CHAOS_INCLUDE_DIRS}
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc)
target_link_libraries(test_gc_debug_contract PRIVATE gtest chaos_runtime_core chaos_common chaos_fmt)
target_link_options(test_gc_debug_contract PRIVATE /FORCE:MULTIPLE)
add_test(NAME test_gc_debug_contract COMMAND test_gc_debug_contract)
set_tests_properties(test_gc_debug_contract PROPERTIES LABELS "unit;gc")

# gc_satb_stress_test — Multithreaded SATB buffer overflow stress test
add_executable(test_gc_satb_stress
    gc_satb_stress_test.cpp ${CHAOS_GC_STUBS}
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc/gc_bgc.cpp)
target_compile_features(test_gc_satb_stress PRIVATE cxx_std_17)
target_compile_definitions(test_gc_satb_stress PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(test_gc_satb_stress PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_include_directories(test_gc_satb_stress PRIVATE ${CHAOS_INCLUDE_DIRS}
    ${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/gc)
target_link_libraries(test_gc_satb_stress PRIVATE gtest ${CHAOS_GC_LIBS})
target_link_options(test_gc_satb_stress PRIVATE /FORCE:MULTIPLE)
add_test(NAME test_gc_satb_stress COMMAND test_gc_satb_stress)
set_tests_properties(test_gc_satb_stress PROPERTIES LABELS "stress;gc")

# gc_soak_test — CRAG GC Arrhenius 72h soak test (G-17)
add_executable(test_gc_soak
    gc_soak_test.cpp ${CHAOS_GC_STUBS})
target_compile_features(test_gc_soak PRIVATE cxx_std_17)
target_compile_definitions(test_gc_soak PRIVATE _CRT_SECURE_NO_WARNINGS)
target_compile_options(test_gc_soak PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_include_directories(test_gc_soak PRIVATE ${CHAOS_INCLUDE_DIRS})
target_link_libraries(test_gc_soak PRIVATE gtest ${CHAOS_GC_LIBS})
target_link_options(test_gc_soak PRIVATE /FORCE:MULTIPLE)
add_test(NAME test_gc_soak COMMAND test_gc_soak)
set_tests_properties(test_gc_soak PROPERTIES LABELS "stress;gc")
