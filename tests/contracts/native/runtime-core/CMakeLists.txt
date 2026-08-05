cmake_minimum_required(VERSION 3.20)

project(chaos_method_aot_map_test LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/native-runtime-core-test")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

# All GC contracts targets are built through the shared chaos_native_add_test()
# factory (cmake/chaos_native_test.cmake): it provides the runtime lib set, the
# /utf-8 + /FORCE:MULTIPLE flags, the shared include dirs, and the codegen-symbol
# stub (gc_test_stubs.cpp) that standalone source builds need.

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/method_aot_map_test.cpp")
    chaos_native_add_test(chaos_method_aot_map_test method_aot_map_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bump_cache_test.cpp")
    chaos_native_add_test(chaos_gc_bump_cache_test gc_bump_cache_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_atomic_alloc_test.cpp")
    chaos_native_add_test(chaos_gc_atomic_alloc_test gc_atomic_alloc_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_region_test.cpp")
    chaos_native_add_test(chaos_gc_region_test gc_region_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_root_scanner_test.cpp")
    chaos_native_add_test(chaos_gc_root_scanner_test gc_root_scanner_test.cpp CXX_STANDARD 20)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/domain_unload_test.cpp")
    chaos_native_add_test(chaos_domain_unload_test domain_unload_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_young_collector_test.cpp")
    chaos_native_add_test(chaos_gc_young_collector_test gc_young_collector_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_finalizer_test.cpp")
    chaos_native_add_test(chaos_gc_finalizer_test gc_finalizer_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_finalizer_integration_test.cpp")
    chaos_native_add_test(chaos_gc_finalizer_integration_test gc_finalizer_integration_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_handle_test.cpp")
    chaos_native_add_test(chaos_gc_handle_test gc_handle_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_poh_test.cpp")
    chaos_native_add_test(chaos_gc_poh_test gc_poh_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_loh_test.cpp")
    chaos_native_add_test(chaos_gc_loh_test gc_loh_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/loh_stress_test.cpp")
    chaos_native_add_test(chaos_gc_loh_stress_test loh_stress_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_finalizer_stress_test.cpp")
    chaos_native_add_test(chaos_gc_finalizer_stress_test gc_finalizer_stress_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/oversized_stress_test.cpp")
    chaos_native_add_test(chaos_gc_oversized_stress_test oversized_stress_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/memory_domain_test.cpp")
    chaos_native_add_test(chaos_memory_domain_test memory_domain_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/threading_smoke.cpp")
    chaos_native_add_test(chaos_threading_smoke threading_smoke.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/execution_context_smoke.cpp")
    chaos_native_add_test(chaos_execution_context_smoke execution_context_smoke.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/synchronization_smoke.cpp")
    chaos_native_add_test(chaos_synchronization_smoke synchronization_smoke.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/monitor_stress_test.cpp")
    chaos_native_add_test(chaos_monitor_stress_test monitor_stress_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/threading_benchmark.cpp")
    chaos_native_add_test(chaos_threading_benchmark threading_benchmark.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/blittable_benchmark.cpp")
    chaos_native_add_test(chaos_blittable_benchmark blittable_benchmark.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/delegate_stress_test.cpp")
    chaos_native_add_test(chaos_delegate_stress_test delegate_stress_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_stress_test.cpp")
    chaos_native_add_test(chaos_gc_stress_test gc_stress_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_worker_pool_smoke.cpp")
    chaos_native_add_test(chaos_gc_worker_pool_smoke gc_worker_pool_smoke.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_sanity_test.cpp")
    chaos_native_add_test(chaos_gc_sanity_test gc_sanity_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bgc_smoke.cpp")
    chaos_native_add_test(chaos_gc_bgc_smoke gc_bgc_smoke.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bgc_race_test.cpp")
    chaos_native_add_test(chaos_gc_bgc_race_test gc_bgc_race_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bgc_root_scan_test.cpp")
    chaos_native_add_test(chaos_gc_bgc_root_scan_test gc_bgc_root_scan_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bgc_stress_test.cpp")
    chaos_native_add_test(chaos_gc_bgc_stress_test gc_bgc_stress_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/exception_sentinel_test.cpp")
    chaos_native_add_test(chaos_exception_sentinel_test exception_sentinel_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/capacity_stress_test.cpp")
    chaos_native_add_test(chaos_capacity_stress_test capacity_stress_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/codegen_native_test.cpp")
    chaos_native_add_test(chaos_codegen_native_test codegen_native_test.cpp CXX_STANDARD 20)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/unwind_info_test.cpp")
    chaos_native_add_test(chaos_unwind_info_test unwind_info_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_scheduler_test.cpp")
    chaos_native_add_test(chaos_gc_scheduler_test gc_scheduler_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_parallel_mark_test.cpp")
    chaos_native_add_test(chaos_gc_parallel_mark_test gc_parallel_mark_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_safepoint_test.cpp")
    chaos_native_add_test(chaos_gc_safepoint_test gc_safepoint_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_old_gen_unit_test.cpp")
    chaos_native_add_test(chaos_gc_old_gen_unit_test gc_old_gen_unit_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_gen1_test.cpp")
    chaos_native_add_test(chaos_gc_gen1_test gc_gen1_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_gen1_benchmark.cpp")
    chaos_native_add_test(chaos_gc_gen1_benchmark gc_gen1_benchmark.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_mark_bitmap_test.cpp")
    chaos_native_add_test(chaos_gc_mark_bitmap_test gc_mark_bitmap_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bit_utils_test.cpp")
    chaos_native_add_test(chaos_gc_bit_utils_test gc_bit_utils_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_layout_test.cpp")
    chaos_native_add_test(chaos_gc_layout_test gc_layout_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_events_test.cpp")
    chaos_native_add_test(chaos_gc_events_test gc_events_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_stats_test.cpp")
    chaos_native_add_test(chaos_gc_stats_test gc_stats_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_card_table_ext_test.cpp")
    chaos_native_add_test(chaos_gc_card_table_ext_test gc_card_table_ext_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_tlab_test.cpp")
    chaos_native_add_test(chaos_gc_tlab_test gc_tlab_test.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_satb_stress_test.cpp")
    chaos_native_add_test(chaos_gc_satb_stress_test gc_satb_stress_test.cpp)
endif()
