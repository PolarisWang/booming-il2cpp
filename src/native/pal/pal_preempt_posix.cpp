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

// ── Phase 2 (C): register-window capture (POSIX/x86-64) ─────────────────
// Pure-PAL per-thread slot pool.  Each thread lazily allocates a slot index;
// the signal handler stores its ucontext under that slot; the GC reads it
// cross-thread via the SAME slot (persisted on ManagedThread::gc_slot).
namespace {
// Fixed slot pool sized for concurrently-suspended threads.  A thread is only
// suspended during an STW safepoint, so a modest bound covers housekeeping + a
// few managed threads; slots are reused across safepoints.
static constexpr int kMaxCaptureSlots = 256;
struct CapturedCtx { std::atomic<const void*> ucontext{nullptr}; };
CapturedCtx g_captured[kMaxCaptureSlots];
std::atomic<int> g_capture_slot_next{0};
thread_local int t_capture_slot = -1;   // lazy per-thread slot index
}  // namespace

int PalGetCaptureSlot() noexcept {
    if (t_capture_slot >= 0)
        return t_capture_slot;
    // Allocate the next free slot (spinning past the bound reuses slot 0; a
    // thread is only captured during STW so collisions cannot overlap).
    int slot = g_capture_slot_next.fetch_add(1, std::memory_order_relaxed);
    if (slot >= kMaxCaptureSlots)
        slot = slot % kMaxCaptureSlots;
    t_capture_slot = slot;
    return slot;
}

void PalSetPreemptContext(int slot, const void* ucontext) noexcept {
    if (slot >= 0 && slot < kMaxCaptureSlots)
        g_captured[slot].ucontext.store(ucontext, std::memory_order_release);
}

bool PalCaptureThreadContext(int slot, uint64_t gpr_values[16], uint32_t* out_num) noexcept {
    if (out_num) *out_num = 0;
    if (slot < 0 || slot >= kMaxCaptureSlots)
        return false;
    const void* uctx = g_captured[slot].ucontext.load(std::memory_order_acquire);
    if (uctx == nullptr)
        return false;
    const auto* uc = static_cast<const ucontext_t*>(uctx);
    const greg_t* g = uc->uc_mcontext.gregs;
    // glibc gregs[] index -> physical x86-64 register (RAX=0..R15=15).
    // glibc: REG_RAX=13 REG_RCX=14 REG_RDX=12 REG_RBX=11 REG_RSI=9 REG_RDI=8
    //        REG_R8=0 REG_R9=1 REG_R10=2 REG_R11=3 REG_R12=4 REG_R13=5
    //        REG_R14=6 REG_R15=7.
    // x64-only for now (G7): ARM64 uses a different GPR count/layout.
    gpr_values[0]  = static_cast<uint64_t>(g[REG_RAX]);
    gpr_values[1]  = static_cast<uint64_t>(g[REG_RCX]);
    gpr_values[2]  = static_cast<uint64_t>(g[REG_RDX]);
    gpr_values[3]  = static_cast<uint64_t>(g[REG_RBX]);
    gpr_values[8]  = static_cast<uint64_t>(g[REG_R8]);
    gpr_values[9]  = static_cast<uint64_t>(g[REG_R9]);
    gpr_values[10] = static_cast<uint64_t>(g[REG_R10]);
    gpr_values[11] = static_cast<uint64_t>(g[REG_R11]);
    gpr_values[12] = static_cast<uint64_t>(g[REG_R12]);
    gpr_values[13] = static_cast<uint64_t>(g[REG_R13]);
    gpr_values[14] = static_cast<uint64_t>(g[REG_R14]);
    gpr_values[15] = static_cast<uint64_t>(g[REG_R15]);
    if (out_num) *out_num = 16;
    return true;
}

}  // namespace chaos::il2cpp::pal
