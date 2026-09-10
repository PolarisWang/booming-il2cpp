cmake_minimum_required(VERSION 3.20)

project(chaos_engine_binding_compile_only_smoke LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/native-engine-binding-smoke")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

add_library(chaos_engine_binding_compile_only_smoke OBJECT compile_only_engine_binding_smoke.cpp)
target_compile_features(chaos_engine_binding_compile_only_smoke PRIVATE cxx_std_17)
target_include_directories(
    chaos_engine_binding_compile_only_smoke
    PRIVATE
        "${REPO_ROOT}/contracts/engine/v0"
        "${REPO_ROOT}/src/native/engine-bridge")
