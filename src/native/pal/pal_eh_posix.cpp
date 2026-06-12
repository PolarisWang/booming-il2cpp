// pal_eh_posix.cpp — POSIX native call wrapper with sigsetjmp/siglongjmp EH
//
// Replaces the previous passthrough implementation with real hardware
// exception protection using sigsetjmp/siglongjmp + SIGSEGV/SIGBUS handler.
//
// Architecture:
//   Thread-local sigjmp_buf saves the execution context before calling fn().
//   If fn() triggers SIGSEGV/SIGBUS, the signal handler checks a TLS flag
//   (g_pal_try_active) and siglongjmps back to the saved context.
//   The signal handler chains to the previous handler when not in a protected
//   region, so JIT signal handling (W^X page protection) is not disrupted.
//
// Thread safety:
//   - All state is either thread-local (sigjmp_buf, active flag) or
//     atomically initialized once (s_handler_installed).
//   - Signal handler reads TLS from the faulting thread — no cross-thread
//     data access.

#include <chaos/pal/pal_eh.h>

#include <atomic>
#include <csetjmp>
#include <csignal>
#include <cstring>
#include <new>

namespace chaos::il2cpp::pal {
namespace {

// ── Alternate signal stack (per-thread) ───────────────────────────────────
// The signal handler uses SA_ONSTACK to run on an alternate stack, but
// sigaltstack() must be called per-thread before any protected region.
// Without this, a stack-overflow SIGSEGV runs on the exhausted main stack
// and double-faults.  Size: 32 KB (16× SIGSTKSZ minimum on Linux).
constexpr std::size_t kAltStackSize = 32768;

struct AltStack {
    char  buffer[kAltStackSize];
    bool  registered = false;
};

thread_local AltStack tls_alt_stack;

static void EnsureAltStack() noexcept {
    if (tls_alt_stack.registered) return;

    stack_t ss;
    std::memset(&ss, 0, sizeof(ss));
    ss.ss_sp    = tls_alt_stack.buffer;
    ss.ss_size  = kAltStackSize;
    ss.ss_flags = 0;

    // Failure is non-fatal: the handler falls back to the main stack.
    // This means stack-overflow recovery is unavailable on this thread,
    // but other protected calls (nullptr deref, div-by-zero) still work.
    if (sigaltstack(&ss, nullptr) == 0) {
        tls_alt_stack.registered = true;
    }
}

}  // anonymous namespace (AltStack)

// ── Thread-local state for PalTryCallNoExcept ────────────────────────────
// g_pal_try_jmp_buf: saved execution context for sigsetjmp/siglongjmp.
// g_pal_try_active / g_pal_try_reentry: declared extern in pal_eh.h so
// PalTryResetState() can be inline (avoids link dependency on libchaos_pal.a
// for test/foundation-dll builds).
thread_local sigjmp_buf g_pal_try_jmp_buf;
thread_local bool g_pal_try_active = false;
thread_local int g_pal_try_reentry = 0;

namespace {

// ── Global saved previous signal handlers (installed once) ──────────────
static struct sigaction s_prev_segv;
static struct sigaction s_prev_bus;
static std::atomic<bool> s_handler_installed{false};

// ── Signal handler for SIGSEGV/SIGBUS ──────────────────────────────────
//
// Chain protocol:
//   1. If g_pal_try_active → siglongjmp back to the PalTryCallNoExcept caller.
//      The caller sees sigsetjmp return non-zero and returns true (exception caught).
//   2. Otherwise → chain to the previously installed handler. This ensures the
//      JIT signal handler (LinuxSehHandler) gets its turn for W^X SIGSEGV events.
//   3. If no previous handler → SIG_DFL (OS default: terminate + core dump).
//
// SA_NODEFER ensures re-entrancy: a SIGSEGV during siglongjmp recovery
// (which should not happen in practice) is not masked.
static void PalEhSignalHandler(int sig, siginfo_t* info, void* ucontext) noexcept {
    if (g_pal_try_active) {
        // We are inside a PalTryCallNoExcept region and the callee crashed.
        // Restore the saved context — execution resumes at the sigsetjmp return
        // point with return value 1 (the "caught" path).
        siglongjmp(g_pal_try_jmp_buf, 1);
    }

    // Chain to the previous handler for this signal.
    struct sigaction* prev = (sig == SIGSEGV) ? &s_prev_segv : &s_prev_bus;
    if (prev->sa_flags & SA_SIGINFO) {
        if (prev->sa_sigaction != nullptr) {
            prev->sa_sigaction(sig, info, ucontext);
            return;
        }
    } else if (prev->sa_handler != SIG_DFL && prev->sa_handler != SIG_IGN) {
        prev->sa_handler(sig);
        return;
    }
    // Chain exhausted (SIG_DFL/SIG_IGN): restore default and re-raise so the
    // OS terminates the process.  Returning silently would resume execution at
    // the faulting instruction, causing an infinite signal loop.
    signal(sig, SIG_DFL);
    raise(sig);
}

// ── One-time signal handler installation ────────────────────────────────
// Installs PalEhSignalHandler for SIGSEGV and SIGBUS, saving the previous
// handlers for chain forwarding. Thread-safe via the atomic flag.
static void EnsureHandlerInstalled() noexcept {
    if (s_handler_installed.load(std::memory_order_acquire)) return;

    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = PalEhSignalHandler;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_NODEFER;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGSEGV, &sa, &s_prev_segv);
    sigaction(SIGBUS, &sa, &s_prev_bus);

    s_handler_installed.store(true, std::memory_order_release);
}

}  // anonymous namespace

bool PalTryCallNoExcept(uint64_t (*fn)(uint64_t, uint64_t, uint64_t, uint64_t,
                                        uint64_t, uint64_t, uint64_t, uint64_t),
                         uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
                         uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7,
                         uint64_t& out_result) noexcept
{
    EnsureHandlerInstalled();
    EnsureAltStack();

    ++g_pal_try_reentry;
    if (g_pal_try_reentry > 1) {
        // Nested call: skip sigsetjmp to preserve the outer context.
        // If the inner call faults, the outer sigsetjmp/siglongjmp catches it.
        out_result = fn(a0, a1, a2, a3, a4, a5, a6, a7);
        --g_pal_try_reentry;
        return false;
    }

    g_pal_try_active = true;
    if (sigsetjmp(g_pal_try_jmp_buf, 1) == 0) {
        // Normal path: call the target function.
        out_result = fn(a0, a1, a2, a3, a4, a5, a6, a7);
        g_pal_try_active = false;
        --g_pal_try_reentry;
        return false;
    }

    // Signal was caught and siglongjmp restored this context.
    g_pal_try_active = false;
    --g_pal_try_reentry;
    return true;
}

}  // namespace chaos::il2cpp::pal
