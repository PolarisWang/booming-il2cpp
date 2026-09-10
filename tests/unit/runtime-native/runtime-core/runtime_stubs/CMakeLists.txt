# Runtime stubs unit tests — Phase 0 test safety net
# Each test target links against CHAOS_COMMON_TEST_LIBS for access to
# chaos_runtime_core.lib and related libraries.

# T0-1: string_stubs unit tests (all 19 public API functions)
# NOTE: string_stubs.cpp is compiled directly (not linked from the lib) because
# the lib uses GNU ar format (GCC/Clang) which MSVC link.exe cannot reliably
# resolve for extern "C" symbols.  We also supply stub GC/threading symbols
# via gc_stubs_for_string_tests.cpp.
chaos_native_add_test(string_stubs_test
    string_stubs_test.cpp
    gc_stubs_for_string_tests.cpp
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/string_stubs.cpp"
    LABELS "unit;core"
)

# T0-3: enum_stubs unit tests
# NOTE: enum_stubs.cpp is compiled directly because the prebuilt lib
# uses GNU ar format which MSVC cannot reliably resolve even with
# extern "C" linkage.  GC stubs from gc_stubs_for_string_tests.cpp.
chaos_native_add_test(enum_stubs_test
    enum_stubs_test.cpp
    gc_stubs_for_string_tests.cpp
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/enum_stubs.cpp"
    LABELS "unit;core"
)

# T0-4: datetime/guid/math stubs unit tests
# NOTE: datetime_stubs.cpp and guid_stubs.cpp are compiled directly (same GNU
# ar issue).  GC stubs from gc_stubs_for_string_tests.cpp for GcAllocateAtomic.
# math_stubs.cpp has no GC dependency but is compiled directly for consistency.
chaos_native_add_test(datetime_guid_math_test
    datetime_guid_math_test.cpp
    gc_stubs_for_string_tests.cpp
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/datetime_stubs.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/guid_stubs.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/math_stubs.cpp"
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
chaos_native_add_test(collection_stubs_test
    collection_stubs_test.cpp
    gc_stubs_for_string_tests.cpp
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/collection_stubs.cpp"
    LABELS "unit;core"
)

# T1a-1: convert/parse/decimal stubs unit tests
# Uses self-contained test-harness implementations matching parse_convert.cpp.
chaos_native_add_test(convert_stubs_test
    convert_stubs_test.cpp
    LABELS "unit;core"
)

# P0-B A2: Unsafe / RuntimeHelpers pointer-intrinsic execute-body tests
# Compiles unsafe_stubs.cpp directly (same GNU ar compat pattern as the other
# stub tests). unsafe_stubs.cpp is self-contained (no GC/threading dependency),
# so no gc_stubs_for_string_tests.cpp is needed.
chaos_native_add_test(unsafe_stubs_test
    unsafe_stubs_test.cpp
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/unsafe_stubs.cpp"
    LABELS "unit;core"
)

# T4-1: Runtime Stubs performance benchmarks
# Compiles stub .cpp directly (same GNU ar compat pattern as unit tests).
chaos_native_add_test(runtime_stubs_benchmark
    runtime_stubs_benchmark.cpp
    gc_stubs_for_string_tests.cpp
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/string_stubs.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/datetime_stubs.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/guid_stubs.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/runtime_stubs/math_stubs.cpp"
    LABELS "benchmark;core"
)
target_compile_definitions(runtime_stubs_benchmark PRIVATE
    NOMINMAX WIN32_LEAN_AND_MEAN
)
target_compile_options(runtime_stubs_benchmark PRIVATE
    /FI"${CMAKE_CURRENT_SOURCE_DIR}/datetime_guid_test_pch.h"
)

# T1a-2b: Half conversion IEEE 754 round-to-nearest-even unit tests
# Includes convert_stubs.h directly (ChaosConvertToInt16FromDouble is FORCEINLINE).
chaos_native_add_test(half_conversion_test
    half_conversion_test.cpp
    LABELS "unit;core"
)
target_compile_options(half_conversion_test PRIVATE /utf-8)

# T1a-2: exception_jmp depth-boundary tests (SETJMP EH mode)
# Uses the same convert_stubs_test.cpp but compiled with CHAOS_IL2CPP_EH_SETJMP
# to enable push_exception_jmp_buf/pop_exception_jmp_buf testing.
# Convert tests that depend on C++ EH (RaiseManagedException) are unaffected
# because they are guarded by existing #if/#elif chains in eh.h.
chaos_native_add_test(convert_stubs_eh_test
    convert_stubs_test.cpp
    LABELS "unit;core"
)
target_compile_definitions(convert_stubs_eh_test PRIVATE
    CHAOS_IL2CPP_EH_SETJMP
)
target_compile_options(convert_stubs_eh_test PRIVATE /utf-8)
