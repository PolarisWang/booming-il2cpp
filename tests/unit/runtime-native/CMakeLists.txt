# tests/unit/runtime-native/CMakeLists.txt — native CTest unit-test root.
#
# Target home for the native gtest unit suites currently under testing/src/native/
# (+ the contracts/native CTest layer). Built through the unified
# chaos_native_add_test() factory (cmake/chaos_native_test.cmake). The factory
# supplies the runtime-lib set, include dirs, /utf-8 + /FORCE:MULTIPLE flags, and
# the codegen-symbol stub, so moved test trees do not re-implement link/flag logic.
#
# Subdomains are added as subdirectories as they migrate in from testing/src/native.

# Bring in the unified native-test factory + its constants (lib set, includes, stub).
include("${CMAKE_CURRENT_LIST_DIR}/../../../cmake/chaos_native_test.cmake")

# ── Migrated native subdomains (from testing/src/native) ──
add_subdirectory(support)
add_subdirectory(abi)
add_subdirectory(bootstrap)
add_subdirectory(common)
add_subdirectory(hot-update)
add_subdirectory(engine-bridge)
