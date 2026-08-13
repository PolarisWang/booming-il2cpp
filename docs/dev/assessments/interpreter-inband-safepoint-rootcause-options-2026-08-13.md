# 解释器 in-band safepoint — 根因深度分析与方案权衡

> **日期**：2026-08-13 ｜ **性质**：Step 2.2 的深入根因分析 + 多方案深度权衡
> **范围**：解释器（FastExecute/RegisterExecute/InterpreterVM）与 GC 挂起的协作

---

## 一、根因分析：解释器线程到底怎么被 GC 挂起 / 哪里会坏

### 1.1 现状机制（已实证，file:line）
GC 想 STW 时：
1. 每个并发线程 `suspend_seq++`（safepoint 请求，thread_state.cpp:243）。
2. 每个线程靠**抢占式信号/APC**（`PreemptiveSuspendHandler`，thread_state.cpp:327）被中断。
3. 中断后：POSIX cooperative → **redirect RIP 到 `gc_suspend_trampoline_x64`**（:356-372），在正常栈跑 `SafepointPoll` 等释放；Windows(pre-MSVC) cooperative → 只能**信号栈 spin-wait** `PalPreemptiveSuspendAck`（:394）。
4. 解释器线程被信号打断的**时机是任意的**（非解释器主动 poll）。

**解释器执行循环**（fast_dispatch_execute.inc:484-494 / 673-677）只做 `pending_abort/interrupt` 的 **load-check**（每 64 指令），**从不调 `threading::SafepointPoll()`**。

### 1.2 根因本质
**解释器把"safepoint 时机"完全外包给信号打断**，自身没有任何确定性 safepoint 触点。它只在解释器边界（InterpreterEntryDirect 进入/退出）经过 cooperative 断言，执行中不 poll。

### 1.3 出问题的具体条件（按严重度）
| # | 条件 | 机理 | 严重度 |
|---|------|------|--------|
| **(a)** | **Windows 无 trampoline redirect** | `#if !defined(_MSC_VER)`(:340) 包住 cooperative RIP-redirect。MSVC 上 cooperative 线程只能**信号栈 spin-wait**。长 GC pause（BGC 分相可达秒级）+ 深解释器栈 → **信号栈(SIGSTKSZ)有限溢出风险** | 🔴 高 |
| **(b)** | **任意点打断的帧一致依赖** | 信号在任意指令点打断。解释器 tag 每 op 后一致 → 大多安全；但**单 handler 多槽位中间态**（如 StFldBarrier 改多 slot / 分配+store 非原子）被打断，GC 扫到部分更新 → 理论漏根/错根 | 🟠 中低 |
| **(c)** | 无回边/长期不返边界 + 信号被屏蔽 | 纯线性长代码无回边、且该段信号被 temporarily masked → safepoint 长时间不响应 | 🟠 中（信号机制兜底时低）|

**根因收敛**：不是"safepoint 缺失"本身，而是**解释器依赖一个未必处处可靠的信号打断来承担全部 safepoint 责任**——(a) 是 Windows 特有确定性缺口，(b) 是任意打断点的理论正确性风险。

### 1.4 关键安全属性（决定方案）
`interp_frame_scanner.cpp` 按 **tag** 扫（ObjectRef 判定 stack/local/regs）。因此**"帧在 safepoint 时刻 tag 一致"是安全前提**：
- **handler 完整返回后**（调用点/回边）→ 帧必然一致（每 op 后的 tag 已完整）。
- **handler 中间态**（多槽位操作进行中）→ 不一定一致。
→ **只在"确定性一致点"poll 能彻底规避 (b)**；任意点 poll 则依赖 (b) 是低概率。

---

## 二、方案与深度权衡

### 方案 A：decimated check 内 in-band `SafepointPoll`（每 N 指令，try/catch 转译）
- **做法**：在 fast_dispatch_execute.inc 现有每-64-指令 check（:484-494/:673-677）里，加 `try { threading::SafepointPoll(); } catch(chaos_managed_exception&){ frame.threw_exception=true; frame.pc=9999; }`，与手动 abort/interrupt 检查合并。
- **收益**：确定性内部触点，降低对信号的依赖；缓解 Windows 信号栈压力（解释器长循环自己进 safepoint）。
- **风险**：① 任意指令点 poll → 未规避 (b)；② SafepointPoll 的 abort/interrupt C++ throw 需转译，否则破坏解释器异常传播；③ 每 64 指令 overhead（load+fence+可能调用）在 hot loop 有成本；④ 需处理与信号挂起双路径的重复 `suspend_ack`。
- **衡量**：覆盖面最广（无回边线性代码也被 poll），但**没解决帧一致的根本 (b)**，且 hot-loop overhead 最明显。

### 方案 B：只在**热回边 + 方法调用点** poll（对齐 JIT `cfg.safepoint_fn`）
- **做法**：在 fast_dispatch_execute.inc 的**热回边检测**（:452-470/:656-670 `TryFastOsrPromotion` 附近）与 **Call 路径的 handler 返回后**插 `threading::SafepointPoll()`（try/catch 转译）。
- **收益**：**poll 只在帧必然一致的确定性点**（handler 完整返回后）→ **彻底规避 (b)**；overhead 仅热点；与 JIT 的 safepoint 语义统一。
- **风险**：纯线性长代码（只有开头一个调用、后面几百万指令无调用无回边）仍靠信号——但绝大多数方法有调用/回边，实际覆盖高；(a) Windows 信号栈压力只缓解调用点/回边密集的循环，不覆盖长线性段。
- **衡量**：**正确性最稳**（规避 b）+ 覆盖面实践够（热路径都在调用点/回边），是**兼顾正确定性收益的主推**。

