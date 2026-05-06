/// l3_verify_main.cpp — Fact L3: Runtime Execution Verification Host
///
/// Compile with native-aot.generated.cpp and runtime libs.
/// Expects expected_checksums.h in the same directory.
///
/// Flow:
///   1. RuntimeInit → ThreadAttach (full runtime, GC initialized)
///   2. Loop: RunNativeAot(i) vs expected checksum
///   3. Report L3: N/M passed
///
/// Exception handling: keeps setjmp/longjmp fallback for methods that throw
/// managed exceptions (since per-family builds don't register framework types
/// like InvalidCastException, so the managed exception path can't resolve them).
/// In Phase 2+ (assembly-level linking), this will be replaced by real EH.

#include <cstdlib>
#include <csetjmp>

#include <chaos/common.h>

#include "runtime_abi.h"
#include "runtime_core.h"

extern "C" int __chaos_assert_failures;
extern "C" int RunNativeAot(int entryIndex);
extern "C" void SetExceptionFallback(void (*fn)());

#include "expected_checksums.h"

static jmp_buf s_verify_buf;

static void exception_fallback() {
    longjmp(s_verify_buf, 1);
}

int main() {
    __chaos_assert_failures = 0;

    // ── Step 1: Initialize runtime ──
    RuntimeInitParams init_params = {};
    init_params.struct_size = sizeof(init_params);
    init_params.init_flags = 0;
    init_params.host_name_utf8 = "l3-verify";
    init_params.runtime_tag_utf8 = nullptr;
    init_params.host_context = nullptr;

    RuntimeConfig config = {};
    config.struct_size = sizeof(config);
    config.allocator = nullptr;   // Use default GC allocator
    config.deallocator = nullptr;

    auto* abi = chaos::il2cpp::runtime_core::GetRuntimeAbiV0();
    if (abi == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR("L3", "GetRuntimeAbiV0() returned null");
        return 1;
    }

    RuntimeState* runtime_state = nullptr;
    RuntimeStatus status = abi->runtime_init(&init_params, &config, &runtime_state);
    if (status != CHAOS_RUNTIME_STATUS_OK || runtime_state == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR_M("L3", "RuntimeInit failed (status={0})", static_cast<int>(status));
        return 1;
    }
    chaos::il2cpp::runtime_core::SetCurrentRuntimeState(runtime_state);

    ThreadState* thread_state = nullptr;
    status = abi->thread_attach(runtime_state, &thread_state);
    if (status != CHAOS_RUNTIME_STATUS_OK || thread_state == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR_M("L3", "ThreadAttach failed (status={0})", static_cast<int>(status));
        abi->runtime_shutdown(runtime_state);
        return 1;
    }
    // ThreadAttach calls SetCurrentThreadState internally

    // Register exception fallback for methods that throw
    SetExceptionFallback(&exception_fallback);

    // ── Step 2: Run entries ──
    int return_value_failures = 0;

    for (int i = 0; i < kExpectedCount; i++) {
        int actual = 0;
        bool threw = false;

        if (setjmp(s_verify_buf) == 0) {
            actual = RunNativeAot(i);
        } else {
            threw = true;
        }

        if (kExpectedChecksums[i] == -1) {
            // Expected throw — skip return value check
            continue;
        }
        if (threw) {
            CHAOS_IL2CPP_LOG_ERROR_M("L3", "FAIL [{0}]: native threw, expected {1}",
                                     i, kExpectedChecksums[i]);
            return_value_failures++;
            continue;
        }
        if (actual != kExpectedChecksums[i]) {
            CHAOS_IL2CPP_LOG_ERROR_M("L3", "FAIL [{0}]: expected {1}, got {2}",
                                     i, kExpectedChecksums[i], actual);
            return_value_failures++;
        }
    }

    SetExceptionFallback(nullptr);

    // ── Step 3: Report ──
    int total_failures = __chaos_assert_failures + return_value_failures;
    int passed = kExpectedCount - return_value_failures;
    CHAOS_IL2CPP_LOG_INFO_M("L3", "{0}/{1} passed (assert_failures={2}, return_failures={3})",
                            passed, kExpectedCount,
                            __chaos_assert_failures, return_value_failures);

    chaos::il2cpp::runtime_core::SetCurrentRuntimeState(nullptr);
    abi->runtime_shutdown(runtime_state);
    return total_failures;
}
