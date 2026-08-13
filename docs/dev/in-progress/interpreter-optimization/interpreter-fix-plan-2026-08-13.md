# 解释器修复计划 — 方案设计与深度分析（含测试补全路线）

> **日期**：2026-08-13 ｜ **前置**：`interpreter-deep-analysis-2026-08-13.md`（4 维度发现）
> **性质**：对问题清单按'正确性 P1 → 正确性 P2 → 性能 → 基建/测试'分轨道，对 6 个架构关键决策点给**多方案 + 深度分析**，落成可执行计划。
> **方法**：先读源码确认方案可行性（RegisterVM tier 位置 / OsrState 字段 / tiering_benchmark PIC），再设计。

---

## 〇、问题全清单（归类矩阵）

| 轨道 | # | 问题 | 严重度 | 证据 | 测试补全 |
|------|---|------|--------|------|---------|
| **A 正确性 P1** | A1 | OSR 跨层 pool-flag 丢失 → 错配 free | P1 | osr_state.h:43-45 无 `tracked_is_pool`；ir_reg_alloc.h:230-236 无条件 FREE | OSR 生命周期 GC-stress（Fast→Reg 跨界 pooled box） |
| | A2 | interpreted→native 无 preemptive 转换 | P1 | precache_dispatch_raw.cpp:277 vs entry_direct.cpp:808 | Native 调用 GC 挂起测试 |
| | A3 | back-edge-only safepoint → straight-line/递归超时 | P1 | fast_dispatch_execute.inc:471/516（kOp_Next 不看 suspend_seq） | 长 straight-line + GC 周期交集测试 |
| **B 正确性 P2** | B1 | tier_state/direct_ptr 读取未配写者 release fence | P2 | entry_direct.cpp:802/842 + tier_manager.cpp:262 | 并发 tier 升级竞态测试 |
| | B2 | LdLoc/StLoc 无 idx<32 / _NC 无 sp 越界 | P2 | fast_dispatch_core.inc:402, fast_dispatch_execute.inc:262 | >32 局部方法 / 深栈方法 |
| | B3 | SEH 异常身份跨帧丢（DoMIC / rethrow） | P2 | fast_dispatch_call.inc:90-95 | 跨调用异常类型断言 |
| | B4 | g_static_fields 双策略跨模式别名 | P2 | interpreter_vm.cpp:587 vs jit_helpers.cpp:462 | 同 static 槽跨 interpreter/JIT 读写 |
| | B5 | double-ack 竞态（in-band + 信号） | P2 | thread_state.cpp:261/344 | 信号 + in-band 并发 GC-stress |
| | B6 | T1 fast path 不递增 call_count | P2 | entry_direct.cpp:696-748 | 热 2-instr getter 升级断言 |
| **C 性能** | C1 | FastExecute 每回边 safepoint+双 OSR | P3 | fast_dispatch_execute.inc:464-499 | tiering_benchmark 前后对比 |
| | C2 | RegisterVM 每 op 函数调用+分支存器文件 | P3 | ir_reg_alloc.cpp:2665, ir_reg_alloc.h:108 | tiering_benchmark 前后对比 |
| | C3 | QuickJIT call count=1 触发 | P3 | tier_manager kQuickJitThreshold=1 | 短生命周期方法升温 |
| | C4 | 热路径隐藏堆分配 | P3 | ArrayStorage vector / SmallFieldArray>2 / call>8 | 分配计数断言 |
| **D 基建/测试** | D1 | 解释器 ns/op 锚未落地 | — | tiering_benchmark 唯一 / histogram 默认关 | 接通 PROFILE build + 接线 |
| | D2 | 跨层一致性门禁 | — | — | 同 IL 三层行为一致测试 |

---

## 一、6 个架构关键决策点 — 多方案深度分析

### 决策 1：修 OSR pool-flag 丢失（A1）—— 3 方案

**背景**：`OsrState` 和 `RegisterFrame` 都只持 `tracked_objs`/`tracked_dtors`，不持 pool 标志。FastFrame 的 pooled boxed 对象（`TrackPool`）过 OSR 边界后，`CleanupTracked` 无条件 `CHAOS_IL2CPP_FREE`。

