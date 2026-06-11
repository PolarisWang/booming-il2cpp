// gc_suspend_trampoline_x64.S — Safepoint trampoline for SIGUSR2 hijack
//
// When a cooperative-mode thread is interrupted by SIGUSR2 (preemptive
// suspend), the signal handler acknowledges the safepoint and redirects
// RIP to this trampoline instead of spin-waiting on the limited signal
// stack.  The trampoline runs on the thread's NORMAL stack, calls
// SafepointPoll() which handles the actual safepoint wait (via event
// or spin), then returns to the original interrupted instruction.
//
// Stack layout at entry (set up by signal handler):
//   [original_rsp - 8]  = original RIP   ← RSP points here
//
// The trampoline pushes 9 scratch registers below that, calls
// SafepointPoll, pops them, then RETs to the original RIP.
//
// Must be in .text section and exported with C-linkage name.

.text
.globl gc_suspend_trampoline_x64
.type   gc_suspend_trampoline_x64, @function

gc_suspend_trampoline_x64:
    // Save caller-saved (scratch) registers that SafepointPoll may clobber.
    // System V AMD64 ABI: RAX, RCX, RDX, RSI, RDI, R8-R11 are scratch.
    push    %rax
    push    %rcx
    push    %rdx
    push    %rsi
    push    %rdi
    push    %r8
    push    %r9
    push    %r10
    push    %r11

    // Call SafepointPoll via the C-linkage wrapper (no arguments, void return).
    call    chaos_safepoint_poll

    // Restore scratch registers (reverse order).
    pop     %r11
    pop     %r10
    pop     %r9
    pop     %r8
    pop     %rdi
    pop     %rsi
    pop     %rdx
    pop     %rcx
    pop     %rax

    // Return to original RIP (pushed onto normal stack by signal handler).
    ret

.size   gc_suspend_trampoline_x64, . - gc_suspend_trampoline_x64
