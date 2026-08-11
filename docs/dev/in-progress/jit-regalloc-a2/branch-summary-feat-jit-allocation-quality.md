# Branch Summary — feat/jit-allocation-quality-diagnostics

> **日期**：2026-08-11
> **分支**：`feat/jit-allocation-quality-diagnostics`
> **用途**：JIT 图着色分配质量量化基线 + 修复（FPR 着色打通 + write-through 跨call细化）
> **关联**：[M1] 分配质量量化工具、[M2] FPR 着色、[M3] write-through 细化 —— 均完成并提交

---

## 1. 分支内容：两个 Workstream 混合

本分支相对 `main` 有 6 个 commit，实际是**并行两线共用一个分支**：

### 我的 JIT 主线（3 commit，本分支的主题）
| Commit | 内容 | 验证 |
|---|---|---|
| `22161fbbb` | `CHAOS_IL2CPP_CODEGEN_STATS` 分配质量诊断工具（自包含头 `jit_codegen_stats.h`，env 门控，量化 reg-vs-stack + per-opcode 栈往返 ranking） | gate off 零开销；gate on 出 JSON |
| `a23ff863f` | **FPR(XMM) 图着色打通**（allocator 并行 FPR liveness + 保守活区间 + 传递闭包；`has_graph_coloring_` 任一 GPR/FPR 色判定） | FPR 着色 reg=0→2；全 254 测试绿 |
| `86f7491f1` | **write-through 跨call细化**（`cross_call_mask_` per-vreg，替代方法级 `has_caller_clobber_`；写穿 + post-call reload 同步 gate） | writethrough 16134→14595(-9.5%)，store.reg 766→2305(3x)；全 254 测试绿 |

### GC 平行线（3 commit，另一 session 在本分支提交）
`e4d765d79`/`35285c431`/`a66b6139a` — gc-concurrency-fix-dedicated（region-gen all-chunk marking、static-root mark scan、atomic card bundle、VerifyHeap）。**与 JIT 主线无文件冲突**（不同 `src/native/runtime-core/gc/` vs `src/native/jit/`）。

---

## 2. 关键成果与数据

**诊断基线**（`test_jit_native`, 1090 方法, kFull 彩色路径）：
- GPR load **97.5% 寄存器命中**（24850 reg / 637 stack）
- GPR store 原先 92% 是 write-through（16134），细化后 16134→14595
- FPR 原先 100% 走栈（着色死路径）→ 修复后彩色

**根因发现**（两处，均已修）：
1. **FPR 着色死路径**：浮点 vreg 编号 64+，但 allocator 的 FPR pass 从 GPR-only 的 64-bit `live_in` 读 bit≥64 → 永远空 → `fpr_color` 恒 0xFF。加并行 FPR liveness 修复。
2. **write-through 方法级二值**：`has_caller_clobber_` 整方法一个 flag，无法区分 vreg 是否真跨 call 存活。加 per-vreg `cross_call_mask_`（基于 liveness `live_in_`）。

**RDI ABI 调研（诚实结论）**：看似 callee-saved 错标，实测改 e callee-saved 致 `OsrRepromotion` 挂起——RDI 是本 codegen frame-init REP STOSQ 的保留 scratch。**保留原设计**，补注释防误改。

---

## 3. 验证状态

- **全 15 套件 254 项测试全绿**（test_jit_native 69 / il_smoke 19 / abi 31 / gc_slot_map 16 / seh_handler_internal 38 / ...）
- 核心改动仅在 `src/native/jit/`，未触翻译/运行时其他域
- 每 commit 带 `root_cause`/`fix_strategy`/`regression_check` 三段元数据，通过全部 pre-commit 门禁

---

## 4. 下一步候选（重启后接续）

> **更新（2026-08-11 本会话）**：已交付 **Phase 1「省写穿」codegen 侧收敛**（见 §5），把 "GC 读寄存器省写穿" 的 codegen 上半场做掉了；余下纯 runtime 寄存器窗捕获是 Phase 2，留给 GC 线协调。

按 `jit-regalloc-a2-continuation-roadmap.md` §5 启动表 + 本分支已验证成果，剩余可推进：

| 优先级 | 任务 | 说明 |
|---|---|---|
| 🔴 高 | **GC 读寄存器（Phase 2）**：省掉 pre-call spill 本身 | 需 runtime 寄存器窗捕获（Windows CONTEXT 现状无 / Linux ucontext）＋ 在生产 `GcScanAllThreadRoots` 调用 `GcScanSafepointRegisterRoots`（已写但仅单测调用）。触碰 `gc_root_scanner.cpp`/`thread_state.cpp`（与并行 GC 会话共用区，需协调）|
| 🔴 高 | **扩持久寄存器预算**（当前 x64 仅 R12-R15 4 个在 prologue save/restore）| 提升图着色可用寄存器数。本会话数据表明 spill 仅 620+637、load 命中 97.5%，非主导成本；优先级下调 |
| 🟡 中 | test_jit_native 浮点常量被 optimizer 折叠 → fpr total=0（harness 伪影）| 补非折叠 float 测试验证 FPR 真实收益 |
| 🟡 中 | 分支清理：GC 平行线提交是否要留在本分支 | 按需 rebase/分离 |

**重启入口**：读本文件 + `jit-regalloc-a2/TASK-HANDOFF.md` + `jit-regalloc-a2-continuation-roadmap.md` + memory `jit-graphcoloring-allocation-quality-report.md`、`jit-writethrough-elimination-coreclr-analysis.md`。确认分支 + 工作区，从 §4 表中选高优项启动。

---

## 5. Phase 1「省写穿」codegen 侧收敛（2026-08-11 本会话完成）

