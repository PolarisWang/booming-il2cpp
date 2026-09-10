cmake_minimum_required(VERSION 3.20)

project(chaos_log_system_test LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(ARTIFACT_ROOT "${REPO_ROOT}/artifacts/native-common-test")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ARTIFACT_ROOT}")

# log_system_test — factory provides flags/libs/includes/stub.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/log_system_test.cpp")
    chaos_native_add_test(chaos_log_system_test log_system_test.cpp
        DEFINITIONS CHAOS_IL2CPP_LOG_LEVEL=3)
endif()
