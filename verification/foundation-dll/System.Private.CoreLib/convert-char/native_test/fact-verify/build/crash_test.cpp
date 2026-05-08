// crash_test.cpp — isolate static init crash in combined TU
#include <cstdio>
#include <cstdlib>
#include <csetjmp>
#include <array>

// Provide minimal stubs needed by generated code
extern "C" int __chaos_assert_failures = 0;
extern "C" void SetExceptionFallback(void (*fn)()) {}
extern "C" void InterpreterEntryDirect(uintptr_t, void*, void*) noexcept {}

static jmp_buf s_verify_buf;
static void exception_fallback() { longjmp(s_verify_buf, 1); }

// Forward declare RunNativeAot
extern "C" int RunNativeAot(int entryIndex);

int main() {
    printf("[TEST] Step 1: Starting\n"); fflush(stdout);
    SetExceptionFallback(&exception_fallback);
    printf("[TEST] Step 2: Fallback registered\n"); fflush(stdout);

    // Test calling RunNativeAot for just entry 0
    printf("[TEST] Step 3: Calling RunNativeAot(0)\n"); fflush(stdout);
    if (setjmp(s_verify_buf) == 0) {
        int result = RunNativeAot(0);
        printf("[TEST] RunNativeAot(0) = %d\n", result);
    } else {
        printf("[TEST] RunNativeAot(0) threw\n");
    }
    fflush(stdout);
    printf("[TEST] Done\n"); fflush(stdout);
    return 0;
}

// Include generated code (provides RunNativeAot and all static init)
#include "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/convert-char/il2cpp_dist/genuine/ConvertCharNativeEntry/generated/native-aot.generated.cpp"