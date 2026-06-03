// pal_preempt_posix.cpp — POSIX (Linux) preemptive suspend (pthread_kill SIGUSR2)

#include <chaos/pal/pal_preempt.h>

#include <chaos/pal/pal_sync.h>
#include <chaos/pal/pal_thread.h>

#include <atomic>
#include <cstring>
#include <signal.h>

namespace chaos::il2cpp::pal {

namespace {

PalPreemptCallback s_preempt_callback = nullptr;
std::atomic<bool> s_handler_installed{false};

/// Signal forwarder: calls the registered consumer callback from
/// SIGUSR2 signal context.  epoch=0 because signal handlers don't
/// receive user data; the callback reads the actual epoch from TLS.
static void PalPreemptSignalForwarder(int sig) noexcept {
    if (sig != SIGUSR2) return;
    if (s_preempt_callback) {
        s_preempt_callback(0);
    }
}

}  // anonymous namespace

void PalPreemptInit(PalPreemptCallback callback) noexcept {
    s_preempt_callback = callback;

    if (!s_handler_installed.load(std::memory_order_acquire)) {
        struct sigaction sa;
        std::memset(&sa, 0, sizeof(sa));
        sa.sa_handler = PalPreemptSignalForwarder;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART | SA_NODEFER;
        sigaction(SIGUSR2, &sa, nullptr);
        s_handler_installed.store(true, std::memory_order_release);
    }
}

bool PalPreemptRequest(void* /*os_handle*/, uint64_t os_thread_id,
                        uint64_t /*epoch*/) noexcept {
    if (os_thread_id == 0) return false;
    return ::pthread_kill(static_cast<pthread_t>(os_thread_id), SIGUSR2) == 0;
}

void PalPreemptiveSuspendAck(uint64_t /*epoch*/, PalEvent* /*suspend_event*/,
                              std::atomic<uint32_t>* suspend_seq,
                              std::atomic<uint32_t>* suspend_ack) noexcept {
    uint32_t seq = suspend_seq->load(std::memory_order_acquire);
    suspend_ack->store(seq, std::memory_order_release);
    while (suspend_seq->load(std::memory_order_acquire) != 0) {
        PalYield();
    }
}

}  // namespace chaos::il2cpp::pal
