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

uint64_t PalGetCurrentThreadId() noexcept {
    return static_cast<uint64_t>(::GetCurrentThreadId());
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

}  // namespace chaos::il2cpp::pal
