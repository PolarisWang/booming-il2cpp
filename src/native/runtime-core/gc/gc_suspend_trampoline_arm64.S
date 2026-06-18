// gc_suspend_trampoline_arm64.S — Safepoint trampoline for SIGUSR2 hijack (ARM64)
//
// When a cooperative-mode thread is interrupted by SIGUSR2 (preemptive
// suspend), the signal handler acknowledges the safepoint and redirects
// PC to this trampoline instead of spin-waiting on the limited signal
// stack.  The trampoline runs on the thread's NORMAL stack, calls
// SafepointPoll() which handles the actual safepoint wait (via event
// or spin), then returns to the original interrupted instruction.
//
// Stack layout at entry (set up by signal handler):
//   [original_sp - 8]  = original PC   ← SP points here
//
// The trampoline pops the return address into X30 (LR), saves 18
// caller-saved GPRs, calls SafepointPoll, restores GPRs, then RETs
// via X30 to the original PC.
//
// Must be in .text section and exported with C-linkage name.

.text
.globl gc_suspend_trampoline_arm64
.type   gc_suspend_trampoline_arm64, @function
.align 2

gc_suspend_trampoline_arm64:
    // Load original PC into X30 from stack, then advance SP past it.
    ldr     x30, [sp], #8

    // Save caller-saved GPRs (X0-X17 per AAPCS64).
    // Store original PC (X30) and X0 as a pair first.
    stp     x30, x0, [sp, #-16]!
    stp     x1, x2, [sp, #-16]!
    stp     x3, x4, [sp, #-16]!
    stp     x5, x6, [sp, #-16]!
    stp     x7, x8, [sp, #-16]!
    stp     x9, x10, [sp, #-16]!
    stp     x11, x12, [sp, #-16]!
    stp     x13, x14, [sp, #-16]!
    stp     x15, x16, [sp, #-16]!
    str     x17, [sp, #-16]!

    // Call SafepointPoll (no arguments, void return).
    bl      chaos_safepoint_poll

    // Restore caller-saved GPRs (reverse order).
    ldr     x17, [sp], #16
    ldp     x15, x16, [sp], #16
    ldp     x13, x14, [sp], #16
    ldp     x11, x12, [sp], #16
    ldp     x9, x10, [sp], #16
    ldp     x7, x8, [sp], #16
    ldp     x5, x6, [sp], #16
    ldp     x3, x4, [sp], #16
    ldp     x1, x2, [sp], #16
    ldp     x30, x0, [sp], #16    // restore original PC into X30

    ret                             // return to original PC via X30

.size   gc_suspend_trampoline_arm64, . - gc_suspend_trampoline_arm64
