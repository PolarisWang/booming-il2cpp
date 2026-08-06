# rtc.cmake — Run-Time Checks (/RTC1) for chaos-il2cpp native tests.
#
# /RTC1 enables both /RTCs (stack frame verification) and /RTCu (use of
# uninitialized variables).  Intended for debug/release builds where ASan
# is too heavy or unavailable.
#
# Usage:
#   include(config/sanitizers/rtc.cmake)
#   chaos_enable_rtc(my_test_target)

if(NOT COMMAND chaos_enable_rtc)
    macro(chaos_enable_rtc target)
        target_compile_options(${target} PRIVATE /RTC1)
    endmacro()
endif()
