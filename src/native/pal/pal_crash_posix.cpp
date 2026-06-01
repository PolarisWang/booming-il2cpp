// pal_crash_posix.cpp — POSIX signal handler crash handling

#include <chaos/pal/pal_crash.h>

#include <signal.h>
#include <csignal>
#include <cstdlib>
#include <cstring>

namespace chaos::il2cpp::pal {

static PalCrashCallback s_crash_callback = nullptr;
static struct sigaction s_prev_handlers[NSIG];  // saved previous handlers

// Signals we handle: SIGSEGV, SIGABRT, SIGBUS, SIGILL, SIGFPE.
static constexpr int kCrashSignals[] = { SIGSEGV, SIGABRT, SIGBUS, SIGILL, SIGFPE };
static constexpr int kCrashSignalCount = sizeof(kCrashSignals) / sizeof(kCrashSignals[0]);

static void SignalHandler(int sig, siginfo_t* info, void* ucontext) noexcept {
    if (!s_crash_callback) {
        // Re-raise with default handler (restored).
        struct sigaction sa;
        std::memset(&sa, 0, sizeof(sa));
        sa.sa_handler = SIG_DFL;
        sigaction(sig, &sa, nullptr);
        raise(sig);
        return;
    }

    CrashContext ctx;
    ctx.signal = sig;
    ctx.fault_address = info ? info->si_addr : nullptr;

    s_crash_callback(ctx);

    // After callback, re-raise with default handler.
    PalRemoveCrashHandler();
    raise(sig);
}

bool PalRegisterCrashHandler(PalCrashCallback callback) noexcept {
    if (s_crash_callback) {
        PalRemoveCrashHandler();
    }
    s_crash_callback = callback;

    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = SignalHandler;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigemptyset(&sa.sa_mask);

    bool ok = true;
    for (int i = 0; i < kCrashSignalCount; ++i) {
        if (sigaction(kCrashSignals[i], &sa, &s_prev_handlers[i]) != 0) {
            ok = false;
        }
    }
    return ok;
}

void PalRemoveCrashHandler() noexcept {
    s_crash_callback = nullptr;
    // Restore saved handlers (SIG_DFL if none was saved).
    for (int i = 0; i < kCrashSignalCount; ++i) {
        sigaction(kCrashSignals[i], &s_prev_handlers[i], nullptr);
    }
}

void PalDebugBreak() noexcept {
    ::raise(SIGTRAP);
}

}  // namespace chaos::il2cpp::pal
