cmake_minimum_required(VERSION 3.20)

project(chaos_unordered_map_bench LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/perf/unordered-map-bench")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

add_executable(chaos_unordered_map_bench
    unordered_map_benchmark.cpp)

target_compile_features(chaos_unordered_map_bench PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(chaos_unordered_map_bench PRIVATE /utf-8 /W0)
else()
    target_compile_options(chaos_unordered_map_bench PRIVATE -w)
endif()

target_link_libraries(
    chaos_unordered_map_bench
    PRIVATE
        chaos_unordered_dense)

# For chaos/unordered_dense.h (identity_hash)
get_filename_component(COMMON_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../src/native/common" ABSOLUTE)
target_include_directories(chaos_unordered_map_bench PRIVATE "${COMMON_ROOT}")
