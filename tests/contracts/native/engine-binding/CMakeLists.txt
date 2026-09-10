cmake_minimum_required(VERSION 3.20)

project(chaos_engine_binding_compile_only_smoke LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${REPO_ROOT}/artifacts/native-engine-binding-smoke")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${REPO_ROOT}/artifacts/native-engine-binding-smoke")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${REPO_ROOT}/artifacts/native-engine-binding-smoke")

# Compile-only engine-bridge ABI surface check (no link, no CTest).
chaos_compile_only_smoke(chaos_engine_binding_compile_only_smoke compile_only_engine_binding_smoke.cpp
    "${REPO_ROOT}/contracts/engine/v0"
    "${REPO_ROOT}/src/native/engine-bridge")
