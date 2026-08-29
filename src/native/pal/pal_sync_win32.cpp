// pal_sync_win32.cpp — Win32 synchronization primitives

#include <chaos/pal/pal_sync.h>

#define NOMINMAX
#include <windows.h>
#include <vector>
#include <cstdlib>

namespace chaos::il2cpp::pal {

// ── Mutex ─────────────────────────────────────────────────────────────

PalMutex* PalMutexCreate() noexcept {
    auto* cs = new CRITICAL_SECTION();
    ::InitializeCriticalSection(cs);
    return reinterpret_cast<PalMutex*>(cs);
}

void PalMutexDestroy(PalMutex* m) noexcept {
    if (!m) return;
    auto* cs = reinterpret_cast<CRITICAL_SECTION*>(m);
    ::DeleteCriticalSection(cs);
    delete cs;
}

void PalMutexLock(PalMutex* m) noexcept {
    ::EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m));
}

bool PalMutexTryLock(PalMutex* m) noexcept {
    return ::TryEnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m)) != FALSE;
}

void PalMutexUnlock(PalMutex* m) noexcept {
    ::LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(m));
}

// ── Condition variable ───────────────────────────────────────────────

PalCondVar* PalCondVarCreate() noexcept {
    auto* cv = new CONDITION_VARIABLE();
    ::InitializeConditionVariable(cv);
    return reinterpret_cast<PalCondVar*>(cv);
}

void PalCondVarDestroy(PalCondVar* cv) noexcept {
    if (!cv) return;
    delete reinterpret_cast<CONDITION_VARIABLE*>(cv);
}

void PalCondVarWait(PalCondVar* cv, PalMutex* m) noexcept {
    ::SleepConditionVariableCS(reinterpret_cast<CONDITION_VARIABLE*>(cv),
                                reinterpret_cast<CRITICAL_SECTION*>(m), INFINITE);
}

bool PalCondVarTimedWait(PalCondVar* cv, PalMutex* m, uint64_t timeout_ms) noexcept {
    DWORD ms = (timeout_ms > INFINITE - 1) ? INFINITE - 1 : static_cast<DWORD>(timeout_ms);
    return ::SleepConditionVariableCS(reinterpret_cast<CONDITION_VARIABLE*>(cv),
                                       reinterpret_cast<CRITICAL_SECTION*>(m), ms) != FALSE;
}

void PalCondVarSignal(PalCondVar* cv) noexcept {
    ::WakeConditionVariable(reinterpret_cast<CONDITION_VARIABLE*>(cv));
}

void PalCondVarBroadcast(PalCondVar* cv) noexcept {
    ::WakeAllConditionVariable(reinterpret_cast<CONDITION_VARIABLE*>(cv));
}

// ── Read-write lock ──────────────────────────────────────────────────

PalRwLock* PalRwLockCreate() noexcept {
    auto* sl = new SRWLOCK();
    ::InitializeSRWLock(sl);
    return reinterpret_cast<PalRwLock*>(sl);
}

void PalRwLockDestroy(PalRwLock* rwl) noexcept {
    if (!rwl) return;
    delete reinterpret_cast<SRWLOCK*>(rwl);
}

void PalRwLockReadLock(PalRwLock* rwl) noexcept {
    ::AcquireSRWLockShared(reinterpret_cast<SRWLOCK*>(rwl));
}

void PalRwLockWriteLock(PalRwLock* rwl) noexcept {
    ::AcquireSRWLockExclusive(reinterpret_cast<SRWLOCK*>(rwl));
}

void PalRwLockUnlock(PalRwLock* rwl) noexcept {
    ::ReleaseSRWLockExclusive(reinterpret_cast<SRWLOCK*>(rwl));
}

// ── Event ─────────────────────────────────────────────────────────────
// Win32 native event object (auto-reset or manual-reset HANDLE).

