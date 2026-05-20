#include <cstdio>
#include <cstdint>
#include <cstdlib>

// Just the declarations we need
extern "C" std::int32_t RunNativeAot(std::int32_t);
extern "C" const int kAotMethodCount;

// Forward declare the hook type
namespace chaos { namespace il2cpp { namespace common {
extern void (*g_chaos_fail_hook)();
}}}

int main() {
    printf("Testing all %d methods with fail_hook...\n", kAotMethodCount);
    int failed_count = 0;
    for (int i = 0; i < kAotMethodCount; i++) {
        chaos::il2cpp::common::g_chaos_fail_hook = []() { throw std::runtime_error("CHAOS_FAIL"); };
        try {
            RunNativeAot(i);
            // printf("  Method %d: OK\n", i);
        } catch (const std::runtime_error& e) {
            printf("FAIL: method %d triggered CHAOS_IL2CPP_FAIL\n", i);
            failed_count++;
        } catch (const std::exception& e) {
            printf("FAIL: method %d caught exception: %s\n", i, e.what());
            failed_count++;
        } catch (...) {
            printf("FAIL: method %d caught unknown exception\n", i);
            failed_count++;
        }
    }
    chaos::il2cpp::common::g_chaos_fail_hook = nullptr;
    int passed = kAotMethodCount - failed_count;
    printf("Passed: %d/%d\n", passed, kAotMethodCount);
    return failed_count;
}
