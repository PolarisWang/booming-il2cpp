#include "thread_state.h"

#include <chaos/log.h>
#include <chaos/profile.h>
#include <chaos/pal/pal_sync.h>
#include <chaos/pal/pal_preempt.h>
#include <chaos/pal/pal_suspend.h>  // A3: PalSuspendThread / PalResumeThread / PalGetThreadContext

#include "gc_region.h"
#include "gc/gc_suspend_trampoline.h"
#include "gc_root_scanner.h"
#include "gc_static_roots.h"
#include "generated_code_compat.h"  // chaos_managed_exception for Thread.Abort throw

#include "forbid_suspend.h"

#include "../jit/jit_seh.h"    // FindNativeCodeByAddress for hybrid GC scanning
#include "../jit/jit_method.h"     // JitMethod (slot_map_data for GcSlotMapV0)

#include <atomic>
#include <cstdio>
#include <thread>
#include <cerrno>
#include <chrono>
#include <cstring>
#if defined(_MSC_VER)
#include <intrin.h>
#include <windows.h>
#else
#include <ucontext.h>  // ucontext_t for preemptive-suspend register-window capture
#endif

// ── Per-thread handshake safepoint ───────────────────────────────────
//
// Each ManagedThread has suspend_seq and suspend_ack fields.
// RequestGlobalSafepoint sets each thread's suspend_seq to a monotonic
// epoch, then waits per-thread for suspend_ack.  SafepointPoll checks
// the thread-local suspend_seq (fast path = 0, ~1 cycle) and if
// non-zero, sets suspend_ack and waits on suspend_event (zero CPU).
// ReleaseGlobalSafepoint clears suspend_seq for all threads and signals
// suspend_event to wake them.
//
// Nesting: thread_local s_safepoint_depth tracks re-entrancy.  Only the
// outermost Request/Release pair performs the full handshake.
//
// Single-owner: s_safepoint_owner CAS prevents two threads from both
// holding the safepoint.

namespace chaos::il2cpp::runtime_core::threading {

using chaos::il2cpp::pal::PalEventWait;
using chaos::il2cpp::pal::PalEventSet;
using chaos::il2cpp::pal::PalPreemptInit;
using chaos::il2cpp::pal::PalPreemptRequest;
using chaos::il2cpp::pal::PalYield;
using chaos::il2cpp::pal::PalHardSuspendSupported;
using chaos::il2cpp::pal::PalSuspendThread;
using chaos::il2cpp::pal::PalResumeThread;
using chaos::il2cpp::pal::PalGetThreadContext;
using chaos::il2cpp::pal::SuspendResult;

namespace {

std::atomic<ManagedThread*> s_safepoint_owner{nullptr};
thread_local int s_safepoint_depth = 0;

/// Monotonic epoch counter for suspend_seq values.
/// Threads compare against their stored value, not this counter directly.
std::atomic<uint32_t> s_safepoint_epoch{1};

// ── Safepoint timeout constants (matching CoreCLR) ──────────────
/// Default safepoint timeout after which non-responding threads are
/// preemptively suspended. 100ms matches CoreCLR's default timeout.
static constexpr uint64_t kSafepointTimeoutNs = 100ULL * 1000000ULL;  // 100 ms

/// Hard timeout: if a thread doesn't ack even after preemptive suspend,
/// force-release the safepoint and proceed with conservative scanning.
static constexpr uint64_t kSafepointHardTimeoutNs = 500ULL * 1000000ULL;  // 500 ms

/// Spin threshold before yielding (matching existing code).
static constexpr int kSpinYieldThreshold = 32768;
}  // anonymous namespace

/// Global returning-thread trap (A3 Hybrid, g_TrapReturningThreads equivalent).
/// Set by RequestGlobalSafepoint before the cooperative→preemptive drive,
/// cleared by ReleaseGlobalSafepoint.  Declared extern in thread_state.h;
/// defined here OUTSIDE the anonymous namespace so the extern declaration in
/// thread_state.h resolves to this definition at link time (a symbol inside an
/// anonymous namespace has internal linkage and would not satisfy the extern).
std::atomic<uint32_t> g_trap_returning_threads{0};

void SafepointPoll() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("SafepointPoll");

