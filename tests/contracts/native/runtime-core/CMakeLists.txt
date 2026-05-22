cmake_minimum_required(VERSION 3.20)

project(chaos_method_aot_map_test LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/native-runtime-core-test")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

# Dependency libraries are built by the debug preset.
link_directories(
    "${REPO_ROOT}/artifacts/presets/debug/src/native/runtime-core/Debug"
    "${REPO_ROOT}/artifacts/presets/debug/src/native/bootstrap/Debug"
    "${REPO_ROOT}/artifacts/presets/debug/src/native/interpreter/Debug"
    "${REPO_ROOT}/artifacts/presets/debug/fmt_build/Debug"
    "${REPO_ROOT}/artifacts/presets/debug/src/native/common/Debug"
    "${REPO_ROOT}/artifacts/presets/debug/src/native/hot-update/Debug"
    "${REPO_ROOT}/artifacts/presets/debug/src/native/support/Debug"
    "${REPO_ROOT}/artifacts/presets/debug/src/native/codegen/Debug"
    "${REPO_ROOT}/artifacts/presets/debug/src/native/codegen/Release")

# Stub symbols for codegen-emitted globals; required by targets linking
# chaos_bootstrap or chaos_interpreter (which reference these symbols).
set(CHAOS_GC_TEST_STUBS gc_test_stubs.cpp)

add_executable(chaos_method_aot_map_test
    method_aot_map_test.cpp ${CHAOS_GC_TEST_STUBS})

target_compile_features(chaos_method_aot_map_test PRIVATE cxx_std_17)
target_compile_definitions(chaos_method_aot_map_test PRIVATE
    _CRT_SECURE_NO_WARNINGS)
target_include_directories(
    chaos_method_aot_map_test
    PRIVATE
        "${REPO_ROOT}/contracts/native/v0"
        "${REPO_ROOT}/src/native/common"
        "${REPO_ROOT}/src/native/runtime-core")

target_link_libraries(
    chaos_method_aot_map_test
    PRIVATE
        chaos_runtime_core
        chaos_bootstrap
        chaos_interpreter
        chaos_codegen)

# All GC test targets need chaos_interpreter because chaos_runtime_core.lib
# contains instantiation_engine.obj which references InterpreterValue symbols,
# and chaos_bootstrap.lib references ChaosResolveExternalRuntimeFnTable
# (defined in hotpatch_resolve.cpp, part of chaos_interpreter).
set(CHAOS_GC_COMMON_LIBS chaos_runtime_core chaos_bootstrap chaos_interpreter chaos_fmt chaos_common chaos_support chaos_hot_update chaos_codegen)

# ═══════════════════════════════════════════════════════════════════════════
# GC Unit Tests — single subsystem, deterministic
# ═══════════════════════════════════════════════════════════════════════════

# gc_bump_cache_test — GcBumpCache size-class allocator unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bump_cache_test.cpp")
    add_executable(chaos_gc_bump_cache_test gc_bump_cache_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_bump_cache_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_bump_cache_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_bump_cache_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_bump_cache_test
        PRIVATE
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_bump_cache_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_region_test — CRAG region framework + card table unit tests (C0.1-C0.3)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_region_test.cpp")
    add_executable(chaos_gc_region_test gc_region_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_region_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_region_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_region_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_region_test
        PRIVATE
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_region_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_root_scanner_test — GcSlotMap registry + hybrid root scanning unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_root_scanner_test.cpp")
    add_executable(chaos_gc_root_scanner_test gc_root_scanner_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_root_scanner_test PRIVATE cxx_std_20)
    target_compile_definitions(chaos_gc_root_scanner_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_root_scanner_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_root_scanner_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_root_scanner_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# domain_unload_test — C4: HotUpdate domain unloader with cross-domain reference checking
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/domain_unload_test.cpp")
    add_executable(chaos_domain_unload_test domain_unload_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_domain_unload_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_domain_unload_test PRIVATE
        
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_domain_unload_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_domain_unload_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_domain_unload_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_young_collector_test — Cheney copying young GC unit tests (C2.3-C2.6)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_young_collector_test.cpp")
    add_executable(chaos_gc_young_collector_test gc_young_collector_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_young_collector_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_young_collector_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_young_collector_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_young_collector_test
        PRIVATE
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_young_collector_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_finalizer_test — Finalizer registration + execution unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_finalizer_test.cpp")
    add_executable(chaos_gc_finalizer_test gc_finalizer_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_finalizer_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_finalizer_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_finalizer_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_finalizer_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_finalizer_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_finalizer_integration_test — End-to-end finalizer via public ABI
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_finalizer_integration_test.cpp")
    add_executable(chaos_gc_finalizer_integration_test gc_finalizer_integration_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_finalizer_integration_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_finalizer_integration_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_finalizer_integration_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_finalizer_integration_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_finalizer_integration_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_handle_test — GCHandle strong/weak/pinned/dependent lifecycle tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_handle_test.cpp")
    add_executable(chaos_gc_handle_test gc_handle_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_handle_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_handle_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_handle_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_handle_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_handle_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_poh_test — POH (Pinned Object Heap) allocation + GC integration tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_poh_test.cpp")
    add_executable(chaos_gc_poh_test gc_poh_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_poh_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_poh_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_poh_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_poh_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_poh_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_loh_test — LOH allocation/sweep/compaction unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_loh_test.cpp")
    add_executable(chaos_gc_loh_test gc_loh_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_loh_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_loh_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_loh_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_loh_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_loh_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# ═══════════════════════════════════════════════════════════════════════════
# GC Stress Tests — multi-thread, long running
# ═══════════════════════════════════════════════════════════════════════════

# gc_loh_stress_test — LOH multi-threaded stress tests (5 scenarios)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/loh_stress_test.cpp")
    add_executable(chaos_gc_loh_stress_test loh_stress_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_loh_stress_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_loh_stress_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_loh_stress_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_loh_stress_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_loh_stress_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_finalizer_stress_test — Finalizer stress tests (5 scenarios)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_finalizer_stress_test.cpp")
    add_executable(chaos_gc_finalizer_stress_test gc_finalizer_stress_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_finalizer_stress_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_finalizer_stress_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_finalizer_stress_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_finalizer_stress_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_finalizer_stress_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# oversized_stress_test — 32-85KB oversized allocation burst with deferred Full GC
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/oversized_stress_test.cpp")
    add_executable(chaos_gc_oversized_stress_test
        oversized_stress_test.cpp
        ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_oversized_stress_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_oversized_stress_test PRIVATE
        ${CHAOS_GC_TEST_DEFS})
    target_compile_options(chaos_gc_oversized_stress_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(chaos_gc_oversized_stress_test PRIVATE
        ${CHAOS_GC_TEST_INCS})
    target_link_libraries(chaos_gc_oversized_stress_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
    set_target_properties(chaos_gc_oversized_stress_test PROPERTIES
        FOLDER "tests/contracts/native/runtime-core")
    if(CHAOS_GC_COPY_TO_BIN)
        list(APPEND CHAOS_GC_DEPLOY_TARGETS chaos_gc_oversized_stress_test)
    endif()
endif()

# memory_domain_test — MemoryDomain lifecycle + tagged allocation unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/memory_domain_test.cpp")
    add_executable(chaos_memory_domain_test memory_domain_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_memory_domain_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_memory_domain_test PRIVATE
        
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_memory_domain_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_memory_domain_test
        PRIVATE
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_memory_domain_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# threading_smoke — thread model + ThreadPool (Phase 5 thread model gap)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/threading_smoke.cpp")
    add_executable(chaos_threading_smoke threading_smoke.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_threading_smoke PRIVATE cxx_std_17)
    target_compile_definitions(chaos_threading_smoke PRIVATE
        
        _CRT_SECURE_NO_WARNINGS)
    target_include_directories(
        chaos_threading_smoke
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc")
    target_link_libraries(
        chaos_threading_smoke
        PRIVATE
            chaos_runtime_core
            chaos_bootstrap
            chaos_interpreter
            chaos_codegen
            chaos_fmt
            chaos_common
            chaos_support
            chaos_hot_update)

    # synchronization_smoke — SemaphoreSlim, RWLock, WaitHandle, TimerQueue
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/synchronization_smoke.cpp")
        add_executable(chaos_synchronization_smoke synchronization_smoke.cpp ${CHAOS_GC_TEST_STUBS})
        target_compile_features(chaos_synchronization_smoke PRIVATE cxx_std_17)
        target_compile_definitions(chaos_synchronization_smoke PRIVATE
            
            _CRT_SECURE_NO_WARNINGS)
        target_include_directories(
            chaos_synchronization_smoke
            PRIVATE
                "${REPO_ROOT}/contracts/native/v0"
                "${REPO_ROOT}/src/native/common"
                "${REPO_ROOT}/src/native/runtime-core")
        target_link_libraries(
            chaos_synchronization_smoke
            PRIVATE
                chaos_runtime_core
                chaos_bootstrap
                chaos_interpreter
                chaos_codegen
                chaos_fmt
                chaos_common
                chaos_support
                chaos_hot_update)
    endif()

    # monitor_stress_test — Monitor Enter/Exit + Wait/Pulse + GC interaction (20 threads)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/monitor_stress_test.cpp")
        add_executable(chaos_monitor_stress_test monitor_stress_test.cpp ${CHAOS_GC_TEST_STUBS})
        target_compile_features(chaos_monitor_stress_test PRIVATE cxx_std_17)
        target_compile_definitions(chaos_monitor_stress_test PRIVATE
            _CRT_SECURE_NO_WARNINGS)
        target_include_directories(
            chaos_monitor_stress_test
            PRIVATE
                "${REPO_ROOT}/contracts/native/v0"
                "${REPO_ROOT}/src/native/common"
                "${REPO_ROOT}/src/native/runtime-core"
                "${REPO_ROOT}/src/native/runtime-core/gc")
        target_link_libraries(
            chaos_monitor_stress_test
            PRIVATE
                chaos_codegen
                chaos_runtime_core
                chaos_bootstrap
                chaos_interpreter
                chaos_fmt
                chaos_common
                chaos_support
                chaos_hot_update)
        target_compile_options(chaos_monitor_stress_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    endif()

    # threading_benchmark — native microbenchmark (Release only)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/threading_benchmark.cpp")
        add_executable(chaos_threading_benchmark threading_benchmark.cpp)
        target_compile_features(chaos_threading_benchmark PRIVATE cxx_std_17)
        target_compile_definitions(chaos_threading_benchmark PRIVATE

            _CRT_SECURE_NO_WARNINGS)
        target_include_directories(
            chaos_threading_benchmark
            PRIVATE
                "${REPO_ROOT}/contracts/native/v0"
                "${REPO_ROOT}/src/native/common"
                "${REPO_ROOT}/src/native/runtime-core")
        target_link_libraries(
            chaos_threading_benchmark
            PRIVATE
                ${CHAOS_GC_COMMON_LIBS})
    endif()
endif()

# blittable_benchmark — native microbenchmark for blittable struct marshalling throughput
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/blittable_benchmark.cpp")
    add_executable(chaos_blittable_benchmark blittable_benchmark.cpp)
    target_compile_features(chaos_blittable_benchmark PRIVATE cxx_std_17)
    target_compile_definitions(chaos_blittable_benchmark PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_include_directories(
        chaos_blittable_benchmark
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core")
    target_link_libraries(
        chaos_blittable_benchmark
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# delegate_stress_test — Delegate subsystem stress (A1, A2, B1, B2, C2, E1, E3, E5)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/delegate_stress_test.cpp")
    add_executable(chaos_delegate_stress_test delegate_stress_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_delegate_stress_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_delegate_stress_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_delegate_stress_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_delegate_stress_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/src/native/hot-update"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_delegate_stress_test
        PRIVATE
            chaos_runtime_core
            chaos_bootstrap
            chaos_interpreter
            chaos_codegen
            chaos_common
            chaos_fmt
            chaos_support
            chaos_hot_update)
endif()

# gc_stress_test — T.3: 100-thread concurrent allocation + GC stress test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_stress_test.cpp")
    add_executable(chaos_gc_stress_test gc_stress_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_stress_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_stress_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_stress_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_stress_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_stress_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# ═══════════════════════════════════════════════════════════════════════════
# GC Smoke Tests — lightweight lifecycle checks
# ═══════════════════════════════════════════════════════════════════════════

# gc_worker_pool_smoke — GcWorkerPool parallel mark/sweep smoke test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_worker_pool_smoke.cpp")
    add_executable(chaos_gc_worker_pool_smoke gc_worker_pool_smoke.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_worker_pool_smoke PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_worker_pool_smoke PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_worker_pool_smoke PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_worker_pool_smoke
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_worker_pool_smoke
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_sanity_test — R12/R13 quick sanity (POH + ThinLock unload, no BDWGC init)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_sanity_test.cpp")
    add_executable(chaos_gc_sanity_test gc_sanity_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_sanity_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_sanity_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_sanity_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_sanity_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_sanity_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_bgc_smoke — BGC concurrent mark/sweep smoke test (P1 verification)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bgc_smoke.cpp")
    add_executable(chaos_gc_bgc_smoke gc_bgc_smoke.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_bgc_smoke PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_bgc_smoke PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_bgc_smoke PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_bgc_smoke
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_bgc_smoke
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_bgc_root_scan_test — BGC Phase 1b young→old root scan verification test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bgc_root_scan_test.cpp")
    add_executable(chaos_gc_bgc_root_scan_test
        gc_bgc_root_scan_test.cpp
        ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_bgc_root_scan_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_bgc_root_scan_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_bgc_root_scan_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_bgc_root_scan_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_bgc_root_scan_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_bgc_stress_test — BGC concurrent mark/sweep/compact stress test (multi-threaded)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bgc_stress_test.cpp")
    add_executable(chaos_gc_bgc_stress_test
        gc_bgc_stress_test.cpp
        ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_bgc_stress_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_bgc_stress_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_bgc_stress_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_bgc_stress_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_bgc_stress_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# exception_sentinel_test — ThreadAbort/ThreadInterrupt sentinel propagation verification
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/exception_sentinel_test.cpp")
    add_executable(chaos_exception_sentinel_test exception_sentinel_test.cpp)
    target_compile_features(chaos_exception_sentinel_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_exception_sentinel_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_exception_sentinel_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_exception_sentinel_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core")
    target_link_libraries(
        chaos_exception_sentinel_test
        PRIVATE
            chaos_runtime_core
            chaos_bootstrap
            chaos_interpreter
            chaos_codegen
            chaos_fmt
            chaos_common
            chaos_support
            chaos_hot_update)
endif()

# capacity_stress_test — 200+ DLL / 100+ hot-update DLL capacity verification
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/capacity_stress_test.cpp")
    add_executable(chaos_capacity_stress_test capacity_stress_test.cpp)
    target_compile_features(chaos_capacity_stress_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_capacity_stress_test PRIVATE
        
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_capacity_stress_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_capacity_stress_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_capacity_stress_test
        PRIVATE
            chaos_runtime_core
            chaos_bootstrap
            chaos_interpreter
            chaos_codegen
            chaos_fmt)
endif()

# codegen_native_test — native code generation + execution integration tests (Phase 3d)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/codegen_native_test.cpp")
    add_executable(chaos_codegen_native_test codegen_native_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_codegen_native_test PRIVATE cxx_std_20)
    target_compile_definitions(chaos_codegen_native_test PRIVATE
        _CRT_SECURE_NO_WARNINGS
        CHAOS_IL2CPP_CODEGEN_TEST_ACTIVE)
    target_compile_options(chaos_codegen_native_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_codegen_native_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/src/native/interpreter"
            "${REPO_ROOT}/src/native/interpreter/generated"
            "${REPO_ROOT}/src/native/codegen"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_codegen_native_test
        PRIVATE
            chaos_runtime_core
            chaos_bootstrap
            chaos_interpreter
            chaos_codegen
            chaos_fmt
            chaos_common
            chaos_support
            chaos_hot_update)
endif()

# unwind_info_test — standalone UNWIND_INFO layout byte-level verification
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/unwind_info_test.cpp")
    add_executable(chaos_unwind_info_test unwind_info_test.cpp)
    target_compile_features(chaos_unwind_info_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_unwind_info_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_unwind_info_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_unwind_info_test
        PRIVATE
            "${REPO_ROOT}/src/native/codegen"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core")
    target_link_libraries(
        chaos_unwind_info_test
        PRIVATE
            chaos_codegen)
endif()

# ============================================================
# Phase A — HIGH priority new tests
# ============================================================

# gc_scheduler_test — GcScheduler unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_scheduler_test.cpp")
    add_executable(chaos_gc_scheduler_test gc_scheduler_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_scheduler_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_scheduler_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_scheduler_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_scheduler_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_scheduler_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_parallel_mark_test — ParallelMark work-stealing unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_parallel_mark_test.cpp")
    add_executable(chaos_gc_parallel_mark_test gc_parallel_mark_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_parallel_mark_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_parallel_mark_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_parallel_mark_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_parallel_mark_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_parallel_mark_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_safepoint_test — Safepoint protocol unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_safepoint_test.cpp")
    add_executable(chaos_gc_safepoint_test gc_safepoint_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_safepoint_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_safepoint_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_safepoint_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_safepoint_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_safepoint_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_old_gen_unit_test — OldGen core operations unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_old_gen_unit_test.cpp")
    add_executable(chaos_gc_old_gen_unit_test gc_old_gen_unit_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_old_gen_unit_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_old_gen_unit_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_old_gen_unit_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_old_gen_unit_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_old_gen_unit_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_gen1_test — Gen1 collection unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_gen1_test.cpp")
    add_executable(chaos_gc_gen1_test gc_gen1_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_gen1_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_gen1_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_gen1_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_gen1_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_gen1_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_gen1_benchmark — Gen1 collection performance baseline
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_gen1_benchmark.cpp")
    add_executable(chaos_gc_gen1_benchmark gc_gen1_benchmark.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_gen1_benchmark PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_gen1_benchmark PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_gen1_benchmark PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_gen1_benchmark
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_gen1_benchmark
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# ============================================================
# Phase B — MEDIUM priority new tests
# ============================================================

# gc_mark_bitmap_test — Mark bitmap unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_mark_bitmap_test.cpp")
    add_executable(chaos_gc_mark_bitmap_test gc_mark_bitmap_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_mark_bitmap_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_mark_bitmap_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_mark_bitmap_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_mark_bitmap_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_mark_bitmap_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_bit_utils_test — Bit utilities unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bit_utils_test.cpp")
    add_executable(chaos_gc_bit_utils_test gc_bit_utils_test.cpp)
    target_compile_features(chaos_gc_bit_utils_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_bit_utils_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_bit_utils_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_bit_utils_test
        PRIVATE
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_bit_utils_test
        PRIVATE
            chaos_fmt
            chaos_common)
endif()

# gc_layout_test — GcLayoutRegistry unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_layout_test.cpp")
    add_executable(chaos_gc_layout_test gc_layout_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_layout_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_layout_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_layout_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_layout_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_layout_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_events_test — GC events unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_events_test.cpp")
    add_executable(chaos_gc_events_test gc_events_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_events_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_events_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_events_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_events_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_events_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_stats_test — GC stats unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_stats_test.cpp")
    add_executable(chaos_gc_stats_test gc_stats_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_stats_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_stats_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_stats_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_stats_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_stats_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_card_table_ext_test — Card table edge case tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_card_table_ext_test.cpp")
    add_executable(chaos_gc_card_table_ext_test gc_card_table_ext_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_card_table_ext_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_card_table_ext_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_card_table_ext_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_card_table_ext_test
        PRIVATE
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_card_table_ext_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()

# gc_tlab_test — TLAB unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_tlab_test.cpp")
    add_executable(chaos_gc_tlab_test gc_tlab_test.cpp ${CHAOS_GC_TEST_STUBS})
    target_compile_features(chaos_gc_tlab_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_tlab_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_tlab_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_tlab_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/src/native/runtime-core/gc"
            "${REPO_ROOT}/third_party/fmt/include"
            "${REPO_ROOT}/third_party/unordered_dense/include")
    target_link_libraries(
        chaos_gc_tlab_test
        PRIVATE
            ${CHAOS_GC_COMMON_LIBS})
endif()
