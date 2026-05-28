cmake_minimum_required(VERSION 3.20)
project(chaos_codegen LANGUAGES CXX)

# Phase 3d placeholder: codegen INTERFACE library.
# Linked by test targets that reference -lchaos_codegen.
# Actual codegen headers/sources are added per-platform in the full build.
add_library(chaos_codegen INTERFACE)
target_include_directories(chaos_codegen INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}")
