// isolate_static_init.cpp — test if static init alone crashes
#include <cstdio>
#include <cstdlib>

extern "C" int __chaos_assert_failures = 0;
extern "C" void SetExceptionFallback(void (*)()) noexcept {}
extern "C" void InterpreterEntryDirect(uintptr_t, void*, void*) noexcept {}

int main() {
    printf("ENTER_MAIN\n"); fflush(stdout);
    printf("EXIT_MAIN\n"); fflush(stdout);
    return 0;
}