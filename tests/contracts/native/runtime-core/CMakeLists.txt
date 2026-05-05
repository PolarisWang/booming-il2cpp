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
