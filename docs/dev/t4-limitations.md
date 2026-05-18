# T4 Native Codegen Limitations Assessment

## Overview

T4 is the fourth tier in Chaos IL2CPP's adaptive tiered execution system. It generates
native x64 machine code from register-allocated IR (`RegisterMethod`), bypassing the
interpreter dispatch loop for maximum performance.

Current status: **Phase E4** — 91/100 opcodes supported

---

## Opcode Coverage

### Supported (91 opcodes)

| Category | Opcodes | Implementation |
|----------|---------|----------------|
| Constants | LdcI4, LdcI8, LdcR4, LdcR8, LdNull | Inlined (mov imm→reg) |
| Arguments/Locals | LdArg, LdLoc, StLoc, StArg, LdArgA, LdLocA | Inlined (stack slot load/store) / no-op stub |
| String | LdStr | Helper call (LoadString) |
| Field access | LdFld, StFld | Helper call (CodegenLdFld/CodegenStFld) |
| Static fields | LdSFld, StSFld | Helper call (CodegenLdSFld/CodegenStSFld) |
| Object access | LdObj, StObj | Helper call (CodegenLdObj/CodegenStObj) |
| Heap allocation | LocAlloc | Helper call (CodegenLocAlloc) |
| Returns | Ret | Inlined (store RAX→ret_buf) |
| Stack | Pop | Inlined (no-op, reg-based IR) |
| Branches | Br | Inlined (jmp rel32) |
| Conditional branches | BrTrue, BrFalse | Inlined (test+jcc rel32) |
| Compare-and-branch | Beq, BneUn, Blt, Bgt, Ble, Bge, BltUn, BgtUn, BleUn, BgeUn | Inlined (cmp+jcc rel32) |
| Arithmetic | Add, Sub, Mul, Div, Rem, Neg, DivUn, RemUn | Inlined (stack load→x64 op→stack store) |
| Overflow arithmetic | AddOvf, SubOvf, MulOvf | Inlined (same as Add/Sub/Mul — V1 omits overflow check) |
| Bitwise | And, Or, Xor, Not, Shl, Shr, ShrUn | Inlined |
| Compare | Ceq, Clt, Cgt | Inlined |
| Conversion | Conv_I4, Conv_I8, Conv_R4, Conv_R8, ConvRUn, ConvI, ConvU | Inlined |
| Overflow conversion | ConvOvfI, ConvOvfI4, ConvOvfI8, ConvOvfU, ConvOvfU4, ConvOvfU8 | Inlined (same as non-checked variants — V1 omits overflow check) |
| Box | Box | Helper call (CodegenBox) |
| Unbox | Unbox | Helper call (CodegenUnbox) |
| NewObj/InitObj | NewObj, InitObj | Helper call (CodegenNewObj/CodegenInitObj) |
| Array | NewArr, LdLen, LdElem, LdElemA, StElem | Helper call (CodegenNewArr/CodegenLdLen/CodegenLdElem/CodegenStElem) |
| Type checks | CastClass, IsInst | Helper call (CodegenCastClass/CodegenIsInst) — no-op (FastExecute convention) |
| Function pointers | LdFtn, LdVirtFtn | Inlined / Helper call (CodegenLdVirtFtn) |
| Runtime tokens | LdToken, SizeOf | Inlined (mov imm.i4→dst) |
| Indirect load/store | LdInd, StInd | Inlined (sized load/store with type discriminator) |
| Memory block | Cpblk, InitBlk | Helper call (CodegenCpblk/CodegenInitBlk) |
| Call (direct) | Call, CallBridge, CallVirtConstrained | Inlined call through direct_ptr; hotpatch-aware dispatch if configured |
| Virtual call | CallVirt | Helper call (CodegenCallVirt) with PIC fast path |

### Not supported (9 opcodes)

| Category | Opcodes | Impact | Blockers |
|----------|---------|--------|----------|
| SEH control flow | Throw, Rethrow, Leave, EndFinally, EndFilter | Exception handling not supported in T4 | Need T4 SEH dispatch for explicit throw |
| Indirect call | Calli | Function-pointer calls not supported | Not wired in register allocator |
| Address-of | LdLocA* | By-ref address operations no-op stub | Real implementation needs stack frame address computation |
| Stack allocation | LocAlloc* | Dynamic stack allocation handled via heap fallback | Use CodegenLocAlloc helper |
| Control flow | Switch | Table-based switch not supported | Register allocator doesn't populate switch_targets count |
| Debugger | Break | Debugger break not supported | Intentional — T4 doesn't support debugger attachment |

*LdLocA, LocAlloc: registered in CanGenerateNativeCode (method enters T4), but semantics are stubs (LdLocA returns null ManagedPtr; LocAlloc allocates on heap not stack).

---

## Known Limitations

### 1. SEH methods — structural support added

`CanGenerateNativeCode()` no longer rejects SEH methods. T4 now emits the SEH clause
table as a data section appended after the generated code (accessible via
`NativeMethod::seh_table_offset`). A VEH handler is registered at startup in
`runtime_init.cpp` that catches hardware exceptions in T4 code and redirects execution
to the matching catch handler via the embedded SEH clause table. FastExecute (T1) has
full SEH handler dispatch.

**V1 VEH handler**: redirects RIP to the handler code on exception. Catch variables
(exception object) are NOT placed in the expected register slot — handlers that
reference the caught exception will see stale data. Filter clauses are treated as
catch-all.

