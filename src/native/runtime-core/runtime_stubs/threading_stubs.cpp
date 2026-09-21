// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// threading_stubs.cpp — Threading stub implementations
#include <chaos/native_types.h>
#include <chaos/thread.h>
#include "generated_code_compat.h"

#include "thread_state.h"
#include "execution_context.h"
#include "runtime_core.h"
#include "bootstrap/bootstrap.h"
#include "gc/gc_transition.h"
#include "core/gc_alloc_stubs.h"
#include "stub_common.h"        // ManagedArrayAccessor / get_managed_array
#include "../exception_helpers.h"  // RaiseArgumentNullException / RaiseArgumentException

#include <memory>
#include <thread>

#if defined(_WIN32)
#include <windows.h>   // GetCurrentProcessorNumber
#elif defined(__linux__)
#include <sched.h>     // sched_getcpu
#endif

namespace chaos::il2cpp::runtime_core {

// NOTE: MSVC generates C-linkage (undecorated) references when names are
// used inside extern "C" blocks, even if brought in by using-declarations
// or nested namespace wrappers.  The calls to EnumerateThreads and access
// to tls_this_thread are therefore delegated to bridge functions declared
// extern "C" in thread_state.h and defined in thread_state.cpp:
//
//   chaos_enumerate_threads()  — wraps threading::EnumerateThreads
//   chaos_get_tls_this_thread() — wraps threading::tls_this_thread

extern "C" {

void chaos_thread_ctor(
    CHAOS_IL2CPP_INTPTR thread_obj,
    CHAOS_IL2CPP_INTPTR thread_start_delegate) noexcept
{
    fprintf(stderr, "[TRACE] chaos_thread_ctor: thread_obj=0x%llx delegate=0x%llx\n",
            (unsigned long long)thread_obj, (unsigned long long)thread_start_delegate);
    auto& entry = chaos::il2cpp::common::require_thread_runtime_entry(thread_obj);
    entry.thread_start_delegate = thread_start_delegate;
    entry.managed_thread_id = chaos::il2cpp::common::allocate_managed_thread_id();
}

void chaos_thread_start(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    fprintf(stderr, "[TRACE] chaos_thread_start: thread_obj=0x%llx\n",
            (unsigned long long)thread_obj);
    auto* entry = chaos::il2cpp::common::try_get_thread_runtime_entry(thread_obj);
    if (entry == nullptr) {
        fprintf(stderr, "[TRACE] chaos_thread_start: entry NOT FOUND!\n");
        return;
    }
    fprintf(stderr, "[TRACE] chaos_thread_start: entry FOUND, delegate=0x%llx\n",
            (unsigned long long)entry->thread_start_delegate);

    auto* runtime_state = GetCurrentRuntimeState();
    if (runtime_state == nullptr) {
        fprintf(stderr, "[TRACE] chaos_thread_start: GetCurrentRuntimeState() returned null!\n");
        return;
    }

    const CHAOS_IL2CPP_INTPTR delegate = entry->thread_start_delegate;

    // Capture the current ExecutionContext to flow to the new thread.
    auto* captured_ctx = chaos_execution_context_capture();

    entry->worker = std::make_unique<std::thread>([runtime_state, delegate, captured_ctx]() {
        fprintf(stderr, "[TRACE] worker thread started, delegate=0x%llx\n",
                (unsigned long long)delegate);
        // Attach this thread to the runtime.
        ThreadState* thread_state = nullptr;
        if (ThreadAttach(runtime_state, &thread_state) != CHAOS_RUNTIME_STATUS_OK) {
            fprintf(stderr, "[TRACE] worker thread: ThreadAttach FAILED\n");
            chaos_execution_context_free(captured_ctx);
            return;
        }
        fprintf(stderr, "[TRACE] worker thread: ThreadAttach OK\n");

        // Invoke the delegate under the captured ExecutionContext.
        chaos_execution_context_run(captured_ctx, [](void* d) {
            auto del = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(d);
            fprintf(stderr, "[TRACE] EC run callback: delegate=0x%llx\n",
                    (unsigned long long)del);
            if (del != 0) {
                fprintf(stderr, "[TRACE] about to call chaos_delegate_object_invoke\n");
                chaos_delegate_object_invoke(del, nullptr, nullptr, 0);
                fprintf(stderr, "[TRACE] chaos_delegate_object_invoke returned\n");
            }
        }, reinterpret_cast<void*>(delegate));
        chaos_execution_context_free(captured_ctx);

        // Detach from runtime.
        ThreadDetach(runtime_state, thread_state);
    });
}

void chaos_thread_join(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    auto* entry = chaos::il2cpp::common::try_get_thread_runtime_entry(thread_obj);
    if (entry == nullptr) return;
    if (entry->worker && entry->worker->joinable()) {
        entry->worker->join();
    }
}

void chaos_thread_interrupt(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using threading::ManagedThread;

    if (thread_obj == 0) return;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    s_target = thread_obj;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            mt->pending_interrupt.store(true, std::memory_order_release);
            s_target = 0;
            return false;  // Found, stop enumeration.
        }
        return true;
    });
}

