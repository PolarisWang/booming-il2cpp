// pal_wakeable_apple.cpp — kqueue+EVFILT_USER wakeable queue (macOS/iOS)
// Apple platforms do not have Linux epoll/eventfd. This implementation
// uses kqueue() with EVFILT_USER for thread-pool wakeup signaling.

#include <chaos/pal/pal_wakeable.h>

#include <cerrno>
#include <sys/event.h>
#include <unistd.h>

namespace chaos::il2cpp::pal {

struct PalWakeable {
    int kq;
};

PalWakeable* PalWakeableCreate() noexcept {
    int kq = ::kqueue();
    if (kq < 0) return nullptr;

    // EVFILT_USER is a user-space event source available on macOS 10.6+ / iOS.
    // EV_CLEAR: event is automatically reset after delivery (edge-triggered).
    // NOTE_FFNOP: no explicit flags on registration (triggered via NOTE_TRIGGER later).
    struct kevent ev;
    EV_SET(&ev, 1, EVFILT_USER, EV_ADD | EV_CLEAR, NOTE_FFNOP, 0, nullptr);
    if (::kevent(kq, &ev, 1, nullptr, 0, nullptr) < 0) {
        ::close(kq);
        return nullptr;
    }

    return new PalWakeable{kq};
}

bool PalWakeablePost(PalWakeable* w, void (* /*callback*/)(void*), void* /*context*/) noexcept {
    // Trigger the EVFILT_USER event.
    struct kevent ev;
    EV_SET(&ev, 1, EVFILT_USER, 0, NOTE_TRIGGER, 0, nullptr);
    return ::kevent(w->kq, &ev, 1, nullptr, 0, nullptr) == 0;
}

int PalWakeableWait(PalWakeable* w, int timeout_ms) noexcept {
    struct kevent events[8];
    struct timespec ts;
    struct timespec* tsp = nullptr;

    if (timeout_ms >= 0) {
        ts.tv_sec = timeout_ms / 1000;
        ts.tv_nsec = static_cast<long>(timeout_ms % 1000) * 1'000'000L;
        tsp = &ts;
    }

    int n = ::kevent(w->kq, nullptr, 0, events, 8, tsp);
    if (n < 0) {
        if (errno == EINTR) return 0;
        return -1;
    }

    int dispatched = 0;
    for (int i = 0; i < n; i++) {
        if (events[i].filter == EVFILT_USER) {
            dispatched++;
        }
    }
    return dispatched;
}

void PalWakeableDestroy(PalWakeable* w) noexcept {
    if (!w) return;
    ::close(w->kq);
    delete w;
}

}  // namespace chaos::il2cpp::pal