- **方案 1A（推荐 · 最小正确）**：给 `OsrState` + `RegisterFrame` 加 `tracked_is_pool[kMaxTracked]` 位图，`CleanupTracked` 按位分池/堆销毁（pool→ReturnBoxToPool，heap→FREE）。改动集中在 osr_state.h/.cpp + ir_reg_alloc.h。成本 ~0.5d。风险低。测试：构造 Fast→OSR→Reg 跨界 pooled box，GC-stress 断言不崩溃。
- **方案 1B（统一 allocator）**：废除 pool/堆分离，boxed 对象统一走 DOMAIN 分配 + GC 根表，从根上消除错配。但推翻 pooled 池设计，改内存模型，风险高（动谷#1）。成本 2-3d。不推荐（本次范围外）。
- **方案 1C（保守：OSR 前显式物化）**：FastFrame 在 OSR 前把所有 pooled tracked 对象转成 heap 版（调用点物化，类似 `LazyBox` 物化）。成本 1d，但引入 OSR 边界拷贝，且与其他物化点（1.2）重叠——用户已中止 1.2。

**决策（推荐 1A）**：加 pool 位图，不动内存模型。测试 TDD：先写「OSR 后 pooled box 不死/不双释放」失败测试。

---

### 决策 2：interpreted→native 无 preemptive 转换（A2）—— 3 方案

**背景**：解释器路径（`precache_dispatch_raw.cpp:277-295`）直接调 native `direct_fn`，不发 `GC_TRANSITION_TO_PREEMPTIVE`；只有 T4/JIT 路径（`entry_direct.cpp:808-811`）有 `GcTransitionGuard`。blocking native 调用期间 GC 无法挂起 → 死锁/延迟窗。

- **方案 2A（推荐 · 对齐 T4）**：在解释器调 native/P/Invoke 的边界包 `GcTransitionGuard`（进入 preemptive → 调用 → 回 cooperative），对齐 T4。改 `precache_dispatch_raw.cpp` 的直调点 + `fast_dispatch_call.inc` 的 native 分支。成本 0.5-1d。风险中（需确认 native 调用返回后必回 cooperative，否则 GC 停不了）。
- **方案 2B（stub 自转）**：codegen 生成的 P/Invoke stub 内部自带转换（已有 POSIX trampoline/Windows APC 说法）。但解释器路径不走这些 stub——是解释器调 native 的缺口，不是 stub 缺口。不适用。
- **方案 2C（统一转换点）**：在 `InterpreterDispatchRaw`/`precache_dispatch_raw` 顶层统一包转换（一旦进入 native 调用就 preemptive）。改动集中但影响所有 native 调用。成本 1d。与 2A 类似但更集中。

**决策（推荐 2A）**：在解释器 native 调用边界包 `GcTransitionGuard`。注意：`GcTransitionGuard` 必须 RAII（scope exit 还原），否则异常路径泄漏 cooperative 态。测试：blocking native stub + 挂起断言。

---

### 决策 3：back-edge-only safepoint 覆盖缺口（A3）—— 4 方案

**背景**：in-band poll 只在 `frame.pc < last_pc`（回边）触发；`kOp_Next` 的 decimated 检查只看 `pending_abort/interrupt`，**不看 `suspend_seq`**。straight-line 长方法/深递归 → 无法主动 drain → 依赖 100ms 抢占 + 500ms 硬超时。

- **方案 3A（推荐 · 并入 decimated）**：把 `suspend_seq` 检查并入现有每 64 指令的 `kOp_Next` decimated 槽（`(++check_counter & 63)==0`）：非回边也每 64 指令查一次 `suspend_seq`，命中才进完整 `SafepointPoll` try/catch（转译 → threw_exception）。保证 straight-line ≤64 指令必 drain，且回边 probe 可从每迭代移到 decimated（顺带消 C1 热点税）。成本 0.5-1d。风险低（复用现有 1/64 槽）。测试：长 straight-line（>64 指令无回边）+ BGC 交集，断言方法可被挂起。
- **方案 3B（每指令 poll）**：每 op 都查 suspend_seq。成本高（回边税变全 op 税，性能回归 C1 恶化）。不推荐。
- **方案 3C（调用点 + 回边双 poll）**：回边 + 每个 Call/NewObj（潜在阻塞点）都 poll。覆盖 blocking 调用，但漏纯算术 straight-line。半覆盖。
- **方案 3D（保持抢占兜底）**：维持现状 + 强化 100ms 抢占的可靠性与 double-ack。不改解释器，但留下"最坏 100ms 才挂"的窗。低风险但治标。

**决策（推荐 3A + D 保底）**：3A 消 straight-line 缺口，D 修 double-ack 边界。3A 与 C1（性能）天然协同（回边 probe 也移到 decimated）。

---

### 决策 4：RegisterVM 存废（C2）—— 4 方案

**背景**（已确认）：RegisterVM（`RegisterExecute`，Step B）是 **核心 tier 路径**（`kTerminalTier = kOptimizedRegister`，JIT disabled 的终态），每 op 独立函数调用 + 分支存器文件，对小型方法反而比 FastExecute 慢。

