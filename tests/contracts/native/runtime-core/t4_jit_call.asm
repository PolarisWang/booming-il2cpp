; t4_jit_call.asm -- invokes a T4-compiled native entry using the JIT register
; convention (RBX = args_buf, RSI = ret_buf), which differs from the Win64 C
; ABI (RCX/RDX/R8/R9) that a bare function-pointer call would use.
;
; Root cause it fixes: the T4 prologue (jit_engine.cpp) sets
;     RBX = args_buf, RSI = ret_buf   (ArchTraits: kArgsBuf=RBX, kRetBuf=RSI)
; and the Ret handler writes the return value through the caller-saved RSI
; (saved by the prologue's `push RSI`, reloaded at `[RBP-16]`).  A Win64 C-ABI
; caller places args/ret in RCX/RDX, so RBX/RSI carry garbage and the return
; value is written to a bogus address.  This thunk re-binds RBX/RSI before
; calling the compiled entry so the test can read the return value correctly.
;
; C signature:  void T4CallNative(const void* entry, void* args, void* ret);
;   entry = RCX, args = RDX, ret = R8   (Win64)

.code

PUBLIC T4CallNative

T4CallNative PROC
    push    rbp
    mov     rbp, rsp
    push    rbx                 ; preserve caller RBX (callee-saved)
    push    rsi                 ; preserve caller RSI (callee-saved)
    sub     rsp, 48             ; shadow space (32) + alignment (16)

    mov     rbx, rdx            ; RBX = args_buf
    mov     rsi, r8             ; RSI = ret_buf
    call    rcx                 ; call compiled entry

    add     rsp, 48
    pop     rsi
    pop     rbx
    pop     rbp
    ret
T4CallNative ENDP

END
