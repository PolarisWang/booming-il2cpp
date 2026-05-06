#include <cstdio>
extern "C" int __chaos_assert_failures;
extern "C" int RunNativeAot(int entryIndex);
int main() {
    __chaos_assert_failures = 0;
    int result = RunNativeAot(0);
    printf("result=%d assert=%d\n", result, __chaos_assert_failures);
    return 0;
}