- **方案 4A（推荐 · 修存器文件 + 内联）**：给 RegisterExecute 接扁平单索引数组（去掉 `reg()/set_reg()` 每操作数 64-bound 分支）+ 内联高频 op（Ldc/Add 等）。成本 1-1.5d。风险中。测试：tiering_benchmark 前后 ns/op 对比 + 全 interpreter 单测。
- **方案 4B（FastExecute 接管 T2，RegisterVM 降级 legacy/SEH-only）**：让简单方法直接走 FastExecute（computed-goto），RegisterVM 只留 SEH/复杂方法。需要重排 tier 路由 + 确认 FastExecute 覆盖。成本 2-3d，架构级。风险高（tier_state/terminal 语义重定义）。测试量大。
- **方案 4C（4A + 抽 LazyBox 到 RegisterVM）**：4A 基础上，RegisterVM 复用 FastExecute 的 LazyBox（int32/int64 免装箱），消 Reg_Box 的 pool-acquire。低成本高值附加。
- **方案 4D（不动）**：接受 RegisterVM 慢，靠 tier 快速升 JIT 掩盖。但短期寿命方法（3-20 calls）在 JIT 前驻留 RegisterVM，性能可感知。

**决策（推荐 4A + 4C，4B 作为后续专项）**：4A 是收益/风险最优；4B 是最终目标但需单独专项（涉及 tier 语义重定义）。4D 只接受现状。

---

### 决策 5：FastExecute 每回边 epilogue 税（C1）—— 2 方案

**背景**：`kOp_Epilogue` 每 op 都 `if (frame.pc < last_pc)` 双查（safepoint + OSR），回边时还 `suspend_seq` acquire-load。紧循环每迭代 ~20-40cy 税。

- **方案 5A（推荐 · 撤回边 probe 到 decimated）**：与 3A 合并——suspend_seq 只在每 64 指令 decimated 槽查，回边只剩 OSR counter（无 acquire-load）。消最热形状的常数税。成本含在 3A。测试：tiering_benchmark 紧循环 ns/op 对比。
- **方案 5B（合并双 `pc<last_pc` 为单查）**：把 epilogue 的 safepoint 和 OSR 两个 `pc<last_pc` 合并成一次求值 + 位复用。成本 0.5d，小值。

**决策（5A 为主，5B 顺带）**：3A 已是 5A，同一改动。

---

### 决策 6：tier_state/direct_ptr 读取竞态（B1）—— 3 方案

**背景**：dispatch 读 `tier_state`（acquire）后单独读 `direct_ptr`，写者是在 `tier_state=kJitted` release-store 之后再 release-store `direct_ptr`。并发 `JitRecompileToTier1` 可让读者看到 `kJitted` + stale `direct_ptr`。

- **方案 6A（推荐 · 结构体合并 CAS）**：把 `tier_state` + `direct_ptr` 合并进一个 aligned 原子结构（类似 hotpatch 的 `flags+method_key` 双字 CAS），一次 load 读一致快照。对齐仓库已有的 hotpatch dispatch 模式。成本 1d。测试：并发 tier 升级压力测试。
- **方案 6B（读侧 paired acquire + 校验）**：dispatch 读 `direct_ptr` 用 acquire，读后校验 tier_state 未再变（重读）。短窗校验，成本 0.5d，但治标（仍有 TOCTOU）。
- **方案 6C（写侧统一 CAS 交换）**：写者用 CAS 把 `tier_state`+`direct_ptr` 一起换（对齐 EJitRecompile 已有的 `JitRecompileToTier1` CAS）。成本 0.5-1d。

**决策（推荐 6A/6C 对齐 hotpatch 双字 CAS 模式）**：一次性读一致快照，消除 TOCTOU。测试：多线程 tier 升级 + 频繁 deopt/recompile。

---

## 二、通用 / 横向测试补全策略

### TDD 纪律（每个修复）
1. 先写**失败测试**（复现缺陷），红 → 修 → 绿。
2. 断言**失败模式**而非泛化（如 OSR 后不崩溃/不双释放；native 调用可挂起）。

### 跨层一致性门禁（D2，横向）
- 对 null/越界/SIMD/SEH 等每个 op，**同一 IL 在 T1-computed-goto / T2/T3-RegisterVM / InterpreterVM 三层行为一致**（抛同样异常/返回同样值）。
- 落成参数化 gtest：`CrossTierConsistency` 族，驱动同 IR 过三层断言结果一致。
- 这是防止"只修一层/层间漂移"的系统门禁（本次 null/越界已踩过）。

