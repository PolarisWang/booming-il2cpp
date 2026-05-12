cmake_minimum_required(VERSION 3.20)

project(chaos_method_aot_map_test LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/native-runtime-core-test")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

add_executable(chaos_method_aot_map_test
    method_aot_map_test.cpp)

target_compile_features(chaos_method_aot_map_test PRIVATE cxx_std_17)
target_compile_definitions(chaos_method_aot_map_test PRIVATE
    CHAOS_RUNTIME_ABI_STATIC
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

# gc_bump_cache_test — GcBumpCache size-class allocator unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_bump_cache_test.cpp")
    add_executable(chaos_gc_bump_cache_test gc_bump_cache_test.cpp)
    target_compile_features(chaos_gc_bump_cache_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_bump_cache_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_include_directories(
        chaos_gc_bump_cache_test
        PRIVATE
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/third_party/fmt/include")
    target_link_libraries(
        chaos_gc_bump_cache_test
        PRIVATE
            chaos_bdwgc
            chaos_fmt)
endif()

# gc_region_test — CRAG region framework + card table unit tests (C0.1-C0.3)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_region_test.cpp")
    add_executable(chaos_gc_region_test gc_region_test.cpp)
    target_compile_features(chaos_gc_region_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_region_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_region_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_region_test
        PRIVATE
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/third_party/bdwgc/include"
            "${REPO_ROOT}/third_party/fmt/include")
    target_link_libraries(
        chaos_gc_region_test
        PRIVATE
            chaos_runtime_core
            chaos_bootstrap
            chaos_fmt
            chaos_bdwgc)
endif()

# gc_root_scanner_test — GcSlotMap registry + hybrid root scanning unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_root_scanner_test.cpp")
    add_executable(chaos_gc_root_scanner_test gc_root_scanner_test.cpp)
    target_compile_features(chaos_gc_root_scanner_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_root_scanner_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_include_directories(
        chaos_gc_root_scanner_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/third_party/bdwgc/include"
            "${REPO_ROOT}/third_party/fmt/include")
    target_link_libraries(
        chaos_gc_root_scanner_test
        PRIVATE
            chaos_runtime_core
            chaos_bootstrap
            chaos_bdwgc
            chaos_fmt)
endif()

# domain_unload_test — C4: HotUpdate domain unloader with cross-domain reference checking
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/domain_unload_test.cpp")
    add_executable(chaos_domain_unload_test domain_unload_test.cpp)
    target_compile_features(chaos_domain_unload_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_domain_unload_test PRIVATE
        CHAOS_RUNTIME_ABI_STATIC
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_domain_unload_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_domain_unload_test
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/third_party/bdwgc/include"
            "${REPO_ROOT}/third_party/fmt/include")
    target_link_libraries(
        chaos_domain_unload_test
        PRIVATE
            chaos_runtime_core
            chaos_bootstrap
            chaos_fmt
            chaos_bdwgc)
endif()

# gc_young_collector_test — Cheney copying young GC unit tests (C2.3-C2.6)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_young_collector_test.cpp")
    add_executable(chaos_gc_young_collector_test gc_young_collector_test.cpp)
    target_compile_features(chaos_gc_young_collector_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_gc_young_collector_test PRIVATE
        _CRT_SECURE_NO_WARNINGS)
    target_compile_options(chaos_gc_young_collector_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    target_include_directories(
        chaos_gc_young_collector_test
        PRIVATE
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/third_party/bdwgc/include"
            "${REPO_ROOT}/third_party/fmt/include")
    target_link_libraries(
        chaos_gc_young_collector_test
        PRIVATE
            chaos_runtime_core
            chaos_bootstrap
            chaos_fmt
            chaos_bdwgc)
endif()

# memory_domain_test — MemoryDomain lifecycle + tagged allocation unit tests
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/memory_domain_test.cpp")
    add_executable(chaos_memory_domain_test memory_domain_test.cpp)
    target_compile_features(chaos_memory_domain_test PRIVATE cxx_std_17)
    target_compile_definitions(chaos_memory_domain_test PRIVATE
        CHAOS_RUNTIME_ABI_STATIC
        _CRT_SECURE_NO_WARNINGS)
    target_include_directories(
        chaos_memory_domain_test
        PRIVATE
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core"
            "${REPO_ROOT}/third_party/bdwgc/include"
            "${REPO_ROOT}/third_party/fmt/include")
    target_link_libraries(
        chaos_memory_domain_test
        PRIVATE
            chaos_runtime_core
            chaos_bootstrap
            chaos_bdwgc
            chaos_fmt)
endif()

# threading_smoke — thread model + ThreadPool (Phase 5 thread model gap)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/threading_smoke.cpp")
    add_executable(chaos_threading_smoke threading_smoke.cpp)
    target_compile_features(chaos_threading_smoke PRIVATE cxx_std_17)
    target_compile_definitions(chaos_threading_smoke PRIVATE
        CHAOS_RUNTIME_ABI_STATIC
        _CRT_SECURE_NO_WARNINGS)
    target_include_directories(
        chaos_threading_smoke
        PRIVATE
            "${REPO_ROOT}/contracts/native/v0"
            "${REPO_ROOT}/src/native/common"
            "${REPO_ROOT}/src/native/runtime-core")
    target_link_libraries(
        chaos_threading_smoke
        PRIVATE
            chaos_runtime_core
            chaos_bdwgc)

    # threading_benchmark — native microbenchmark (Release only)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/threading_benchmark.cpp")
        add_executable(chaos_threading_benchmark threading_benchmark.cpp)
        target_compile_features(chaos_threading_benchmark PRIVATE cxx_std_17)
        target_compile_definitions(chaos_threading_benchmark PRIVATE
            CHAOS_RUNTIME_ABI_STATIC
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
                chaos_interpreter)
    endif()
endif()

# gc_stress_test — T.3: 100-thread concurrent allocation + GC stress test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gc_stress_test.cpp")
    add_executable(chaos_gc_stress_test gc_stress_test.cpp)
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
            "${REPO_ROOT}/third_party/bdwgc/include"
            "${REPO_ROOT}/third_party/fmt/include")
    target_link_libraries(
        chaos_gc_stress_test
        PRIVATE
            chaos_runtime_core
            chaos_bootstrap
            chaos_fmt
            chaos_bdwgc)
endif()