    auto* thread = tls_this_thread;
    if (thread == nullptr) return;

    // Fast path: single atomic load of suspend_seq.
    uint32_t seq = thread->suspend_seq.load(std::memory_order_acquire);
    if (seq == 0) [[likely]] {
        // pending_abort check (same as before, ~0.5ns when clear).
        if (thread->pending_abort.load(std::memory_order_acquire)) {
            thread->pending_abort.store(false, std::memory_order_release);
            throw chaos_managed_exception{kManagedExceptionThreadAbort};
        }
        // pending_interrupt check (lower priority than abort).
        if (thread->pending_interrupt.load(std::memory_order_acquire)) {
            thread->pending_interrupt.store(false, std::memory_order_release);
            throw chaos_managed_exception{kManagedExceptionThreadInterrupt};
        }
        return;
    }

    // ── Slow path: safepoint is active ────────────────────────────

    // Acknowledge the safepoint request.
    thread->suspend_ack.store(seq, std::memory_order_release);

    // Preemptive mode: confirm and return immediately.
    // The thread is in native code and will not access managed heap.
    if (thread->gc_mode.load(std::memory_order_acquire) == kGcModePreemptive) {
        return;
    }

    // ForbidSuspend is active: acknowledge but don't block.
    // The critical section must complete without waiting. The GC proceeds
    // thinking this thread is at a safe point; when the ForbidSuspendScope
    // exits, the next SafepointPoll will properly wait if the safepoint
    // is still active.
    if (tls_forbid_suspend_depth > 0) [[unlikely]] {
        return;
    }

    // Cooperative mode: wait on event (zero CPU, infinite wait).
    // ReleaseGlobalSafepoint will set the event when all threads are done.
    //
    // CRITICAL: the safepoint OWNER must NOT wait here — it set suspend_seq
    // for itself (including in RequestGlobalSafepoint).  If the owner enters
    // SafepointPoll from within a GC cycle (e.g., via a ScopedPreemptiveMode
    // guard in a GC helper function that toggles modes via the GcSpinLock
    // spin toggle), it must not block on its own event.  The owner's thread
    // is the GC driver and should never wait for the GC.
    if (s_safepoint_owner.load(std::memory_order_acquire) == thread) {
        return;
    }
    //
    // Re-check suspend_seq after waking (lost-wakeup protection).  Without
    // this re-check, a thread woken by the N-th release may miss epoch N+1
    // already being published while we were parked (the auto-reset event
    // was Signaled once, consumed by the wake, and the publisher of N+1
    // never re-signals it because the thread's suspend_ack still shows the
    // old epoch, so the coordinator counts it unresponsive → hard timeout
    // forced-release → conservative scan races the mutator → OOM/crash).
    if (thread->suspend_event != nullptr) {
        for (;;) {
            uint32_t cur_seq = thread->suspend_seq.load(std::memory_order_acquire);
            if (cur_seq == 0) break;
            thread->suspend_ack.store(cur_seq, std::memory_order_release);
            PalEventWait(thread->suspend_event, UINT64_MAX);
        }
    } else {
        // Fallback: spin if event not available.
        while (thread->suspend_seq.load(std::memory_order_acquire) != 0) {
            CHAOS_IL2CPP_PAUSE_HINT();
        }
    }
}

