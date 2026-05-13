cmake_minimum_required(VERSION 3.15)
project(chaos_benchmark_host LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_library(chaos_benchmark_host STATIC
    native_aot_main.cpp
    fact_runtime_main.cpp
    native_verify_main.cpp)

target_compile_features(chaos_benchmark_host PUBLIC cxx_std_17)
target_compile_options(chaos_benchmark_host PRIVATE /utf-8)
target_link_libraries(chaos_benchmark_host PUBLIC chaos_runtime_core chaos_bootstrap)
target_include_directories(chaos_benchmark_host PUBLIC "${CMAKE_CURRENT_LIST_DIR}")