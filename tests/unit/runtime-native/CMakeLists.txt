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
