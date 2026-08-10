# Codegen subsystem tests
# jit_native_test.cpp is x64-only (guarded via #ifdef _M_X64)

set(CHAOS_CODEGEN_LIBS
    chaos_jit
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_hot_update
    chaos_common
    chaos_support
    chaos_fmt
    chaos_debugger
)

# T4-compiled entries use the JIT register convention (RBX=args_buf, RSI=ret_buf)
# rather than the Win64 C ABI.  Tests that invoke a compiled entry through their
# own ExecuteNative() (rather than through JitStubDispatch) link this x64 thunk
# to re-bind RBX/RSI so the return value lands in the caller's ret_buf.
set(CHAOS_T4_THUNK_ASM
    ${CMAKE_SOURCE_DIR}/tests/contracts/native/runtime-core/t4_jit_call.asm)
if(MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    enable_language(ASM_MASM)
    set_source_files_properties(${CHAOS_T4_THUNK_ASM} PROPERTIES LANGUAGE ASM_MASM)
endif()

# N2 — Codegen native test (x64)
# CHAOS_IL2CPP_JIT_TEST_ACTIVE=1 suppresses the codegen stubs in
# gc_test_stubs.cpp so the real implementations from chaos_jit.lib
# are used instead.
# gc_test_stubs.cpp is referenced via relative path from runtime-core/gc/
chaos_native_add_test(test_jit_native
    jit_native_test.cpp
    ${CMAKE_SOURCE_DIR}/src/native/runtime-core/gc/gc_bgc.cpp
    ${CMAKE_SOURCE_DIR}/src/native/jit/jit_helpers.cpp
    ${CHAOS_T4_THUNK_ASM}
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
# T4-compiled entries use the JIT register convention (RBX=args, RSI=ret); the
# x64 asm thunk re-binds these before invoking a compiled entry (see CHAOS_T4_THUNK_ASM).
target_compile_definitions(test_jit_native PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
target_include_directories(test_jit_native PRIVATE
    ${CMAKE_SOURCE_DIR}/src/native/runtime-core/gc)
set_tests_properties(test_jit_native PROPERTIES TIMEOUT 120)

# B-1 — Codegen IL smoke test
# Tests basic IL opcode sequences through the codegen pipeline.
# gc_test_stubs.cpp provides stubs for external symbols (aot_hotpatch_module,
# kChaosExternalRuntime*) normally supplied by codegen output.
chaos_native_add_test(test_jit_il_smoke
    jit_il_smoke_test.cpp
    ${CHAOS_T4_THUNK_ASM}
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
target_compile_definitions(test_jit_il_smoke PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)

# S-1 — Codegen SIMD test
# Tests SIMD IR instructions (SimdAdd/Sub/Mul/Min/Max/Div) through the
# T4 JIT pipeline.  Validates the EmitSimd switch cases for all supported
# element types and sub-operations.
chaos_native_add_test(test_jit_simd
    jit_simd_test.cpp
    ${CHAOS_T4_THUNK_ASM}
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
target_compile_definitions(test_jit_simd PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
set_tests_properties(test_jit_simd PROPERTIES TIMEOUT 60)

# B-1 — Codegen ABI contract test
# Verifies codegen_bridge.h struct layouts, constants, and helpers.
# gc_test_stubs.cpp provides stubs for external symbols normally supplied
# by codegen output (needed because bootstrap.lib references them).
chaos_native_add_test(test_jit_abi
    jit_abi_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
target_compile_definitions(test_jit_abi PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)

# E-1 — Codegen GC slot map test
# Tests GcRegisterSlotMap, GcUnregisterSlotMap, GcLookupSlotMap,
# GcScanPreciseFrame, GcScanFrameHybrid, and GcRegisterSlotMapsFromSection.
chaos_native_add_test(test_jit_gc_slot_map
    jit_gc_slot_map_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
target_compile_definitions(test_jit_gc_slot_map PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
set_tests_properties(test_jit_gc_slot_map PROPERTIES TIMEOUT 30)

# E-2 — Codegen OSR test
# Tests OsrResolveLoopHeader edge cases and DeoptTlsState management.
chaos_native_add_test(test_jit_osr
    jit_osr_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
target_compile_definitions(test_jit_osr PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
set_tests_properties(test_jit_osr PROPERTIES TIMEOUT 30)

# E-3 — Codegen unwind info test
# Byte-level verification of EmitUnwindInfo, EmitDwarfCie, EmitDwarfFde,
# and AllocRuntimeFunction output.
chaos_native_add_test(test_jit_unwind
    jit_unwind_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
target_compile_definitions(test_jit_unwind PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
set_tests_properties(test_jit_unwind PROPERTIES TIMEOUT 30)

# E-4 — Codegen SEH public API test
# Tests RegisterNativeCodeSection, UnregisterNativeCodeSection, FindNativeCodeByAddress, and
# RegisterJitSehHandler through library linkage.
chaos_native_add_test(test_jit_seh
    jit_seh_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
target_compile_definitions(test_jit_seh PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
set_tests_properties(test_jit_seh PROPERTIES TIMEOUT 30)

# E-5 — JIT SEH handler internal logic tests (38 tests)
# This test includes jit_seh_handler_internal.cpp directly to access static/internal
# functions (BuildSehExceptionDispatch, FindSehHandlerForOffset, etc.) that
# are private members of WinSehHandler in the production code path.
# Does NOT link chaos_jit.lib (which provides the production ISehHandler
# forwarding layer) to avoid duplicate symbol conflicts.
chaos_native_add_test(test_jit_seh_handler_internal
    jit_seh_handler_test.cpp
    LIBS
    LABELS "unit;jit"
)

# P-1 — Binary IR serialization roundtrip test
# Tests SerializeBinaryIr + DeserializeBinaryIr + ValidateBinaryIr
# for single-method RegisterMethod data.
chaos_native_add_test(test_jit_binary_ir
    jit_binary_ir_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
target_compile_definitions(test_jit_binary_ir PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
set_tests_properties(test_jit_binary_ir PROPERTIES TIMEOUT 30)

# P-2 — JIT mode end-to-end test
# Tests JitPrecode state machine, JitStubDispatchImpl dispatch, PrecodeArena
# trampoline allocation, and the full T4 JIT dispatch pipeline.
chaos_native_add_test(test_jit_mode
    jit_mode_test.cpp
    ${CHAOS_T4_THUNK_ASM}
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
target_compile_definitions(test_jit_mode PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
set_tests_properties(test_jit_mode PROPERTIES TIMEOUT 30)

# P-4 — PGO Tier 0→Tier 1 upgrade test
# Tests PGO call counting, Tier 1 recompilation, and direct_ptr patching
# behavior with enable_pgo=true/false.
chaos_native_add_test(test_jit_pgo
    jit_pgo_test.cpp
    ${CHAOS_T4_THUNK_ASM}
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit;pgo"
)
target_compile_definitions(test_jit_pgo PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
set_tests_properties(test_jit_pgo PROPERTIES TIMEOUT 600)

# B-2 — JIT benchmark (Tier 0 vs Tier 1 performance comparison)
# Measures compile time, execution time, code size, and correctness
# across methods of varying complexity for both compilation tiers.
# gc_test_stubs.cpp provides stubs for external symbols normally supplied
# by codegen output (needed because bootstrap.lib references them).
chaos_native_add_test(test_jit_bench
    bench_jit_vs_aot.cpp
    ${CHAOS_T4_THUNK_ASM}
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit;bench"
)
target_compile_definitions(test_jit_bench PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
set_tests_properties(test_jit_bench PROPERTIES TIMEOUT 120)

# wf4-debug — JIT debug info / SOS contract tests
# Validates JitDebugInfo emission in code buffer, MetadataRegistry::GetMethodName,
# and debug contract sync via RegisterNativeCodeSection.
chaos_native_add_test(test_jit_debug_info
    jit_debug_info_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
target_compile_definitions(test_jit_debug_info PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
set_tests_properties(test_jit_debug_info PROPERTIES TIMEOUT 30)

# I-1 — Inlining infrastructure unit tests
# Tests EvaluateInline heuristics (callee lookup, SEH rejection, cost model)
# and end-to-end inline compilation through the JIT pipeline.
chaos_native_add_test(test_jit_inline
    jit_inline_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit"
)
target_compile_definitions(test_jit_inline PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
set_tests_properties(test_jit_inline PROPERTIES TIMEOUT 30)
