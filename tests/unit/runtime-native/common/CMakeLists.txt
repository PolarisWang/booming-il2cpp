# Common library tests (chaos_common module)

chaos_native_add_test(test_common_checked_conv
    common_checked_conv_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_collection
    common_collection_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_json_reader
    common_json_reader_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_format
    common_format_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_async
    common_async_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_async_extended
    common_async_extended_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_thread
    common_thread_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_monitor
    common_monitor_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_profile
    common_profile_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_span
    common_span_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_native_types
    common_native_types_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_arithmetic
    common_arithmetic_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_checked_conv_ext
    common_checked_conv_test_ext.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_type_info
    common_type_info_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_config
    common_config_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_log
    common_log_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_trace
    common_trace_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_eh
    common_eh_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_compiler_hints
    common_compiler_hints_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_load_store
    common_load_store_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_ptr_tag
    common_ptr_tag_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_finally
    common_finally_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_static_init
    common_static_init_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_status
    common_status_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_tls_buffer
    common_tls_buffer_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_log_stress
    common_log_stress_test.cpp
    LABELS "stress;common"
)

chaos_native_add_test(test_common_trace_stress
    common_trace_stress_test.cpp
    LABELS "stress;common"
)

chaos_native_add_test(test_common_async_stress
    common_async_stress_test.cpp
    LABELS "stress;common"
)

chaos_native_add_test(test_common_monitor_stress
    common_monitor_stress_test.cpp
    LABELS "stress;common"
)

chaos_native_add_test(test_common_json_reader_fuzz
    common_json_reader_fuzz_test.cpp
    LABELS "fuzz;common"
)

# ═══════════════════════════════════════════════════════════════════════════
# Phase F — Special module tests
# ═══════════════════════════════════════════════════════════════════════════

chaos_native_add_test(test_common_asan
    common_asan_test.cpp
    LABELS "unit;common"
)

chaos_native_add_test(test_common_unordered_dense
    common_unordered_dense_test.cpp
    LABELS "unit;common"
)

# profile_enabled needs CHAOS_IL2CPP_PROFILE_ENABLED=1 and profile_globals.cpp
chaos_native_add_test(test_common_profile_enabled
    common_profile_enabled_test.cpp
    "${CHAOS_CMAKE_ROOT}/src/native/common/chaos/profile_globals.cpp"
    DEFINITIONS CHAOS_IL2CPP_PROFILE_ENABLED=1
    LABELS "unit;common"
)