void chaos_thread_abort(CHAOS_IL2CPP_INTPTR thread_obj, CHAOS_IL2CPP_INTPTR state_obj) noexcept
{
    (void)state_obj; // Accepted for ABI compatibility with Thread.Abort(object state);
                     // the abort signal itself is sufficient for testing purposes.
    using threading::ManagedThread;

    if (thread_obj == 0) return;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    s_target = thread_obj;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            mt->pending_abort.store(true, std::memory_order_release);
            s_target = 0;
            return false;
        }
        return true;
    });
}

CHAOS_IL2CPP_INT32 chaos_thread_reset_abort(void) noexcept
{
    auto* thread = chaos_get_tls_this_thread();
    if (thread == nullptr) return 0;

    bool was_pending = thread->pending_abort.load(std::memory_order_acquire);
    if (was_pending) {
        thread->pending_abort.store(false, std::memory_order_release);
        return 1;
    }
    return 0;
}

CHAOS_IL2CPP_INT32 chaos_thread_yield(void) noexcept
{
    std::this_thread::yield();
    return 1;
}

// ── Thread.GetDomainID ──────────────────────────────────────────────────────
//
// .NET Core has a single AppDomain, so this is 1 by construction — verified
// against .NET 8 (returns 1 on every run).  This is a real value, not a
// placeholder: an earlier state returned 0 through the external-runtime
// catch-all and made `Assert.AreEqual(1, Thread.GetDomainID())` fail.
//
// The .NET Framework semantics (per-domain increasing ids) do not apply here
// because this runtime does not implement multiple domains.
CHAOS_IL2CPP_INT32 chaos_thread_get_domain_id(void) noexcept
{
    return 1;
}

// ── Thread.GetCurrentProcessorId ────────────────────────────────────────────
//
// Returns the processor the CALLING THREAD is currently running on — a
// per-call host property, NOT the machine's processor count and NOT stable
// across calls (the OS may migrate the thread between samples).  .NET 8
// documents it as "for diagnostic use; the value is not guaranteed to be
// consistent between calls".
//
// This exists because without it the callee matched no shape and lowered to the
// zero-argument external-runtime catch-all, which returns 0.  The ATG probe had
// captured ITS OWN host's value as the expectation (`Assert.AreEqual(6, …)` on a
// 6-CPU probe box), so no fixed return value can satisfy the assertion.  The
// expectation is therefore classified envSensitive at the fact layer
// (_get_env_sensitive_subject_ids in stages/fact_chunk.py) — the same treatment
// Thread.Yield already gets.  Returning a real reading here keeps the method
// implemented and observable rather than silently 0.
CHAOS_IL2CPP_INT32 chaos_thread_get_current_processor_id(void) noexcept
{
#if defined(_WIN32)
    // Processor-group-local index of the calling thread.  windows.h is already
    // used by sibling stub translation units (interop_stubs.cpp, pal_stubs.cpp);
    // including it here keeps the platform call self-contained.
    return static_cast<CHAOS_IL2CPP_INT32>(::GetCurrentProcessorNumber());
#elif defined(__linux__)
    const int cpu = ::sched_getcpu();
    return cpu < 0 ? 0 : static_cast<CHAOS_IL2CPP_INT32>(cpu);
#else
    // No portable PAL accessor for "which CPU am I on".  Answer 0 (the valid
    // index of the first processor) rather than guessing; the fact layer treats
    // this subject as envSensitive regardless of the value.
    return 0;
#endif
}

