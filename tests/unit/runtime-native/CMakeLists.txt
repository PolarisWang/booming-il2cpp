# tests/unit/runtime-native/CMakeLists.txt — native CTest unit-test root.
#
# Target home for the native gtest unit suites currently under testing/src/native/
# (+ the contracts/native CTest layer). Built through the unified
# chaos_native_add_test() factory (cmake/chaos_native_test.cmake). The factory
# supplies the runtime-lib set, include dirs, /utf-8 + /FORCE:MSibling flags, and
# the codegen-symbol stub, so moved test trees do not re-implement link/flag logic.
#
# Populated incrementally by the migration plan (loop tasks): subdomains are added
# as subdirectories as they move in. Empty until then.
if(NOT TARGET chaos_test_libs_v0)
    # Ensure the factory's aggregate lib target exists when this tree is built
    # standalone (before the root include of chaos_native_test.cmake is reached).
    add_library(chaos_test_libs_v0 INTERFACE)
endif()