void EnterCooperativeMode() noexcept {
    auto* thread = tls_this_thread;
    if (thread == nullptr) return;

    // Ref-counted (CoreCLR preemptive_count semantics): EnterPreemptiveMode /
    // EnterCooperativeMode may nest via ScopedPreemptiveMode guards and the
    // GcSpinLock safepoint-aware spin (WaitLongerNoInstru).  Only the
    // OUTERMOST exit (depth transitions to 0) actually switches gc_mode back
    // to cooperative and participates in a pending safepoint.  Inner exits
    // (still inside another preemptive region) must NOT rendezvous — doing so
    // would block the thread that still holds a lock the GC may need.
    if (--tls_preemptive_depth > 0) return;

    // ── A3 Hybrid: returning-thread trap check (DisablePreemptiveGC semantics) ──
    // If a safepoint is in progress (trap set), a thread about to enter
    // cooperative mode must NOT enter managed code mid-GC.  It renders a
    // rendezvous (ack any request + wait on suspend_event) until the trap
    // clears, then enters cooperative.  This prevents a window where a thread
    // switches to cooperative after the owner set the trap but before setting
    // this thread's suspend_seq.
    //
    // CRITICAL: the safepoint OWNER (the GC thread holding the safepoint) MUST
    // NOT rendezvous here — it set the trap itself and would otherwise wait on
    // its own suspend_event forever (deadlock).  The owner drives the GC
    // directly and is already outside the managed heap contract.
    while (TrapReturningThreads() && s_safepoint_owner.load(std::memory_order_acquire) != thread) {
        // Ack any pending request (suspend_seq may be set or about to be set).
        if (thread->suspend_seq.load(std::memory_order_acquire) != 0) {
            thread->suspend_ack.store(thread->suspend_seq.load(std::memory_order_acquire),
                                      std::memory_order_release);
            if (thread->suspend_event != nullptr) {
                // Rendezvous: block on the GC-completion event until release.
                PalEventWait(thread->suspend_event, UINT64_MAX);
            }
        } else {
            // Request not yet published; yield to let the owner proceed.
            PalYield();
        }
    }

    thread->gc_mode.store(kGcModeCooperative, std::memory_order_release);
    // After switching to cooperative, check if a safepoint is already active.
    // If so, this thread must participate in the safepoint.
    SafepointPoll();
}

void EnterPreemptiveMode() noexcept {
    auto* thread = tls_this_thread;
    if (thread == nullptr) return;
    // Ref-counted (CoreCLR preemptive_count): only the FIRST entry flips
    // gc_mode; nested entries (ScopedPreemptiveMode inside ScopedPreemptiveMode,
    // or a GcSpinLock spin toggle inside a guard) just bump the depth.
    if (++tls_preemptive_depth == 1) {
        // Mark as preemptive BEFORE any subsequent SafepointPoll sees the flag.
        thread->gc_mode.store(kGcModePreemptive, std::memory_order_release);
    }
}

/// C-linkage wrapper for SafepointPoll, called from the assembly trampoline
/// (gc_suspend_trampoline_x64).  The assembler cannot call C++ mangled names
/// directly, so this bridge provides a stable extern "C" entry point.
extern "C" void chaos_safepoint_poll() noexcept {
    SafepointPoll();
}

