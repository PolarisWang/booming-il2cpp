# Hot-update subsystem tests
# Note: with STATIC IMPORTED targets, ALL transitive library dependencies must
# be listed explicitly.

set(CHAOS_HOTUPDATE_LIBS
    chaos_hot_update
    chaos_bootstrap
    chaos_runtime_core
    chaos_jit
    chaos_interpreter
    chaos_debugger
    chaos_common
    chaos_support
    chaos_fmt
)

set(CHAOS_HOTUPDATE_SMOKE_LIBS
    chaos_hot_update
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_jit
    chaos_debugger
    chaos_common
    chaos_support
    chaos_fmt
)

add_chaos_test(test_method_replacement_smoke
    method_replacement_smoke_test.cpp
    LIBS ${CHAOS_HOTUPDATE_SMOKE_LIBS}
    LABELS "unit;hotupdate"
)

add_chaos_test(test_hotupdate_verification
    hotupdate_verification_test.cpp
    LIBS ${CHAOS_HOTUPDATE_LIBS}
    LABELS "unit;hotupdate"
)

# hotupdate_package_e2e_test — E2E package loading with real directory structure
add_chaos_test(test_hotupdate_package_e2e
    hotupdate_package_e2e_test.cpp
    LIBS ${CHAOS_HOTUPDATE_LIBS}
    LABELS "unit;hotupdate"
)

# interpreter_mixed_execution_test — InterpreterEntryDirect + method_replacement integration
add_chaos_test(test_interpreter_mixed_execution
    interpreter_mixed_execution_test.cpp
    LIBS ${CHAOS_HOTUPDATE_LIBS}
    LABELS "unit;hotupdate;interpreter"
)

# hotupdate_vtable_sync_test — Cross-module VTable sync with method_replacement
add_chaos_test(test_hotupdate_vtable_sync
    hotupdate_vtable_sync_test.cpp
    LIBS ${CHAOS_HOTUPDATE_LIBS}
    LABELS "unit;hotupdate"
)

# hotupdate_generics_test — Generic context registration and dispatch
add_chaos_test(test_hotupdate_generics
    hotupdate_generics_test.cpp
    LIBS ${CHAOS_HOTUPDATE_LIBS}
    LABELS "unit;hotupdate"
)

# hotupdate_replacement_benchmark_test — Benchmark & stress for method_replacement
add_chaos_test(test_hotupdate_replacement_benchmark
    hotupdate_replacement_benchmark_test.cpp
    LIBS ${CHAOS_HOTUPDATE_LIBS}
    LABELS "unit;hotupdate;benchmark"
)
