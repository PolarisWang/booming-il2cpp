cmake_minimum_required(VERSION 3.20)

project(chaos_mobile_ios_host LANGUAGES CXX OBJCXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(
    chaos_mobile_ios_host
    STATIC
    bridge/mobile_host_bridge.mm
    "${REPO_ROOT}/src/mobile/shared/host_main.cpp"
    "${REPO_ROOT}/src/mobile/shared/result_reporter.cpp"
)

target_compile_features(chaos_mobile_ios_host PRIVATE cxx_std_17)
target_include_directories(chaos_mobile_ios_host PRIVATE "${REPO_ROOT}/src/mobile/shared")
target_link_libraries(chaos_mobile_ios_host PRIVATE chaos_hot_update)