/// Unified preemptive suspend handler — called from Windows APC and
/// POSIX SIGUSR2 signal context.  Acknowledges the safepoint and waits
/// for release.
///
/// On POSIX with SA_SIGINFO: captures the interrupted thread's register
/// state (ucontext_t) from the PAL handler TLS so the GC coordinator
/// can perform precise root scanning of the hijacked thread.
///
/// == Trampoline redirect (Phase 2) ==
/// For cooperative-mode threads, instead of spin-waiting on the limited
/// signal stack (SIGSTKSZ), we acknowledge the safepoint and redirect RIP
/// to gc_suspend_trampoline_x64 via ucontext modification.  The trampoline
/// runs SafepointPoll() on the thread's NORMAL stack, avoiding signal
/// stack overflow during long GC pauses.
static void PreemptiveSuspendHandler(uint64_t epoch) noexcept {
    auto* thread = tls_this_thread;
    if (thread == nullptr) return;

    // ── Barrier critical-section guard (A2b leaf, applies to ALL platforms) ──
    // If this thread is inside a write-barrier store→card critical section
    // (BarrierCriticalSectionScope raised tls_forbid_suspend_depth and set
    // barrier_inflight), it MUST ack-and-continue and NOT park.  Parking here
    // (PalEventWait / signal-stack spin / trampoline redirect) would strand the
    // thread mid-window with barrier_inflight still 1 — the coordinator blocks on
    // barrier_inflight==0 and eventually force-releases at the hard timeout, then
    // scans with the store's card still clean → A2b cross-gen UAF.  Acking-and-
    // continuing lets the thread finish the store + card-dirty and exit the scope
    // (barrier_inflight→0, release), satisfying the coordinator's drain wait; the
    // NEXT SafepointPoll (after the scope exits) properly parks if the safepoint
    // is still active.
    uint32_t seq = thread->suspend_seq.load(std::memory_order_acquire);
    thread->suspend_ack.store(seq, std::memory_order_release);
    if (tls_forbid_suspend_depth > 0 ||
        thread->barrier_inflight.load(std::memory_order_acquire) != 0) [[unlikely]] {
        return;   // ack-and-continue; coordinator waits on barrier_inflight==0
    }

    // Capture ucontext from the PAL signal handler (only available
    // on POSIX with SA_SIGINFO).  Phase 2 C: the ucontext is owned by PAL and
    // keyed by this thread's capture slot (persisted so the GC can read it
    // cross-thread).  The GC coordinator reads the register window via
    // PalCaptureThreadContext during the safepoint wait loop.
    // Defaults are safe on non-POSIX: slot -1 makes PalSetPreemptContext a no-op
    // and PalCaptureThreadContext return false → GC keeps gc_num_gprs=0.
    int cap_slot = -1;
    const void* uctx = nullptr;
#if !defined(_MSC_VER)
    uctx = chaos::il2cpp::pal::PalPreemptGetUcontext();
    thread->gc_capture_slot = chaos::il2cpp::pal::PalGetCaptureSlot();
    cap_slot = thread->gc_capture_slot;

    // Cooperative mode: redirect to trampoline instead of spin-waiting
    // on the signal stack.  The trampoline calls SafepointPoll() on the
    // thread's normal stack, which handles the actual safepoint wait.
    if (thread->gc_mode.load(std::memory_order_acquire) == kGcModeCooperative) {
        // Acknowledge the safepoint so the coordinator sees our response.
        // (seq already loaded + acked at function top by the barrier guard.)
        // Redirect RIP to the trampoline via ucontext modification.
        // This runs on the signal stack but writes to the normal stack
        // (original_rsp - 8) — both are in the same address space.
        if (uctx != nullptr) {
            auto* uc = const_cast<ucontext_t*>(
                static_cast<const ucontext_t*>(uctx));
            uint64_t original_rip =
                static_cast<uint64_t>(uc->uc_mcontext.gregs[REG_RIP]);
            uint64_t original_rsp =
                static_cast<uint64_t>(uc->uc_mcontext.gregs[REG_RSP]);

            // Push original RIP onto the normal stack (at RSP - 8) so the
            // trampoline's RET will return to the interrupted instruction.
            uint64_t new_rsp = original_rsp - sizeof(uint64_t);
            *reinterpret_cast<uint64_t*>(new_rsp) = original_rip;
            uc->uc_mcontext.gregs[REG_RSP] = static_cast<greg_t>(new_rsp);

            // Redirect RIP to the assembly trampoline.
            uc->uc_mcontext.gregs[REG_RIP] =
                reinterpret_cast<uint64_t>(&gc_suspend_trampoline_x64);
        } else {
            // No ucontext available (non-POSIX or missing SA_SIGINFO) —
            // fallback to standard ack + spin-wait on signal stack.
            chaos::il2cpp::pal::PalPreemptiveSuspendAck(
                epoch, thread->suspend_event,
                &thread->suspend_seq, &thread->suspend_ack);
        }

        // Clear ucontext — the trampoline handles the actual wait.  Cooperative
        // mode redirects RIP/RSP, so the saved ucontext is NOT a reliable JIT
        // register window (G5): clear it so the GC sees no window for this thread.
        chaos::il2cpp::pal::PalSetPreemptContext(cap_slot, nullptr);
        return;
    }
#endif // !defined(_MSC_VER)

    // Preemptive mode: standard ack + spin-wait on signal stack.  Expose the
    // captured ucontext as this thread's register window so the GC reads it
    // cross-thread during the safepoint (release ordering pairs with the GC's
    // acquire on PalCaptureThreadContext).
    chaos::il2cpp::pal::PalSetPreemptContext(cap_slot, uctx);
    chaos::il2cpp::pal::PalPreemptiveSuspendAck(
        epoch, thread->suspend_event,
        &thread->suspend_seq, &thread->suspend_ack);

    // Clear ucontext after the safepoint is released (the GC has scanned by now).
    chaos::il2cpp::pal::PalSetPreemptContext(cap_slot, nullptr);
}

