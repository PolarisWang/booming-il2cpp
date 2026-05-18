#include <cstdio>
#include <cstdlib>

// Need chaos_managed_exception type definition
#include "runtime_core.h"

extern "C" void ConvertCharSubjects_ConvertCharSubjects_Subject_0(void);

extern "C" void SafeWrapperSubject0() {
    try {
        ConvertCharSubjects_ConvertCharSubjects_Subject_0();
    } catch (const chaos_managed_exception& e) {
        std::fprintf(stderr, "SAFE_WRAPPER_CATCH obj=%lld\n", (long long)e.object_value);
    } catch (...) {
        std::fprintf(stderr, "SAFE_WRAPPER_ELLIPSIS\n");
        throw;
    }
}
