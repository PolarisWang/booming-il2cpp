# Codegen subsystem tests
# codegen_native_test.cpp is x64-only (guarded via #ifdef _M_X64)

set(CHAOS_CODEGEN_LIBS
    chaos_codegen
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_hot_update
    chaos_common
    chaos_support
    chaos_fmt
)

# N2 — Codegen native test (x64)
# CHAOS_IL2CPP_CODEGEN_TEST_ACTIVE=1 suppresses the codegen stubs in
# gc_test_stubs.cpp so the real implementations from chaos_codegen.lib
# are used instead.
# gc_test_stubs.cpp is referenced via relative path from runtime-core/gc/
add_chaos_test(test_codegen_native
    codegen_native_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;codegen"
)
target_compile_definitions(test_codegen_native PRIVATE
    CHAOS_IL2CPP_CODEGEN_TEST_ACTIVE=1
)
set_tests_properties(test_codegen_native PROPERTIES TIMEOUT 120)

# B-1 — Codegen IL smoke test
# Tests basic IL opcode sequences through the codegen pipeline.
# gc_test_stubs.cpp provides stubs for external symbols (aot_hotpatch_module,
# kChaosExternalRuntime*) normally supplied by codegen output.
add_chaos_test(test_codegen_il_smoke
    codegen_il_smoke_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;codegen"
)
target_compile_definitions(test_codegen_il_smoke PRIVATE
    CHAOS_IL2CPP_CODEGEN_TEST_ACTIVE=1
)

# B-1 — Codegen ABI contract test
# Verifies codegen_bridge.h struct layouts, constants, and helpers.
# gc_test_stubs.cpp provides stubs for external symbols normally supplied
# by codegen output (needed because bootstrap.lib references them).
add_chaos_test(test_codegen_abi
    codegen_abi_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;codegen"
)
target_compile_definitions(test_codegen_abi PRIVATE
    CHAOS_IL2CPP_CODEGEN_TEST_ACTIVE=1
)

# E-1 — Codegen GC slot map test
# Tests GcRegisterSlotMap, GcUnregisterSlotMap, GcLookupSlotMap,
# GcScanPreciseFrame, GcScanFrameHybrid, and GcRegisterSlotMapsFromSection.
add_chaos_test(test_codegen_gc_slot_map
    codegen_gc_slot_map_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;codegen"
)
target_compile_definitions(test_codegen_gc_slot_map PRIVATE
    CHAOS_IL2CPP_CODEGEN_TEST_ACTIVE=1
)
set_tests_properties(test_codegen_gc_slot_map PROPERTIES TIMEOUT 30)

# E-2 — Codegen OSR test
# Tests OsrResolveLoopHeader edge cases and DeoptTlsState management.
add_chaos_test(test_codegen_osr
    codegen_osr_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;codegen"
)
target_compile_definitions(test_codegen_osr PRIVATE
    CHAOS_IL2CPP_CODEGEN_TEST_ACTIVE=1
)
set_tests_properties(test_codegen_osr PROPERTIES TIMEOUT 30)

# E-3 — Codegen unwind info test
# Byte-level verification of EmitUnwindInfo, EmitDwarfCie, EmitDwarfFde,
# and AllocRuntimeFunction output.
add_chaos_test(test_codegen_unwind
    codegen_unwind_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;codegen"
)
target_compile_definitions(test_codegen_unwind PRIVATE
    CHAOS_IL2CPP_CODEGEN_TEST_ACTIVE=1
)
set_tests_properties(test_codegen_unwind PROPERTIES TIMEOUT 30)

# E-4 — Codegen SEH public API test
# Tests RegisterT4Code, UnregisterT4Code, FindT4CodeByAddress, and
# RegisterT4SehHandler through library linkage.
add_chaos_test(test_codegen_seh
    codegen_seh_test.cpp
    ../runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;codegen"
)
target_compile_definitions(test_codegen_seh PRIVATE
    CHAOS_IL2CPP_CODEGEN_TEST_ACTIVE=1
)
set_tests_properties(test_codegen_seh PROPERTIES TIMEOUT 30)
