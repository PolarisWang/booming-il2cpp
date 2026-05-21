# Support library for native tests — shared test utilities

# Log system unit tests
add_chaos_test(test_log_system
    log_system_test.cpp
    LIBS chaos_common chaos_fmt
    LABELS "unit;support"
)
