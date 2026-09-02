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
            # corruption, uninitialized locals) in Debug builds only.
            #
            # /RTC1 is only valid in Debug configurations:
            #   - /RTC1 with /O2 emits D9025 (compiler warning: overriding
            #     /RTC1 with /O2) — the compiler *silently disables* /RTC1,
            #     not a hard error.  Before the generator-expression guard
            #     below, a Release build with /RTC1 would produce no UBSan
            #     coverage AND no build failure, silently wasting the request.
            #   - /RTC1 requires /MDd (debug CRT); Release uses /MD, causing
            #     a link mismatch if the target is not already /MDd.
            # Guard with a generator expression to keep Release builds clean.
            # NOTE: when SANITIZE "ubsan" is requested for a Release MSVC
            # target, the generator expression makes this macro a no-op —
            # no runtime checks are active.  This matches the pre-guard
            # behavior (D9025 silently disabled /RTC1) but is now honest:
            # the caller asked for a sanitizer that MSVC does not implement
            # in Release config.
            target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:/RTC1>)
        endif()
    endmacro()
endif()