void chaos_thread_sleep(CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
}

CHAOS_IL2CPP_INT32 chaos_thread_is_background(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using threading::ManagedThread;

    if (thread_obj == 0) return 0;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = 0;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            s_result = mt->is_background ? 1 : 0;
            s_target = 0;
            return false;
        }
        return true;
    });
    return s_result;
}

void chaos_thread_set_background(CHAOS_IL2CPP_INTPTR thread_obj, CHAOS_IL2CPP_INT32 value) noexcept
{
    using threading::ManagedThread;

    if (thread_obj == 0) return;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_new_bg = 0;
    s_target = thread_obj;
    s_new_bg = value;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            mt->is_background = (s_new_bg != 0);
            s_target = 0;
            return false;
        }
        return true;
    });
}

CHAOS_IL2CPP_INT32 chaos_thread_get_state(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using threading::ManagedThread;

    if (thread_obj == 0) return 0;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = 0;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            s_result = static_cast<CHAOS_IL2CPP_INT32>(mt->managed_state);
            s_target = 0;
            return false;
        }
        return true;
    });
    return s_result;
}

CHAOS_IL2CPP_INT32 chaos_thread_get_priority(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using threading::ManagedThread;
    using threading::ManagedThreadPriority;

    if (thread_obj == 0) return static_cast<CHAOS_IL2CPP_INT32>(ManagedThreadPriority::Normal);

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = static_cast<CHAOS_IL2CPP_INT32>(ManagedThreadPriority::Normal);

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            s_result = static_cast<CHAOS_IL2CPP_INT32>(mt->priority);
            s_target = 0;
            return false;
        }
        return true;
    });
    return s_result;
}

void chaos_thread_set_priority(CHAOS_IL2CPP_INTPTR thread_obj, CHAOS_IL2CPP_INT32 value) noexcept
{
    using threading::ManagedThread;
    using threading::ManagedThreadPriority;

    if (thread_obj == 0) return;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_new_pri = 0;
    s_target = thread_obj;
    s_new_pri = value;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            mt->priority = static_cast<ManagedThreadPriority>(s_new_pri);
            s_target = 0;
            return false;
        }
        return true;
    });
}

CHAOS_IL2CPP_INT32 chaos_thread_is_threadpool(CHAOS_IL2CPP_INTPTR thread_obj) noexcept
{
    using threading::ManagedThread;

    if (thread_obj == 0) return 0;

    static CHAOS_IL2CPP_INTPTR s_target = 0;
    static CHAOS_IL2CPP_INT32 s_result = 0;
    s_target = thread_obj;
    s_result = 0;

    chaos_enumerate_threads([](ManagedThread* mt) -> bool {
        if (mt != nullptr && mt->managed_object == reinterpret_cast<void*>(s_target)) {
            s_result = mt->is_threadpool ? 1 : 0;
            s_target = 0;
            return false;
        }
        return true;
    });
    return s_result;
}

