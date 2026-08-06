# cmake/chaos_native_test.cmake — unified factory for native (C++) test targets.
#
# Single source of truth for the runtime-lib set, include dirs, compiler/link
# flags (/utf-8, /FORCE:MULTIPLE), and the codegen-generated symbol stub that a
# standalone source build needs. Used by BOTH test trees so we stop re-implementing
# these in every CMakeLists.txt:
#   - tests/contracts/native/**   (the contracts-native CTest layer)
#   - testing/src/native/**       (the gtest unit/stress/benchmark layer)
#
# The foundation-dll pipeline (TPG scriban template) is a SEPARATE generator and is
# deliberately NOT wired through this module — it links real native-aot.generated.cpp
# (which defines the codegen symbols) and sets its own /FORCE:MULTIPLE, so it is left
# untouched.

include_guard(GLOBAL)

# ── Path anchors (self-contained: anchor to the repo root, not to tree-private vars) ──
set(CHAOS_CMAKE_ROOT "${CMAKE_SOURCE_DIR}")

# ── Unified runtime link-lib set (replaces the repeated per-tree vars:
#    CHAOS_GC_COMMON_LIBS / CHAOS_FULL_LIBS / CHAOS_THREADING_LIBS / CHAOS_COMMON_TEST_LIBS) ──
# The libs are imported GLOBAL (add_chaos_imported_lib) or are real targets from the
# src/native build; either way the target NAMES are the single entry point.
set(CHAOS_RUNTIME_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_jit
    chaos_common
    chaos_support
    chaos_hot_update
    chaos_fmt
    chaos_codegen
    chaos_debugger)

# INTERFACE aggregate so test CMakeLists never list individual lib targets.
# Tries the real/imported target; if absent (e.g. some configs) it's a no-op list.
if(NOT TARGET chaos_test_libs_v0)
    add_library(chaos_test_libs_v0 INTERFACE)
endif()
target_link_libraries(chaos_test_libs_v0 INTERFACE ${CHAOS_RUNTIME_LIBS})

# ── Unified include set (single source; mirrors the old per-tree CHAOS_INCLUDE_DIRS) ──
set(CHAOS_NATIVE_INCLUDE_DIRS
    "${CHAOS_CMAKE_ROOT}/src/native"
    "${CHAOS_CMAKE_ROOT}/src/native/common"
    "${CHAOS_CMAKE_ROOT}/src/native/interpreter"
    "${CHAOS_CMAKE_ROOT}/src/native/interpreter/generated"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core"
    "${CHAOS_CMAKE_ROOT}/src/native/bootstrap"
    "${CHAOS_CMAKE_ROOT}/src/native/hot-update"
    "${CHAOS_CMAKE_ROOT}/src/native/support"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/threading"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/tiering"
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/gc"
    "${CHAOS_CMAKE_ROOT}/src/native/pal"
    "${CHAOS_CMAKE_ROOT}/src/native/jit"
    "${CHAOS_CMAKE_ROOT}/src/native/engine-bridge"
    "${CHAOS_CMAKE_ROOT}/contracts/native/v0"
    "${CHAOS_CMAKE_ROOT}/contracts/engine/v0"
    "${CHAOS_CMAKE_ROOT}/third_party/unordered_dense/include"
    "${CHAOS_CMAKE_ROOT}/third_party/fmt/include")

# ── Unified codegen-symbol stub ──
# In a source-only build the runtime libs reference codegen-generated extern symbols
# (kChaosExternalRuntime*, chaos_il2cpp_aot_hotpatch_module) that only exist after a
# foundation-dll codegen run. Standalone test targets must define them; gc_test_stubs.cpp
# is the canonical, null/count-0-safe stub (the runtime paths guard on count<=0 / null).
set(CHAOS_CODEGEN_STUB
    "${CHAOS_CMAKE_ROOT}/tests/contracts/native/runtime-core/gc_test_stubs.cpp")
if(EXISTS "${CHAOS_CODEGEN_STUB}")
    set(CHAOS_CODEGEN_STUB_EXISTS TRUE)
else()
    set(CHAOS_CODEGEN_STUB_EXISTS FALSE)
endif()

