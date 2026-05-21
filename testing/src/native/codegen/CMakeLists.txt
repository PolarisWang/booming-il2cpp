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
)
target_compile_definitions(test_codegen_native PRIVATE
    CHAOS_IL2CPP_CODEGEN_TEST_ACTIVE=1
)
set_tests_properties(test_codegen_native PROPERTIES TIMEOUT 120)
