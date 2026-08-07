#if defined(_WIN32)
#include <windows.h>
#include <dbghelp.h>
#include <cstdio>

LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS* ep) {
    printf("=== EXCEPTION at 0x%p, code=0x%08X ===\n",
           ep->ExceptionRecord->ExceptionAddress,
           ep->ExceptionRecord->ExceptionCode);
    fflush(stdout);

    HANDLE hProcess = GetCurrentProcess();
    HANDLE hFile = CreateFileA("crash.dmp", GENERIC_WRITE, 0, NULL,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION mei;
        mei.ThreadId = GetCurrentThreadId();
        mei.ExceptionPointers = ep;
        mei.ClientPointers = FALSE;
        MiniDumpWriteDump(hProcess, GetCurrentProcessId(), hFile,
                          MiniDumpNormal, &mei, NULL, NULL);
        CloseHandle(hFile);
        printf("Wrote crash.dmp\n");
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

int main() {
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    // Run the test
    system("build/testing/jit/RelWithDebInfo/test_jit_mode.exe --gtest_filter=*Tier0CompilesAddMethod*");
    return 0;
}
#endif  // _WIN32
