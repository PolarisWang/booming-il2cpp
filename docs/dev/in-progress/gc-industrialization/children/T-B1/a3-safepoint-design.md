# A3 Safepoint 设计 — CoreCLR 对齐 + 跨平台验证

> 标题：A3 深度对齐 safepoint 设计（CoreCLR Hybrid 方向）
> 作者：Chaos IL2CPP 基础设施
> 日期：2026-08-31
> 上位文档：`roadmap-v1-01.md` T-B1 / 前序研究 `a3-safepoint-research.md`
> 状态：设计定稿（跨平台已验证）

---

## 0. 摘要（一句话）

**修正后的 A3 不是「全线程物理挂起停留到 GC 完成」，而是「软协作主路径 + 硬驱赶兜底」的 Hybrid 方案——全局 trap 标志让线程自己协作排队，`SuspendThread`（Windows）/ `SIGUSR2`（Linux）仅把「久久不撤的 cooperative 线程」逼到安全点并立即放行，根扫描在 rendezvous（协作队列）中做，绝不在挂起态读寄存器。此方案跨平台可行：每层都能落地，且核心正确性在所有平台上通过「软协作 + 强制驱赶」共同达成。**

---

## 1. 关键认知修正（推翻 brainstorm 阶段预设）

| 我们 brainstorm 阶段的预设 | CoreCLR 研究结论 |
|:---|:---|
| A3 = `SuspendThread` + `GetThreadContext` 全线程物理挂起后扫描 | **CoreCLR 绝不在挂起态扫描根** — OS `GetThreadContext` 无法保证一致寄存器（Cutler 注释） |
| 硬 STW 是 safepoint 主机制 | **软协作是主路径**（`g_TrapReturningThreads` + `WaitUntilGCComplete` 事件排队）；硬 STW 只用于把「卡在 JIT 长区间」的线程拽到安全点 |
| store+barrier 靠原子指令对 | **靠 mode switch 保证** — barrier 只在 cooperative 模式触发；一旦线程 preemptive 就不再执行 barrier |
| A3 用硬 STW 替代 handshake | **在 handshake 上叠加强制收编层**，不替换主机制 |

**核心引用**（threadsuspend.cpp:3111）：
> "你可能会想简单地挂起线程并继续 GC。但历史 OS bug 证明这不可行——我们无法保证挂起态的 CONTEXT 准确。所以我们改为 redirect，改写 EIP 放行。"

---

## 2. 修正后的 A3 分层架构

```
┌─────────────────────────────────────────────────────────┐
│ CRAG 现有: per-thread handshake (suspend_seq/ack)       │ ← 软，主路径（保留）
│   - 每个 ManagedThread 有 suspend_seq/suspend_ack/event │
│   - SafepointPoll 快速路径 = 单原子读 (~1 cycle)        │
│   - 慢路径 = ack + suspend_event 阻塞等待 (零 CPU)      │
└─────────────────────────────▲───────────────────────────┘
                              │ 叠加
┌─────────────────────────────┴───────────────────────────┐
│ 新: 全局 trap 标志 + 强制收编层 (CoreCLR 对齐)          │
│   (i)  全局 g_trap_returning_threads 原子标志            │
│        + process-wide memory barrier                     │
│   (ii) 每个线程 cooperative→preemptive 切换自检 trap     │
│        命中则 WaitUntilGCComplete() 事件排队              │
│   (iii) 强制驱赶兜底:                                    │
│        - Windows: SuspendThread→GETcontext→redirect→Resume│
│        - Linux:   SIGUSR2 → trampoline → SafepointPoll   │
│        - 已有（Apple/Android 降级为纯软等待）            │
│   (iv)  m_dwForbidSuspendThread 互斥护栏 + 挂起期禁 alloc │
└──────────────────────────────────────────────────────────┘
```

### 各层核心机制

**(i) 全局 trap 标志**
- `std::atomic<uint32_t> g_trap_returning_threads`，SuspendEE 时置 1，RestartEE 时清 0
- 设置后执行 process-wide memory barrier（全核 flush store buffer）
- 全平台统一，无成本

**(ii) 协作排队（主路径）**
- 每个线程从 preemptive → cooperative 时自检 trap 标志（`DisablePreemptiveGC` 语义）
- 命中则退回 preemptive 模式，`WaitUntilGCComplete()` 事件阻塞等待 GC 结束
- 零 OS syscall，常态 0 额外开销
- **根扫描在这里做**（rendezvous），非挂起态

**(iii) 强制驱赶兜底**
- 只有「卡在纯 JIT 长代码区间、不撞 coop→preempt 切换点」的线程需要
- 挂起后做 redirect（改 EIP/返回地址），**立即 ResumeThread**
- Windows `SuspendThread` / Linux `SIGUSR2`（已有 trampoline）

