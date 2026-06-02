// pal_low_mem_linux.cpp — Linux low-memory notification via
// /proc/pressure/memory (PSI) with poll(POLLPRI).

#include <chaos/pal/pal_low_mem.h>

#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <cerrno>

namespace chaos::il2cpp::pal {

struct PalLowMemMonitor {
    int psi_fd;         // /proc/pressure/memory fd
    int shutdown_fd;    // Self-pipe read end for shutdown signaling
    int shutdown_write; // Self-pipe write end
};

/// Open the PSI memory pressure file and configure the notification threshold.
static int OpenPsiFd() noexcept {
    int fd = ::open("/proc/pressure/memory", O_RDWR | O_CLOEXEC);
    if (fd < 0) return -1;

    const char* threshold = "some 50000 1000000";
    ssize_t written = ::write(fd, threshold, std::strlen(threshold));
    if (written < 0) {
        ::close(fd);
        return -1;
    }
    return fd;
}

PalLowMemMonitor* PalLowMemCreate() noexcept {
    int psi_fd = OpenPsiFd();
    if (psi_fd < 0) return nullptr;

    int pipe_fds[2];
    if (::pipe2(pipe_fds, O_CLOEXEC) != 0) {
        ::close(psi_fd);
        return nullptr;
    }

    return new PalLowMemMonitor{psi_fd, pipe_fds[0], pipe_fds[1]};
}

int PalLowMemWait(PalLowMemMonitor* monitor, int /*timeout_ms*/) noexcept {
    struct pollfd pfds[2];
    pfds[0].fd = monitor->psi_fd;
    pfds[0].events = POLLPRI;
    pfds[1].fd = monitor->shutdown_fd;
    pfds[1].events = POLLIN;

    int ret = ::poll(pfds, 2, -1);
    if (ret < 0) {
        if (errno == EINTR) return -1;
        return -1;
    }

    if (pfds[1].revents & POLLIN) {
        // Shutdown signaled — consumer checks shutdown_ flag.
        return 1;
    }

    if (pfds[0].revents & POLLPRI) {
        // Drain the PSI event so poll() will block again.
        char buf[256];
        ssize_t n = ::read(monitor->psi_fd, buf, sizeof(buf) - 1);
        (void)n;
        return 1;  // Low-memory signal received.
    }

    return 0;  // Timeout or spurious wakeup.
}

void PalLowMemSignalShutdown(PalLowMemMonitor* monitor) noexcept {
    char c = 0;
    ssize_t written = ::write(monitor->shutdown_write, &c, 1);
    (void)written;
}

void PalLowMemDestroy(PalLowMemMonitor* monitor) noexcept {
    if (!monitor) return;
    ::close(monitor->psi_fd);
    ::close(monitor->shutdown_fd);
    ::close(monitor->shutdown_write);
    delete monitor;
}

}  // namespace chaos::il2cpp::pal
