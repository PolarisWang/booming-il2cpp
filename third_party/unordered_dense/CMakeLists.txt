cmake_minimum_required(VERSION 3.20)
project(chaos_unordered_dense LANGUAGES CXX)

add_library(chaos_unordered_dense INTERFACE)
target_include_directories(chaos_unordered_dense INTERFACE "${CMAKE_CURRENT_LIST_DIR}/include")

# Suppress vendored library warnings.
if(MSVC)
    target_compile_options(chaos_unordered_dense INTERFACE /w)
else()
    target_compile_options(chaos_unordered_dense INTERFACE -w)
endif()
