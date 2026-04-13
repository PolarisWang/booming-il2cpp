cmake_minimum_required(VERSION 3.20)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(legacy_gate_linux_packaging_wrapper LANGUAGES NONE)
endif()

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../integration/platform-hosts/linux-packaging" "${CMAKE_CURRENT_BINARY_DIR}/canonical")
