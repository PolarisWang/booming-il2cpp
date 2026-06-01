// pal_sync_posix.cpp — POSIX synchronization primitives (pthreads)

#include <chaos/pal/pal_sync.h>

#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <atomic>
#include <cstdlib>
#include <new>

namespace chaos::il2cpp::pal {

// ── Mutex ─────────────────────────────────────────────────────────────

PalMutex* PalMutexCreate() noexcept {
    auto* m = new (std::nothrow) pthread_mutex_t();
    if (!m) return nullptr;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_DEFAULT);
    int rc = pthread_mutex_init(m, &attr);
    pthread_mutexattr_destroy(&attr);
    if (rc != 0) {
        delete m;
        return nullptr;
    }
    return reinterpret_cast<PalMutex*>(m);
}

void PalMutexDestroy(PalMutex* m) noexcept {
    if (!m) return;
    pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(m));
    delete reinterpret_cast<pthread_mutex_t*>(m);
}

void PalMutexLock(PalMutex* m) noexcept {
    pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(m));
}

bool PalMutexTryLock(PalMutex* m) noexcept {
    return pthread_mutex_trylock(reinterpret_cast<pthread_mutex_t*>(m)) == 0;
}

void PalMutexUnlock(PalMutex* m) noexcept {
    pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(m));
}

// ── Condition variable ───────────────────────────────────────────────

PalCondVar* PalCondVarCreate() noexcept {
    auto* cv = new (std::nothrow) pthread_cond_t();
    if (!cv) return nullptr;
    if (pthread_cond_init(cv, nullptr) != 0) {
        delete cv;
        return nullptr;
    }
    return reinterpret_cast<PalCondVar*>(cv);
}

void PalCondVarDestroy(PalCondVar* cv) noexcept {
    if (!cv) return;
    pthread_cond_destroy(reinterpret_cast<pthread_cond_t*>(cv));
    delete reinterpret_cast<pthread_cond_t*>(cv);
}

void PalCondVarWait(PalCondVar* cv, PalMutex* m) noexcept {
    pthread_cond_wait(reinterpret_cast<pthread_cond_t*>(cv),
                       reinterpret_cast<pthread_mutex_t*>(m));
}

bool PalCondVarTimedWait(PalCondVar* cv, PalMutex* m, uint64_t timeout_ms) noexcept {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ts.tv_sec += static_cast<time_t>(timeout_ms / 1000);
    ts.tv_nsec += static_cast<long>(timeout_ms % 1000) * 1'000'000L;
    if (ts.tv_nsec >= 1'000'000'000L) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1'000'000'000L;
    }
    int rc = pthread_cond_timedwait(reinterpret_cast<pthread_cond_t*>(cv),
                                     reinterpret_cast<pthread_mutex_t*>(m), &ts);
    return rc == 0;
}

void PalCondVarSignal(PalCondVar* cv) noexcept {
    pthread_cond_signal(reinterpret_cast<pthread_cond_t*>(cv));
}

void PalCondVarBroadcast(PalCondVar* cv) noexcept {
    pthread_cond_broadcast(reinterpret_cast<pthread_cond_t*>(cv));
}

// ── Read-write lock ──────────────────────────────────────────────────

PalRwLock* PalRwLockCreate() noexcept {
    auto* rwl = new (std::nothrow) pthread_rwlock_t();
    if (!rwl) return nullptr;
    if (pthread_rwlock_init(rwl, nullptr) != 0) {
        delete rwl;
        return nullptr;
    }
    return reinterpret_cast<PalRwLock*>(rwl);
}

void PalRwLockDestroy(PalRwLock* rwl) noexcept {
    if (!rwl) return;
    pthread_rwlock_destroy(reinterpret_cast<pthread_rwlock_t*>(rwl));
    delete reinterpret_cast<pthread_rwlock_t*>(rwl);
}

void PalRwLockReadLock(PalRwLock* rwl) noexcept {
    pthread_rwlock_rdlock(reinterpret_cast<pthread_rwlock_t*>(rwl));
}