**Todo**:
- Place exception object in the correct register slot for catch handlers that
  reference the exception variable
- Support filter clauses (SEHFlags::Filter) properly

### 2. Allocation-heavy methods use helper calls

Box, NewObj, LdStr are implemented as C helper calls, not inlined allocation
sequences. Each call goes through the full x64 call sequence (shadow space, call,
stack cleanup). This adds ~30-40ns overhead per allocation.

**Improvement path**: Inline GC allocation fast path (bump-pointer) for NewObj and
Box when the type is known at JIT time.

### 3. ConvOvf opcodes omit overflow checking (V1)

AddOvf, SubOvf, MulOvf and all 6 ConvOvf variants are handled by the same x64
instructions as their non-checked counterparts. x64 arithmetic naturally wraps
on overflow; no `System.OverflowException` is thrown. This is consistent with
the CastClass/IsInst no-op convention — T4 V1 prioritizes execution speed over
exception fidelity for edge cases.

**Improvement path**: Use thread-local "T4 exception pending" state or
VEH-based overflow detection (via `RaiseException(STATUS_INTEGER_OVERFLOW)`)
to redirect to the InterpreterVM for proper `System.OverflowException` dispatch.

### 4. CallVirt PIC has 3-slot limit

The PIC chain has a maximum of 3 type slots. Megamorphic call sites (>3 receiver
types) always fall through to `InterpreterDispatchRaw` (the slow interpreter path).
The current PIC generator only fills 1-3 slots based on profile data (90% threshold
for single slot, 95% for 2-3).

**Improvement path**: Switch to megamorphic dispatch (virtual table indirection) for
CallVirt, or tier up to a global lookup cache.

### 5. No deoptimization support

When `enable_deopt` is true in `CodeGenConfig`, deopt metadata is recorded, but there
is no mechanism to recover register state for safe on-stack replacement. T4-generated
code runs to completion once entered — it cannot be interrupted for deoptimization.

**Impact**: Methods with guards (range checks, type checks) that fail at runtime
cannot be re-directed to the interpreter. The current workaround is to not generate
T4 code for methods with runtime guards.

### 6. GC safepoints are instruction-count-based

Safepoint polling uses an instruction-count interval (default: every 64 instructions
if no loop back edge is detected). This is a simple heuristic — tight loops without
back edges (e.g., `LdLoc+Add+StLoc+Br` with forward-only branches) may not poll
frequently enough.

**Improvement path**: Use loop back-edge detection for safepoint placement instead of
instruction counting.

### 7. Register caching is minimal

`enable_register_caching` in `CodeGenConfig` is declared but not implemented as a
full register allocator. The current implementation uses a stack-slot register file
(load→compute→store). Hot registers are not promoted to x64 registers across
instructions.

**Impact**: Each arithmetic instruction does at least 2 stack loads + 1 stack store.
A proper register allocator would reduce this to register-to-register operations.

**Improvement path**: Linear-scan register allocation for T4, or reuse the graph
coloring results from the register allocator phase.

### 8. No debug info emission

T4-generated native code does not emit debug information (line number mappings,
variable locations). Debugger attachment to T4 code will show raw machine code.

### 9. Single-module hotpatch

The hotpatch-aware dispatch (C1) supports per-module hotpatch tables. Cross-module
hotpatch (patching a method in module A from module B) requires both modules to
share the same `HotpatchNameRegistry`.

---

## Performance Profile (x64, Ryzen 7840H)

| Scenario | T4 (ns/op) | vs T1 | vs T2 | vs T3 |
|----------|------------|-------|-------|-------|
| Pure arithmetic (21 instrs) | — | 11204 | 728 | 1061 |
| LdFld | 68 | 240 | 214 | 201 |
| Native call+ret (4 instrs) | 112 | 326 | 255 | 231 |
| CallVirt PIC hit | 6 | — | — | 75 |
| CallVirt PIC miss | fallback | — | — | 128 |

Notes:
- T4 is not yet measured for bench_arithmetic (method has no Call instructions,
  so T4 promotion does not trigger with current thresholds).
- CallVirt PIC hit (6ns) is measured via direct native_entry call, not through
  InterpreterEntryDirect tier promotion.
- LdFld T4 (68ns) uses a helper call — the full call overhead is visible.

---

## Files

| File | Purpose |
|------|---------|
| `src/native/codegen/code_generator.h` | CodeGenConfig struct, GenerateNativeCode/NativeMethod declarations |
| `src/native/codegen/code_generator.cpp` | x64 code emission for each supported opcode |
| `src/native/codegen/x64_encoder.h` | x64 instruction encoding primitives |
| `src/native/codegen/codegen_helpers.h/cpp` | C helper functions (LdFld, StFld, Box, NewObj, CallVirt, LdObj, LocAlloc, LdVirtFtn, InitObj) |
| `src/native/codegen/native_method.h` | NativeMethod struct (code, size, metadata) |
| `src/native/codegen/deopt_runtime.cpp` | Deoptimization runtime (stub — not yet functional) |
| `src/native/codegen/t4_seh_handler.h/cpp` | T4 code registry + VEH handler for SEH dispatch |
| `src/native/codegen/native_method.h` | NativeMethod struct (code, size, metadata) |
| `src/native/codegen/deopt_runtime.cpp` | Deoptimization runtime (stub — not yet functional) |