// ── LazyInitializer.EnsureInitialized<T> ────────────────────────────────────
//
// Shared body for the 3-arg and 4-arg (factory) overloads.  `value_factory` is
// 0 for the non-factory form.
//
// Semantics verified against .NET 8:
//   *initialized == false → store the value, set *initialized = true, assign a
//                           non-null sentinel to *syncLock, return the value.
//   *initialized == true  → touch nothing, return what *target already holds.
//
// The value is written and read back through the CALLER'S storage so the
// by-ref parameter observes it, which is why `carrier_width` is needed rather
// than a plain return: a 1/2/4/8-byte T must not be written as a full
// pointer-sized slot (it would clobber the adjacent bool and reference slots
// in the caller's frame).
namespace {

// The syncLock sentinel.  The real BCL allocates a fresh object; the generated
// assertions only require it to become non-null, and one process-wide instance
// keeps repeated EnsureInitialized calls identity-stable (which .NET also
// guarantees — the same object is observed across calls).
CHAOS_IL2CPP_INTPTR LazyInitSyncSentinel() noexcept
{
    // 16 bytes is enough for an object header on this runtime; the allocation
    // is only ever used as a non-null identity, never dereferenced.
    static CHAOS_IL2CPP_INTPTR s_sentinel = []() -> CHAOS_IL2CPP_INTPTR {
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(GcAllocateFast(16));
    }();
    return s_sentinel;
}

// Read/write a T of the given width through an untyped by-ref slot.
CHAOS_IL2CPP_INTPTR LazyInitLoad(CHAOS_IL2CPP_INTPTR slot, CHAOS_IL2CPP_INT32 width) noexcept
{
    switch (width) {
        case 1: return static_cast<CHAOS_IL2CPP_INTPTR>(*reinterpret_cast<CHAOS_IL2CPP_UINT8*>(slot));
        case 2: return static_cast<CHAOS_IL2CPP_INTPTR>(*reinterpret_cast<CHAOS_IL2CPP_UINT16*>(slot));
        case 4: return static_cast<CHAOS_IL2CPP_INTPTR>(*reinterpret_cast<CHAOS_IL2CPP_UINT32*>(slot));
        default: return *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(slot);
    }
}

void LazyInitStore(CHAOS_IL2CPP_INTPTR slot, CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INT32 width) noexcept
{
    switch (width) {
        case 1: *reinterpret_cast<CHAOS_IL2CPP_UINT8*>(slot) = static_cast<CHAOS_IL2CPP_UINT8>(value); break;
        case 2: *reinterpret_cast<CHAOS_IL2CPP_UINT16*>(slot) = static_cast<CHAOS_IL2CPP_UINT16>(value); break;
        case 4: *reinterpret_cast<CHAOS_IL2CPP_UINT32*>(slot) = static_cast<CHAOS_IL2CPP_UINT32>(value); break;
        default: *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(slot) = value; break;
    }
}

CHAOS_IL2CPP_INTPTR LazyInitEnsure(
    CHAOS_IL2CPP_INTPTR target_ref,
    CHAOS_IL2CPP_INTPTR initialized_ref,
    CHAOS_IL2CPP_INTPTR sync_lock_ref,
    CHAOS_IL2CPP_INTPTR value_factory,
    CHAOS_IL2CPP_INT32 carrier_width) noexcept
{
    // A null target cannot be written or read — answer with a zero rather than
    // dereferencing it.  The caller supplied no storage to initialise.
    if (target_ref == 0) return 0;

    // `initialized` is a managed bool (1 byte), not a pointer-sized slot.
    auto* initialized = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(initialized_ref);
    const bool already = (initialized != nullptr) && (*initialized != 0);

    if (already) {
        // Contract: do not re-run the factory, do not touch the lock.
        return LazyInitLoad(target_ref, carrier_width);
    }

    CHAOS_IL2CPP_INTPTR value;
    if (value_factory != 0) {
        // `Func<T>` with no parameters; its return lands in ret.
        CHAOS_IL2CPP_INTPTR ret = 0;
        chaos::il2cpp::runtime_core::chaos_delegate_object_invoke(
            value_factory, nullptr, &ret, 0);
        value = ret;
    } else {
        // No factory: the BCL stores default(T).  Zero, narrowed to the slot.
        value = 0;
    }

    LazyInitStore(target_ref, value, carrier_width);

    if (initialized != nullptr) *initialized = 1;
    if (sync_lock_ref != 0) {
        *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(sync_lock_ref) = LazyInitSyncSentinel();
    }

    return value;
}

}  // namespace

