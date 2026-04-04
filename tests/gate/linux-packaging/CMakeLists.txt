cmake_minimum_required(VERSION 3.20)

project(roadmap0_linux_packaging_shell LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/platform/linux-packaging")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

add_library(linux_packaging_shell STATIC packaging_shell.cpp)
target_compile_features(linux_packaging_shell PRIVATE cxx_std_17)
