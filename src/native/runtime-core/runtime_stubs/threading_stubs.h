// ── Threading stub declarations ────────────────────────────────
#pragma once

CHAOS_IL2CPP_INTPTR chaos_thread_get_current(void) noexcept;
CHAOS_IL2CPP_INT32 ChaosMonitorTryEnter(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 timeout) noexcept;

// Thread lifecycle: .ctor stores the delegate, Start spawns a native thread,
// Join waits for completion.  All take the managed Thread object as first arg.
void chaos_thread_ctor(CHAOS_IL2CPP_INTPTR thread_obj, CHAOS_IL2CPP_INTPTR thread_start_delegate) noexcept;
void chaos_thread_start(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;
void chaos_thread_join(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;

// Thread.Interrupt: signal a thread to throw ThreadInterruptedException
// at the next safepoint poll.
void chaos_thread_interrupt(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;

// Thread.Abort: signal a thread to throw ThreadAbortException
// at the next safepoint poll.
void chaos_thread_abort(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;

// Thread.Yield: yield the current thread's time slice.
void chaos_thread_yield(void) noexcept;

// Thread.IsBackground: get/set background status.
// Background threads do not prevent process exit.
CHAOS_IL2CPP_INT32 chaos_thread_is_background(CHAOS_IL2CPP_INTPTR thread_obj) noexcept;
void chaos_thread_set_background(CHAOS_IL2CPP_INTPTR thread_obj, CHAOS_IL2CPP_INT32 value) noexcept;