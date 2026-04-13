cmake_minimum_required(VERSION 3.20)

project(mobile_hello_world_android_host LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../../../../.." ABSOLUTE)
set(
    CHAOS_SUBJECT_ANDROID_ARTIFACT_ROOT
    "${REPO_ROOT}/artifacts/platform/mobile-hello-world-proof/android"
    CACHE PATH
    "Runtime artifact root for the subject-owned Android host")
set(ARTIFACT_ROOT "${CHAOS_SUBJECT_ANDROID_ARTIFACT_ROOT}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

add_library(
    mobile_hello_world_android_host
    SHARED
    mobile_host_entry.cpp
    "${REPO_ROOT}/src/mobile/shared/host_main.cpp"
    "${REPO_ROOT}/src/mobile/shared/result_reporter.cpp"
)

add_executable(
    mobile_hello_world_android_host_runtime
    mobile_runtime_main.cpp
    "${REPO_ROOT}/src/mobile/shared/host_main.cpp"
    "${REPO_ROOT}/src/mobile/shared/result_reporter.cpp"
)

target_compile_features(mobile_hello_world_android_host PRIVATE cxx_std_17)
target_compile_features(mobile_hello_world_android_host_runtime PRIVATE cxx_std_17)
target_include_directories(mobile_hello_world_android_host PRIVATE "${REPO_ROOT}/src/mobile/shared")
target_include_directories(mobile_hello_world_android_host_runtime PRIVATE "${REPO_ROOT}/src/mobile/shared")
