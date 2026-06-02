// pal_wakeable_win32.cpp — IOCP-based wakeable queue (Windows)

#include <chaos/pal/pal_wakeable.h>

#define NOMINMAX
#include <windows.h>

namespace chaos::il2cpp::pal {

struct PalWakeable {
    HANDLE iocp;
};

PalWakeable* PalWakeableCreate() noexcept {
    HANDLE port = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
    if (!port || port == INVALID_HANDLE_VALUE) return nullptr;
    return new PalWakeable{port};
}

bool PalWakeablePost(PalWakeable* w, void (*callback)(void*), void* context) noexcept {
    return ::PostQueuedCompletionStatus(
        w->iocp, 0, reinterpret_cast<ULONG_PTR>(callback),
        reinterpret_cast<OVERLAPPED*>(context)) != FALSE;
}

int PalWakeableWait(PalWakeable* w, int timeout_ms) noexcept {
    DWORD bytes = 0;
    ULONG_PTR key = 0;
    OVERLAPPED* overlapped = nullptr;

    DWORD ms = (timeout_ms < 0) ? INFINITE : static_cast<DWORD>(timeout_ms);
    BOOL ok = ::GetQueuedCompletionStatus(w->iocp, &bytes, &key, &overlapped, ms);

    if (!ok) {
        if (GetLastError() == WAIT_TIMEOUT) return 0;
        return -1;
    }

    if (key != 0) {
        auto cb = reinterpret_cast<void (*)(void*)>(key);
        cb(overlapped);
        return 1;
    }

    return 0;
}

void PalWakeableDestroy(PalWakeable* w) noexcept {
    if (!w) return;
    ::CloseHandle(w->iocp);
    delete w;
}

}  // namespace chaos::il2cpp::pal