### GC-stress 生命周期族（A1/A3/B5/D 横向）
- `InterpGcLifecycleStress`：长 straight-line 方法 / 深递归 / OSR 跨界 / native 调用 / 信号 + in-band 并发，在 BGC 周期下持续跑，断言无悬挂/崩溃/错配 free。
- 复用 `chaos_gc_region_barrier_stress_test` 的 stress label 机制，但**不进 -LE 排除**（或用已知-fail 标注残余）。

### 性能回归（C/D 横向）
- 接通 `tiering_benchmark`（见决策），加 PROFILE build 跑真实 opcode 直方图。
- 增量基线：每次性能改动前后跑，登记 ns/op 差异表。
- 性能改动 gate：任何函数 >10% 变化标记，>50% 需重设计。

---

## 三、合成执行计划（分阶段，每阶段含测试）

### Phase P0（正确性 P1，先做）—— 独立、可单测
| 任务 | 方案 | 改动 | 测试 |
|------|------|------|------|
| A1 OSR pool-flag | 1A | osr_state.h/.cpp + ir_reg_alloc.h 加 `tracked_is_pool` 位图 | OSR 生命周期 GC-stress |
| A2 native preemptive | 2A | native 调用边界 `GcTransitionGuard`（RAII） | blocking native 挂起测试 |
| A3 + C1 safepoint/epilogue | 3A+5A | suspend_seq 并入 decimated，撤回边 probe | 长 straight-line GC-stress + 紧循环 ns/op |

### Phase P1（正确性 P2）—— 需更细
| 任务 | 方案 | 改动 | 测试 |
|------|------|------|------|
| B6 T1 call_count | — | entry_direct fast path 补 fetch_add | 热 2-instr 升级断言 |
| B2 越界 | — | LdLoc/StLoc 加 idx<32 / _NC 加 sp 检查 | >32 局部 / 深栈方法 |
| B1 tier_state 竞态 | 6A/6C | 双字 CAS 快照 | 并发 tier 升级压力 |
| B3 SEH 异常身份 | — | DoMIC 补异常对象 / rethrow 用原 throw pc | 跨调用异常类型断言 |
| B4 g_static_fields | — | 统一双路 barrier 策略 | 同 static 槽跨模式读写 |
| B5 double-ack | — | 分离"already-parked"与"hijacked-mid-run" | 信号+in-band GC-stress |

### Phase P2（性能结构）
| 任务 | 方案 | 改动 | 测试 |
|------|------|------|------|
| C2 RegisterVM | 4A+4C | 扁平存器文件+内联+LazyBox | tiering_benchmark 对比 |
| C3 QuickJIT 升温 | — | 阈值 >=1 守卫 + interpret-first（评估） | 短生命周期方法 |
| C4 热路径堆分配 | — | call>8 用池 / SmallFieldArray 预分配评估 | 分配计数断言 |

### Phase P3（基建/测试）
| 任务 | 方案 | 改动 | 测试 |
|------|------|------|------|
| D1 ns/op 锚 | — | tiering_benchmark 接通 PROFILE + 直方图接线（已做 entry hook） | 全 tier ns/op 基线落盘 |
| D2 跨层一致性 | — | CrossTierConsistency 参数化 gtest | 同 IL 三层一致 |

### 依赖与顺序
1. **P0 独立可先做**（A1/A2/A3 互不阻塞，各自 TDD）。
2. **P1** 依赖 P0 稳定基线；B1（CAS 快照）与 hotpatch 双字模式对齐。
3. **P2** 依赖 P1 后基线 + D1 测量锚（先能量化再优化）。
4. **D2 跨层门禁**应尽早建（P0 前），防后续各修复引入层间漂移。

---

## 四、风险与未知
- **A2（native preemptive）**：`GcTransitionGuard` 必须 RAII 且覆盖所有退出路径（异常），漏还原会反向让 GC 停不了。需确认 native 调用返回路径。
- **B1（tier CAS）**：改 dispatch 热读路径，需保证一致性 CAS 不引入额外 acquire 开销。
- **C2（RegisterVM）**：4A 扁平存器文件可能改变 reg 布局，影响 deopt/OSR 的 reg 恢复（CaptureNativeFrame 靠 reg index）——需连带验证。
- **D1 测量**：性能改动若无量化锚，= 盲优化。P0 后立即建锚。

---

## 五、关联
- 发现：`interpreter-deep-analysis-2026-08-13.md`
- 上轮已修：`interpreter-review-current-2026-08-13.md`
- 峰平谷：`interpreter-capability-peak-plateau-valley-2026-08-13.md`
