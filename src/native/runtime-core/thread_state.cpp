#include "thread_state.h"

#include <chaos/asan_interface.h>
#include <chaos/log.h>
#include <chaos/profile.h>
#include <chaos/pal/pal_sync.h>
#include <chaos/pal/pal_thread.h>
#include <chaos/pal/pal_preempt.h>   // PalCaptureThreadContext for GC register-window capture
#include <chaos/pal/pal_suspend.h>  // A3: PalSuspendThread / PalResumeThread / PalGetThreadContext

#include "gc_region.h"
#include "gc_root_scanner.h"
#include "gc_static_roots.h"
#include "gc_card_table.h"
#include "gc_heap_manager.h"
#include "gc_young_collector.h"

#include "forbid_suspend.h"

#include "../jit/jit_seh.h"    // FindNativeCodeByAddress for hybrid GC scanning
#include "../jit/jit_method.h"     // JitMethod (slot_map_data for GcSlotMapV0)

#include <atomic>
#include <cstdio>
#include <new>
#include <cstdlib>
#include <thread>
#include <cerrno>
#include <chrono>
#include <cstring>
#if defined(_MSC_VER)
#include <intrin.h>    // _AddressOfReturnAddress()
#include <windows.h>
#else
// GCC/Clang: no standard intrinsic for "address of this frame's return-address
// slot".  _AddressOfReturnAddress() must return a STACK address (return-address
// slot), NOT the return-address value (a code address) — __builtin_return_address(0)
// returns the latter and is semantically wrong here.  On x86-64 SysV and
// AArch64 AAPCS64 the return address sits one pointer above the frame pointer,
// so __builtin_frame_address(0) + sizeof(void*) is the slot address.  (Matches
// WinSehHandler/LinuxSehHandler, which compute g_jit_frame_rsp the same way.)
#define _AddressOfReturnAddress() \
    (reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(__builtin_frame_address(0)) + sizeof(void*)))
#include <ucontext.h>  // ucontext_t for preemptive-suspend register-window capture
#endif

