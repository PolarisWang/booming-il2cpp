#include <cstdio>
#include <chrono>
extern "C" int __chaos_assert_failures;
extern "C" int RunNativeAot(int entryIndex);
int main() {
    __chaos_assert_failures = 0;
    printf("{\"results\":[");
    for (int i = 0; i < 18; i++) {
        auto t0 = std::chrono::steady_clock::now();
        int result = RunNativeAot(i);
        auto t1 = std::chrono::steady_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
        if (i > 0) printf(",");
        printf("{\"method\":%d,\"ms\":%f,\"ret\":%d}", i, ms, result);
    }
    printf("],\"assert_failures\":%d}", __chaos_assert_failures);
    return 0;
}
