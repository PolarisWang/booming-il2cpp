# 立项 — hard-STW 专项（对齐 CoreCLR，A2b 唯一根治路径）v7-立项

> 域：GC(6) + 运行时/线程safepoint(2) + 平台抽象(9) + CodeGen(4)。状态：**立项 / ready-to-plan**。
> 日期：2026-08-13。
> 前置输入：`charter-hardstw-storecard-atomicity-2026-08-13.md`（A2b 根因+世界盘点）已把 hard-STW 列为「远期 T-4」。
> 本立项**把 hard-STW 从「远期」提为第一类项目**，并给出可执行的分阶段路线。**架构裁决升级**：此前把 hard-STW 判为「方案 1 单独不足」而搁置——本立项纠正这个框架误区，见 §2。

---

## 1. 立项背景（为何现在单独立项 hard-STW）

`gc_region_barrier_stress_test` 的 A2b 跨代 UAF（**store-then-barrier 非原子竞态**）已被全域调查锁死根因：

- 读侧(卡侧 3 修 / base 对齐 / seg_addr) + FULL mark + 晋升机制 + 卡读侧不变量审计：**全部排除或 no-op**。
- 唯一剩余机制 = `store nursery_ptr→old slot` 与 `dirty_card` 之间，GC safepoint 落在线程中间 → Phase-1 扫该页时卡仍干净 → 仅从此槽可达的 nursery 对象被回收 → dangling。

此前的失败史（γ' 收窄 32→95-117 但恒 thread-7 / A'=γ' re-derivation 实测 74-117 未修）+ 卡读侧数学自洽证明：**A2b 无法靠 C++ 写屏障原子性 + 协作式 poll 收尾**。这触发三条修复规则停线，归档 known-hard，根因收敛指向唯一未试的根治基建 = **hard-STW**。

**为什么现在立项**：整个九月线（M1-A2b）已把根因 100% 闭锁，且 hard-STW 所需的**关键前置（Linux SIGUSR2 可靠捕获、统一 PAL 挂起原语面、suspend trampoline）已存在**。立项已具备全部投入条件，不再发散。

---

## 2. 架构裁决升级：修正「hard-STW 单独不足」的框架误区

历史上内存与 charter 反复记录「**硬挂起仍可停 store/card 之间 → 方案1 单独不足 → 转到方案 2a(原子化) 为主线，hard-STW 远期**」。这个结论**本身正确但不完整**——它是把 hard-STW 当作「用来直接消灭竞态窗口的手段」来评估的。**这框架错了。**

**CoreCLR 的写法（真实机制）**：CoreCLR 的 STW 由 ① **SuspendThread 停所有线程到一致点** + ② **写屏障是 assembly LEAF（无 poll，store+card 一体，内部无任何 safepoint/阻塞点）** 两部分叠加构成。**安全性不是来自挂起机制本身，而是来自「写屏障内不可落 safepoint」这一 codegen 属性**。SuspendThread 的意义是：让被挂线程**必然停在 safepoint 处**（而非任意中间指令），从而 GC 能精确扫描。

所以 **hard-STW 与 写屏障 leaf（store+card 原子）是一体两面、互为前提**：

| 组件 | 作用 | 若缺 |
|------|------|------|
| **SuspendThread STW** | 保证所有线程停到**安全点**（不落任意指令）| 线程可能停在不安全点（持锁/分配中途）→ 不能安全扫描 |
| **写屏障 leaf（store+card 无 poll）** | 保证**安全点内部不落在 store 与 card 之间** → card 必被置 | 线程停在 store/card 中间 → 扫到干净卡 → 漏 nursery 边 |

**结论（立项裁决）**：A2b 的根治 = **hard-STW（停一致点）+ leaf 写屏障（码 barrier 在扫前必置）两者叠加**。方案 2a（store+card 原子化）本质是**换一种方式实现「写屏障 leaf」**（用 forbid_depth 让 safepoint 不落在 store/card 之间），但它没有真正的 SuspendThread 停一致点支撑——这正是它 C++ 实现收窄不根治的原因。**hard-STW 立项 = 补齐两样：真 STW + leaf 屏障的确定性保证。**

> 注：方案 2a 的 `BarrierCriticalSectionScope`（commit 25c66b016）已实现 leaf 屏障的**一部分**（store+card 包 forbid_depth，让 SafepointPoll 不 block）。它缺的正是「**真 STW 停一致点**」另一半。本项目把两半合一。

---

## 3. 平台现状（实读源码，立项输入）

三层平台现状（Explore 实测）：