namespace chaos::il2cpp::runtime_core::threading {

using chaos::il2cpp::pal::PalEvent;
using chaos::il2cpp::pal::PalEventCreate;
using chaos::il2cpp::pal::PalEventDestroy;
using chaos::il2cpp::pal::PalGetCurrentThreadId;
using chaos::il2cpp::pal::PalGetStackBounds;
using chaos::il2cpp::pal::PalDuplicateCurrentThreadHandle;
using chaos::il2cpp::pal::PalSetThreadPriority;

// ── T4 frame layout constant (mirrors code_generator.cpp kFrameSize) ─
// The T4 native prologue establishes:
//   push rbp; mov rbp, rsp; push rbx; push rsi; sub rsp, <frame_size>
// where frame_size = 864 (32 shadow + 512 GPR file + 256 FPR file + 64 CallVirtArgs).
// RSP after prologue = entry_rsp - 32 - 864.
// RBP = entry_rsp - 16.
// For GC scanning: frame_ptr (base of GcSlotMap offsets) = RSP = RBP - 880.
static constexpr uint32_t kJitFrameSize = 864;
static constexpr uint32_t kJitRbpToFramePtr = 16 + 32 + kJitFrameSize;  // 880

// ── TLS definitions ──────────────────────────────────────────────────

thread_local ManagedThread* tls_this_thread  = nullptr;
thread_local int32_t        tls_this_thread_id = kMainThreadId;

thread_local int32_t        tls_forbid_suspend_depth = 0;

/// Ref-counted preemptive-mode depth (CoreCLR preemptive_count analog).
/// EnterPreemptiveMode bumps, EnterCooperativeMode decrements; only the
/// outermost transitions flip gc_mode / rendezvous.  Enables safe nesting of
/// ScopedPreemptiveMode guards and the GcSpinLock safepoint-aware spin loop.
thread_local int32_t        tls_preemptive_depth = 0;

namespace {

/// Lock-free singly-linked list of all registered ManagedThread entries.
/// Head is updated atomically on register; entries are never removed
/// (marked !is_running instead) to avoid ABA during concurrent iteration.
std::atomic<ManagedThread*> s_thread_list{nullptr};

/// Monotonically increasing thread ID allocator.
std::atomic<int32_t> s_next_thread_id{kMainThreadId + 1};

/// Map ManagedThreadPriority to OS thread priority.
/// Called on RegisterThread and chaos_thread_set_priority.
int32_t OsThreadPriorityFromManaged(ManagedThreadPriority pri) noexcept {
    int level = static_cast<int>(pri);
    if (!PalSetThreadPriority(level)) {
        CHAOS_IL2CPP_LOG_WARN_M("Thread", "PalSetThreadPriority failed for level {0}", level);
    }
    // Return the priority value for diagnostic purposes.
    return level;
}

}  // anonymous namespace

void RegisterThread(int32_t managed_id, void* managed_obj) noexcept {
    auto* thread = new ManagedThread();
    thread->managed_id     = managed_id;
    thread->managed_object = managed_obj;
    thread->is_running     = true;
    SetThreadState(*thread, ManagedThreadState::Running);

    // Create auto-reset event for safepoint wait (initially non-signaled).
    thread->suspend_event = PalEventCreate(false, false);

    // Store OS thread ID for signal-based preemptive suspend (Linux).
    thread->os_thread_id = PalGetCurrentThreadId();

    // Duplicate OS thread handle for APC-based safepoint fallback (Windows).
    thread->os_handle = PalDuplicateCurrentThreadHandle();

    // Capture stack bounds for conservative root scanning during full GC.
    PalGetStackBounds(thread->stack_base, thread->stack_limit);

    // Publish to TLS for O(1) self-lookup.
    tls_this_thread    = thread;
    tls_this_thread_id = managed_id;

    // Lock-free insert at head of the global thread list.
    ManagedThread* expected = s_thread_list.load(std::memory_order_acquire);
    do {
        thread->next.store(expected, std::memory_order_relaxed);
    } while (!s_thread_list.compare_exchange_weak(expected, thread,
        std::memory_order_release, std::memory_order_acquire));

    // Server GC: bind this thread to its NUMA-aware heap.
    SetThreadHeap();
}

void UnregisterThread() noexcept {
    auto* thread = tls_this_thread;
    if (thread == nullptr) return;

    thread->is_running = false;
    SetThreadState(*thread, ManagedThreadState::Stopped);

    // Return the TLS nursery to the region manager before clearing TLS.
    // Otherwise the nursery region leaks until process exit.
    TeardownTlsNursery();

    // TLS still points to the entry so EnumerateThreads callbacks can
    // safely query the current thread.  The entry is leaked intentionally
    // (lives for process lifetime like most runtime-instantiated metadata).

    // Close the safepoint event handle.
    if (thread->suspend_event != nullptr) {
        PalEventDestroy(thread->suspend_event);
        thread->suspend_event = nullptr;
    }

    // Server GC: clear heap binding before TLS clear.
    ClearThreadHeap();

    tls_this_thread    = nullptr;
    tls_this_thread_id = 0;

    // Unregister from the profile system so ProfileDump does not read freed
    // thread_local storage (g_tls_profile is destroyed after this function
    // returns).
    chaos::il2cpp::common::UnregisterThread(g_tls_profile);
}

int32_t AllocateThreadId() noexcept {
    return s_next_thread_id.fetch_add(1, std::memory_order_relaxed);
}

void EnumerateThreads(bool (*callback)(ManagedThread*)) noexcept {
    ForbidSuspendScope forbid;
    for (auto* entry = s_thread_list.load(std::memory_order_acquire);
         entry != nullptr;
         entry = entry->next.load(std::memory_order_acquire)) {
        if (entry->is_running) {
            if (!callback(entry)) break;
        }
    }
}

int32_t GetThreadCount() noexcept {
    int32_t count = 0;
    for (auto* entry = s_thread_list.load(std::memory_order_acquire);
         entry != nullptr;
         entry = entry->next.load(std::memory_order_acquire)) {
        if (entry->is_running) ++count;
    }
    return count;
}

// ── extern "C" bridges for threading_stubs ────────────────────────
// MSVC generates C-linkage (undecorated) references from inside
// extern "C" blocks, so threading_stubs.cpp cannot call
// EnumerateThreads or access tls_this_thread directly.  These bridges
// are defined here where C++ name lookup resolves correctly, and the
// extern "C" linkage on the function names matches the C-linkage
// references from threading_stubs.cpp.

extern "C" void chaos_enumerate_threads(bool (*callback)(ManagedThread*)) noexcept {
    EnumerateThreads(callback);
}

extern "C" ManagedThread* chaos_get_tls_this_thread() noexcept {
    return tls_this_thread;
}

/// Phase 2 (C): populate @a thread's register window (gc_reg_file[16],
/// gc_num_gprs) via the cross-platform PAL capture primitive, from the thread's
/// own capture slot (set in PreemptiveSuspendHandler).  Indexed by physical x64
/// GPR (RAX=0..R15=15), matching the register encodings in GcSafepointV0.
///   - Reliability gate: only a preemptively-suspended thread with a PAL-captured
///     context yields a window.  Cooperative/trampoline-redirected threads and
///     Windows (no reliable capture) leave gc_num_gprs=0 → register-root
///     reporting is skipped (stack-slot floor preserved, never under-retains).
static void CaptureThreadRegisterWindow(ManagedThread* thread) noexcept {
    thread->gc_num_gprs = 0;
    // Runtime-mode gate: only a preemptively-suspended thread is parked at its
    // own JIT safepoint (cooperative threads are trampoline-redirected/cleared).
    if (!thread->preemptive_suspended.load(std::memory_order_acquire))
        return;
    // Platform gate: PAL returns false when no reliable capture exists for this
    // slot (Windows APC-park) → gc_num_gprs stays 0.
    uint64_t tmp[16];
    uint32_t n = 0;
    if (chaos::il2cpp::pal::PalCaptureThreadContext(thread->gc_capture_slot, tmp, &n) &&
        n > 0) {
        std::memcpy(thread->gc_reg_file, tmp, n * sizeof(uint64_t));
        thread->gc_num_gprs = n;
    }
}

void GcScanAllThreadRoots(void (*callback)(void* root_addr, bool is_interior, void* user_data), void* user_data) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("GcScanAllThreadRoots");