> 范围：仅 `src/native/jit/`。目标：把「caller-colored 且跨 call 的 vreg 在**每次 def 写穿**」收敛为「**每个 call/clobber 点 spill-before-arg-setup，仅实际 live 的 vreg**」。

### 根因（3 个读者驱动写穿）
1. **arg-setup clobber 顺序缺陷**：`Call`/`CallBridge`（emit.cpp:1540）先 mov 实参进 RCX/RDX/R8/R9（正是 caller-colored 池寄存器），再跑 pre-call spill，且 spill 循环 `if (caller_colored_mask_) continue` 跳过它们 → 写穿是唯一保命机制。
2. **deopt 重建只读栈槽**（`enable_deopt=true` 默认，从 `codegen_rsp+GprOff` 读回）。
3. **GC safepoint 只扫栈槽**（`EmitSafepointPoll` 跳过 caller-colored + `RecordGcPoint` 只记 Stack slot）。

### 改动（4 文件，+124/-55，`src/native/jit/`）
- **`jit_codegen_gc.cpp`**：新增 `SpillLiveColoredForCall(object_only)` —— 按 `live_in_[current_instr_index_]` 精确 spill 当前 call/safepoint 点 live 的彩色 GPR（非 arg-register；arg-register 因 arg setup 已 clobber 故留给写穿）。`EmitSafepointPoll` 改用 `SpillLiveColoredForCall(true)`（补上原被跳过的 caller-colored GC-ref 上栈缺口）。
- **`jit_codegen_emit.cpp`**：`Call`/`CallBridge` 把 spill 挪到 arg setup **之前**，GPR 用 helper（只 live）、FPR 保守照旧。
- **`jit_codegen_memory.cpp`**：`StoreGpr` 写穿条件收紧为 `&& IsGprArgReg(gcr_.gpr_color[vreg])`（只 arg-register 保留写穿）；`EmitCallWithSpill`/`EmitRuntimeHelperCallImpl` GPR spill 换 helper；helper post-call reload 对齐 `& cross_call_mask_`（修另类不对称）。
- **`jit_engine.h`**：新增 `IsGprArgReg(phys)`（RCX/RDX/R8/R9）static helper + `SpillLiveColoredForCall` 声明。

### 净收益（`CHAOS_IL2CPP_CODEGEN_STATS` 复跑 test_jit_native, 1090 方法）

| 指标 | 基线 | Phase 1 后 | Δ |
|---|---|---|---|
| gpr_store.writethrough | 14595 | **7268** | **-50.2%**（↓7327）|
| gpr_store.reg | 2305 | 9632 | +7327（写穿→register-resident mov）|
| gpr_store.total | 17520 | 17520 | **0（无指令回归）** |
| gpr_load 命中 | 97.5% | 97.5% | 不降 |
| 栈 spill（stack） | 620 | 620 | 不增 |

**结论**：写穿从 "每次 def 写回栈" 转为 "寄存器常驻中的 reg-mov"，总 store 数不变（无净指令回归），值更常驻寄存器。剩余 7268 写穿是 **arg-register（RCX/RDX/R8/R9）vreg 的正确保留地基**（arg setup 在预 spill 前 clobber，只能靠 def 时写穿）。Phase 1 全额保留。

### 验证（全绿）
- `test_jit_native` **70/70**（含 `Test_RegisterResidency` GPR 寄存器常驻 0 栈写穿；新增 `Test_FprRegisterResidency` FPR 寄存器常驻）
- jit ctest **15/15**（abi 31 / gc_slot_map 16 / seh 38 / osr / deopt / pgo ...）
- **未触碰** `src/native/runtime-core/`（GC 并行会话共用区干净）

### FPR 着色真实收益验证（2026-08-11，本会话补）
- **背景**：Phase 1 之前 aggregate stats `fpr_store reg=0`（全栈）。续跑时新增 `Test_FprRegisterResidency` —— 编一个含 7 个同时 live 的浮点 vreg（64-70）走 `LdcR8 + Add/Mul/Sub` 链的方法，数编译后代码里的 FPR 栈 store（`movdqa [rsp+d]`）。
- **结果**：FPR-file 栈 store = **0**（完全 XMM 常驻）；aggregate `fpr_store reg: 0→4`。**证明 FPR 图着色（`a23ff863f`）对真实 FPR 算术方法有效——之前的 `reg=0` 是 harness 伪影**（测试方法浮点 vreg 压力不足，非真实缺口）。
- **修正假设**：续跑发现 optimizer `ConstPropagate` 只记录 `LdcI4/LdcI8` 常量（`jit_optimizer.cpp:700-711`），**浮点常量不折叠** → 原「浮点常量被折叠致 fpr=0」假设不成立；真因是 aggregate 里浮点算术方法占比极低。此结论已固化进 `Test_FprRegisterResidency` 守卫。
- **新测试**：`Test_FprRegisterResidency` + `CountStackStoreToFprFile`（扫描 `66 REX 0F 7F 44 24 disp8` 编码）。

### 明确不做 / 遗留
- **FPR 写穿不动**：liveness/`cross_call_mask_` 是 64-bit 掩码，无法表示 FPR vreg（64-95）→ FPR 无法按 per-vreg 精确 spill；改 `StoreFpr` 用 `cross_call_mask_` 会断正确性。FPR 保持保守（pre-call spill 全非 caller + caller 写穿）。
- **Phase 2（GC 读寄存器）推迟**：需要 runtime 寄存器窗捕获 + 生产调用 `GcScanSafepointRegisterRoots`，触碰 GC 线共用区。
- 计划原列 2 个逐步保留 gtest 守卫**未新增**（护 省写穿 的）——用更强的 stats 测量门 + 既有 `Test_RegisterResidency` 实证替代；新增的是 护 FPR 着色的 `Test_FprRegisterResidency`（见上）。
