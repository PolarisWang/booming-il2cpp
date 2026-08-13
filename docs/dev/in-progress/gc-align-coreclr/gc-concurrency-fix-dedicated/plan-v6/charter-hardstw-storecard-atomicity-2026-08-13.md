# 立项 — A2b 根治：hard-STW / store+card 原子化专项（2026-08-13）

> 域：GC(6) + 运行时(2) + CodeGen(4)。状态：**立项草案**。前置：A2b 主 dangling 根因已被全域调查锁死。
> 目标：为 `gc_region_barrier_stress_test` 的 store-then-barrier 竞态提供可实现的根治路径。

## 1. 立项背景（为何需要本项目）

`gc_region_barrier_stress_test`（committed known-failing reproducer）长期 dangling（32-117/1024, thread-7）。
经历卡侧(A2b 审计)+FULL mark(domain A) 两轮系统性调查后，三角定位把根因收敛到**唯一剩余机制**：

| 域 | 尝试 | 结论 |
|----|------|------|
| 卡侧索引 | Option B / base 对齐 / seg_addr 派生 | 3 次 no-op → 索引自洽 |
| FULL mark | C1-C5 / BlockSizeAtPayloadOffset / 页级标记 | FULL 塌缩真 bug 但保留不降 dangling |
| young 晋升 | Phase-1 `promoted_back==scavenged` | 晋升机制正确 |

**锁死根因 = store-then-barrier 竞态**（test 自身注释 gc_region_barrier_stress_test.cpp:119-126 即权威表述）：
mutator `store` nursery ref 到 old slot → GC 在 `barrier` 前扫该页(卡干净) + reset nursery → 仅从此槽可达的 nursery 对象被回收 → 稍后 barrier 已晚。

## 2. 约束（已实证，决定方案可行性）

- **ForbidSuspendScope 单独不闭合**：`tls_forbid_suspend_depth`(thread_state.cpp:274) 是 **ack-and-continue**（深度>0 时 SafepointPoll 只 ack 不 block）。GC 请求 safepoint 落在线程处于 store→barrier 之间时：线程 ack（GC 不无限等）→ GC 直接进 Phase-1+reset，此时线程仍在 store/barrier 间 + 卡未置 → 漏仍发生。
- **γ' 收窄未根治**：`barrier_inflight` 原子段 + safepoint 等 `barrier_inflight==0`（attempt 3）把 dangling 32-307→95-117，仍恒 thread-7。`ForbidSuspendScope`(ack) 不足。
- **硬 SuspendThread 未实现**：pal_preempt_win32.cpp:52 注明 `(SuspendThread + GetThreadContext)` 是 separate effort (plan 2b/B)。Windows 用 QueueUserAPC+PalEventWait，**park 在 APC 内** → GetThreadContext 拿到的是 suspend 机制帧而非中断点寄存器 → `PalCaptureReliable()==false`(pal_preempt.h:108)。Linux(SIGUSR2 ucontext) 已可靠捕获。
- **发射契约**：store→barrier 在三套发射路径是独立指令（jit_helpers / 解释器 .inc / LinearEmission），见 [章节 4]（Explore 待填）。

## 3. 候选方案（二选一或组合）

### 方案 1：hard-STW（真 SuspendThread）— 对齐 CoreCLR
把 Windows 的 APC-park suspend 升级为真 `SuspendThread + GetThreadContext`（PalCaptureReliable 翻 true）。配合 safepoint：GC 在入口**真停**所有线程到一致点，根除「线程停在 store 与 barrier 之间」的窗口。
- 前提：Windows AP 无法在 APC 内捕可靠 reg → 需 SuspendThread；这是 [memory jit-phase2] 的方案 2/B，"远期"。
- 优点：根治、对齐 CoreCLR（CoreCLR 用 SuspendThread 实现 STW）。
- 缺点：跨平台改造 pal_preempt + safepoint 语义；Windows 线程被 SuspendThread 停后可能停在任意指令（含持锁、分配中途）→ 硬 STW 单独仍可能停在 store/barrier 之间（这正是 memory M1 曾证：硬挂起仍可停 store/card 之间）。

### 方案 2：store+card 原子化（码 barrier 在 GC 扫前已置）
让 store 与 barrier 之间**不可被 GC 打断**，或 barrier 前置：
- 2a：共享 helper（若 jit_helpers CodegenSt* 已收口 store+barrier）内包临界区 + **safepoint 真等**（补 ack-and-continue 缺陷：scope 内不准 ack）。问题是改 safepoint 等待语义全局影响。
- 2b：**card-before-store**（barrier 前置）—— .NET 写屏障是 post-store，语义不符，弃。
- 2c：Phase-1 后对 「未 ack 线程涉及的页」重扫（candidate D）—— 但 naive 全槽重扫曾污染（1023/1024），且需精确布局（OldMessage 无 layout）已证伪。

> M1 3 层汇聚曾推荐组合：**方案2a 主线（helper 内禁 GC 临界区）+ Phase-1 重扫兜底**，发射 0 改动（helper 天然收口）。这仍是当前最可行路线。

## 4. 实现范围（世界选项盘点 — 已探明）

