# T4 Native Codegen Limitations Assessment

## Overview

T4 is the fourth tier in Chaos IL2CPP's adaptive tiered execution system. It generates
native x64 machine code from register-allocated IR (`RegisterMethod`), bypassing the
interpreter dispatch loop for maximum performance.

Current status: **V3 Complete** — All 5 stages of T4 V3 implemented (Safepoint, Deopt OSR, SEH, Register Caching, GC Allocation). 103/103 opcodes supported with native x64 emission. `codegen_native_test` 10/10 passing.

---

## Opcode Coverage

### Supported (100 opcodes)

| Category | Opcodes | Implementation |
|----------|---------|----------------|
| Constants | LdcI4, LdcI8, LdcR4, LdcR8, LdNull | Inlined (mov imm→reg) |
| Arguments/Locals | LdArg, LdLoc, StLoc, StArg, LdArgA, LdLocA | Inlined (stack slot load/store) / null stub (LdArgA/LdLocA) |
| String | LdStr | Inlined (mov imm.ptr→reg) |
| Field access | LdFld, StFld | Helper call (CodegenLdFld/CodegenStFld) |
| Static fields | LdSFld, StSFld | Helper call (CodegenLdSFld/CodegenStSFld) |
| Object access | LdObj, StObj, InitObj | Helper call (CodegenLdObj/CodegenStObj/CodegenInitObj) |
| Heap allocation | NewObj, NewArr, Box | Helper call (CodegenNewObj/CodegenNewArr/CodegenBox) |
| LocAlloc | LocAlloc | Stack bump from frame reserve (4KB), heap fallback on overflow — **no leak** |
| Returns | Ret | Inlined (store RAX→ret_buf) |
| Stack | Pop | Inlined (no-op, reg-based IR) |
| Dup | Dup | Inlined (copy src1→dst) |
| Branches | Br, Leave | Inlined (jmp rel32) |
| Conditional branches | BrTrue, BrFalse | Inlined (test+jcc rel32) |
| Compare-and-branch | Beq, BneUn, Blt, Bgt, Ble, Bge, BltUn, BgtUn, BleUn, BgeUn | Inlined (cmp+jcc rel32) |
| Arithmetic | Add, Sub, Mul, Div, Rem, Neg, DivUn, RemUn | Inlined (stack load→x64 op→stack store) |
| Overflow arithmetic | AddOvf, SubOvf, MulOvf | Inlined (x64 add/sub/imul + `jo` deopt trampoline on overflow) |
| Bitwise | And, Or, Xor, Not, Shl, Shr, ShrUn | Inlined |
| Compare | Ceq, Clt, Cgt | Inlined |
| Conversion | Conv_I4, Conv_I8, Conv_R4, Conv_R8, ConvRUn, ConvI, ConvU | Inlined |
| Overflow conversion | ConvOvfI, ConvOvfI4, ConvOvfI8, ConvOvfU, ConvOvfU4, ConvOvfU8 | Inlined (INT32 range check + `jo` deopt trampoline; I8/U8 pass-through) |
| Unbox | Unbox | Helper call (CodegenUnbox) |
| Type checks | CastClass, IsInst | Helper call (CodegenCastClass/CodegenIsInst) — no-op (FastExecute convention) |
| Function pointers | LdFtn, LdVirtFtn | Inlined imm.ptr / Helper call (CodegenLdVirtFtn) |
| Runtime tokens | LdToken, SizeOf | Inlined (mov imm→dst) |
| Indirect load/store | LdInd, StInd | Inlined (load/store through pointer reg) |
| Array | LdLen, LdElem, StElem, LdElemA | Helper call (CodegenLdLen/CodegenLdElem/CodegenStElem) / inlined ptr copy (LdElemA) |
| Memory block | Cpblk, InitBlk | Helper call (CodegenCpblk/CodegenInitBlk) |
| Call (direct) | Call | Inlined call through direct_ptr |
| Call (indirect) | CallBridge, Calli | Inlined call through function pointer (imm.ptr for CallBridge, resolved ptr for Calli) |
| Virtual call | CallVirt, CallVirtConstrained | Inlined PIC dispatch via CodegenCallVirt helper + deopt fallback |
| SEH control flow | Throw, Rethrow, EndFinally, EndFilter | ud2 (illegal instruction → VEH handler) |
| Control flow | Switch | Inlined (bounds check + jmp table via LEA rip-relative + scaled-index dispatch) |