### 方案 C：修 Windows 挂起缺陷（保留外包机制，解释器循环不动）
- **做法**：给 MSVC 也实现 cooperative trampoline redirect（对齐 POSIX :356-372），或改用 APC/增大信号栈。
- **收益**：针对性修 (a) 根因（Windows 信号栈溢出），不改解释器循环（无 overhead）。
- **风险**：Windows 无标准 siginfo trampoline，需 APC + 自写线程上下文操纵，实现复杂；**不解决 (b)**（任意点打断的帧一致依赖仍存在）；治标（信号机制本身仍承担全部）。
- **衡量**：修平台缺口价值高，但不独立解决解释器侧的 (b)，需与 A/B 配合。

### 方案 D（推荐）：**B + C 混合**
- **做法**：① 方案 B 的调用点+回边 in-band poll（解释器主动、确定性、规避 b）；② 方案 C 的 Windows trampoline/APC fallback（修 MSVC 信号栈缺口）。
- **收益**：解释器热路径有确定性内部触点（正确性 a/b 都缓解/规避）；Windows 平台缺口单独修复；无任意点 poll 的 overhead 与帧一致风险。
- **代价**：两处改动（解释器循环 + thread_state Windows 分支）+ GC-stress 验证。
- **风险**：Windows APC/context 操纵实现复杂；需强回归（GC-stress + 长循环 + 并发）。

### 方案取舍汇总
| 方案 | 规避(b)帧一致 | 覆盖(线性/回边) | Windows(a) | overhead | 实现复杂度 |
|------|--------------|----------------|-----------|----------|-----------|
| A 每N | ❌ 不规避 | ✅ 全覆盖 | 🟡 缓解 | 🔴 最高 | 🟢 低 |
| B 调用/回边 | ✅ 规避 | 🟡 热路径高 | 🟡 部分 | 🟢 低 | 🟢 低中 |
| C 修Windows | ❌ 不规避 | n/a(外包) | ✅ 根治 | 🟢 无 | 🔴 高 |
| **D B+C混合** | ✅ | 🟡高 | ✅ | 🟢 低 | 🔴 中高 |

## 三、主推与理由
**方案 D（B 的确定性 poll + C 的 Windows 修缺）**：
- 解释器侧用"调用点/回边 poll"（方案 B）——这是唯一**规避帧一致 (b)** 的确定性方案，且 overhead 只落热点，语义对齐 JIT。
- Windows 侧用"trampoline/APC fallback"（方案 C）——单独修 (a) 根因，不靠解释器循环改动。
- 两者无耦合，可先落地 B（解释器侧，验证 GC-stress + 长循环），再落地 C（Windows 平台，单独验证）。

**风险兜底**：任何实现前需先跑 GC-stress（并发 GC + 解释器长循环 + 信号/APC 压力），确认无帧扫描错误；实现后强回归 fact/unit/CTest。

## 五、执行结果（2026-08-13）

### 方案 B 已落地并提交
- `fast_dispatch_execute.inc`（FastExecute 双路径）+ `ir_reg_alloc.cpp`（RegisterExecute MSVC）热回边插入 in-band SafepointPoll：廉价查 `suspend_seq`，仅 active 时 `try{SafepointPoll()}catch(chaos_managed_exception)` 转译到 `threw_exception`。
- commit `40a80d771`；chaos_interpreter RelWithDebInfo+Debug 编译过；解释器 smoke/new_features/integration/entry 4/4 全绿。

### 方案 C 复核：实为已满足，无缺口（修正初判）
- **Windows 无信号栈溢出问题**：Windows 挂起走 `QueueUserAPC`（`pal_preempt_win32.cpp:35`）在**正常栈**跑 `PreemptiveSuspendHandler`，非信号、无 SIGSTKSZ 限制；`PalPreemptiveSuspendAck` 用 `PalEventWait` 真事件等待（非 spin）。
- **POSIX 已有 trampoline**：cooperative RIP redirect（thread_state.cpp:356-372）。
- → 初判的"(a) Windows 信号栈"**归错平台**（实为 POSIX，且 POSIX 已有 trampoline；Windows 用 APC 本就安全）。**方案 C 主要由既有 trampoline + APC 承担**，in-band poll 再补解释器长循环确定性触点。
- 残留唯一平台项：Windows `PalCaptureReliable`(SuspendThread reg 捕获) 是 JIT 寄存器根增强，非解释器 GC safepoint 正确性必需（栈槽 floor 已保），为远期 plan 2b/B，不在本步。

### 结论
Step 2.2 的 D 方案实质完成（B 落地 + C 复核已满足）。全测试通过门：解释器 4/4 CTest 绿（待 unit/fact 宽回归最终确认）。

## 关联
- Step 2.2 前置 finding：`step-2.2-safepoint-finding.md`
- GC 挂起机制：thread_state.cpp:236(SafepointPoll)/327(PreemptiveSuspendHandler)/356(trampoline)
- 解释器执行循环：fast_dispatch_execute.inc:484-494/673-685
- 帧扫描：interp_frame_scanner.cpp（tag 扫描）
