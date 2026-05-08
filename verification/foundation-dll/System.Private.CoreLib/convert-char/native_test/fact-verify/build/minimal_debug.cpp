// minimal_debug.cpp — structured debug of fact_static startup
#include <cstdio>
#include <cstdlib>
#include <csetjmp>

extern "C" int __chaos_assert_failures;
extern "C" void SetExceptionFallback(void (*fn)());
extern "C" void InterpreterEntryDirect(uintptr_t, void*, void*) noexcept {}

static jmp_buf s_verify_buf;
static void exception_fallback() { longjmp(s_verify_buf, 1); }

extern "C" int RunNativeAot(int);

#include "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/convert-char/il2cpp_dist/genuine/ConvertCharNativeEntry/generated/native-aot.generated.cpp"

constexpr int kExpectedCount = 18;
constexpr int kExpectedChecksums[kExpectedCount] = {
    -1, 42, 65, -1, -1, -1, 42, 42, 42, 42, 42, 42, -1, -1, -1, 42, 42, 42
};

int main() {
    printf("MAIN_ENTER\n"); fflush(stdout);

    __chaos_assert_failures = 0;
    printf("ASSERT_RESET\n"); fflush(stdout);

    SetExceptionFallback(&exception_fallback);
    printf("FALLBACK_SET\n"); fflush(stdout);

    for (int i = 0; i < kExpectedCount; i++) {
        printf("ENTRY_%d\n", i); fflush(stdout);

        int actual = 0;
        bool threw = false;

        if (setjmp(s_verify_buf) == 0) {
            actual = RunNativeAot(i);
        } else {
            threw = true;
        }

        if (kExpectedChecksums[i] == -1) {
            printf("SKIP_%d\n", i); fflush(stdout);
            continue;
        }
        if (threw) {
            printf("FAIL_THREW_%d\n", i); fflush(stdout);
        } else if (actual != kExpectedChecksums[i]) {
            printf("FAIL_MISMATCH_%d expected=%d actual=%d\n", i, kExpectedChecksums[i], actual); fflush(stdout);
        } else {
            printf("PASS_%d=%d\n", i, actual); fflush(stdout);
        }
    }

    printf("MAIN_DONE\n"); fflush(stdout);
    return 0;
}