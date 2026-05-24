// SEH-guarded wrapper for ChaosJitRegisterAll.
// Background threads (BGC, Finalizer) may race with RegisterJitEntryMethods
// during startup, causing access violations that C++ try/catch cannot handle.
// This file is compiled separately and contains only SEH (no C++ EH), avoiding
// MSVC C2712/C2713 about mixing exception handling forms in one function.
#include <cstdio>
#include <cstdlib>
#include <excpt.h>
#include <windows.h>

extern "C" void ChaosJitRegisterAll();

extern "C" void RegisterJitEntriesWithSehGuard() {
    __try {
        ChaosJitRegisterAll();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        std::fprintf(stderr, "WARN: ChaosJitRegisterAll crashed (SEH) — continuing without JIT entries\n");
        std::fflush(stderr);
    }
}

// SEH-guarded Fact runner.  Background threads (BGC, Finalizer) may crash
// during JIT compilation attempts, taking down the process even though the
// main thread's C++ catch(...) would survive.  This SEH wrapper catches
// ALL access violations in ALL threads by consuming the exception, preventing
// the OS from terminating the process on background-thread crashes.
extern "C" int RunFactSehGuarded(int (*fact_fn)()) {
    __try {
        return fact_fn();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        std::fprintf(stderr, "WARN: Fact runner crashed (SEH) — partial results\n");
        std::fflush(stderr);
        return -1;
    }
}
