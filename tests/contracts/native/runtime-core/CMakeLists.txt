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
    "${REPO_ROOT}/artifacts/presets/debug/src/native/support/Debug")

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
        chaos_interpreter)

# All GC test targets need chaos_interpreter because chaos_runtime_core.lib
# contains instantiation_engine.obj which references InterpreterValue symbols,
# and chaos_bootstrap.lib references ChaosResolveExternalRuntimeFnTable
# (defined in hotpatch_resolve.cpp, part of chaos_interpreter).
set(CHAOS_GC_COMMON_LIBS chaos_runtime_core chaos_bootstrap chaos_interpreter chaos_fmt chaos_common chaos_support chaos_hot_update)

# gc_bump_cache_test — GcBumpCache size-class allocator unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bump_cache_test.cpp")
    add_executable(chaos_gc_bump_cache_test gc_bump_cache_test.cpp)
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
            chaos_fmt
            chaos_common)
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
                chaos_fmt
                chaos_common
                chaos_support
                chaos_hot_update)
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
                chaos_runtime_core
                chaos_bootstrap
                chaos_interpreter
                chaos_fmt
                chaos_common
                chaos_support
                chaos_hot_update)
    endif()
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
            chaos_fmt)
endif()