| 平台 | 当前挂起原语 | `PalCaptureReliable()` | 是否是真 STW 一致点 | 差距 |
|------|-------------|------------------------|--------------------|------|
| **Linux** | `SIGUSR2`(pthread_kill) + `SA_SIGINFO` ucontext | `==true` (`pal_preempt_posix.cpp:110`) | **接近**：`PreemptiveSuspendHandler`(`thread_state.cpp:348-386`) 改 RIP → `gc_suspend_trampoline_x64`, 然后 trampoline 在**正常栈**上 `SafepointPoll` 等 `suspend_event` | 已具备可靠寄存器窗捕获 + 一致点 trampoline |
| **Windows** | `QueueUserAPC` + `PalEventWait`(park 在 APC 内) | `==false` (`pal_preempt_win32.cpp:54`) | **否**：park 在 APC 内，GetThreadContext 拿到的是挂起机制帧非中断点 | **真正缺口**。需真 `SuspendThread`+`GetThreadContext`+`ResumeThread` |
| Apple / Android | no-op (`pal_preempt_apple.cpp:17`, `_android.cpp:17`) | `==false` | 否 | 待移植（平台占有信号） |

**统一 PAL 面**：所有挂起/捕获经 `src/native/pal/chaos/pal/pal_preempt.h` 单一原语面（`PalPreemptRequest` / `PalPreemptiveSuspendAck` / `PalCaptureReliable` / `PalCaptureThreadContext`）。**hard-STW 落地 = 替换 per-platform 的 `PalPreemptRequest`/park 实现 + 翻转 `PalCaptureReliable()`**。核心结构字段已预留：`os_handle`(`thread_state.h:137`, 注释明言"for SuspendThread/ResumeThread on Windows"), `gc_capture_slot`/`gc_reg_file`/`gc_num_gprs`(`:168/177/178`)。**基建全就绪。**

**协调器现状**（`thread_state.cpp:411` `RequestGlobalSafepoint`）：spin-with-timeout + `PalPreemptRequest` 兜底(:515-526 / :534-557) + 硬 watchdog `kSafepointHardTimeoutNs=500ms` force-release(:559-565)。**`preemptive_suspended` 字段只被置、从未被用于真挂起**。

---

## 4. 目标（验收判据）

1. `gc_region_barrier_stress_test` **20x 全 0 dangling**（现状 4-5/10 失败, 32-235/1024, 恒 thread-7）。
2. **Linux 优先达成**（SIGUSR2 near-STW 已具备）→ 证明 hard-STW(一致点) + leaf 屏障是充分条件。
3. Windows 真 SuspendThread 落地面：`PalCaptureReliable()==true`。
4. `ctest -R gc_ -L unit` 全绿 + `CHAOS_GC_HeapVerify=2` under stress 无 UAF。
5. 无死锁：STW 阶段所有线程在有限时间内到达一致点（不触发 500ms watchdog 假释放）。

## 5. 分阶段计划（每阶段自洽、单提交、可回滚）

### Phase A（Linux，实证近 STW + leaf 组合；0 新平台代码）❤️ 先做
**验证不靠新增，靠测量**：Linux 已具备 near-STW（SIGUSR2 一致点 trampoline）+ `BarrierCriticalSectionScope` leaf。A2b 若真能由「一致的挂起点 + leaf 屏障」根治，则 **Linux 上 stress 应已归零**。

- A-1：跑 `ctest -R gc_region_barrier_stress_test` 于 Linux，记录当前 dangling（若已 0，则 leaf+近STW 已足够 → Windows 复制即可；若仍 >0，则证明需真 STW 一致点）。
- A-2：若仍 >0 → 确认 Linux 的 `PreemptiveSuspendHandler` 是否真把**每个 cooperative 线程**停到一致点（读 `thread_state.cpp:348-386` 覆盖 cooperative/preemptive 两条路径；确认 trampoline 路径无中间指令逃逸）。
- A-3：输出 A 的实证结论（数据或根因），作为 Phase B 输入。
- **验收**：A 产出一份「Linux 下 A2b 是否被 near-STW+leaf 根治」的实证报告。

### Phase B（Windows 真 SuspendThread）
- B-1：`pal_preempt_win32.cpp` 实现 `PalPreemptRequest` 用 `::SuspendThread(os_handle)`；`PalCaptureThreadContext` 用 `::GetThreadContext` 捕 `CONTEXT`；释放 `PalResumeThread`。翻转 `PalCaptureReliable()→true`。
- B-2：wire 进协调器——`preemptive_suspended` 从「只置位」变为 ACT 动作：`SuspendThread` 列表 + 扫描 `GetThreadContext` + `ResumeThread`（`thread_state.cpp:534-557`）。
- B-3：Windows stress 跑绿 20x。核对 `CHAOS_GC_HeapVerify=2`。
- **风险**：SuspendThread 可停线程在任意指令（持锁/分配中途）→ 需与 leaf 屏障 + 协调器「等 `barrier_inflight==0`」配合，保证每条线程停点都安全。**这是本项目最需要系统性调试风险控制的部分**（`pal_preempt_win32.cpp:46-52` 注释明示的危险）。