/// One-time initialization of the preemptive suspend subsystem.
static std::atomic<bool> s_preempt_inited{false};
static void EnsurePreemptInit() noexcept {
    if (!s_preempt_inited.load(std::memory_order_acquire)) {
        PalPreemptInit(PreemptiveSuspendHandler);
        s_preempt_inited.store(true, std::memory_order_release);
    }
}

extern "C" uint32_t RequestGlobalSafepoint() noexcept {
    // Support nesting: if the calling thread already holds the safepoint,
    // just bump the depth counter and return the current epoch.
    if (s_safepoint_depth > 0) {
        s_safepoint_depth++;
        return s_safepoint_epoch.load(std::memory_order_acquire);
    }

    // Acquire process-level safepoint ownership via CAS.
    auto* self = tls_this_thread;
    if (self != nullptr) {
        ManagedThread* expected = nullptr;
        if (!s_safepoint_owner.compare_exchange_strong(expected, self,
                std::memory_order_acq_rel, std::memory_order_acquire)) {
            // Another thread holds the safepoint — spin-wait with pause.
            // Must call SafepointPoll() to acknowledge a pending safepoint
            // from the owner; otherwise this thread and the owner deadlock
            // (owner waits for our ack, we wait for the owner to release).
            //
            // ForbidSuspendScope prevents SafepointPoll from blocking if
            // the owner requests a new safepoint while we're waiting for
            // ownership. Without this, we'd wait on suspend_event and never
            // acquire the safepoint to release it.
            for (;;) {
                {
                    ForbidSuspendScope forbid;
                    SafepointPoll();
                    CHAOS_IL2CPP_PAUSE_HINT();
                }
                expected = nullptr;
                if (s_safepoint_owner.compare_exchange_strong(expected, self,
                        std::memory_order_acq_rel, std::memory_order_acquire)) {
                    break;
                }
            }
        }
    }

    // Bump the epoch counter.  This becomes the suspend_seq value for
    // all cooperative threads.
    uint32_t epoch = s_safepoint_epoch.fetch_add(1, std::memory_order_acq_rel) + 1;
    s_safepoint_depth = 1;

    // ── A3 Hybrid: set the global returning-thread trap ─────────────────
    // This flag is checked by threads entering cooperative mode (in
    // EnterCooperativeMode, after gc_mode store).  If set, the thread
    // falls back to preemptive and waits on suspend_event (rendezvous)
    // instead of entering managed code mid-GC.
    //
    // The trap is set BEFORE suspend_seq, so a racing thread observing
    // suspend_seq==0 but then enabling cooperative sees the trap and
    // rendezvous — never enters managed code with a safepoint in progress.
    SetTrapReturningThreads();
    // Full memory barrier so the trap is visible cross-thread before the
    // per-thread suspend_seq stores below.  Process-wide on platforms that
    // support it; otherwise the compiler/CPU barrier suffices for the
    // acquire/release pairs on suspend_seq (the existing handshake contract).
    std::atomic_thread_fence(std::memory_order_seq_cst);

    // Set suspend_seq for every cooperative thread.
    // Preemptive threads are excluded: they don't access managed heap,
    // so we don't need to wait for them.
    // NOTE: EnumerateThreads requires C function pointers, so we use
    // file-static helper variables (no captures).  Safe because only
    // one GC thread runs at a time (safepoint owner CAS guarantees this).
    {
        static uint32_t s_set_epoch = 0;
        s_set_epoch = epoch;
        EnumerateThreads([](ManagedThread* t) -> bool {
            if (t->gc_mode.load(std::memory_order_acquire) == kGcModePreemptive)
                return true;
            t->suspend_seq.store(s_set_epoch, std::memory_order_release);
            return true;
        });
    }

    // Wait for each cooperative thread to acknowledge.
    // Spin with pause for ~1ms, then yield to avoid starving the very
    // threads we're waiting on (critical for oversubscribed scenarios).
    // After kSafepointTimeoutNs (100ms), use preemptive suspend fallback
    // matching CoreCLR: QueueUserAPC on Windows, pthread_kill on POSIX.
    // After kSafepointHardTimeoutNs (500ms), force-release with diagnostic.
    {
        static uint32_t s_confirm_epoch = 0;
        static int s_remaining = 0;
        s_confirm_epoch = epoch;

        auto wait_start = std::chrono::steady_clock::now();
        bool preemptive_attempted = false;
        bool hard_timeout = false;

        for (int spin = 0; ; spin++) {
            s_remaining = 0;
            EnumerateThreads([](ManagedThread* t) -> bool {
                if (t->gc_mode.load(std::memory_order_acquire) == kGcModePreemptive)
                    return true;
                if (t == tls_this_thread) return true;
                if (t->suspend_ack.load(std::memory_order_acquire) != s_confirm_epoch)
                    ++s_remaining;
                // A thread in a write-barrier store→card critical section has
                // acked-but-continued (tls_forbid_suspend_depth>0 makes
                // SafepointPoll ack without blocking).  suspend_ack is therefore
                // NOT sufficient to conclude the thread is at a safe point: it may
                // still be between the store and the card-dirty.  Wait for
                // barrier_inflight to reach 0 too, so young-GC Phase-1 never scans
                // an old page whose just-stored nursery edge has a still-clean card
                // (the A2b cross-gen UAF window).
                if (t->barrier_inflight.load(std::memory_order_acquire) != 0)
                    ++s_remaining;
                return true;
            });
            if (s_remaining == 0) break;
            if (hard_timeout) break;  // force-release after hard timeout

            if (spin < kSpinYieldThreshold) {
                CHAOS_IL2CPP_PAUSE_HINT();
            } else {
                PalYield();
                // After yielding for ~100ms with no response, try APC fallback
                // (Windows only — cooperative threads stuck in native code).
                if (spin >= kSpinYieldThreshold + 100000) {
                    EnumerateThreads([](ManagedThread* t) -> bool {
                        if (t->gc_mode.load(std::memory_order_acquire) == kGcModePreemptive)
                            return true;
                        if (t == tls_this_thread) return true;
                        if (t->suspend_ack.load(std::memory_order_acquire) != s_confirm_epoch) {
                            PalPreemptRequest(t->os_handle, t->os_thread_id, s_confirm_epoch);
                        }
                        return true;
                    });
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }

                // Check real-time timeout every ~5000 iterations (~5ms).
                if (spin % 5000 == 0 && !hard_timeout) {
                    auto elapsed = std::chrono::steady_clock::now() - wait_start;
                    uint64_t elapsed_ns = std::chrono::duration_cast<
                        std::chrono::nanoseconds>(elapsed).count();

                    if (!preemptive_attempted && elapsed_ns >= kSafepointTimeoutNs) {
                        preemptive_attempted = true;
                        CHAOS_IL2CPP_LOG_WARN_M("Safepoint",
                            "safepoint timeout: {0} threads unresponsive after {1}ms, "
                            "attempting preemptive suspend",
                            s_remaining, elapsed_ns / 1000000);

                        EnsurePreemptInit();
                        EnumerateThreads([](ManagedThread* t) -> bool {
                            if (t->gc_mode.load(std::memory_order_acquire) == kGcModePreemptive)
                                return true;
                            if (t == tls_this_thread) return true;
                            if (t->suspend_ack.load(std::memory_order_acquire) != s_confirm_epoch) {
                                // First try the soft preemptive request (APC / SIGUSR2).
                                if (PalPreemptRequest(t->os_handle, t->os_thread_id, s_confirm_epoch)) {
                                    t->preemptive_suspended.store(true, std::memory_order_release);
                                } else {
                                    CHAOS_IL2CPP_LOG_WARN_M("Safepoint",
                                        "thread {0} unresponsive, cannot preemptively suspend",
                                        t->managed_id);
                                }
                            }
                            return true;
                        });
                    }

                    // ── A3: hard-suspension drive (pal_suspend.h) ─────────────
                    // After the soft preemptive request (APC/SIGUSR2) has been
                    // delivered and had time to take effect, threads that are still
                    // unresponsive are likely stuck in a long cooperative-mode
                    // native code path that does not poll.  Use PalSuspendThread
                    // (Windows) / kUnsupported (POSIX → retry PalPreemptRequest) to
                    // force a context switch and redirect.
                    //
                    // This matches CoreCLR's Thread::Hijack (threadsuspend.cpp:~3370):
                    // SuspendThread → GetThreadContext → redirect → ResumeThread.
                    // The thread is never scanned while physically suspended.
                    if (preemptive_attempted && !hard_timeout &&
                        elapsed_ns >= kSafepointTimeoutNs + 150ULL * 1000000ULL) {
                        EnumerateThreads([](ManagedThread* t) -> bool {
                            if (t->gc_mode.load(std::memory_order_acquire) == kGcModePreemptive)
                                return true;
                            if (t == tls_this_thread) return true;
                            if (t->suspend_ack.load(std::memory_order_acquire) != s_confirm_epoch) {
                                // Forbid-suspend check: if the target thread's
                                // forbid_suspend_count > 0, skip it — it holds a
                                // lock we may need.  The soft preemptive request
                                // (APC/SIGUSR2) may still deliver.
                                if (t->forbid_suspend_count.load(std::memory_order_acquire) != 0)
                                    return true;

                                if (PalHardSuspendSupported()) {
                                    // Step 1: Acquire the forbid-suspend latch
                                    // ourselves (so no other thread tries to
                                    // suspend us while we hold the target).
                                    // (ForbidSuspendThreadHolder is in the
                                    // threading namespace, same as this file.)
                                    threading::ForbidSuspendThreadHolder self_forbid;

                                    // Step 2: Suspend the target thread.
                                    auto result = PalSuspendThread(t->os_handle);
                                    if (result == SuspendResult::kSuccess ||
                                        result == SuspendResult::kSuspended) {
                                        // Step 3: Read the target's IP (for
                                        // diagnostic / redirect decision).
                                        uint64_t ip = 0;
                                        PalGetThreadContext(t->os_handle, &ip, nullptr, nullptr);

                                        // Re-check forbid_suspend_count on the
                                        // target (CoreCLR pattern: a thread may
                                        // have entered a critical section between
                                        // our first check and the suspend).
                                        if (t->forbid_suspend_count.load(
                                                std::memory_order_acquire) != 0) {
                                            PalResumeThread(t->os_handle);
                                            CHAOS_IL2CPP_LOG_DEBUG_M("Safepoint",
                                                "A3 suspended thread {0} (IP=0x{1:x}) "
                                                "but target forbid >0, resuming",
                                                t->managed_id, ip);
                                            return true;
                                        }

                                        // Step 4: Mark the thread as preemptively
                                        // suspended (the coordinator will skip
                                        // register-root scanning for it).
                                        t->preemptive_suspended.store(true,
                                            std::memory_order_release);

                                        // Step 5: Resume the thread immediately
                                        // (CoreCLR pattern: never hold a thread
                                        // physically suspended — OS GetThreadContext
                                        // is unreliable for root scanning).
                                        PalResumeThread(t->os_handle);

                                        CHAOS_IL2CPP_LOG_DEBUG_M("Safepoint",
                                            "A3 hard-suspended thread {0} (IP=0x{1:x})",
                                            t->managed_id, ip);
                                    } else if (result == SuspendResult::kForbidden) {
                                        CHAOS_IL2CPP_LOG_DEBUG_M("Safepoint",
                                            "A3 cannot suspend thread {0}: forbidden",
                                            t->managed_id);
                                    } else {
                                        CHAOS_IL2CPP_LOG_DEBUG_M("Safepoint",
                                            "A3 PalSuspendThread failed for thread {0}",
                                            t->managed_id);
                                    }
                                } else {
                                    // Non-Windows: retry the soft preemptive
                                    // request (SIGUSR2 trampoline).
                                    PalPreemptRequest(t->os_handle, t->os_thread_id,
                                        s_confirm_epoch);
                                }
                            }
                            return true;
                        });
                    }

                    if (preemptive_attempted && elapsed_ns >= kSafepointHardTimeoutNs) {
                        hard_timeout = true;
                        CHAOS_IL2CPP_LOG_ERROR_M("Safepoint",
                            "safepoint hard timeout: {0} threads still unresponsive "
                            "after {1}ms, forcing release",
                            s_remaining, elapsed_ns / 1000000);
                    }
                }
            }
        }
    }

    return epoch;
}

