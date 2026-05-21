# ABI subsystem tests
# Naming convention: test_abi_<subsystem>

set(CHAOS_ABI_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_codegen
    chaos_hot_update
    chaos_common
    chaos_support
    chaos_fmt
)

# method_table_abi_test — method_table + ABI manifest integration tests
add_chaos_test(test_abi_method_table
    method_table_abi_test.cpp
    LIBS ${CHAOS_ABI_LIBS}
    LABELS "unit;abi"
)
