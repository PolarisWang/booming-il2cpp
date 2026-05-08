// isolate_crash_lib.cpp — link with chaos_runtime_core.lib (no redefined stubs)
#include <cstdio>
#include <cstdlib>
#include <csetjmp>

// Do NOT define InterpreterEntryDirect or __chaos_assert_failures here —
// they're provided by chaos_runtime_core.lib (runtime_stubs.obj).

static jmp_buf s_verify_buf;
static void exception_fallback() { longjmp(s_verify_buf, 1); }

extern "C" int RunNativeAot(int);

#include "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/convert-char/il2cpp_dist/genuine/ConvertCharNativeEntry/generated/native-aot.generated.cpp"

int main() {
    printf("MAIN_STARTED\n"); fflush(stdout);

    for (int i = 0; i < 3; i++) {
        printf("Calling RunNativeAot(%d)...\n", i); fflush(stdout);
        if (setjmp(s_verify_buf) == 0) {
            int result = RunNativeAot(i);
            printf("RunNativeAot(%d) = %d\n", i, result);
        } else {
            printf("RunNativeAot(%d) threw\n", i);
        }
        fflush(stdout);
    }

    printf("MAIN_END\n"); fflush(stdout);
    return 0;
}