# Hybrid verification native build
# Builds the hybrid mode test executables against the family's AOT codegen output.

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

# Hybrid mode core verification
add_chaos_test(test_jit_hybrid_mode
    ${CMAKE_CURRENT_LIST_DIR}/../../../../src/native/jit/jit_hybrid_mode_test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../../src/native/runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit;hybrid"
)
target_compile_definitions(test_jit_hybrid_mode PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)

# Hybrid end-to-end verification
add_chaos_test(test_jit_hybrid_e2e
    ${CMAKE_CURRENT_LIST_DIR}/../../../../src/native/jit/jit_hybrid_e2e_test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../../../src/native/runtime-core/gc/gc_test_stubs.cpp
    LIBS ${CHAOS_CODEGEN_LIBS}
    LABELS "unit;jit;hybrid"
)
target_compile_definitions(test_jit_hybrid_e2e PRIVATE
    CHAOS_IL2CPP_JIT_TEST_ACTIVE=1
)
