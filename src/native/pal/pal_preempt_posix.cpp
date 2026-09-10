// pal_preempt_posix.cpp — POSIX (Linux) preemptive suspend (pthread_kill SIGUSR2)
//
// Uses SA_SIGINFO to capture the interrupted thread's register state
// (ucontext_t) for precise root scanning and optional RIP redirect
// (thread hijacking trampoline).

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

/// Thread-local storage for the ucontext_t captured by SA_SIGINFO.
/// Set before calling the callback; cleared after the callback returns.
/// Valid only within the scope of a PalPreemptCallback invocation.
thread_local const void* tls_preempt_ucontext = nullptr;

/// Signal forwarder: called from SIGUSR2 signal context with SA_SIGINFO.
/// Saves the ucontext_t to TLS, then calls the registered consumer callback
/// with epoch=0 (the callback reads the actual epoch from TLS suspend_seq).
static void PalPreemptSignalForwarder(int sig, siginfo_t* /*info*/, void* ucontext) noexcept {
    if (sig != SIGUSR2) return;
    // Save ucontext before calling callback so the consumer can access
    // interrupted register state (RIP, RSP, RBP on x64).
    tls_preempt_ucontext = ucontext;
    if (s_preempt_callback) {
        s_preempt_callback(0);
    }
    tls_preempt_ucontext = nullptr;
}

}  // anonymous namespace

void PalPreemptInit(PalPreemptCallback callback) noexcept {
    s_preempt_callback = callback;

    if (!s_handler_installed.load(std::memory_order_acquire)) {
        struct sigaction sa;
        std::memset(&sa, 0, sizeof(sa));
        sa.sa_sigaction = PalPreemptSignalForwarder;
        sigemptyset(&sa.sa_mask);
        // SA_SIGINFO: provides ucontext_t with register state at interruption.
        // SA_RESTART: transparently restart interrupted syscalls.
        // SA_NODEFER: allow re-entrant SIGUSR2 (important for nested handling).
        // SA_ONSTACK: use alternate signal stack to avoid stack overflow in
        //   deep call chains (sigaltstack set up by pal_eh_posix.cpp).
        sa.sa_flags = SA_SIGINFO | SA_RESTART | SA_NODEFER | SA_ONSTACK;
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

const void* PalPreemptGetUcontext() noexcept {
    return tls_preempt_ucontext;
}

}  // namespace chaos::il2cpp::pal
