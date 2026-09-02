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
# GCC/Clang (Linux): -fsanitize=undefined (halt-on-error via -fno-sanitize-recover)
# MSVC: UBSan is not available — use /RTC1 as partial substitute.

if(NOT COMMAND chaos_enable_ubsan)
    macro(chaos_enable_ubsan target)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target} PRIVATE -fsanitize=undefined -fno-sanitize-recover=all)
            target_link_options(${target} PRIVATE -fsanitize=undefined)
            # Blacklist known UB patterns that are intentional in the runtime
            # (e.g., type-punning in GC, reinterpret_cast in ABI layer).
            # Use: -fsanitize-blacklist=<file> for targeted suppression.
        elseif(MSVC)
            # MSVC has no UBSan; /RTC1 catches some runtime errors (stack frame
            # corruption, uninitialized locals).
            #
            # /RTC1 is only valid in Debug configurations:
            #   - /RTC1 with /O2 emits D9025 warning and silently disables /RTC1
            #   - /RTC1 requires /MDd (debug CRT); Release uses /MD, causing link
            #     mismatch (/RTC1 in Release on /MDd targets is handled by CMake's
            #     generator, but explicit /RTC1 on a Release target with /MT is
            #     a D9002 error pattern)
            # Guard with a generator expression so Release builds remain clean.
            target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:/RTC1>)
        endif()
    endmacro()
endif()