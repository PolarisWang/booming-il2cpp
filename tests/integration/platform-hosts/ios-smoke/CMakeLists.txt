cmake_minimum_required(VERSION 3.20)

project(roadmap0_ios_packaging_smoke LANGUAGES CXX OBJCXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/platform/ios-smoke")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

add_library(
    ios_packaging_smoke
    STATIC
    smoke_entry.mm
    "${REPO_ROOT}/src/mobile/shared/host_main.cpp"
    "${REPO_ROOT}/src/mobile/shared/result_reporter.cpp"
)
target_compile_features(ios_packaging_smoke PRIVATE cxx_std_17)
target_include_directories(ios_packaging_smoke PRIVATE "${REPO_ROOT}/src/mobile/shared")
