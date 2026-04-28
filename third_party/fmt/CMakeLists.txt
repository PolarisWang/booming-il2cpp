cmake_minimum_required(VERSION 3.20)
project(chaos_fmt LANGUAGES CXX)

add_library(chaos_fmt STATIC
    src/format.cc
    src/os.cc)

target_compile_features(chaos_fmt PUBLIC cxx_std_17)
target_include_directories(chaos_fmt PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")

# Static linking — do NOT define FMT_SHARED (undefined = static, no DLL annotations).
# FMT_USE_WINDOWS_H=0 prevents fmt from pulling in <windows.h> implicitly.
target_compile_definitions(chaos_fmt PUBLIC FMT_USE_WINDOWS_H=0)

# Suppress vendored library warnings and enable Unicode for MSVC.
if(MSVC)
    target_compile_options(chaos_fmt PRIVATE /w /utf-8)
else()
    target_compile_options(chaos_fmt PRIVATE -w)
endif()
