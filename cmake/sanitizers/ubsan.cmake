# ubsan.cmake — UndefinedBehaviorSanitizer configuration for chaos-il2cpp native tests.
#
# Include this file to enable UBSan for a specific test target.
# Usage in add_chaos_test:
#   add_chaos_test(test_name test.cpp LIBS ... SANITIZE "ubsan")
#
# UBSan detects:
#   - integer overflow
#   - shift base/exponent errors
#   - misaligned / null pointer dereference
#   - signed integer overflow
#   - type mismatches (e.g., vtable call on wrong object)
#   - function pointer mismatches
#
# GCC/Clang (Linux): -fsanitize=undefined (default halt-on-error)
# MSVC: UBSan is not available — use /RTC1 as partial substitute.

if(NOT COMMAND chaos_enable_ubsan)
    macro(chaos_enable_ubsan target)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target} PRIVATE -fsanitize=undefined)
            target_link_options(${target} PRIVATE -fsanitize=undefined)
            # Blacklist known UB patterns that are intentional in the runtime
            # (e.g., type-punning in GC, reinterpret_cast in ABI layer).
            # Use: -fsanitize-blacklist=<file> for targeted suppression.
        elseif(MSVC)
            # MSVC has no UBSan; /RTC1 catches some runtime errors.
            # message(STATUS "UBSan not available on MSVC — skipping ${target}")
        endif()
    endmacro()
endif()