# Parallel.For A/B + crash-reproduction probes (T1.4 verification).
#
# These are STANDALONE executables, not ctest cases: they are investigation
# instruments, run by hand, that measure one dispatch policy against another
# and reproduce the large-range truncation.  They are excluded from the default
# build (EXCLUDE_FROM_ALL) so the normal test build never pays for them; build
# a target by name when you need one.
#
# They link the chaos static libraries directly and supply their own main(),
# which is why they cannot use the chaos_native_add_test factory.
#
# Usage (from the repo root):
#   cmake --build artifacts/presets/windows-x64-reference --config Release \
#         --target parallel_ab_post parallel_ab_pre
#   ./artifacts/presets/windows-x64-reference/tests/.../parallel_ab_post

cmake_minimum_required(VERSION 3.20)
project(parallel_probe CXX)

# The runtime headers use nested namespace definitions, which need C++17.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(_probe_dir "${CMAKE_CURRENT_SOURCE_DIR}")
function(parallel_probe_add target main_src impl_src)
    add_executable(${target} EXCLUDE_FROM_ALL
        "${main_src}"
        "ab_stubs.cpp"
        "${impl_src}"
    )
    target_include_directories(${target} PRIVATE
        "${CHAOS_CMAKE_ROOT}/src/native/runtime-core"
        "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/gc"
        "${CHAOS_CMAKE_ROOT}/src/native/pal"
        "${CHAOS_CMAKE_ROOT}/src/native/common"
        "${CHAOS_CMAKE_ROOT}/src/native/diagnostics"
        "${CHAOS_CMAKE_ROOT}/third_party/fmt/include"
        "${CHAOS_CMAKE_ROOT}/third_party/unordered_dense/include"
    )
    target_compile_definitions(${target} PRIVATE
        CHAOS_PLATFORM_WINDOWS=1 WIN32_LEAN_AND_MEAN NOMINMAX
    )
    target_compile_options(${target} PRIVATE /utf-8 /EHsc)
    # /FORCE:MULTIPLE tolerates the duplicate stubs the static libs carry.
    target_link_options(${target} PRIVATE /FORCE:MULTIPLE)
    # Prebuilt static libs from the reference preset.  CHAOS_REF_ROOT defaults to
    # that preset; override it if you build the runtime elsewhere.
    set(_ref "${CHAOS_CMAKE_ROOT}/artifacts/presets/windows-x64-reference")
    if(DEFINED CHAOS_REF_ROOT)
        set(_ref "${CHAOS_REF_ROOT}")
    endif()
    target_link_directories(${target} PRIVATE
        "${_ref}/src/native/runtime-core/Release"
        "${_ref}/src/native/jit/Release"
        "${_ref}/src/native/interpreter/Release"
        "${_ref}/src/native/support/Release"
        "${_ref}/src/native/bootstrap/Release"
        "${_ref}/src/native/diagnostics/debugger/Release"
        "${_ref}/src/native/hot-update/Release"
        "${_ref}/src/native/common/Release"
        "${_ref}/src/native/pal/Release"
        "${_ref}/fmt_build/Release"
    )
    target_link_libraries(${target} PRIVATE
        chaos_runtime_core chaos_interpreter chaos_jit chaos_support
        chaos_bootstrap chaos_debugger chaos_hot_update chaos_common chaos_pal chaos_fmt
    )
endfunction()

# A/B: same harness, two dispatch policies, one binary each.  Two binaries (not
# one) because the first run warms the pool for the second, and the worker-count
# delta is the headline metric.
parallel_probe_add(parallel_ab_post "parallel_ab_main.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/parallel.cpp")
parallel_probe_add(parallel_ab_pre  "parallel_ab_main.cpp"
    "parallel_prefix.cpp")

# Per-index coverage probe: records which indices the body actually saw, so a
# lost invocation cannot hide behind an aggregate counter.
parallel_probe_add(parallel_cov "coverage_main.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/parallel.cpp")

# Instrumented copy: prints every chunk claim/release so the claim span and the
# body-invocation count can be compared directly.
parallel_probe_add(parallel_inst "crash_repro_main.cpp"
    "parallel_instrumented.cpp")

# Sanity probe: same body, three delivery paths (direct / delegate / Parallel),
# to isolate whether a lost count is the runtime's or the recording's.
parallel_probe_add(parallel_sanity "sanity_main.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/parallel.cpp")

# Rendezvous probe: does the caller wait for the chunks it dispatched?  Uses a
# slow body so a still-running chunk is unambiguous, and reports the completion
# count at the instant the caller returns.
parallel_probe_add(parallel_race "race_main.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/parallel.cpp")

# Span probe: which indices a fast body actually visits, across a range of
# sizes, to locate where the shortfall starts.
parallel_probe_add(parallel_span "span_main.cpp"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/parallel.cpp")