# ── Unified native test factory ──
#   chaos_native_add_test(name SOURCES s1.cpp [s2.cpp]
#                         [LIBS a b c]            # default: ${CHAOS_RUNTIME_LIBS}
#                         [INCLUDES dir ...]      # default: ${CHAOS_NATIVE_INCLUDE_DIRS}
#                         [LABELS l1;l2]
#                         [WITHOUT_GTEST]         # no gtest_main link (non-gtest exe)
#                         [WITHOUT_CODEGEN_STUB]) # don't link the codegen stub
# Registers a CTest target with the standard MSVC/Linux flags and include dirs.
function(chaos_native_add_test name)
    cmake_parse_arguments(ARG "WITHOUT_GTEST;WITHOUT_CODEGEN_STUB;WITHOUT_UTF8;GTEST_LIB_ONLY"
                          "CXX_STANDARD" "LIBS;INCLUDES;LABELS;DEFINITIONS" ${ARGN})

    set(_sources ${ARG_UNPARSED_ARGUMENTS})
    if(NOT ARG_WITHOUT_CODEGEN_STUB AND CHAOS_CODEGEN_STUB_EXISTS)
        list(APPEND _sources ${CHAOS_CODEGEN_STUB})
    endif()

    add_executable(${name} ${_sources})
    if(ARG_CXX_STANDARD)
        target_compile_features(${name} PRIVATE cxx_std_${ARG_CXX_STANDARD})
    else()
        target_compile_features(${name} PRIVATE cxx_std_17)
    endif()
    set(_defs _CRT_SECURE_NO_WARNINGS)
    if(ARG_DEFINITIONS)
        list(APPEND _defs ${ARG_DEFINITIONS})
    endif()
    target_compile_definitions(${name} PRIVATE ${_defs})

    set(_libs ${ARG_LIBS})
    if(NOT _libs)
        set(_libs chaos_test_libs_v0)
    endif()
    if(ARG_WITHOUT_GTEST)
        # no gtest at all (bare executable)
    elseif(ARG_GTEST_LIB_ONLY)
        set(_libs gtest ${_libs})          # custom main() tests: link gtest lib only
    else()
        set(_libs gtest_main ${_libs})     # default GoogleTest entry point
    endif()

    if(MSVC)
        target_link_libraries(${name} PRIVATE ${_libs})
        target_link_options(${name} PRIVATE /FORCE:MULTIPLE)
        set(_opts /GS- /W3 /bigobj)
        if(NOT ARG_WITHOUT_UTF8)
            list(APPEND _opts /utf-8)
        endif()
        target_compile_options(${name} PRIVATE ${_opts})
    else()
        # Linux: wrap the static chaos libs in --start-group/--end-group to resolve
        # circular deps; allow duplicate TLS defs like the old add_chaos_test.
        target_link_libraries(${name} PRIVATE
            ${_libs}
            -Wl,--start-group
            ${_libs}
            -Wl,--end-group)
        target_link_options(${name} PRIVATE
            -Wl,--allow-multiple-definition
            -Wl,--undefined=JitRecompileToTier1)
    endif()

    set(_inc ${CHAOS_NATIVE_INCLUDE_DIRS})
    if(ARG_INCLUDES)
        list(APPEND _inc ${ARG_INCLUDES})
    endif()
    target_include_directories(${name} PRIVATE ${_inc})

    add_test(NAME ${name} COMMAND ${name})
    if(ARG_LABELS)
        set_tests_properties(${name} PROPERTIES LABELS "${ARG_LABELS}")
    endif()
endfunction()

# ── Auto-discovery helper ──
#   chaos_native_glob_add_tests(TARGET_PREFIX GLOB_PATTERN
#                               [EXCLUDE a.cpp b.cpp] [EXTRA_NAME_WE ...])
#
# file(GLOB)s sources matching GLOB_PATTERN (CONFIGURE_DEPENDS) and creates one
# standard chaos_native_add_test per file:  <prefix>_<name_we> from <name_we>.cpp.
# Sources listed under EXCLUDE (multi-file, GLOB-relative to the dir) are skipped so
# callers can keep special targets (custom CXX_STANDARD / extra sources) explicit.
# Returns the created target names in a caller variable `CHACREATED`.
function(chaos_native_glob_add_tests prefix pattern)
    cmake_parse_arguments(A "" "" "EXCLUDE" "" ${ARGN})
    file(GLOB_RECURSE _srcs CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${pattern}")
    foreach(_src IN LISTS _srcs)
        get_filename_component(_name_we ${_src} NAME_WE)
        set(_skip FALSE)
        foreach(_ex IN LISTS A_EXCLUDE)
            get_filename_component(_ex_we ${_ex} NAME_WE)
            if(_name_we STREQUAL _ex_we)
                set(_skip TRUE)
                break()
            endif()
        endforeach()
        if(NOT _skip)
            chaos_native_add_test(${prefix}_${_name_we} ${_src})
        endif()
    endforeach()
endfunction()

# ── Compile-only ABI/build smoke helper ──
#   chaos_compile_only_smoke(name source [include_dir ...])
#
# Builds header/ABI contract checkers as OBJECT libraries (no link, no CTest) —
# the pattern used by tests/contracts/native/{abi,engine-binding,bridge}. Removes
# the duplicated REPO_ROOT / ARTIFACT_ROOT / cxx_std_17 / include-dir boilerplate.
function(chaos_compile_only_smoke name source)
    add_library(${name} OBJECT ${source})
    target_compile_features(${name} PRIVATE cxx_std_17)
    target_include_directories(${name} PRIVATE ${ARGN})
endfunction()
