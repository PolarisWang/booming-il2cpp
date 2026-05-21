# Bootstrap subsystem tests (chaos_bootstrap module)

set(CHAOS_BOOTSTRAP_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_codegen
    chaos_hot_update
    chaos_common
    chaos_support
    chaos_fmt
)

add_chaos_test(test_bootstrap_smoke
    bootstrap_test.cpp
    LIBS ${CHAOS_BOOTSTRAP_LIBS}
    LABELS "unit;bootstrap"
)

add_chaos_test(test_bootstrap_stress
    bootstrap_stress.cpp
    LIBS ${CHAOS_BOOTSTRAP_LIBS}
    LABELS "stress;bootstrap"
)