    // Walk all registered threads and conservatively scan their stacks.
    // Since we hold the global safepoint (suspend_seq non-zero), all managed
    // threads are either waiting on suspend_event or have acknowledged the
    // safepoint — their stacks are in a consistent state for scanning.
    //
    // We scan every pointer-aligned slot in the stack range and report
    // it as a potential root.  The mark phase's caller will check whether
    // the value at each slot points into GC-managed memory.

    // Use file-scoped statics to bridge the C function pointer API of
    // EnumerateThreads.  This is safe because GcScanAllThreadRoots is only
    // ever called from within a STW safepoint (generation is odd), and
    // the safepoint protocol ensures only one GC thread is active at a time.
    // No concurrent clobbering is possible.
    static void (*s_callback)(void*, bool, void*) = nullptr;
    static void* s_user_data = nullptr;
    s_callback = callback;
    s_user_data = user_data;

    EnumerateThreads([](ManagedThread* thread) -> bool {

        // If the current thread is calling this, skip self.


        // Phase 2: capture this thread's register window (physical GPR values
        // at GC suspension) for safepoint register-root reporting.  Populated
        // from the ucontext (Linux) or GetThreadContext (Windows, 2b).  When no
        // window is available (gc_num_gprs==0) register roots are skipped and
        // stack-slot scanning remains the sole source — never under-retains.
        CaptureThreadRegisterWindow(thread);
        const void* const* gpr_window =
            (thread->gc_num_gprs > 0) ? reinterpret_cast<const void* const*>(thread->gc_reg_file) : nullptr;

        // Conservatively scan the full stack range.
        // BOUNDARY FIX (see below): for the calling thread (self), use the
        // CURRENT live frame pointer as the scan lower bound instead of the
        // stale thread->stack_limit captured at RegisterThread.
        bool is_self = (thread == tls_this_thread);
        char* scan_start;
        if (is_self) {
            // _AddressOfReturnAddress() gives the address of the return address
            // on the current frame — i.e. the current stack pointer.  Use this
            // as the live lower bound so we never read below the active frame.
            scan_start = static_cast<char*>(_AddressOfReturnAddress());
        } else {
            // Other threads (BGC, finalizer, workers) are parked at a GC
            // safepoint; their entry-time limit is no worse than current code.
            scan_start = static_cast<char*>(thread->stack_limit);
        }
        char* scan_end   = static_cast<char*>(thread->stack_base);


        uintptr_t start_aligned = (reinterpret_cast<uintptr_t>(scan_start) + sizeof(void*) - 1)
            & ~static_cast<uintptr_t>(sizeof(void*) - 1);
        uintptr_t end_aligned = reinterpret_cast<uintptr_t>(scan_end)
            & ~static_cast<uintptr_t>(sizeof(void*) - 1);

        // ── Phase 1: Full-stack conservative scan ─────────────────
        // Pre-filter candidates: keep the cheap old-gen-base fast path, but
        // ALSO accept nursery pointers that fall below g_heap_base (old-gen
        // base is NOT the whole-heap lower bound — nursery regions are
        // allocated separately via RegionManager and can sit below it).
        // The mark phase caller performs the authoritative GC-heap-membership
        // test, so this pre-filter only decides "worth reporting as a candidate".
        //
        // BOUNDARY FIX (see above): the scan lower bound for the calling
        // thread is now the live frame pointer, so the loop below no longer
        // reads ASan redzones between the live frames.
        //
        // Stack-interior pointer filter (CoreCLR-aligned, gcenv.ee.cpp L160-176):
        // a stack slot whose VALUE points inside this thread's own stack
        // ([stack_limit, stack_base)) is an INTERIOR stack pointer (e.g. a
        // `&local` address a native frame may hold), NOT a GC-heap root.
        // Without this filter, a value that coincidentally falls into the
        // [g_heap_base, ...) or nursery address range would be reported as a
        // candidate root — the mark phase catches it, but the cost of a false
        // positive is a wasted candidate that could (in rare address-space
        // overlap scenarios) cause incorrect relocation.  The filter uses
        // thread->stack_limit (the full registered stack extent), not scan_start,
        // because interior pointers can legitimately point to any part of the
        // thread's stack, including frames below the current live frame.
        uintptr_t th_lo = reinterpret_cast<uintptr_t>(thread->stack_limit);
        uintptr_t th_hi = reinterpret_cast<uintptr_t>(thread->stack_base);
        for (uintptr_t slot = start_aligned; slot < end_aligned; slot += sizeof(void*)) {
            auto* val_ptr = reinterpret_cast<void**>(slot);
            // Probe sheds ASan only for genuinely poisoned redzone slots; live
            // stack slots stay instrumented (review #2/#4).
            if (auto* read = static_cast<void*>(
                    chaos::il2cpp::common::AsanReadPtrNoCheck(val_ptr));
                read != nullptr &&
                (reinterpret_cast<uintptr_t>(read) >= g_heap_base ||
                 IsInNursery(read))) {
                // Skip stack-interior pointers: a value pointing within the
                // scanned thread's stack is an interior reference, not a heap
                // root.  (CoreCLR conservatively reports everything as
                // INTERIOR|PINNED and never relocates it; here we must not even
                // report it, since our relocation phase writes conservative
                // root slots back.)
                //
                // Safety floor: even if a value passes the pre-filter above
                // (heap/nursery range) AND lands inside the stack range, the
                // mark phase (TryMarkRoot) is authoritative — FindPage on a
                // stack-range value returns nullptr / non-in-use / non-scanning
                // page, so it is rejected regardless.  Discarding here only
                // avoids firing a callback for a value the mark phase would
                // reject anyway; it cannot drop a live heap root unless the
                // heap and this thread's stack share address space, which does
                // not occur in practice (heap and stack are disjoint regions).
                uintptr_t rv = reinterpret_cast<uintptr_t>(read);
                if (rv >= th_lo && rv <= th_hi) {
                    continue;
                }
                s_callback(reinterpret_cast<void*>(slot), /*is_interior=*/false, s_user_data);
            }
        }


        // Scan for return addresses that fall within registered T4 code
        // ranges. Each T4 frame has a known stack layout:
        //   [rbp + 8] = return address    (slot we're scanning)
        //   [rbp + 0] = saved old RBP     (must be a stack address)
        //   frame_ptr = RSP = rbp - 880   (base for GcSlotMap offsets)
        for (uintptr_t slot = start_aligned; slot < end_aligned; slot += sizeof(void*)) {
            void* val = static_cast<void*>(
                chaos::il2cpp::common::AsanReadPtrNoCheck(
                    reinterpret_cast<void*>(slot)));
            const auto* nm = chaos::il2cpp::jit::FindNativeCodeByAddress(val);
            if (nm == nullptr) continue;
            if (nm->slot_map_data == nullptr) continue;

            if (slot < start_aligned + sizeof(void*)) continue;
            uintptr_t saved_rbp = reinterpret_cast<uintptr_t>(
                chaos::il2cpp::common::AsanReadPtrNoCheck(
                    reinterpret_cast<void*>(slot - sizeof(void*))));
            if (saved_rbp < reinterpret_cast<uintptr_t>(scan_start) ||
                saved_rbp > reinterpret_cast<uintptr_t>(scan_end)) continue;

            uintptr_t t4_rbp = slot - sizeof(void*);
            // Use per-method RBP-to-RSP offset (register caching changes the
            // distance between RBP and the stack frame base).
            uint32_t rbpoff = nm->rbp_to_rsp_offset;
            if (rbpoff == 0) rbpoff = kJitRbpToFramePtr;  // legacy fallback
            void* frame_ptr = reinterpret_cast<uint8_t*>(t4_rbp) - rbpoff;
            auto* sm = static_cast<const GcSlotMapV0*>(nm->slot_map_data);

            ManagedFrameInfo info;
            info.frame_ptr = frame_ptr;
            info.frame_size = sm->frame_size;
            info.return_address = val;

            // T2.2-A: prefer per-safepoint precise scanning when a GcPointMapV0
            // is available — reports only the roots live at this return offset
            // (binary-searched), instead of the whole-method union GcSlotMapV0.
            // Register roots (Task B) are added when num_live_regs is populated.
            const auto* point_map = static_cast<const GcPointMapV0*>(nm->gc_point_map_data);
            if (point_map != nullptr) {
                // Phase 2 (2a): pass this thread's captured register window so a
                // safepoint's live volatile-register roots are also scanned
                // (additive to the stack slots below).  gpr_window is nullptr
                // when no window was captured (no under-retain).
                GcScanPreciseSafepoint(info, *point_map, nm->code, gpr_window,
                                       thread->gc_num_gprs, s_callback, s_user_data);
            } else {
                GcScanPreciseFrame(info, *sm, s_callback, s_user_data);
            }
        }

        // ── Phase 2b: Interpreter frame precise scanning ─────────
        // Walk the interpreter frame chain (FastFrame/RegisterFrame) and
        // precisely scan each frame using runtime type tags.
        // Unlike JIT frames (which have static GcSlotMaps), interpreter
        // frames have dynamic stack/locals arrays where each slot carries
        // a ValueTag byte indicating whether it's an ObjectRef.
        auto* interp_scanner = GcGetInterpFrameScanner();
        if (interp_scanner != nullptr) {
            void* interp_root = thread->current_interp_frame.load(
                std::memory_order_acquire);
            if (interp_root != nullptr) {
                interp_scanner(interp_root, s_callback, s_user_data);
            }
        }

        return true;  // continue enumeration
    });

