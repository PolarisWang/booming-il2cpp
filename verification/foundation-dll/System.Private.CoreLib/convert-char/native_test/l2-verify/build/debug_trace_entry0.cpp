#include <cstdio>
#include <cstdlib>

// Include generated code for RunNativeAot and all static initializers
#include "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/convert-char/il2cpp_dist/genuine/ConvertCharNativeEntry/generated/native-aot.generated.cpp"

extern "C" int __chaos_assert_failures;
extern "C" void SetExceptionFallback(void (*fn)());
extern "C" int RunNativeAot(int entryIndex);

#include "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/convert-char/native_test/l2-verify/build/expected_checksums.h"

#include <csetjmp>
static jmp_buf s_verify_buf;
static void exception_fallback() { longjmp(s_verify_buf, 1); }

int main() {
    printf("main() reached\n");
    fflush(stdout);

    __chaos_assert_failures = 0;
    SetExceptionFallback(&exception_fallback);
    printf("fallback registered\n");
    fflush(stdout);

    printf("kExpectedCount=%d, kExpectedChecksums[0]=%d\n", kExpectedCount, kExpectedChecksums[0]);
    fflush(stdout);

    printf("calling RunNativeAot(0)...\n");
    fflush(stdout);

    int actual = 0;
    bool threw = false;
    if (setjmp(s_verify_buf) == 0) {
        printf("setjmp OK, calling...\n");
        fflush(stdout);
        actual = RunNativeAot(0);
        printf("RunNativeAot(0) returned: %d\n", actual);
        fflush(stdout);
    } else {
        threw = true;
        printf("RunNativeAot(0) threw\n");
        fflush(stdout);
    }

    printf("Done. threw=%d, actual=%d, expected=%d\n", threw, actual, kExpectedChecksums[0]);
    fflush(stdout);
    return 0;
}
