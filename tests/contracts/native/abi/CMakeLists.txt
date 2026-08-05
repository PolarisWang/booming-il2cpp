cmake_minimum_required(VERSION 3.20)

project(chaos_runtime_abi_compile_only_smoke LANGUAGES CXX)

# chaos_runtime_core.lib carries instantiation_engine.obj duplicating runtime_core.obj
# symbols; /FORCE:MULTIPLE tolerates it (same as add_chaos_test).
if(MSVC)
    add_link_options(/FORCE:MULTIPLE)
endif()

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/native-abi-smoke")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

add_library(runtime_abi_compile_only OBJECT compile_only_runtime_abi_smoke.cpp)
target_compile_features(runtime_abi_compile_only PRIVATE cxx_std_17)
target_include_directories(
    runtime_abi_compile_only
    PRIVATE
        "${REPO_ROOT}/contracts/native/v0")

# ── method_table + ABI manifest executable test ──────────────────────────────
add_executable(chaos_method_table_abi_test
    method_table_abi_test.cpp)
target_compile_features(chaos_method_table_abi_test PRIVATE cxx_std_17)
target_include_directories(
    chaos_method_table_abi_test
    PRIVATE
        "${REPO_ROOT}/src/native/runtime-core"
        "${REPO_ROOT}/src/native/common"
        "${REPO_ROOT}/contracts/native/v0")
target_link_libraries(
    chaos_method_table_abi_test
    PRIVATE
        chaos_runtime_core
        chaos_bootstrap
        chaos_interpreter
        chaos_common
        chaos_hot_update
        chaos_support
        chaos_fmt
        chaos_codegen)