**(iv) forbid-suspend 护栏**
- 挂起他人前自增自己 forbid 计数，再查目标；挂起后重读目标 forbid，非零则 resume 重试
- 防 A、B 两线程互相挂（并发 GC 双触发死锁）
- 挂起期间禁 alloc（被停线程可能持堆锁）

---

## 3. 跨平台可验证性（本文件核心结论：✅ 可行）

### 3.1 每层跨平台成本矩阵

| 层 | Windows | Linux (POSIX) | Apple/macOS/iOS | Android | 成本 |
|:---|:---:|:---:|:---:|:---:|:---:|
| 全局 trap 标志 | ✅ | ✅ | ✅ | ✅ | 0 |
| 事件排队 (PalEventWait) | ✅ | ✅ | ✅ | ✅ | 0 |
| `SuspendThread` 驱赶 | ✅ Win32 | ⚠️ 降级 SIGUSR2 | ⚠️ 降级纯软 | ⚠️ 降级纯软 | 需新 PAL |
| SIGUSR2 trampoline | — | ✅ 已有(x64/ARM64) | — | — | 0 |
| forbid-suspend 护栏 | ✅ | ✅ | ✅ | ✅ | 0 |
| 根扫描 (rendezvous 中) | ✅ | ✅ | ✅ | ✅ | 0 |

### 3.2 为何跨平台 OK（三核判断）

1. **CoreCLR 自己就这么做。** CoreCLR 在非 Windows 用 `#ifndef DISABLE_THREADSUSPEND` 裁剪 `SuspendThread`，改用 `InjectActivation`（信号注入）。CRAG 的 `PalPreemptRequest` 已做同样抽象——`pal_preempt_posix.cpp` 发 SIGUSR2，`pal_preempt_win32.cpp` 发 APC。

2. **CRAG 已有 trampoline 在 ARM64 就位。** `gc_suspend_trampoline_arm64.S` 与 `gc_suspend_trampoline_x64.S` 均存在，无需新写汇编。

3. **「不在挂起态扫描根」天然跨平台。** 根扫描在 rendezvous（协作排队）中做，不依赖 OS `GetThreadContext` 一致性——恰好绕开 Windows `SuspendThread` 缺失 / Linux 无对应 API 的问题。

### 3.3 唯一需要新增的 PAL 抽象

把 `SuspendThread` + `GetThreadContext` + `ResumeThread` 封装为：

```cpp
// chaos/pal/pal_suspend.h
bool PalSuspendThread(void* os_handle, void** out_context);   // Win32 OK; 其它返回 false
bool PalResumeThread(void* os_handle);                        // Win32 OK; 其它返回 false
bool PalGetThreadContext(void* os_handle, void* out_ctx);     // Win32 OK; 其它返回 false
```

- **Windows**: 走 Win32 API，返回 true
- **Linux**: 返回 false → 降级为 SIGUSR2 驱赶（已有）
- **Apple/Android**: 返回 false → 降级为纯软等待（已有，no-op）

**已有基础**：`thread_state.h:137` 注释已明确预留——
> `os_handle` 用于 Windows 上的 APC fallback；以及 Windows-only 的 `SuspendThread`/`ResumeThread`（当 `preemptive_suspended` 时）

### 3.4 现有 CRAG PAL 设施盘点（已就绪，无需重建）

| 设施 | 位置 | 状态 |
|:---|:---|:---:|
| `PalPreemptRequest(os_handle, os_thread_id, epoch)` | `pal_preempt.h:50` | ✅ 已有 |
| `PalPreemptGetUcontext()` | `pal_preempt.h:76` | ✅ 已有 |
| `PalGetCaptureSlot()` / `PalCaptureThreadContext()` | `pal_preempt.h:112/122` | ✅ 已有 |
| `PalCaptureReliable()` 可靠性门控 | `pal_preempt.h:108` | ✅ 已有 |
| `pal_preempt_posix.cpp` (SIGUSR2) | `pal_preempt_posix.cpp` | ✅ 已有 |
| `pal_preempt_win32.cpp` (APC) | `pal_preempt_win32.cpp` | ✅ 已有 |
| `gc_suspend_trampoline_x64.S` / `_arm64.S` | `gc/gc_suspend_trampoline_*.S` | ✅ 已有 |
| `pal_preempt_apple.cpp` / `_android.cpp` (no-op) | `pal_preempt_apple.cpp` | ✅ 已有 |

---

## 4. 对 A2b 的根本性影响

研究澄清了 A2b（store+barrier 竞态）的**正解不是原子化 store+barrier**，而是 **mode switch 保证**：

> "WriteBarrier 触发只在 cooperative mode；一旦线程被判定 preemptive，它不再执行 barrier。store+barrier 的一致性由模式切换保证，而非由 barrier 本身 LEAF/原子保证。"

**CRAG 的修正路径**：
1. 进入 barrier 前确保线程已处于 cooperative 且已完成 rendezvous 检测
2. 全局 trap 标志保证:GC 启动时，cooperative 线程被驱赶到安全点;preemptive 线程不做 barrier
3. 不再需要「把 store+barrier 做成原子」或「GcVerify 检测遗漏」

