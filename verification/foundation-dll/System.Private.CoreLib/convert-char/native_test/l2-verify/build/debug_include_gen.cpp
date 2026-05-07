/// Debug: include generated code but just return from main
/// This tests if any static initializer in the generated code crashes.
#include <cstdio>

extern "C" int __chaos_assert_failures;

extern "C" void SetExceptionFallback(void (*fn)());

#include "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/convert-char/native_test/l2-verify/build/expected_checksums.h"

// The generated code is included for its static initializers
#include "D:/agent/booming-il2cpp/verification/foundation-dll/System.Private.CoreLib/convert-char/il2cpp_dist/genuine/ConvertCharNativeEntry/generated/native-aot.generated.cpp"

int main() {
    printf("main() reached\n");
    fflush(stdout);
    return 0;
}
