// isolate_crash.cpp — determine whether static init or runtime code crashes
#include <cstdio>
#include <cstdlib>
#include <csetjmp>

// NOTE: no stubs here — generated code pulls in real headers

// Just include the generated code — its static init runs here
#include "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/convert-char/il2cpp_dist/genuine/ConvertCharNativeEntry/generated/native-aot.generated.cpp"

extern "C" int RunNativeAot(int);

int main() {
    printf("MAIN_STARTED\n"); fflush(stdout);
    (void)RunNativeAot;
    printf("MAIN_END\n"); fflush(stdout);
    return 0;
}