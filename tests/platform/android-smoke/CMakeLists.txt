cmake_minimum_required(VERSION 3.20)

project(roadmap0_android_startup_smoke LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/platform/android-smoke")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

add_library(android_startup_smoke SHARED smoke_entry.cpp)
target_compile_features(android_startup_smoke PRIVATE cxx_std_17)
