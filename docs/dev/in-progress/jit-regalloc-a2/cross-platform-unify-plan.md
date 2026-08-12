# JIT 分配质量 — 跨平台方案统一：病灶分析 + 方案落点

> **日期**：2026-08-12
> **分支**：`main`（= `feat/jit-allocation-quality-diagnostics` 已快进）
> **用途**：重新梳理交接 §3 候选方向（A/B/其它），回应「跨平台方案统一、整体架构更一致」诉求。
> **关联**：`session-handoff-2026-08-11-jit-allocation-quality.md`（交接）、`jit-regalloc-a2-continuation-roadmap.md`（全局 roadmap）、memory `jit-phase2-register-capture`、`jit-writethrough-phase1-converge`

---

## 0. 结论速览（TL;DR）

**统一的关键不是让两平台写同构代码，而是把「省 pre-call spill」从依赖「捕获可靠性」中解耦出来。**
- 病灶唯一 = Windows 挂起架构（QueueUserAPC）拿不到 JIT safepoint 寄存器 → 捕获存根返回 false。
- 主骨架（推荐）= **方案 4**：复用 T2.3 deopt 契约，让 deopt 重建**先读寄存器文件 → 再读栈槽** → 去捕获可靠性硬依赖。
- 承载架构 = **方案 3**：捕获原语下沉为「一份存储契约 + 一个特性闸 `PalCaptureReliable()`」，平台差异封在 PAL 最底层。
- 低风险前置 = **方案 1**：仅 Linux 开豁免，借同一个特性闸。
- 远期项 = **方案 2**（Windows SuspendThread 补全捕获），独立 ownership，不塞普 JIT commit。

**代码现状核对**（2026-08-12 实测，未过期）：
- `kReliableRegisterCapture` **尚未存在**（方案 A 门控待建）。
- `PalCaptureThreadContext`/`PalGetCaptureSlot`/`PalSetPreemptContext` 已落地：POSIX 真实现 + Win32 存根（`getslot=-1`、capture=false）。
- 平台闸已就位：`thread_state.cpp:620` — PAL 返回 false → `gc_num_gprs=0` → 栈槽 floor，**永不欠保留**。

---

## 1. 三环依赖链与病灶定位

```
省 pre-call spill（写穿/预 spill 再降）
   │ 依赖
   ▼
可靠寄存器窗捕获（GC 读物理寄存器，避免 spill 全上栈）
   │ 依赖
   ▼
挂起时能取到「被中断的 JIT safepoint 寄存器」★ 病灶
```

| 平台 | 挂起机制 | 寄存器捕获 | 可靠性 |
|---|---|---|---|
| POSIX | SA_SIGINFO / ucontext | **真实现**（ucontext→TLS→slot）| ✅ 可靠 |
| Windows | QueueUserAPC + PalEventWait | **存根**（-1 / false）| ❌ 不可靠 |

现状 = **安全但性能不对称**（正确性崩坏无，能力未打通）。

现行 A/B 割裂的本质：同一功能被平台切成两半，各自半吊子。
- A（仅 Linux 豁免）：测试台（Windows）看不到收益 → 收益"不可见"。
- B（Windows SuspendThread）：补捕获，但风险高，且仍是一端凑一个实现。

**要真正"统一"**，不能靠"两端各写一半"，要靠**改变依赖结构**，让捕获可靠性不再是省写穿的硬前提。

---

## 2. 方案对比表

| 方案 | 统一度 | 风险 | 修改域 | 碰挂起深水区 | 测试台验证 |
|---|---|---|---|---|---|
| **1** Linux 豁免落地 | ❌ 低 | 低 | JIT | 否 | ❌ 不可见 |
| **2** Windows SuspendThread | ✅ 高 | 🔴 高 | PAL+thread_state | **是** | ✅ 可见 |
| **3** 捕获原语下沉（承载）| ✅✅ 最高 | 低 | JIT+Pal 接口 | 否 | ✅ |
| **4** deopt 重建读寄存器（**推荐**）| ✅ 高 | 🟡 中 | JIT+thread_state | **否** | ✅ 可见 |

---

