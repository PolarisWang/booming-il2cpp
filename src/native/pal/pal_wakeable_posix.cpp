// pal_wakeable_posix.cpp — epoll+eventfd-based wakeable queue (Linux)

#include <chaos/pal/pal_wakeable.h>

#include <cerrno>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

namespace chaos::il2cpp::pal {

struct PalWakeable {
    int epoll_fd;
    int event_fd;
};

PalWakeable* PalWakeableCreate() noexcept {
    int epoll_fd = ::epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0) return nullptr;

    int event_fd = ::eventfd(0, EFD_CLOEXEC);
    if (event_fd < 0) {
        ::close(epoll_fd);
        return nullptr;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = event_fd;
    if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event_fd, &ev) != 0) {
        ::close(event_fd);
        ::close(epoll_fd);
        return nullptr;
    }

    return new PalWakeable{epoll_fd, event_fd};
}

bool PalWakeablePost(PalWakeable* w, void (*/*callback*/)(void*), void* /*context*/) noexcept {
    // epoll path uses a separate dispatch mechanism; for thread pool
    // wakeup we just signal the eventfd.
    return ::eventfd_write(w->event_fd, 1) == 0;
}

int PalWakeableWait(PalWakeable* w, int timeout_ms) noexcept {
    struct epoll_event events[8];
    int n = ::epoll_wait(w->epoll_fd, events, 8, timeout_ms);

    if (n < 0) {
        if (errno == EINTR) return 0;
        return -1;
    }

    int dispatched = 0;
    for (int i = 0; i < n; i++) {
        if (events[i].data.fd == w->event_fd) {
            // Drain eventfd to clear the notification.
            uint64_t val;
            ::read(w->event_fd, &val, sizeof(val));
            dispatched++;
        }
    }
    return dispatched;
}

void PalWakeableDestroy(PalWakeable* w) noexcept {
    if (!w) return;
    ::close(w->event_fd);
    ::close(w->epoll_fd);
    delete w;
}

}  // namespace chaos::il2cpp::pal