PalEvent* PalEventCreate(bool initially_set, bool manual_reset) noexcept {
    HANDLE h = ::CreateEventA(nullptr, manual_reset ? TRUE : FALSE,
                               initially_set ? TRUE : FALSE, nullptr);
    return reinterpret_cast<PalEvent*>(h);
}

void PalEventDestroy(PalEvent* e) noexcept {
    if (!e) return;
    ::CloseHandle(reinterpret_cast<HANDLE>(e));
}

void PalEventSet(PalEvent* e) noexcept {
    ::SetEvent(reinterpret_cast<HANDLE>(e));
}

void PalEventReset(PalEvent* e) noexcept {
    ::ResetEvent(reinterpret_cast<HANDLE>(e));
}

bool PalEventWait(PalEvent* e, uint64_t timeout_ms) noexcept {
    DWORD ms = (timeout_ms == UINT64_MAX) ? INFINITE
              : (timeout_ms > INFINITE - 1) ? INFINITE - 1
              : static_cast<DWORD>(timeout_ms);
    HANDLE h = reinterpret_cast<HANDLE>(e);
    // Alertable wait so a queued APC (the preemptive-suspend fallback, see
    // pal_preempt_win32.cpp QueueUserAPC) can interrupt the park and run on
    // this thread.  A cooperative thread blocked in the safepoint wait would
    // otherwise never process the APC (WaitForSingleObject is non-alertable),
    // so the coordinator deems it unresponsive until the hard timeout forces
    // an unsafe release — the root of the "safepoint hard timeout ... forcing
    // release" stress crash.  WAIT_IO_COMPLETION means an APC ran; loop and
    // re-wait (the event, if signaled later, still wakes us).
    DWORD r = ::WaitForSingleObjectEx(h, ms, TRUE);
    while (r == WAIT_IO_COMPLETION) {
        r = ::WaitForSingleObjectEx(h, ms, TRUE);
    }
    return r == WAIT_OBJECT_0;
}

int32_t PalEventWaitAny(PalEvent* const* events, size_t count, uint64_t timeout_ms) noexcept {
    if (count == 0) return -1;

    // Build HANDLE array from PalEvent pointers.
    // For small counts, use stack allocation to avoid heap.
    HANDLE stack_buf[64];
    std::vector<HANDLE> heap_buf;
    HANDLE* handles = stack_buf;
    if (count > 64) {
        heap_buf.resize(count);
        handles = heap_buf.data();
    }
    for (size_t i = 0; i < count; i++) {
        handles[i] = reinterpret_cast<HANDLE>(events[i]);
    }

    DWORD ms = (timeout_ms == UINT64_MAX) ? INFINITE
              : (timeout_ms > INFINITE - 1) ? INFINITE - 1
              : static_cast<DWORD>(timeout_ms);

    DWORD ret = ::WaitForMultipleObjects(static_cast<DWORD>(count), handles, FALSE, ms);
    if (ret >= WAIT_OBJECT_0 && ret < WAIT_OBJECT_0 + count) {
        return static_cast<int32_t>(ret - WAIT_OBJECT_0);
    }
    return -1;
}

int32_t PalEventWaitAll(PalEvent* const* events, size_t count, uint64_t timeout_ms) noexcept {
    if (count == 0) return -1;

    HANDLE stack_buf[64];
    std::vector<HANDLE> heap_buf;
    HANDLE* handles = stack_buf;
    if (count > 64) {
        heap_buf.resize(count);
        handles = heap_buf.data();
    }
    for (size_t i = 0; i < count; i++) {
        handles[i] = reinterpret_cast<HANDLE>(events[i]);
    }

    DWORD ms = (timeout_ms == UINT64_MAX) ? INFINITE
              : (timeout_ms > INFINITE - 1) ? INFINITE - 1
              : static_cast<DWORD>(timeout_ms);

    DWORD ret = ::WaitForMultipleObjects(static_cast<DWORD>(count), handles, TRUE, ms);
    return (ret == WAIT_OBJECT_0) ? 0 : -1;
}

}  // namespace chaos::il2cpp::pal
