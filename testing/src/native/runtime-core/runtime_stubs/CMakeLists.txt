# Runtime stubs unit tests — Phase 0 test safety net
# Each test target links against CHAOS_COMMON_TEST_LIBS for access to
# chaos_runtime_core.lib and related libraries.

# T0-1: string_stubs unit tests (all 19 public API functions)
# NOTE: string_stubs.cpp is compiled directly (not linked from the lib) because
# the lib uses GNU ar format (GCC/Clang) which MSVC link.exe cannot reliably
# resolve for extern "C" symbols.  We also supply stub GC/threading symbols
# via gc_stubs_for_string_tests.cpp.
add_chaos_test(string_stubs_test
    string_stubs_test.cpp
    gc_stubs_for_string_tests.cpp
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/runtime_stubs/string_stubs.cpp"
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)
