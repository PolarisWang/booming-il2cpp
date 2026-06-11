#ifndef CHAOS_IL2CPP_GC_SUSPEND_TRAMPOLINE_H_
#define CHAOS_IL2CPP_GC_SUSPEND_TRAMPOLINE_H_

/// Assembly trampoline for SIGUSR2 safepoint hijack.
/// When a cooperative-mode thread is interrupted by SIGUSR2, the signal
/// handler acknowledges the safepoint and redirects RIP to this trampoline
/// (set up via ucontext modification).  The trampoline runs on the thread's
/// normal stack, calls SafepointPoll(), then returns to the original
/// instruction.
extern "C" void gc_suspend_trampoline_x64() noexcept;

#endif  // CHAOS_IL2CPP_GC_SUSPEND_TRAMPOLINE_H_
