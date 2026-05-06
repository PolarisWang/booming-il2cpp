cmake_minimum_required(VERSION 3.20)

project(chaos_log_system_test LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/native-common-test")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

add_executable(chaos_log_system_test
    log_system_test.cpp)

target_compile_features(chaos_log_system_test PRIVATE cxx_std_17)
target_compile_definitions(chaos_log_system_test PRIVATE
    CHAOS_IL2CPP_LOG_LEVEL=3
    _CRT_SECURE_NO_WARNINGS)

if(MSVC)
    target_compile_options(chaos_log_system_test PRIVATE /utf-8)
endif()
target_include_directories(
    chaos_log_system_test
    PRIVATE
        "${REPO_ROOT}/src/native/common"
        "${REPO_ROOT}/src/native/runtime-core")
target_link_libraries(
    chaos_log_system_test
    PRIVATE
        chaos_common)