**这比联想中的「原子 barrier」更简单、更符合 CoreCLR 真实设计。** 也意味着 B3/B4/B5 的概念方向要随此调整（见 §6）。

---

## 5. Store+Barrier 原子性（A2b）最终裁决

| 旧思路（被推翻） | 新裁决（CoreCLR 对齐） |
|:---|:---|
| store+barrier 做成原子指令对 | mode switch 排除 store 出可 suspend 窗口 |
| LEAF barrier = 总线级原子化 | LEAF barrier = 编译期可证明的原子片段契约（无跨 suspend 点） |
| GcVerify 检测遗漏后重做 mark | trap 标志保证 cooperative 线程已到安全点，无遗漏 |
| `SuspendThread` 物理挂住扫描 | rendezvous 协作排队中扫栈（帧链/GcSlotMap） |

---

## 6. 对 roadmap T-B 系列其余任务的方向修正

| 任务 | 原设计方向 | 修正后方向 |
|:---|:---|:---|
| **B3 (LEAF barrier)** | 把 store+barrier 原子化 | LEAF 化保证「无跨 suspend 点」+ 绑定 mode switch（barrier 仅在 coop 触发） |
| **B4 (codegen/JIT 契约)** | store+barrier 原子窗口 | 契约改为「barrier 只在 cooperative 模式触发 + 进入 barrier 已完成 rendezvous 检测」 |
| **B5 (forbid_suspend 废弃)** | 废弃 `forbid_suspend.h` | **保留并强化**为 `m_dwForbidSuspendThread` 互斥护栏（CoreCLR 核心件） |
| **B2 (单 region 分配器)** | 独立设计 | 不受影响，继续 |

**关键**：B5 的「废弃」方向现在是**错的**——CoreCLR 恰恰把 forbid-suspend 作为核心互斥护栏。CRAG 现有 `forbid_suspend.h` 的 ack-and-continue 应**升级**而非删除。

---

## 7. 已拍板决策（resolved_decisions）

- [x] A3 采用 Hybrid（软主路径 + 硬驱赶兜底），**不**采用「全线程物理挂起停留」
- [x] 根扫描在 rendezvous 中做，**不**在挂起态读寄存器根
- [x] store+barrier 一致性由 mode switch 保证，**不**靠原子指令对
- [x] `SuspendThread` 驱赶仅 Windows，Linux 用 SIGUSR2（已有），Apple/Android 降级纯软
- [x] 新增 `PalSuspendThread/PalResumeThread/PalGetThreadContext` PAL 抽象
- [x] `forbid_suspend.h` **保留并强化**为互斥护栏，不废弃
- [x] 下列已拍板**不**解决 Watch Item 3（epoch 机制冲突）——该未决项与上述决策**并行存在**，实现前必须先统一（见结论§9 前提项 2）

## 8. Watch Items（执行中观察，非阻塞）

- [ ] Apple/Android 无 `SuspendThread` 也无比信号——纯软等待下，若某 JIT 方法长时间不撞 cooperative 切换点，GC 可能等待较久（需评估是否影响 latency 目标）
- [ ] Windows `SuspendThread` 的 `GetThreadContext` 一致性风险——即使按 CoreCLR「挂起后立即 resume + redirect」走，仍需确认「识别安全点位置」这一读不依赖 OS 保证的寄存器
- [ ] 全局 trap 标志 + process-wide barrier 的引入是否与 CRAG 现有 per-thread handshake 的 epoch 机制冲突（需设计时统一两者）

---

## 9. 结论

**修正后的 A3（Hybrid）跨平台架构方向成立（前提项见下）。** 它比 brainstorm 阶段的「硬 STW 主路径」更安全（绕开 OS 寄存器不可靠）、更符合 CoreCLR 真实架构（软主路径 + 硬兜底），且 CRAG 现有 PAL 层已具备大部分前置设施（trampoline、`PalPreemptRequest`、`PalCaptureReliable` 可靠性门控）。唯一新增是 `pal_suspend.h` 三个接口 + Windows 实现。

**⚠️ 可行性结论的未验证前提（接续者必须保留，勿据此直接拉闸定稿）：**
1. **Apple/Android latency**：两平台退化为纯软等待无硬驱赶兜底；若某 JIT 方法长时间不撞 cooperative 切换点，GC 等待可能超 target latency（Watch Item 1）。此点未纳入「可行」结论，需在实现期评估——跨平台「✅可行」是**架构方向**结论，非**延迟满足**结论。
2. **epoch 机制冲突**：全局 trap 标志 + process-wide barrier 与 CRAG 现有 per-thread handshake 的 epoch 机制是否冲突，仍是 Watch Item 3 的未决项——**尚未随本设计决策解决**，实现 handshake 叠加层时必须先统一两者（见 §8）。

下游 B3/B4/B5 需按 §6 修正方向，B2 不受影响。