## 3. 各方案详情

### 方案 1 — Linux 豁免落地（低风险增量）
交接方案 A 补编译期门控 `kReliableRegisterCapture`（Linux=on / Win=off），只在可信平台对寄存器根可达的非 arg GC-ref 免除 pre-call spill。平台闸向下兼容 Windows。
- 改动：`src/native/jit/` + 门控宏，零协调风险。
- 代价：**加深割裂**，Windows 收益不可见。

### 方案 2 — Windows SuspendThread（补齐另一端，真全平台豁免）
`pal_preempt_win32.cpp` 加 `SuspendThread`/`ResumeThread`（`os_handle` 补 `THREAD_GET_CONTEXT`），preemptive-suspend 改 SuspendThread 冻结（或 APC+SuspendThread 并存），`PalCaptureThreadContext` 用 `GetThreadContext` 实取。两端同时开豁免 → 一个方案实现全平台。
- 风险：SuspendThread 冻结 + 锁-free runtime 时序/共享 GC 区协调。寄存器根是加法、栈槽兜底，失真不致命。
- 同交接判定：**独立 ownership、独立里程碑**。

### 方案 3 — 捕获原语下沉（跨平台统一的承载架构）
当前 `PalCaptureThreadContext(slot, gpr[16], &n)` 让 GC 从 slot 拷寄存器。但**GC 读的永远是 `thread->gc_reg_file[16]` + `gc_num_gprs`（两端已统一）**；差异只在最底层从哪拷（ucontext vs CONTEXT）。
- 统一动作：引入平台特性断言 `PalCaptureReliable()`（POSIX=true / Windows=SuspendThread 后=true），豁免代码只认这一个布尔。
- 效果：方案 1 的 `kReliableRegisterCapture` 从"两套 #ifdef"收敛为**一个平台特性查询**，数据流共享同一套代码。方案 1/2 降格为同一方案的两种可靠性输入。

### 方案 4 — deopt 重建读寄存器（主推荐：绕开改挂起架构）
**重新审视交接 §3 的前提** —— "省 pre-call spill 依赖寄存器捕获"成立，仅因为**把 spill 当 GC 扫根的唯一安全来源**。但 T2.3（deopt-spill 契约）已揭示另一条路：
- 真正必须上栈的 = 会被 deopt 重建器读回的 vreg + safepoint 处 live 且被扫的 vreg。
- 若 **deopt 重建先读 `gc_reg_file` → 再读栈槽**（T2.3 契约扩展），则**无需把所有寄存器根 spill 上栈**——捕获从"spill 的前提"降级为"spill 的可选加速"。
- Windows 即便捕获不可靠，也能靠"栈槽兜底 + 少量精确 spill"拿到**与方案 1 相当收益**，且**完全不碰挂起架构**。

**为何这是"统一"的本质**：它把跨平台差异从依赖链移出，换成两平台**天然同构**的 deopt 契约；复杂度在 JIT 内部（不动 GC，SuspendThread 完全不碰），风险比方案 2 低一个量级。

---

## 4. 主推荐组合（P1: 性能 / P2: 架构一致）

**方案 4（主骨架）+ 方案 3（承载）+ 方案 1（Linux 前置）；方案 2（Windows SuspendThread）降级远期。**

理由：
1. **方案 4 是"统一"的本质** —— 把捕获可靠性移出依赖链，换两平台同构的 deopt 契约，Windows 不带风险拿收益。
2. **方案 2 高风险 ÷ 非主导回报不匹配** —— spill 总量 620+637、load 命中 97.5%（非主导），为它改锁-free 挂起不划算（类同交接"扩持久寄存器预算优先级已下调"）。
3. **方案 3+1 是低风险先行** —— 为方案 4 铺路，Linux 拿确定增量，Windows 拿基础收益。

### 落地顺序
```
Step 1: 方案 3 — 引入 PalCaptureReliable() 特性闸，两平台统一返回（存量 620 处读寄存器路径复用）
Step 2: 方案 1 — Linux 开 kReliableRegisterCapture（对齐特性闸），安全增量
Step 3: 方案 4 — T2.3 deopt 重建扩展为「先读寄存器文件→再读栈槽」，去捕获可靠性硬依赖（核心）
Step 4: 方案 2 — Windows SuspendThread 补全捕获（远期，独立 ownership）
```