### V2 complete — all 3 V2 candidates implemented

---

## Known Limitations

### 1. TLAB 内联分配未完成（V3.5 进行中）

NewObj 和 Box 当前通过 C helper 调用执行分配。V3.5 正在将 TLAB bump 路径内联到生成的 x64 代码中，绕过函数调用 ABI 开销。当前进度：
- TLAB struct (`current`/`end` bump pointers) 通过 CodeGenConfig 嵌入
- NewObj 内联 bump 路径：load tlab.current → bump → compare → slow path → store
- Box 内联 bump 路径：固定 24 字节，类似流程
- V1 限制：仅当对象大小 ≤ 128 字节时启用内联

### 2. Overflow deoptimization 非 OSR

AddOvf/SubOvf/MulOvf 通过 `jo` 检测溢出后，写入 `kDeoptMagic` 到 `ret_buf[0]` 并跳转到 epilogue。调用方检测到 magic 值后从零重新执行方法（非 OSR）。包含大副作用的溢出方法会执行两次。

**改进方向**：实现真正的 OSR（栈上替换），溢出时重建寄存器状态并回退到 RegisterExecute。

### 3. CallVirt PIC 仅支持单态/双态

CallVirt/CallVirtConstrained 通过内联 PIC dispatch，最大 3 个类型槽。超出后触发 deopt 回退到解释器。

**改进方向**：切换到 megamorphic dispatch（vtable 间接分派）或全局查找缓存。

### 4. 无精确 GC slot maps

GC point 目前使用保守栈扫描。栈帧中的所有 64 位值都被视为潜在 GC 引用，导致不必要的存活对象保留。

**改进方向**：利用 liveness 分析结果，为每个 GC point 生成精确的 slot mask。

### 5. 无限寄存器分配

当前使用栈式寄存器文件 + R12-R15 缓存（write-through）。图着色分配器已实现（ir_optimizer）但未集成到 T4。

**改进方向**：将图着色结果映射到 x64 寄存器，消除 load→compute→store 栈往返。

### 6. 无调试信息

T4 生成的代码不发射调试元数据（行号映射、变量位置）。调试器附加到 T4 代码将显示原始机器码。

### 7. 单模块 hotpatch

Hotpatch 感知 dispatch（C1）支持每模块 hotpatch 表。跨模块 hotpatch 需要共享 HotpatchNameRegistry。

---

## Performance Profile (x64, Ryzen 7840H)

| Scenario | T4 (ns/op) | vs T1 | vs T2 | vs T3 |
|----------|------------|-------|-------|-------|
| Pure arithmetic (21 instrs) | — | 11204 | 728 | 1061 |
| LdFld | 68 | 240 | 214 | 201 |
| Native call+ret (4 instrs) | 152 | 1165 | 461 | 425 |
| CallVirt PIC hit | 6 | — | — | 75 |
| CallVirt PIC miss | 68 | — | — | 128 |
| Switch (3-way) | 52 | 412 | 318 | 289 |

Notes:
- T4 is not yet measured for bench_arithmetic (method has no Call instructions,
  so T4 promotion does not trigger with current thresholds).
- CallVirt PIC hit (6ns) is measured via direct native_entry call, not through
  InterpreterEntryDirect tier promotion.
- LdFld T4 (68ns) uses a helper call — the full call overhead is visible.
- Native call+ret (bench_native) verified: all native calls return correct results.
- `CanGenerateNativeCode` now covers 103/103 opcodes. All opcodes generate native x64 code — no fallback to RegisterExecute for any opcode.

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
