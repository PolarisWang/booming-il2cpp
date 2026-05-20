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
)

# gc_bump_cache_test — GcBumpCache size-class allocator unit tests
add_chaos_test(test_gc_bump_cache
    gc_bump_cache_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_layout_test — GcLayoutRegistry unit tests
add_chaos_test(test_gc_layout
    gc_layout_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
)

# ═══════════════════════════════════════════════════════════════════════════
# Batch B — Standard unit tests (need stubs)
# ═══════════════════════════════════════════════════════════════════════════

# gc_region_test — CRAG region framework + card table unit tests
add_chaos_test(test_gc_region
    gc_region_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_card_table_ext_test — Card table edge case tests
add_chaos_test(test_gc_card_table_ext
    gc_card_table_ext_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_events_test — GC events unit tests
add_chaos_test(test_gc_events
    gc_events_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_stats_test — GC stats unit tests
# NOTE: GcGetSnapshot() reads from g_gc_stats in chaos_runtime_core.lib
# (GNU ar archive), but MSVC link.exe creates a duplicate variable that
# isn't properly shared with inline functions. The alloc counters are
# verified via direct g_gc_stats access in the test instead.
add_chaos_test(test_gc_stats
    gc_stats_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_scheduler_test — GcScheduler unit tests
add_chaos_test(test_gc_scheduler
    gc_scheduler_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
)

# ═══════════════════════════════════════════════════════════════════════════
# Batch C — GC functional tests (need stubs + young gen init)
# ═══════════════════════════════════════════════════════════════════════════

# gc_root_scanner_test — GcSlotMap registry + precise/conservative/hybrid scanning
add_chaos_test(test_gc_root_scanner
    gc_root_scanner_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_handle_test — GCHandle strong/weak/pinned/dependent lifecycle
add_chaos_test(test_gc_handle
    gc_handle_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_poh_test — Pinned Object Heap allocation + handles
add_chaos_test(test_gc_poh
    gc_poh_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_loh_test — Large Object Heap allocation/sweep/compaction
add_chaos_test(test_gc_loh
    gc_loh_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_tlab_test — Thread-Local Allocation Buffer unit tests
add_chaos_test(test_gc_tlab
    gc_tlab_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
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
    LIBS ${CHAOS_GC_LIBS}
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
)

# gc_gen1_test — Gen1 collection (empty/single/mixed/fragmentation)
add_chaos_test(test_gc_gen1
    gc_gen1_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_finalizer_test — Finalizer registration + execution
add_chaos_test(test_gc_finalizer
    gc_finalizer_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_safepoint_test — Safepoint protocol unit tests
add_chaos_test(test_gc_safepoint
    gc_safepoint_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_parallel_mark_test — ParallelMark work-stealing unit tests
add_chaos_test(test_gc_parallel_mark
    gc_parallel_mark_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_mark_bitmap_test — GcMarkBitmap unit tests
add_chaos_test(test_gc_mark_bitmap
    gc_mark_bitmap_test.cpp ${CHAOS_GC_STUBS}
    LIBS ${CHAOS_GC_LIBS}
)

# gc_sanity_test — POH, ThinLock, GCMemoryInfo sanity
add_chaos_test(test_gc_sanity
    gc_sanity_test.cpp ${CHAOS_GC_STUBS} ${CHAOS_GC_BASE}
    LIBS ${CHAOS_GC_LIBS}
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
set_tests_properties(test_gc_gen1_benchmark PROPERTIES LABELS "benchmark;gc")

# ═══════════════════════════════════════════════════════════════════════════
# Original smoke test
# ═══════════════════════════════════════════════════════════════════════════

add_chaos_test(gc_test gc_smoke_test.cpp)
