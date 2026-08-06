cmake_minimum_required(VERSION 3.20)

project(chaos_runtime_abi_compile_only_smoke LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${REPO_ROOT}/artifacts/native-abi-smoke")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${REPO_ROOT}/artifacts/native-abi-smoke")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${REPO_ROOT}/artifacts/native-abi-smoke")

# Compile-only ABI surface check (no link, no CTest).
chaos_compile_only_smoke(runtime_abi_compile_only compile_only_runtime_abi_smoke.cpp
    "${REPO_ROOT}/contracts/native/v0")

# method_table + ABI manifest executable test — factory provides flags/libs/includes/stub.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/method_table_abi_test.cpp")
    chaos_native_add_test(chaos_method_table_abi_test method_table_abi_test.cpp)
endif()