---

## 5. 冲突线 / 依赖

- **方案 4 不改 `src/native/runtime-core/gc/` 的挂起路径**，仅 touch `thread_state.cpp` 的 JIT 帧扫描读侧（`GcScanAllThreadRoots` 消费闸附近）——与并行 GC 会话共用区冲突**低**，但仍需协调（交接 §2.1：勿动 GC 线未提交改动）。
- **方案 2** 若启动需独立 branch + ownership，不与普通 JIT commit 混合。
- 依赖：方案 4 依赖 **T2.3 deopt-spill 契约**先做扎实（重建读寄存器→读栈槽 fallback 顺序）。T2.3 文档：`T2.3-spill-deopt-contract-hotupdate-task.md`。

---

## 6. 遗留决策点（需用户拍板）

1. **主线取向**：方案 4（deopt 契约省写穿，不碰挂起）vs 方案 2（捕获能力完整）。选 4 则 Step 3 优先；选 2 则 Step 4 前移为主骨架。
2. **Windows 捕获能力的长期必要性**：若未来 OSR / stack-walk 深场景需要 Windows 捕获，则方案 2 不可完全搁置。

> 本 session 结论：**默认按方案 4 主线**（P1 性能在此不是瓶颈，P2 架构一致 + P3 稳定收益更优），待用户确认后启动 Step 1-3。

---

## 7. ⚠️ 2026-08-12 深度执行矫正：方案 1 原命题证伪（重要）

进入实现时，对 `RecordGcPoint` / `EmitSafepointPoll` / `SpillLiveColoredForCall` / `ReconstructRegisterFile` 逐行核对，**证伪了交接含方案 1 的"省 spill 依赖寄存器窗可靠"核心假设**。这是对 §0-§4 计划的关键修正，后续续跑必须先读本节。

### 7.1 硬结论：call 路径的 spill 是 GC 正确性载重，不可因"捕获可靠"跳过
`SpillLiveColoredForCall` 在 call 点（`jit_codegen_emit.cpp:1552`）spill 的是 `live_in_[current_instr_index_]` = **跨 call 存活的 vreg = `cross_call_mask_` 成员**。这些 vreg 必须上栈，原因：
- GC 的寄存器窗（`gc_reg_file`）抓的是**被挂线程"当前帧"的物理寄存器**（挂起发生在 `EmitSafepointPoll` 的 `EmitCallRipRel`/callee 内），此时 caller-colored 寄存器的值**已被 callee clobber**。
- 所以即便 Linux 捕获可靠，拿到的是 callee 帧寄存器，**不是 caller 跨 call 存活值**。
- 跳过 spill → 该 GC-ref 既不在栈、也不在可靠窗口 → **欠保留 → use-after-free**。

**结论**："GC 读寄存器省 spill"只对**非跨 call、当前正停在寄存器**的值成立——而这正是图着色已经在寄存器里的部分。**call 跨存活值的 spill 在结构上不可避免**（除非换成 CoreCLR 式 per-live-range spill map，那也仍需"精确 spill"而非"省略 spill"）。

### 7.2 deopt 重建已从寄存器读（非栈槽）
`ReconstructRegisterFile`（jit_deopt.cpp:50-79）无论 `is_spilled` 真假都读 `ctx.gpr[reg_index]`（`NativeContext` 在 trap 点捕获的物理寄存器）。栈槽批量路径（`DeoptTrap` 的 `ReadSpillSlot`，32+vr*8）是**并行保守填冲**，非 schema 驱动。故：**deopt 正确性不依赖 stack slot**（它从 trap 时寄存器重建）。

