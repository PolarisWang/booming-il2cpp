cmake_minimum_required(VERSION 3.20)

project(chaos_mobile_android_host LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(
    chaos_mobile_android_host
    SHARED
    jni/mobile_host_jni.cpp
    "${REPO_ROOT}/src/mobile/shared/host_main.cpp"
    "${REPO_ROOT}/src/mobile/shared/result_reporter.cpp"
)

target_compile_features(chaos_mobile_android_host PRIVATE cxx_std_17)
target_include_directories(chaos_mobile_android_host PRIVATE "${REPO_ROOT}/src/mobile/shared")
target_link_libraries(chaos_mobile_android_host PRIVATE chaos_hot_update)