    // Phase 3: Scan registered static root ranges (ALC-isolated static fields).
    GcScanStaticRoots(s_callback, s_user_data);
}

// ── extern "C" write-barrier critical-section bridge (generated AOT code) ──
// The managed Codegen emitter cannot see forbid_suspend.h / thread_state.h, so
// it emits these two pairing calls around a store→card sequence instead of the
// native RAII scope.  Same semantics as BarrierCriticalSectionScope: enter
// BEFORE the object store (ack-and-continue + barrier_inflight=1), exit AFTER
// the card is dirtied (release clear of barrier_inflight=0).  The safepoint
// coordinator waits for barrier_inflight to reach 0 before young-GC Phase-1.
extern "C" void chaos_barrier_enter() noexcept {
    using namespace chaos::il2cpp::runtime_core::threading;
    ++tls_forbid_suspend_depth;                      // anti-deadlock: ack-and-continue
    if (auto* t = tls_this_thread; t != nullptr)
        t->barrier_inflight.store(1, std::memory_order_relaxed);
}

extern "C" void chaos_barrier_exit() noexcept {
    using namespace chaos::il2cpp::runtime_core::threading;
    if (auto* t = tls_this_thread; t != nullptr)
        t->barrier_inflight.store(0, std::memory_order_release);
    --tls_forbid_suspend_depth;
}

}  // namespace chaos::il2cpp::runtime_core::threading