### store→barrier 发射点（三套后端，最终汇入同一 C++ 卡函数）
| 后端 | 位置 | store/barrier 关系 | 屏障 |
|------|------|-------------------|------|
| **JIT 共享 helper**（自然收口） | `jit_helpers.cpp` `CodegenStFld`:82/99/101, `CodegenStElem`:543/554/556, `CodegenStElemNoCheck`:587, `CodegenStObj`:681/694/696, `CodegenCpblk`:702 | **同一 C++ 函数内相邻语句** | 单参 `chaos_gc_dirty_card` |
| 解释器 | `interpreter_vm.cpp`:834/857/1573/1849 + `fast_dispatch_object.inc`:110/155/228/344 + `fast_dispatch_arithmetic.inc`:455 | 同 case/handle 内相邻 | 单参 `chaos_gc_dirty_card` |
| managed AOT 发射器 | `LinearEmission.cs` `EmitLinearStoreIndirect`:69-92(store:86→barrier:89) + `ExceptionEmission.EmitInstruction.cs` stfld:1231/1234 | 同一 `{}` 块内相邻语句 | 双参 `chaos_gc_dirty_card_dst_ref` |

- **关键**：JIT helper 是**唯一汇聚点** —— store+barrier 已在同一 C++ 函数相邻。方案 2a 只需在该处 + 解释器 + managed 发射器三处包临界区（M1 3 层早已验证 helper 天然收口，发射 0 改动）。
- **屏障语义分叉**（需裁决）：JIT/解释器用**单参** `chaos_gc_dirty_card`(保守)，managed AOT 用**双参** `_dst_ref`(generational)。原子化需决定统一走 `_dst_ref` 还是保留两套。
- 静态字段 `CodegenStSFld`:471 用 `BgcRecordRootChange`，**不 card**（根跟踪非卡），在原子化范围外。

### safepoint 等待（决定方案 2a 可行性）
- `tls_forbid_suspend_depth`(thread_state.cpp:66) = ack-and-continue（SafepointPoll:274 只 ack 不 block）。**核心缺陷**：scope 内线程 ack 后 GC 不无限等 → 线程仍可能在 store/barrier 间。
- `RequestGlobalSafepoint`(:411)：只给 **cooperative** 线程设 `suspend_seq=epoch`(:463-468)，busy-wait 计数非 ack(:488)。timeout 100ms(SIGUSR2/APC)→500ms force-release(:549)。
- **native Worker 场景**：`g_old_slot[i]=x; barrier()` 无 poll 调用 → **cooperative 模式不强制 mid-store 停**；preemptive 模式被 SIGUSR2/APC 打断（可在 store/barrier 中间）。→ 这正是竞态窗口的机制佐证。

### hard-STW 现状（方案 1 前提）
- Win：`QueueUserAPC` + `PalEventWait`（park 在 APC 内）→ `PalCaptureReliable()==false`(:54)。**无 SuspendThread**（plan 2b/B 远期, :51）。
- Linux：`SIGUSR2` + `SA_SIGINFO` ucontext → `PalCaptureReliable()==true`(:110)。
- **结论**：无「native 非 safepoint 代码的真 cooperative STW」；仅 cooperative-poll + 超时 preemptive suspend。方案 1(真 SuspendThread) 是独立基建，且 M1 曾证「硬挂起仍可停 store/card 之间」→ **方案 1 单独不足**，须配方案 2（码 barrier 在扫前已置）。

### 收敛建议
**方案 2a 为主线**（共享 helper 包临界区 + 修 safepoint 等待语义让 scope 内真等），**方案 1(hard-STW) 为远期**对齐 CoreCLR。方案 2b(card-before-store) 因 .NET 写屏障语义(post-store)不适用；2c(Phase-1 重扫 naive) 已污染证伪。

## 5. 分阶段计划（下一专门会话）
1. **T-1（裁决屏障语义 + 收口确认）**：统一单参/双参屏障 or 保留两套；确认 jit_helpers 6 处 + 解释器 + managed 发射器的收口清单（已探明）。
2. **T-2（方案 2a 主线）**：在共享 helper(`CodegenStFld`/`StElem`/`StObj`)内包 `ForbidSuspendScope`(store+barrier) + **修 safepoint 等待语义**（scope 内线程不可 ack，GC 真等其退出 scope → 补 ack-and-continue 缺陷）。解释器 + managed 发射器同步。验证 stress。
3. **T-3（若仍漏）**：Phase-1 对「未 ack/临界区线程涉及的页」精确重扫（兜底）。
4. **T-4（远期）**：hard-STW（SuspendThread, Linux 优先；Win 需 plan 2b/B 基建）。

## 6. 验收
- `gc_region_barrier_stress_test` **20x 全 0**（现状 32-117/1024）。
- `ctest -R gc_ -L unit` 全绿（stress 从 known-failing 翻转 PASS）+ jit/解释器无回归。
- `CHAOS_GC_HeapVerify=2` under stress 无 UAF。
- 多 Run 单调（≥20 次 Run 覆盖不同时序）。

## 7. 依赖 / 前置
- A2b 已锁死 store-then-barrier 竞态（本立项输入，`gc_region_barrier_stress_test.cpp:119-126`）。
- 方案 2a 需改 safepoint 等待语义（影响全局）→ 需回归全 ctest；方案 1 需 pal_preempt 平台改造。
- 相关 memory：`jit-phase2-register-capture`(方案2/B SuspendThread 远期)、`jit-cross-platform-register-capture-unify`(PalCaptureReliable 4平台)、`gc-crossgen-stress-true-rootcause`。