extern "C" void ReleaseGlobalSafepoint(uint32_t /*epoch*/) noexcept {
    // Support nesting: decrement depth counter.  Only do the full
    // release when the outermost release occurs.
    if (s_safepoint_depth > 1) {
        s_safepoint_depth--;
        return;
    }

    // ── Release all threads from safepoint ────────────────────────────
    // Order is critical:
    //   1. Clear suspend_seq so threads stop waiting
    //   2. Signal events to wake threads
    //   3. THEN release ownership — releasing ownership before clearing
    //      suspend_seq creates a window where a new acquirer sets new
    //      seq values that get cleared by the ongoing iteration, causing
    //      the next safepoint to spin forever waiting for acks that will
    //      never come (the threads see suspend_seq=0 and run freely).

    // Clear suspend_seq for all threads and signal their events.
    EnumerateThreads([](ManagedThread* t) -> bool {
        t->suspend_seq.store(0, std::memory_order_release);
        if (t->suspend_event != nullptr) {
            PalEventSet(t->suspend_event);
        }
        // Resume preemptively suspended threads.
        if (t->preemptive_suspended.load(std::memory_order_acquire)) {
            t->preemptive_suspended.store(false, std::memory_order_release);
        }
        t->safepoint_wait_start_ns = 0;
        return true;
    });

    // ── A3 Hybrid: clear the global returning-thread trap AFTER all threads
    // are woken, so a thread re-entering cooperative mode does not rendezvous
    // again for a safepoint that is over.
    ClearTrapReturningThreads();

    // Release safepoint ownership AFTER all threads are woken.
    s_safepoint_owner.store(nullptr, std::memory_order_release);

    s_safepoint_depth = 0;
}

}  // namespace chaos::il2cpp::runtime_core::threading