cmake_minimum_required(VERSION 3.15)
project(chaos_benchmark_host LANGUAGES CXX)

get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

# ── Static library: native AOT verification host ───────────────────────────
add_library(chaos_benchmark_host STATIC
    native_aot_main.cpp)

target_compile_features(chaos_benchmark_host PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(chaos_benchmark_host PRIVATE /utf-8)
endif()
target_link_libraries(chaos_benchmark_host PUBLIC chaos_runtime_core chaos_bootstrap)
target_include_directories(chaos_benchmark_host PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

# ── EH mechanism micro-benchmarks ──────────────────────────────────────────
# Three variants of the same benchmark, each exercising a different EH backend.
# Run individually or back-to-back to compare throw/catch and happy-path
# latency across C++ exceptions, setjmp/longjmp, and Win32 SEH.
#
# Build:  cmake --build <build-dir> --target chaos_eh_benchmark_cpp_throw
# Run:    <build-dir>/src/native/benchmark-host/<config>/chaos_eh_benchmark_cpp_throw.exe

# CPP_THROW variant — standard C++ try/catch
add_executable(chaos_eh_benchmark_cpp_throw
    eh_benchmark.cpp)
target_compile_features(chaos_eh_benchmark_cpp_throw PRIVATE cxx_std_17)
target_compile_definitions(chaos_eh_benchmark_cpp_throw PRIVATE CHAOS_IL2CPP_EH_CPP_THROW)
if(MSVC)
    target_compile_options(chaos_eh_benchmark_cpp_throw PRIVATE /utf-8 /EHsc)
endif()

# SETJMP variant — setjmp/longjmp non-local goto
add_executable(chaos_eh_benchmark_setjmp
    eh_benchmark.cpp)
target_compile_features(chaos_eh_benchmark_setjmp PRIVATE cxx_std_17)
target_compile_definitions(chaos_eh_benchmark_setjmp PRIVATE CHAOS_IL2CPP_EH_SETJMP)
if(MSVC)
    target_compile_options(chaos_eh_benchmark_setjmp PRIVATE /utf-8 /EHsc)
endif()

# WIN32_SEH variant — structured exception handling via RaiseException/__except
if(WIN32)
add_executable(chaos_eh_benchmark_win32_seh
    eh_benchmark.cpp)
target_compile_features(chaos_eh_benchmark_win32_seh PRIVATE cxx_std_17)
target_compile_definitions(chaos_eh_benchmark_win32_seh PRIVATE CHAOS_IL2CPP_EH_WIN32_SEH)
if(MSVC)
    target_compile_options(chaos_eh_benchmark_win32_seh PRIVATE /utf-8 /EHca)
endif()
endif()

# Aggregate target to build all three EH benchmark variants
add_custom_target(chaos_eh_benchmark_all
    DEPENDS
        chaos_eh_benchmark_cpp_throw
        chaos_eh_benchmark_setjmp
        $<$<BOOL:$<PLATFORM_ID:Windows>>:chaos_eh_benchmark_win32_seh>
    COMMENT "Building all EH benchmark variants")