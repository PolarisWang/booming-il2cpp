// pal_thread_win32.cpp — Win32 threading primitives

#include <chaos/pal/pal_thread.h>

#define NOMINMAX
#include <windows.h>
#include <processthreadsapi.h>
#include <intrin.h>

namespace chaos::il2cpp::pal {

struct PalThread {
    HANDLE handle;
};

PalThread* PalThreadCreate(PalThreadProc proc, void* arg) noexcept {
    auto* t = new (std::nothrow) PalThread();
    if (!t) return nullptr;
    t->handle = ::CreateThread(nullptr, 0,
        [](void* p) -> DWORD {
            return static_cast<DWORD>(reinterpret_cast<uintptr_t>(proc(p)));
        }, arg, 0, nullptr);
    if (!t->handle) {
        delete t;
        return nullptr;
    }
    return t;
}

void* PalThreadJoin(PalThread* thread) noexcept {
    if (!thread) return nullptr;
    ::WaitForSingleObject(thread->handle, INFINITE);
    DWORD exit_code = 0;
    ::GetExitCodeThread(thread->handle, &exit_code);
    ::CloseHandle(thread->handle);
    void* result = reinterpret_cast<void*>(static_cast<uintptr_t>(exit_code));
    delete thread;
    return result;
}

void PalSleepMs(uint64_t ms) noexcept {
    DWORD m = (ms > INFINITE - 1) ? INFINITE - 1 : static_cast<DWORD>(ms);
    ::Sleep(m);
}

void PalYield() noexcept {
    ::SwitchToThread();
}

void PalGetStackBounds(void*& out_base, void*& out_limit) noexcept {
    NT_TIB* tib = static_cast<NT_TIB*>(::__readgsqword(0x30));
    out_base  = tib->StackBase;
    out_limit = tib->StackLimit;
}

void* PalDuplicateCurrentThreadHandle() noexcept {
    HANDLE hProcess = ::GetCurrentProcess();
    HANDLE hThread  = ::GetCurrentThread();
    ::DuplicateHandle(hProcess, hThread, hProcess, &hThread,
                      THREAD_SET_CONTEXT, FALSE, 0);
    return hThread;
}

void PalCloseThreadHandle(void* handle) noexcept {
    if (handle) {
        ::CloseHandle(static_cast<HANDLE>(handle));
    }
}

bool PalSetThreadPriority(int level) noexcept {
    static constexpr int kPriorityMap[] = {
        THREAD_PRIORITY_LOWEST,      // 0 = Lowest
        THREAD_PRIORITY_BELOW_NORMAL, // 1 = BelowNormal
        THREAD_PRIORITY_NORMAL,      // 2 = Normal
        THREAD_PRIORITY_ABOVE_NORMAL, // 3 = AboveNormal
        THREAD_PRIORITY_HIGHEST,     // 4 = Highest
    };
    if (level < 0 || level > 4) level = 2;
    return ::SetThreadPriority(::GetCurrentThread(), kPriorityMap[level]) != FALSE;
}

}  // namespace chaos::il2cpp::pal
