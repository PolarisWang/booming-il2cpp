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
# and the codegen-symbol stub. GoogleTest is provisioned by the root CMakeLists (L11:
# the legacy "testing" CMake root, which used to own the googletest FetchContent on the
# MSVC path, is being removed).

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
