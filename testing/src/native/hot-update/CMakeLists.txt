# Hot-update subsystem tests
# Note: with STATIC IMPORTED targets, ALL transitive library dependencies must
# be listed explicitly.

set(CHAOS_HOTUPDATE_LIBS
    chaos_hot_update
    chaos_bootstrap
    chaos_runtime_core
    chaos_codegen
    chaos_interpreter
    chaos_common
    chaos_support
    chaos_fmt
)

set(CHAOS_HOTUPDATE_SMOKE_LIBS
    chaos_hot_update
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_codegen
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
