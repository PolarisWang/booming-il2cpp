// isolate_crash1.cpp — test if just including generated code crashes (static init test)
#include <cstdio>
#include <cstdlib>

extern "C" int __chaos_assert_failures = 0;
extern "C" void SetExceptionFallback(void (*)()) noexcept {}
extern "C" void InterpreterEntryDirect(uintptr_t, void*, void*) noexcept {}

// Test: does #include itself crash?
int main() {
    printf("BEFORE_INCLUDE\n"); fflush(stdout);

    // Don't include generated code - just test that a basic exe works
    printf("AFTER_INCLUDE\n"); fflush(stdout);
    return 0;
}