void PalRwLockWriteLock(PalRwLock* rwl) noexcept {
    pthread_rwlock_wrlock(reinterpret_cast<pthread_rwlock_t*>(rwl));
}

void PalRwLockUnlock(PalRwLock* rwl) noexcept {
    pthread_rwlock_unlock(reinterpret_cast<pthread_rwlock_t*>(rwl));
}

// ── Event ────────────────────────────────────────────────────────────
// POSIX implementation of PalEvent using mutex + condvar + bool.
// Supports manual-reset, auto-reset, and multi-wait (WaitAny/WaitAll)
// via an internal Treiber-stack waiter list (same pattern as
// wait_handle.cpp's signal-based dispatch).

struct PalEventWaiter {
    pthread_cond_t*       cv;
    pthread_mutex_t*      mtx;
    std::atomic<bool>*    wake_flag;
    size_t                event_index;
    PalEventWaiter*       next;
};

struct PalEvent {
    pthread_mutex_t  mutex;
    pthread_cond_t   cond;
    bool             set;
    bool             manual_reset;
    PalEventWaiter*  waiter_list{nullptr};
};

PalEvent* PalEventCreate(bool initially_set, bool manual_reset) noexcept {
    auto* e = new (std::nothrow) PalEvent();
    if (!e) return nullptr;
    pthread_mutex_init(&e->mutex, nullptr);
    pthread_cond_init(&e->cond, nullptr);
    e->set = initially_set;
    e->manual_reset = manual_reset;
    e->waiter_list = nullptr;
    return e;
}

void PalEventDestroy(PalEvent* e) noexcept {
    if (!e) return;
    pthread_mutex_destroy(&e->mutex);
    pthread_cond_destroy(&e->cond);
    delete e;
}

void PalEventSet(PalEvent* e) noexcept {
    pthread_mutex_lock(&e->mutex);
    e->set = true;

    // Notify Treiber-stack waiters (registered by PalEventWaitAny/WaitAll).
    PalEventWaiter* w = e->waiter_list;
    while (w) {
        pthread_mutex_lock(w->mtx);
        w->wake_flag->store(true, std::memory_order_relaxed);
        pthread_mutex_unlock(w->mtx);
        pthread_cond_signal(w->cv);
        w = w->next;
    }

    // Notify direct PalEventWait waiters.
    pthread_cond_broadcast(&e->cond);
    pthread_mutex_unlock(&e->mutex);
}

void PalEventReset(PalEvent* e) noexcept {
    pthread_mutex_lock(&e->mutex);
    e->set = false;
    pthread_mutex_unlock(&e->mutex);
}

bool PalEventWait(PalEvent* e, uint64_t timeout_ms) noexcept {
    pthread_mutex_lock(&e->mutex);
    if (e->set) {
        if (!e->manual_reset) e->set = false;
        pthread_mutex_unlock(&e->mutex);
        return true;
    }
    if (timeout_ms == UINT64_MAX) {
        pthread_cond_wait(&e->cond, &e->mutex);
        bool was_set = e->set;
        if (was_set && !e->manual_reset) e->set = false;
        pthread_mutex_unlock(&e->mutex);
        return was_set;
    }
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ts.tv_sec += static_cast<time_t>(timeout_ms / 1000);
    ts.tv_nsec += static_cast<long>(timeout_ms % 1000) * 1'000'000L;
    if (ts.tv_nsec >= 1'000'000'000L) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1'000'000'000L;
    }
    int rc = pthread_cond_timedwait(&e->cond, &e->mutex, &ts);
    bool was_set = e->set;
    if (was_set && !e->manual_reset) e->set = false;
    pthread_mutex_unlock(&e->mutex);
    return rc == 0 && was_set;
}

// ── Multi-wait helpers ───────────────────────────────────────────────

