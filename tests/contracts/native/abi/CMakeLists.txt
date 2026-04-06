cmake_minimum_required(VERSION 3.20)

project(chaos_runtime_abi_compile_only_smoke LANGUAGES CXX)

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