CHAOS_IL2CPP_INTPTR chaos_lazy_initializer_ensure_initialized(
    CHAOS_IL2CPP_INTPTR target_ref,
    CHAOS_IL2CPP_INTPTR initialized_ref,
    CHAOS_IL2CPP_INTPTR sync_lock_ref,
    CHAOS_IL2CPP_INT32 carrier_width) noexcept
{
    return LazyInitEnsure(target_ref, initialized_ref, sync_lock_ref, 0, carrier_width);
}

CHAOS_IL2CPP_INTPTR chaos_lazy_initializer_ensure_initialized_factory(
    CHAOS_IL2CPP_INTPTR target_ref,
    CHAOS_IL2CPP_INTPTR initialized_ref,
    CHAOS_IL2CPP_INTPTR sync_lock_ref,
    CHAOS_IL2CPP_INTPTR value_factory,
    CHAOS_IL2CPP_INT32 carrier_width) noexcept
{
    return LazyInitEnsure(target_ref, initialized_ref, sync_lock_ref, value_factory, carrier_width);
}

// ── WaitHandle static overloads: argument validation ───────────────────────
//
// Contract measured against .NET 8 and net10 (identical): a null array raises
// ArgumentNullException, an empty array raises ArgumentException, and a null
// ELEMENT also raises ArgumentNullException.  The check is the same for every
// overload, so the timeout/exitContext operands are simply not read here.
//
// CHECK ORDER IS PART OF THE CONTRACT: null before empty.  `WaitAll(null, …)`
// must be an ArgumentNullException; reversing the two would report it as an
// ArgumentException and the subject's Assert.Throws would fail on the type.
//
// The array body is read through ManagedArrayAccessor rather than a raw offset
// so the layout stays tied to the one `static_assert`-guarded definition in
// stub_common.h.  Element storage is contiguous after the 32-byte header.
CHAOS_IL2CPP_INT32 chaos_wait_handle_validate(CHAOS_IL2CPP_INTPTR wait_handles) noexcept
{
    if (wait_handles == 0)
    {
        RaiseArgumentNullException("waitHandles");
    }

    const ManagedArrayAccessor* arr = get_managed_array(wait_handles);
    const CHAOS_IL2CPP_INTPTR length = arr->length;
    if (length == 0)
    {
        RaiseArgumentException(
            "Array lengths must be greater than zero.");
    }

    // A null element is not a usable handle, and .NET raises the same
    // ArgumentNullException it uses for a null array itself.
    const CHAOS_IL2CPP_INTPTR* elements = accessor_get_elements(arr);
    for (CHAOS_IL2CPP_INTPTR i = 0; i < length; ++i)
    {
        if (elements[i] == 0)
        {
            RaiseArgumentNullException("waitHandles");
        }
    }

    // Validation passed.  The blocking wait itself is NOT modelled here.
    //
    // ⚠️ The 0 returned on this path is "validated", not "the wait completed" —
    // and for WaitAny specifically, 0 is also the index of the FIRST ready
    // handle, so a subject that asserted on a real wait result could read this
    // as a pass it did not earn.  That cannot happen with the subjects this
    // currently serves: every probed call site passes null, an empty array, or
    // an array of nulls (measured across all 60 WaitHandle cases — see
    // waithandle-contracts.md §五), so this path is unreachable from them.
    //
    // If a future subject does call these with live handles, this function must
    // grow a real wait (or raise NotImplemented) rather than keep returning 0.
    return 0;
}

CHAOS_IL2CPP_INT32 chaos_wait_handle_validate_pair(
    CHAOS_IL2CPP_INTPTR to_signal,
    CHAOS_IL2CPP_INTPTR to_wait_on) noexcept
{
    if (to_signal == 0)
    {
        RaiseArgumentNullException("toSignal");
    }
    if (to_wait_on == 0)
    {
        RaiseArgumentNullException("toWaitOn");
    }
    return 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