static void UnregisterWaiter(PalEvent* ev, PalEventWaiter* node) noexcept {
    PalEventWaiter** pp = &ev->waiter_list;
    while (*pp) {
        if (*pp == node) {
            *pp = node->next;
            return;
        }
        pp = &(*pp)->next;
    }
}

int32_t PalEventWaitAny(PalEvent* const* events, size_t count, uint64_t timeout_ms) noexcept {
    if (count == 0) return -1;

    // 1. Quick poll — check each event once.
    for (size_t i = 0; i < count; i++) {
        pthread_mutex_lock(&events[i]->mutex);
        if (events[i]->set) {
            if (!events[i]->manual_reset) events[i]->set = false;
            pthread_mutex_unlock(&events[i]->mutex);
            return static_cast<int32_t>(i);
        }
        pthread_mutex_unlock(&events[i]->mutex);
    }

    if (timeout_ms == 0) return -1;

    // 2. Stack-allocated waiter state.
    pthread_mutex_t waiter_mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  waiter_cv;
    pthread_cond_init(&waiter_cv, nullptr);
    std::atomic<bool> wake_flag{false};

    // 3. Allocate waiter nodes (one per event, heap since count varies).
    auto* nodes = new PalEventWaiter[count];
    size_t registered = 0;
    for (size_t i = 0; i < count; i++) {
        PalEventWaiter* n = &nodes[registered];
        n->cv = &waiter_cv;
        n->mtx = &waiter_mtx;
        n->wake_flag = &wake_flag;
        n->event_index = i;

        pthread_mutex_lock(&events[i]->mutex);
        n->next = events[i]->waiter_list;
        events[i]->waiter_list = n;
        pthread_mutex_unlock(&events[i]->mutex);
        registered++;
    }

    // 4. Re-check all events after registration (covers Set-vs-poll race).
    int32_t already_idx = -1;
    for (size_t ri = 0; ri < registered; ri++) {
        size_t i = nodes[ri].event_index;
        pthread_mutex_lock(&events[i]->mutex);
        if (events[i]->set) {
            already_idx = static_cast<int32_t>(i);
            pthread_mutex_unlock(&events[i]->mutex);
            break;
        }
        pthread_mutex_unlock(&events[i]->mutex);
    }

    if (already_idx >= 0) {
        // Unregister all and return.
        for (size_t ri = 0; ri < registered; ri++) {
            pthread_mutex_lock(&events[nodes[ri].event_index]->mutex);
            UnregisterWaiter(events[nodes[ri].event_index], &nodes[ri]);
            pthread_mutex_unlock(&events[nodes[ri].event_index]->mutex);
        }
        // Consume auto-reset for the signalled event.
        if (!events[already_idx]->manual_reset) {
            pthread_mutex_lock(&events[already_idx]->mutex);
            events[already_idx]->set = false;
            pthread_mutex_unlock(&events[already_idx]->mutex);
        }
        delete[] nodes;
        pthread_cond_destroy(&waiter_cv);
        return already_idx;
    }

    // 5. Block on waiter CV.
    bool woken = false;
    if (timeout_ms == UINT64_MAX) {
        pthread_mutex_lock(&waiter_mtx);
        while (!wake_flag.load(std::memory_order_relaxed)) {
            pthread_cond_wait(&waiter_cv, &waiter_mtx);
        }
        pthread_mutex_unlock(&waiter_mtx);
        woken = true;
    } else {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        ts.tv_sec += static_cast<time_t>(timeout_ms / 1000);
        ts.tv_nsec += static_cast<long>(timeout_ms % 1000) * 1'000'000L;
        if (ts.tv_nsec >= 1'000'000'000L) {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1'000'000'000L;
        }
        pthread_mutex_lock(&waiter_mtx);
        while (!wake_flag.load(std::memory_order_relaxed)) {
            int rc = pthread_cond_timedwait(&waiter_cv, &waiter_mtx, &ts);
            if (rc == ETIMEDOUT) break;
        }
        woken = wake_flag.load(std::memory_order_relaxed);
        pthread_mutex_unlock(&waiter_mtx);
    }

    // 6. Unregister from all events.
    for (size_t ri = 0; ri < registered; ri++) {
        size_t i = nodes[ri].event_index;
        pthread_mutex_lock(&events[i]->mutex);
        UnregisterWaiter(events[i], &nodes[ri]);
        pthread_mutex_unlock(&events[i]->mutex);
    }

    delete[] nodes;
    pthread_cond_destroy(&waiter_cv);

    if (woken) {
        // Find which event is signalled.
        for (size_t i = 0; i < count; i++) {
            pthread_mutex_lock(&events[i]->mutex);
            if (events[i]->set) {
                if (!events[i]->manual_reset) events[i]->set = false;
                pthread_mutex_unlock(&events[i]->mutex);
                return static_cast<int32_t>(i);
            }
            pthread_mutex_unlock(&events[i]->mutex);
        }
    }

    return -1;
}

