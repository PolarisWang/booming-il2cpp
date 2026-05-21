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
