cmake_minimum_required(VERSION 3.20)

project(mobile_hello_world_ios_host LANGUAGES CXX OBJCXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/platform/mobile-hello-world-proof/ios")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

add_library(
    mobile_hello_world_ios_host
    STATIC
    mobile_host_entry.mm
    "${REPO_ROOT}/src/mobile/shared/host_main.cpp"
    "${REPO_ROOT}/src/mobile/shared/result_reporter.cpp"
)
target_compile_features(mobile_hello_world_ios_host PRIVATE cxx_std_17)
target_include_directories(mobile_hello_world_ios_host PRIVATE "${REPO_ROOT}/src/mobile/shared")
