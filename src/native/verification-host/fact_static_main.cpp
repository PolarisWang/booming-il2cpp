/// fact_static_main.cpp — Fact Static: Self-verifying native AOT host.
///
/// Compile with native-aot.generated.cpp and runtime_stubs.cpp.
/// Expects expected_checksums.h in the same directory.
///
/// Uses printf for output (CHAOS_IL2CPP_LOG macros can crash before runtime init).
#include <cstdio>
#include <cstdlib>
#include <csetjmp>

#include <chaos/common.h>

extern "C" int __chaos_assert_failures;
extern "C" void SetExceptionFallback(void (*fn)());
extern "C" int RunNativeAot(int entryIndex);

#include "expected_checksums.h"

static jmp_buf s_verify_buf;
static void exception_fallback() {
    longjmp(s_verify_buf, 1);
}

int main() {
    __chaos_assert_failures = 0;
    SetExceptionFallback(&exception_fallback);

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
            continue;
        }
        if (threw) {
            printf("FAIL [%d]: native threw, expected %d\n", i, kExpectedChecksums[i]);
            fflush(stdout);
            return_value_failures++;
            continue;
        }
        if (actual != kExpectedChecksums[i]) {
            printf("FAIL [%d]: expected %d, got %d\n", i, kExpectedChecksums[i], actual);
            fflush(stdout);
            return_value_failures++;
        }
    }

    SetExceptionFallback(nullptr);

    int total_failures = __chaos_assert_failures + return_value_failures;
    int passed = kExpectedCount - return_value_failures;
    printf("[FactStatic] %d/%d passed (assert_failures=%d, return_failures=%d)\n",
           passed, kExpectedCount,
           __chaos_assert_failures, return_value_failures);
    fflush(stdout);

    return total_failures;
}