int32_t PalEventWaitAll(PalEvent* const* events, size_t count, uint64_t timeout_ms) noexcept {
    if (count == 0) return -1;

    struct timespec deadline = {};
    if (timeout_ms != UINT64_MAX) {
        clock_gettime(CLOCK_MONOTONIC, &deadline);
        deadline.tv_sec += static_cast<time_t>(timeout_ms / 1000);
        deadline.tv_nsec += static_cast<long>(timeout_ms % 1000) * 1'000'000L;
        if (deadline.tv_nsec >= 1'000'000'000L) {
            deadline.tv_sec += 1;
            deadline.tv_nsec -= 1'000'000'000L;
        }
    }

    // Quick poll first.
    auto check_all = [&]() -> bool {
        for (size_t i = 0; i < count; i++) {
            pthread_mutex_lock(&events[i]->mutex);
            bool s = events[i]->set;
            pthread_mutex_unlock(&events[i]->mutex);
            if (!s) return false;
        }
        return true;
    };

    if (check_all()) {
        // Consume auto-reset signals.
        for (size_t i = 0; i < count; i++) {
            if (!events[i]->manual_reset) {
                pthread_mutex_lock(&events[i]->mutex);
                events[i]->set = false;
                pthread_mutex_unlock(&events[i]->mutex);
            }
        }
        return 0;
    }

    if (timeout_ms == 0) return -1;

    // Poll + wait on first event's CV with remaining time.
    for (;;) {
        struct timespec remaining;
        if (timeout_ms != UINT64_MAX) {
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            if (now.tv_sec > deadline.tv_sec ||
                (now.tv_sec == deadline.tv_sec && now.tv_nsec >= deadline.tv_nsec)) {
                break;
            }
            remaining.tv_sec = deadline.tv_sec - now.tv_sec;
            remaining.tv_nsec = deadline.tv_nsec - now.tv_nsec;
            if (remaining.tv_nsec < 0) {
                remaining.tv_sec -= 1;
                remaining.tv_nsec += 1'000'000'000L;
            }
        }

        // Wait on first event's CV.
        pthread_mutex_lock(&events[0]->mutex);
        if (timeout_ms != UINT64_MAX) {
            pthread_cond_timedwait(&events[0]->cond, &events[0]->mutex, &remaining);
        } else {
            pthread_cond_wait(&events[0]->cond, &events[0]->mutex);
        }
        pthread_mutex_unlock(&events[0]->mutex);

        // Re-check all events.
        if (check_all()) {
            for (size_t i = 0; i < count; i++) {
                if (!events[i]->manual_reset) {
                    pthread_mutex_lock(&events[i]->mutex);
                    events[i]->set = false;
                    pthread_mutex_unlock(&events[i]->mutex);
                }
            }
            return 0;
        }

        if (timeout_ms == UINT64_MAX) continue;  // wait forever
        // Check timeout on next iteration.
    }

    return -1;
}

}  // namespace chaos::il2cpp::pal
