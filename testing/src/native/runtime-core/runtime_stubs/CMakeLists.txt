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

# T0-3: enum_stubs unit tests
# NOTE: enum_stubs.cpp is compiled directly because the prebuilt lib
# uses GNU ar format which MSVC cannot reliably resolve even with
# extern "C" linkage.  GC stubs from gc_stubs_for_string_tests.cpp.
add_chaos_test(enum_stubs_test
    enum_stubs_test.cpp
    gc_stubs_for_string_tests.cpp
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/runtime_stubs/enum_stubs.cpp"
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)

# T0-4: datetime/guid/math stubs unit tests
# NOTE: datetime_stubs.cpp and guid_stubs.cpp are compiled directly (same GNU
# ar issue).  GC stubs from gc_stubs_for_string_tests.cpp for GcAllocateAtomic.
# math_stubs.cpp has no GC dependency but is compiled directly for consistency.
add_chaos_test(datetime_guid_math_test
    datetime_guid_math_test.cpp
    gc_stubs_for_string_tests.cpp
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/runtime_stubs/datetime_stubs.cpp"
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/runtime_stubs/guid_stubs.cpp"
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/runtime_stubs/math_stubs.cpp"
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)
# datetime/guid need NOMINMAX + WIN32_LEAN_AND_MEAN because the stub .cpp files
# include Windows.h too broadly, which collides with std::min/max and defines
# GetCurrentThreadId that thread_state.h also declares.
target_compile_definitions(datetime_guid_math_test PRIVATE
    NOMINMAX WIN32_LEAN_AND_MEAN
)
target_compile_options(datetime_guid_math_test PRIVATE
    /FI"${CMAKE_CURRENT_SOURCE_DIR}/datetime_guid_test_pch.h"
)
# T0-2: collection_stubs unit tests (all 14 public API functions)
# NOTE: collection_stubs.cpp is compiled directly (same GNU ar issue as
# string_stubs).  GC stubs from gc_stubs_for_string_tests.cpp are reused
# since CollectionListToArray calls GcAllocateAtomic.
add_chaos_test(collection_stubs_test
    collection_stubs_test.cpp
    gc_stubs_for_string_tests.cpp
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/runtime_stubs/collection_stubs.cpp"
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)

# T1a-1: convert/parse/decimal stubs unit tests
# Uses self-contained test-harness implementations matching parse_convert.cpp.
add_chaos_test(convert_stubs_test
    convert_stubs_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)

# T1a-2: exception_jmp depth-boundary unit tests
# Tests push_exception_jmp_buf/pop_exception_jmp_buf at depths 1/16/32/64/65.
# Self-contained: provides its own TLS definitions for the jmp_buf stack.
# Forces SETJMP mode to test the jmp_buf stack boundary (WIN32_SEH does not
# use push/pop).  On Windows/MSVC the default is WIN32_SEH, so we define
# CHAOS_IL2CPP_EH_SETJMP before any includes.
add_chaos_test(exception_jmp_depth_test
    exception_jmp_depth_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)
target_compile_definitions(exception_jmp_depth_test PRIVATE
    CHAOS_IL2CPP_EH_SETJMP
)
target_compile_options(exception_jmp_depth_test PRIVATE /utf-8)

# T4-1: Runtime Stubs performance benchmarks
# Compiles stub .cpp directly (same GNU ar compat pattern as unit tests).
add_chaos_test(runtime_stubs_benchmark
    runtime_stubs_benchmark.cpp
    gc_stubs_for_string_tests.cpp
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/runtime_stubs/string_stubs.cpp"
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/runtime_stubs/datetime_stubs.cpp"
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/runtime_stubs/guid_stubs.cpp"
    "${CHAOS_PROJECT_ROOT}/../src/native/runtime-core/runtime_stubs/math_stubs.cpp"
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "benchmark;core"
)
target_compile_definitions(runtime_stubs_benchmark PRIVATE
    NOMINMAX WIN32_LEAN_AND_MEAN
)
target_compile_options(runtime_stubs_benchmark PRIVATE
    /FI"${CMAKE_CURRENT_SOURCE_DIR}/datetime_guid_test_pch.h"
)
