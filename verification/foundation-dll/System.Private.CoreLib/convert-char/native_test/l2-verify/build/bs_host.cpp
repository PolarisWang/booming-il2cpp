#include <cstdio>
#include <csetjmp>
extern "C" int __chaos_assert_failures;
extern "C" void SetExceptionFallback(void (*fn)());
extern "C" int RunNativeAot(int entryIndex);
#include "expected_checksums.h"
static jmp_buf buf;
static void fb() { longjmp(buf, 1); }
int main() {
    __chaos_assert_failures = 0;
    SetExceptionFallback(fb);
    int rf = 0;
    printf("main() starting, kExpectedCount=%d\n", kExpectedCount);
    for (int i = 0; i < kExpectedCount; i++) {
        int actual = 0; bool threw = false;
        if (setjmp(buf) == 0) { actual = RunNativeAot(i); }
        else { threw = true; }
        if (kExpectedChecksums[i] == -1) continue;
        if (threw) { printf("FAIL [%d]: threw, expected %d\n", i, kExpectedChecksums[i]); rf++; continue; }
        if (actual != kExpectedChecksums[i]) { printf("FAIL [%d]: expected %d, got %d\n", i, kExpectedChecksums[i], actual); rf++; }
    }
    SetExceptionFallback(nullptr);
    printf("L2: %d/%d passed\n", kExpectedCount-rf, kExpectedCount);
    return __chaos_assert_failures + rf;
}