### 7.3 修正后的真实主线
- 方案 4 的**精确 spill 契约（T2.3）**才是真正安全、可省指令的方向：不是"省略跨 call 的栈槽"，而是"用 liveness 只在**必须有**的精确点 spill，且每点只 spill 真 live 的"（Phase 1 已做一半，slots 从 全量 → 精确）。
- 方案 1 的"双平台豁免 + 只 Linux 开"**应放弃或重构**为"精确 spill 的 liveness 收敛"，不再以"捕获可靠"为前提。
- 方案 3（`PalCaptureReliable()` 特性闸）**已落地并构建通过**（§8），作为未来真·寄存器根的原语，但当前不作为省 spill 的前提。

### 7.4 待办重定
- Task#2（方案 1）→ 从"跳过 spill"重构为"精确 spill 收敛"（并入 T2.3），或标记 blocked。
- Task#3（方案 4/T2.3）→ 主骨架，保留。
- **验收以 `test_jit_native` + jit ctest 全绿为准**；任何"省 spill"改动必须证明 GC 扫描仍覆盖所有存活 ref（不欠保留）。

---

## 9. 性能线推进记录（2026-08-12，转 T4.1 / 3.1）

### 9.1 寄存器分配已达硬件极限（数据确认）
`CHAOS_IL2CPP_CODEGEN_STATS` 实测 1091 方法：GPR load 命中 97.5%、spill 623、writethrough 7268(arg-reg 基线)。x64 持久池=仅 R12-R15(Win64 加 RDI)，RAX/RCX/RDX 隐式被 Div/Mul/Shift clobber、RBX/RSI/RBP 架构保留不可动 → **扩持久寄存器预算无 x64 余量且非主导成本**（已放弃，Task#4 关闭方向）。真实余量在 ARM64(12 vs allocator 10)，但测试盲区。

### 9.2 T4.1 基准可信度 — 已由既有 harness 覆盖（Task#5 完成）
核查确认：benchmark_report.py 已有 mean/median/coverage-asymmetry/GC-comp 与 GC-overwrite bug 已修(commit 前)；benchmark_chunk.py 已有 adaptive+outlier+FP-12 零时长守卫+`_MIN_ELAPSED_FLOOR`+`_write_combined_perf_store`(修 overwrite)。native 侧计时真实循环 iterations 调 `ChaosDispatchMethodBenchDirect`→direct_ptr，非短路。**残余验证需跑真实 foundation-dll chunk pipeline（环境依赖）**，无新增伪影修复待做。

### 9.3 消除中间层 dispatch（Task#6 in_progress，需 profile）
已定位中间层：`ExceptionEmission.Linear.cs:717/721` + `ExceptionEmission.Utilities.cs:342/346` 生成 `kChaosExternalRuntimeFnTable[idx](params)` 间接调用，用于「callee 无 shape-matching ExternalRuntimeHelper 定义、函数符号链接期不存在」的跨 assembly 方法。直接替代 = `TargetSymbol(params)`。**关键判断**：间接调用仅 1-3 cyc，不足以解释 2.5-30× —— 真相更可能是 kChaosExternalRuntimeFnTable 路径让 callee **落 interpreter/reflection 而非 native 码**。需 benchmark profile 定真因（indirect vs interpreter-fallback）后针改。范围大（跨 assembly call-site generation + 符号 link 可用性架构），需完整 build+pipeline 验证。

### 9.4 后续建议
- Task#6 先跑一次真实 benchmark profile（chaos-aot vs net8 各方法的 bottleneck 分类来自 benchmark_report 的 `dispatch_overhead`/`alloc_hot`），确认 2.5-30× 是真.interpreter-fallback 还是间接调用。
- 若 interpreter-fallback：修复方向=让跨 assembly 符号可链接（外部导出/准直接符号），非压榨间接调用。
- 依赖：需 foundation-dll chunk pipeline 可用（fd-verification 线）。

### 9.5 Task6 符号前置调研完成（文档交付）
见 `dispatch-intermediate-layer-research.md`。**真因确认**：2.5-30× 非间接调用开销，而是 `kChaosExternalRuntimeFnTable` 路由的跨 assembly / 无 shape callee **落解释器**（`InterpreterEntryDirect`），无 native AOT 码。正确修复=让这些 callee 有 native 码，方向 A(InternalCall 编译期符号解析, 代码注释预定的 future enhancement) 优先。待 benchmark profile 定覆盖优先级后实施。
