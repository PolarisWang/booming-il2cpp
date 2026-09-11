cmake_minimum_required(VERSION 3.20)
# CMake 4.x requires an explicit cmake_minimum_required + direct project() call at
# the top of every top-level CMakeLists (CMP0000 is a hard error, not a warning).
# ASM_MASM is enabled conditionally in the jit subdir (MSVC x64 only), so it is not
# declared here — that would break the Linux configuration.
project(chaos_native_unit_tests LANGUAGES CXX)

# tests/unit/runtime-native/CMakeLists.txt — native CTest unit-test root.
#
# Target home for the native gtest unit suites (formerly under testing/src/native).
# Built through the unified chaos_native_add_test() factory (cmake/chaos_native_test.cmake).
# The factory supplies the runtime-lib set, include dirs, /utf-8 + /FORCE:MULTIPLE flags,
# and the codegen-symbol stub.
#
# GoogleTest provisioning: normally the root CMakeLists does it (per-preset
# branch).  But this directory is ALSO configured as a standalone project by
# jit-unit.yml ("cmake -S tests/unit/runtime-native -B ..."), and in that mode
# the root never runs — leaving gtest_main/gtest undefined and producing
#   C1083: cannot open include file: 'gtest/gtest.h'
#   LNK1181: cannot open input file 'gtest_main.lib'
# Provision it here when it is not already available, so both invocation modes
# work.  The `if(NOT TARGET gtest_main)` guard keeps the nested case a no-op
# (root already created the targets).
if(NOT TARGET gtest_main)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
    )
    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    else()
        set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
    endif()
    FetchContent_MakeAvailable(googletest)
endif()

# Bring in the unified native-test factory + its constants (lib set, includes, stub).
include("${CMAKE_CURRENT_LIST_DIR}/../../../cmake/chaos_native_test.cmake")

# ── Migrated native subdomains ──
add_subdirectory(support)
add_subdirectory(abi)
add_subdirectory(bootstrap)
add_subdirectory(common)
add_subdirectory(hot-update)
add_subdirectory(diagnostics)
add_subdirectory(engine-bridge)
add_subdirectory(runtime-core)
add_subdirectory(jit)
add_subdirectory(fuzz)
