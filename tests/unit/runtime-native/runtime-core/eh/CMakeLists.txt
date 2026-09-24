# ── EH mode-equivalence suite ────────────────────────────────────────────
#
# The SAME source file is compiled three times, once per delivery mechanism.
# Every case asserts a mode-independent outcome, so a regression in any one
# expansion turns exactly one target red.  This is the CI gate that iron
# rule 10 ("改动 EH 必跑三模式用例") depends on.
#
# Why not fold this into the existing convert_stubs_eh_test target: that one
# exercises SETJMP only, via convert stubs, and its assertions are about
# conversion semantics.  Here the subject under test is chaos/eh.h itself.
#
# Cost note: each target is a single small TU with no codegen dependency, so
# the ×3 is cheap — unlike recompiling a whole chunk per mode, which was
# explicitly rejected during planning.

set(_eh_suite "${CMAKE_CURRENT_SOURCE_DIR}/eh_mode_equivalence_test.cpp")

# Non-gtest: the suite has its own main() and prints its own summary.
#
# The shared codegen stub (gc_test_stubs.cpp) MUST be linked: chaos_runtime_core
# references codegen-emitted symbols (kChaosExceptionTypes, kChaosExternalRuntime*,
# chaos_il2cpp_aot_hotpatch_module) that only exist after a foundation-dll run.
# Without it the build stops at LNK2001/LNK1120 rather than at a test failure.
set(_eh_common_args
    ${_eh_suite}
    LABELS "unit;core;eh"
    WITHOUT_GTEST
)

# ── Mode 1: CPP_THROW (macOS / Linux) ────────────────────────────────────
# The define is REQUIRED, not optional.  chaos/config.h picks a mode from the
# target platform when none is given — on Windows that default is WIN32_SEH —
# so a "default mode" target would silently exercise a different expansion
# than its name claims.  It was observed doing exactly that: the suite printed
# "[WIN32_SEH]" while running the cpp_throw target.  Same trap on Linux, where
# the implicit default is CPP_THROW and the setjmp target must still be explicit.
chaos_native_add_test(eh_mode_equivalence_cpp_throw
    ${_eh_common_args}
    DEFINITIONS CHAOS_IL2CPP_EH_CPP_THROW
)

# ── Mode 2: WIN32_SEH (Windows) ──────────────────────────────────────────
# MSVC-only: __try/__except are Microsoft extensions.  Guarding on the
# compiler (not on CMAKE_SYSTEM_NAME) keeps a MinGW or clang-cl build from
# silently declaring the SEH expansion green when it never compiled.
if(MSVC)
    chaos_native_add_test(eh_mode_equivalence_win32_seh
        ${_eh_common_args}
        DEFINITIONS CHAOS_IL2CPP_EH_WIN32_SEH
    )
endif()

# ── Mode 3: SETJMP (iOS / Android) ───────────────────────────────────────
# Portable — setjmp/longjmp are standard C.  Runs on every platform so the
# mobile delivery path is exercised even on a Windows/Linux dev box.
chaos_native_add_test(eh_mode_equivalence_setjmp
    ${_eh_common_args}
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/exception_helpers.cpp"
    DEFINITIONS CHAOS_IL2CPP_EH_SETJMP
)

# ── EH L3 type-matching suite ────────────────────────────────────────────
# Second subject: not "did the exception reach a handler" (covered above) but
# "did the CORRECT handler run".  A mismatched catch silently taking an
# exception is the failure mode that makes wrong assertions look green.
#
# Same three-mode treatment as the mode-equivalence suite, same rationale for
# explicit DEFINITIONS (chaos/config.h otherwise picks a mode from the host
# platform, so a target named cpp_throw would silently run the SEH expansion —
# observed happening for real).
set(_eh_l3_suite "${CMAKE_CURRENT_SOURCE_DIR}/eh_type_matching_test.cpp")

chaos_native_add_test(eh_type_matching_cpp_throw
    ${_eh_l3_suite}
    LABELS "unit;core;eh"
    WITHOUT_GTEST
    DEFINITIONS CHAOS_IL2CPP_EH_CPP_THROW
)

if(MSVC)
    chaos_native_add_test(eh_type_matching_win32_seh
        ${_eh_l3_suite}
        LABELS "unit;core;eh"
        WITHOUT_GTEST
        DEFINITIONS CHAOS_IL2CPP_EH_WIN32_SEH
    )
endif()

chaos_native_add_test(eh_type_matching_setjmp
    ${_eh_l3_suite}
    "${CHAOS_CMAKE_ROOT}/src/native/runtime-core/exception_helpers.cpp"
    LABELS "unit;core;eh"
    WITHOUT_GTEST
    DEFINITIONS CHAOS_IL2CPP_EH_SETJMP
)
