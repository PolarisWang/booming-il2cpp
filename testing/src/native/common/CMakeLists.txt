# Common library tests (chaos_common module)

set(CHAOS_COMMON_LIBS
    chaos_common
    chaos_fmt
)

add_chaos_test(test_common_checked_conv
    common_checked_conv_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_collection
    common_collection_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_json_reader
    common_json_reader_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_format
    common_format_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_async
    common_async_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_async_extended
    common_async_extended_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_thread
    common_thread_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_monitor
    common_monitor_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_profile
    common_profile_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_span
    common_span_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_native_types
    common_native_types_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_arithmetic
    common_arithmetic_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_checked_conv_ext
    common_checked_conv_test_ext.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_type_info
    common_type_info_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_config
    common_config_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_log
    common_log_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_trace
    common_trace_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_eh
    common_eh_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_compiler_hints
    common_compiler_hints_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_load_store
    common_load_store_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_ptr_tag
    common_ptr_tag_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_finally
    common_finally_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_static_init
    common_static_init_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_status
    common_status_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_tls_buffer
    common_tls_buffer_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_log_stress
    common_log_stress_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "stress;common"
)

add_chaos_test(test_common_trace_stress
    common_trace_stress_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "stress;common"
)

add_chaos_test(test_common_async_stress
    common_async_stress_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "stress;common"
)

add_chaos_test(test_common_monitor_stress
    common_monitor_stress_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "stress;common"
)

add_chaos_test(test_common_json_reader_fuzz
    common_json_reader_fuzz_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "fuzz;common"
)

# ═══════════════════════════════════════════════════════════════════════════
# Phase F — Special module tests
# ═══════════════════════════════════════════════════════════════════════════

add_chaos_test(test_common_asan
    common_asan_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_unordered_dense
    common_unordered_dense_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

# profile_enabled needs CHAOS_IL2CPP_PROFILE_ENABLED=1 and profile_globals.cpp
add_executable(test_common_profile_enabled
    common_profile_enabled_test.cpp
    "${CHAOS_PROJECT_ROOT}/../src/native/common/chaos/profile_globals.cpp"
)
target_link_libraries(test_common_profile_enabled PRIVATE gtest_main ${CHAOS_COMMON_LIBS})
target_include_directories(test_common_profile_enabled PRIVATE ${CHAOS_INCLUDE_DIRS})
target_compile_definitions(test_common_profile_enabled PRIVATE CHAOS_IL2CPP_PROFILE_ENABLED=1)
target_link_options(test_common_profile_enabled PRIVATE /FORCE:MULTIPLE)
add_test(NAME test_common_profile_enabled COMMAND test_common_profile_enabled)
set_tests_properties(test_common_profile_enabled PROPERTIES LABELS "unit;common")
