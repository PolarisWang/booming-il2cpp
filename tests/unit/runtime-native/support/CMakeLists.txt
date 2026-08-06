# Support library for native tests — shared test utilities

# Log system unit tests — built via the unified chaos_native_add_test factory.
chaos_native_add_test(test_log_system
    log_system_test.cpp
    LABELS "unit;support"
)