### Phase C（跨平台统一 + 一致性契约）
- C-1：定义统一 STW 契约：「所有线程到一致点（安全点或 barrier leaf 之外）→ GC 精确根扫 → 释放」。
- C-2：Linux/Windows 收敛到同一协调器语义（Phase A 实证 + Phase B 落地后对齐）。
- C-3：Apple/Android 移植（若需）：复用统一 PAL 原语面，平台占有信号 → 独立跟进。

### Phase D（回归 + CI 可见性）
- D-1：`gc_region_barrier_stress_test` 从 known-fail/被排除，转成 **nightly 必跑**（mechanism 已探明：`--stress-only` / `CHAOS_GC_TEST_STRESS_ONLY=1` 翻转 `ctest -L stress|soak|benchmark`，`tests/runner/adapters/native.py:79-83, test_driver.py:357-387`）。
- D-2：已知失败基线 reconciliation：stress 未修复前加 `chaos_gc_region_barrier_stress_test` 到 `tests/runner/baselines/known-failures.integration.yaml`（`stale_known` 会在它转绿时告警 = 修复信号，test_driver.py:448-453）。**修复后移除**。
- D-3：全 `--layer all` 回归（Part E-MANAGED 若落地需 regen + snapshot baselines）。

## 6. 依赖 / 前置（全部已就绪）

| 前置 | 状态 |
|------|------|
| A2b 根因 100% 闭锁（store-then-barrier） | ✅ `charter-hardstw-…` + `gc-a2b-known-hard-archive` |
| Linux SIGUSR2 可靠捕获（`PalCaptureReliable()==true`）| ✅ `pal_preempt_posix.cpp:110` |
| Windows `os_handle` 字段预留（SuspendThread 用途注释） | ✅ `thread_state.h:137` |
| 统一 PAL 原语面 `pal_preempt.h` | ✅ |
| leaf 屏障半成品 `BarrierCriticalSectionScope` + `barrier_inflight` + `chaos_barrier_enter/exit` | ✅ commit 25c66b016 |
| suspend trampoline（`gc_suspend_trampoline_x64`）| ✅ |
| stress 测试 CI 门控机制（`-LE`/`known-fail`/`--stress-only`） | ✅ 已探明 |

## 7. 风险

1. **Windows SuspendThread 停任意指令**（持锁/GC 前状态）→ 需 leaf 屏障 + 协调器等 inflight 收敛；`pal_preempt_win32.cpp:46-52` 明示危险。**系统性调试前置**，禁止盲改。
2. **硬 STW 死锁**：被停线程持锁，GC 需该锁 → 需识别不可挂起区域（fork 为不可挂起/STW 语义）。
3. **性能**：STW = 全量停线程，对低时延场景是回归 → 设计需保留协作式 fast path（非每次 GC 都硬 STW）。
4. **跨平台**（Apple/Android）：平台占有信号，真 STW 需平台原生原语 → 独立立项，不在本专项阻塞。

## 8. 一页真相表

| 项 | 值 |
|----|-----|
| 目标 | A2b（cross-gen store-then-barrier UAF）消除 |
| 根治组合 | **hard-STW（停一致点）+ leaf 写屏障（码 card 在扫前必置）** |
| 真正缺口 | Windows `SuspendThread`（Linux near-STW + leaf 已近证明） |
| 首个实证里程碑 | Phase A：Linux stress 现状（应归零若组合充分） |
| 验收 | stress 20x 全 0 + `ctest -R gc_ -L unit` 绿 + HeapVerify=2 无 UAF |
| 失败史 | γ'(32→95-117) / A'(=γ' 74-117) / 卡侧×3 / FULL×保留 / 晋升×正确 — 均不足，因缺 STW 一致点半 |
| 冲刺 | 三规则已触发过，本立项为**独立新专项**（非继续盲改）|

---

## 9. 相关
- 根因：`charter-hardstw-storecard-atomicity-2026-08-13.md`、`design-m1-a2b-2026-08-12.md`、memory `gc-crossgen-stress-true-rootcause`。
- 平台面：`pal_preempt.h` / `pal_preempt_posix.cpp` / `pal_preempt_win32.cpp`、memory `jit-cross-platform-register-capture-unify`（PalCaptureReliable 4 平台）。
- CoreCLR 对照：`gc-fix-proposals-1-2-coreclr-deepdive-2026-08-13.md`、memory `coreclr-region-barrier-solution-reference`。
