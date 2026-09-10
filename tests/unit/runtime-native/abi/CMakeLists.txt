# ABI subsystem tests
# Naming convention: test_abi_<subsystem>

# method_table_abi_test — method_table + ABI manifest integration tests.
# Built via the unified chaos_native_add_test factory (defaults to the full runtime
# lib set + include dirs), replacing the old explicit CHAOS_ABI_LIBS.
chaos_native_add_test(test_abi_method_table
    method_table_abi_test.cpp
    LABELS "unit;abi"
)
