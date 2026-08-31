# CoreCLR Workstation GC 硬 STW (Hard Suspension) Safepoint 机制研究

> 标题：CoreCLR WKS 硬 STW safepoint 机制研究
> 作者：Chaos IL2CPP 基础设施研究
> 日期：2026-08-31
> 用途：CRAG GC A3 深度对齐的前置研究 —— 理解 dotnet/runtime 如何用 `SuspendThread + GetThreadContext` 作为 GC safepoint 主机制，以及它与 per-thread handshake 软协调的真实关系。
> 源码来源：`dotnet/runtime` main 分支，`src/coreclr/vm/threadsuspend.cpp`（6254 行）、`threadsuspend.h`、`threads.h`、`fcall.h`、`gchelpers.h`、`gcheaputilities.h`。标识符（函数/常量/字段名）均取自上述文件原文，行号为近似值（精确度见下行声明）。
>
> **行号精度声明**：本研究中引用的行号为**近似参考**（标记 `~`，如 `~3370`/`~230`/`:~3000`），用于指示代码区域而非逐一精确核验的锚点。标识符（函数/常量/字段名）取自原文，但行号可能随 dotnet/runtime 版本漂移 ±数百行。接续者如需精确位置，应以当前 dotnet/runtime checkout 重新定位，勿以本处近似行号作为权威依据。

---

## 1. 核心机制概览（一句话）

> **CoreCLR 的 GC 暂停不是"先把所有线程物理挂起"再扫描，而是"持锁 + 置 GcInProgress + 用软/硬混合手段把每个还在 cooperative 模式的线程驱赶到一个 GC 事件上排队等 GC 结束"，其中硬 STW（`SuspendThread`+`GetThreadContext`）只是把线程拖到"安全点"然后**立即放行让其 rendezvous（汇合）**的手段，不是让线程在物理挂起状态里被扫描。**

关键结论（先给出，全文展开）：
- **硬 STW 不是主扫描路径。** 线程绝不在 `SuspendThread` 挂起状态下被扫描根。物理挂起只用于**瞬间检查/改写线程上下文**（redirect/hijack），随即 `ResumeThread` 放行。
- **真正的"停止"是软性的：cooperative 线程在 `RareDisablePreemptiveGC()` 里被要求"被困（trapped）"，切到 preemptive 模式并 `WaitUntilGCComplete()` 阻塞等待 GC 结束事件**。
- 所以 CoreCLR 是**混合**：hard STW = 加速/强制驱赶；soft rendezvous = 实际排队等待。对 CRAG 的启示：**"硬 STW + 在挂起上下文里直接扫描"是 CoreCLR 刻意不去做（甚至记载 OS bug 无法可靠做）的方案**，A3 若照抄"全线程物理挂起后扫描"会踩进 CoreCLR 已证伪的坑。

---

## 2. per-thread handshake 相对硬 STW 的架构对比表

| 维度 | CoreCLR 硬 STW（`Thread::SuspendThread`） | CoreCLR 软协调 handshake / cooperative rendezvous |
|---|---|---|
| **触发** | `ThreadSuspend::SuspendAllThreads` → 对**还没离开 cooperative 的线程**逐个调 `Thread::Hijack()` | `RareDisablePreemptiveGC()`：线程每次从 preemptive→cooperative 时自检 `ThreadStore::IsTrappingThreadsForSuspension()` |
| **谁停下** | GC 发起线程（持 ThreadStore 锁者）停**别的**线程 | 每个 managed 线程**自己**切到 preemptive 并堵在 `WaitUntilGCComplete()` |
| **扫描根时线程状态** | **不扫描**（挂起仅用于读寄存器/改写 EIP，随即 resume） | 线程在 preemptive 模式下，由 GC 侧基于 `m_pFrame` 链做栈遍历（`StackWalkFrames`） |
| **核心同步原语** | `SuspendThread`/`ResumeThread` + `GetThreadContext` | ThreadStore 锁 + `GCHeapUtilities::GetGCHeap()->ResetWaitForGCEvent()`/`SetEvent` 事件 + `g_TrapReturningThreads` 全局标志 |
| **等待方式** | `EnsureThreadIsSuspended` 内嵌 loop / `minipal_microdelay` 轮询 | `WaitUntilGCComplete()` 事件阻塞（事件驱动，不轮询） |
| **成本特性** | 每次 GC 都 syscall 级挂起/恢复，昂贵；只对"还没自己撤的线程"用 | 常态 0 额外 syscall：线程在 cooperative 模式进入点处一次普通内存读判 `g_TrapReturningThreads` |
| **失败/死锁风险面** | 线程挂起瞬间可能正持其他锁 → 需 `m_dwForbidSuspendThread` 互斥护栏 | 只需保证 `g_TrapReturningThreads` 与 `m_fPreemptiveGCDisabled` 写入顺序可见（配 process-wide barrier） |
| **平台可用性** | **非 Windows 被裁剪**：`#ifndef DISABLE_THREADSUSPEND` 包裹，注释明言 "On non-Windows CORECLR platforms remove Thread::SuspendThread support" | 全平台（Unix 用 `InjectActivation` 替代 hard suspend 做驱赶） |
| **角色定位** | SuspendEE 的**快路径外救援**：硬驱赶拖到安全点（redirect/hijack） | **主路径**：常态 managed 线程自己 rendezvous 排队 |

> **一句话**：CoreCLR 的"暂停"是**事件驱动的协作排队**，hard `SuspendThread` 只是对"没来得及自己撤的线程"的**兜底强制**——因为 managed code（JIT'd code）**不做主动 poll**，GC 发起方不能干等它自己撞安全点，于是用挂起+redirect 把它**逼**到安全点放行。

---

## 3. 硬 STW 的精确流程（SuspendThread → GetThreadContext → scan → RestartThread）

以下流程是 `Thread::SuspendThread` + `Thread::Hijack` + `ThreadSuspend::SuspendEE/RestartEE` 的组合，来自 `threadsuspend.cpp`。注意：**没有"挂起态扫描"这一步——scan 发生在放行后的 rendezvous 里**。

### 3.0 宏观编排（SuspendEE，threadsuspend.cpp:5515）

```text
ThreadSuspend::SuspendEE(SUSPEND_REASON reason)
  ├─ ThreadSuspend::LockThreadStore(reason)     // 持 ThreadStore 锁，直到 RestartEE
  ├─ m_suspendReason = reason
  ├─ GCHeapUtilities::GetGCHeap()->SetGCInProgress(true)   // 全局"GC 进行中"
  ├─ (提升优先级、profiler 钩子…)
  └─ SuspendAllThreads()        // 核心：驱赶所有线程到安全点并排队
```

### 3.1 SuspendAllThreads（threadsuspend.cpp:3215）—— 主循环

```text
ThreadSuspend::SuspendAllThreads():
  g_pSuspensionThread = 当前线程
  GCHeapUtilities::GetGCHeap()->ResetWaitForGCEvent()   // 重置全局 GC 完成事件
  ThreadStore::SetThreadTrapForSuspension()             // 置 g_TrapReturningThreads=true
  minipal_memory_barrier_process_wide()                 // 全核 flush store buffer，保证：
                                                          //  (a) m_fPreemptiveGCDisabled 判读可靠
                                                          //  (b) 其它线程看到 trap 标志

  while (true):
    remaining = 0
    for 每个 pTargetThread ∈ ThreadStore::GetThreadList():
      if pTargetThread == 当前线程: continue
      if pTargetThread.m_fPreemptiveGCDisabled 仍为 1:     // 还在 cooperative
        remaining++
        if !observeOnly: pTargetThread->Hijack()            // 硬驱赶
    if remaining == 0: break
    # 未见进展(delta 变小) → 观察 5us；无进展 → microdelay 并按 2 倍递增重挂起(上限100us)
    SwitchToThread()  if 等待>1ms                      # 让被停线程有机会跑
  # ARM/ARM64 再 process-wide barrier（弱内存序同步 preemptive 线程的写）
```

**要点**：循环只关心 `m_fPreemptiveGCDisabled`（cooperative 标志）。一旦某线程已被 `Hijack()` 成功（redirect/hijack 落定），它会被 `SetThreadState(TS_GCSuspendRedirected)` 标记，下一轮 `Hijack()` 见标跳过。**循环退出 ≠ 线程被物理挂起**，而是"所有线程要么 preemptive、要么已被驱赶到 rendezvous（`WaitUntilGCComplete`）"。

### 3.2 Thread::Hijack（threadsuspend.cpp:~3370）—— 单线程硬驱赶

```text
Thread::Hijack():
  if IsGCSpecial(): return                 // GC/GC-helper 线程不可被强停
  if !UseContextBasedThreadRedirection():
      # Unix/无 CONTEXT 改写能力平台：用信号/APC 注入激活(InjectActivation, SuspendForGC)
      InjectActivation(SuspendForGC); return
  # ---- Windows 硬 STW 主路径 (FEATURE_HIJACK && !TARGET_UNIX) ----
  if 已 TS_GCSuspendRedirected: return     // 重挂无意义，它已会排队
  ThreadStore::AllocateOSContext()          // 挂起前预分配 OS CONTEXT，避免挂起后 alloc 死锁
  str = SuspendThread(fOneTryOnly=TRUE)     # 3.3
  switch str:
    STR_Success -> 继续
    STR_Forbidden/STR_NoStressLog/STR_UnstartedOrDead/STR_Failure -> return   # 放弃该线程
  if !m_fPreemptiveGCDisabled.LoadWithoutBarrier():
      ResumeThread(); return                 # 挂起期间它自己偷偷撤到 preemptive => 已在安全点，放行
  # 现在还确定在 cooperative 且停在 JIT'd code 里：
  WorkingOnThreadContextHolder;              # 保护 setcontext 不被并发改
  if HandledJITCase():                       # 判断 IP 是否在 fully-interruptible 安全点
      if !CheckForAndDoRedirectForGC():
          # GetThreadContext 捕获的 CONTEXT 因 OS bug 不可靠 => 放弃 redirect（统计 cntFailedRedirections）
      else:
          SetThreadState(TS_GCSuspendRedirected)
  ResumeThread()                             # ★ 立即放行！不在此状态下扫描
```

**为什么挂起后不直接扫描**（threadsuspend.cpp:3111 注释）：
> "If the thread is already at a safe point, you might think we could simply leave it suspended and proceed with the GC. ... However, various historical OS bugs prevent this from working. The problem is that we are not guaranteed to capture an accurate CONTEXT (register state) for a suspended thread. So instead, we `redirect` the thread, by overwriting its instruction pointer."

即：**OS `GetThreadContext` 无法保证给出一致/准确的寄存器窗口**（Cutler：SuspendThread 只是插一个 APC 块 + 请求 IPI，不等线程真正停下；GetContext 也只在"返回时代理已送达事件"意义上可靠）。因此 CoreCLR 选择**绝不在物理挂起态扫描根**，宁可把 EIP 改写指向 redirect stub 放行，让线程自己走到注册窗口干净的 rendezvous。

### 3.3 Thread::SuspendThread（threadsuspend.cpp:~230）—— 单线程 OS 挂起

```text
SuspendThread():
  取句柄 hThread  (INVALID -> STR_UnstartedOrDead)
  # 互斥挂起护栏：先自增自身 m_dwForbidSuspendThread，再查目标
  ForbidSuspendThreadHolder forbidSuspend      # 自增
  InterlockedOr(&m_dwForbidSuspendThread,0)    # 查目标，配合 full fence 建全序，防两线程互相挂
      if 非0 -> retry(goto)
  dwSuspendCount = ClrSuspendThread(hThread)   # OS SuspendThread（APC+IPI，异步）
  if dwSuspendCount >= 0:
      EnsureThreadIsSuspended(hThread, this)   # = ::GetThreadContext(hThread,&ctx) 等真正停
      # 挂起后重读目标 m_dwForbidSuspendThread；若挂了它正 forbid suspend => Resume + retry
      if 目标 forbidSuspend != 0: ClrResumeThread(hThread); retry
      IncCantAllocCount()                      # 挂起期间禁止 alloc（被停线程可能持堆锁）
      m_ThreadHandleForResume = hThread
      return STR_Success
```

**`EnsureThreadIsSuspended`（threadsuspend.cpp:108）实际上就是一次 `::GetThreadContext`**：
```cpp
BOOL EnsureThreadIsSuspended (HANDLE hThread, Thread* pThread)
{
    CONTEXT ctx;  ctx.ContextFlags = CONTEXT_INTEGER;
    return ::GetThreadContext(hThread, &ctx);
}
```
其语义（Cutler 注释）：*"Get context delivers a APC to the target thread and waits on an event that will be set when the target thread has delivered its context."* —— 用它**当 fence**，确认线程已真正停在一个可读取状态。

### 3.4 RestartEE / ResumeAllThreads（threadsuspend.cpp:5375）—— GC 后恢复

```text
ThreadSuspend::RestartEE(SuspendSucceeded):
  s_fSuspended = false
  SyncClean::CleanUp()                          # 无人在跑 managed code，回收安全
  GCHeapUtilities::GetGCHeap()->SetGCInProgress(false)
  ResumeAllThreads(SuspendSucceeded)            # 对每个线程 PrepareForEERestart：unhijack + 清挂起标志
  ThreadSuspend::UnlockThreadStore()            # 释放 TSL，放行所有线程
```

---

## 4. cooperative/preemptive 与物理挂起的交集

### 4.1 两个模式的物理定义（threads.h:40-57, threads.h:881）

```cpp
Volatile<ULONG> m_fPreemptiveGCDisabled;   // 0 => preemptive；非0 => cooperative
```
- **Cooperative 模式**（`m_fPreemptiveGCDisabled` 非 0）：线程**可能正在改 GC 引用**，运行时必须配合它到达 GC-safe 位置才能枚举引用。**JIT'd code 里几乎永远在此模式**（除 inline P/Invoke 过渡）。
- **Preemptive 模式**（`m_fPreemptiveGCDisabled == 0`）：线程保证 **(a)** 不运行操作 GC 引用的代码，**(b)** 已将 `m_pFrame` 指针设为一个 `Frame` 子类，标记栈上最后一个 managed 方法帧位置（GC 从这里开始的栈区间做扫描入口）。

### 4.2 进入/离开 cooperative（threads.h:1228-1273）

- `DisablePreemptiveGC()`（coop 进入，**非嵌套**）：先 `m_fPreemptiveGCDisabled = 1`，**再**查 `g_TrapReturningThreads`；为真则走 `RareDisablePreemptiveGC()`。
- `EnablePreemptiveGC()`（coop 离开）：`m_fPreemptiveGCDisabled = 0`。无 volatile 读之外的同步。
- **顺序关键**：`DisablePreemptiveGC` 是"先标 coop、后查 trap"——注释明确说明先做检查会开竞态：
  > "我们只是想在边界上检查 GC 是否进行中。但如果挂在检查之前做，会开一个竞态：GC 在检查之后启动，而 SuspendRuntime 因为看到该线程在 EE 外而忽略它，于是线程在 GC 期间乱跑。"

### 4.3 挂起与模式交集（`RareDisablePreemptiveGC`，threadsuspend.cpp:2052）

线程尝试从 preemptive 进入 cooperative 时，若发现 `ThreadStore::IsTrappingThreadsForSuspension()`（GC 正 suspend）：

```text
RareDisablePreemptiveGC():
  while true:
    if 持 ThreadStore 锁: return                   # GC 发起者自身不受困
    # debug	suspend 优先级、stack crawl…
    if ThreadStore::IsTrappingThreadsForSuspension():
        SetThreadState(TS_SuspensionTrapped)       # 标记"被困"
        EnablePreemptiveGC()                       # ★ 退回到 preemptive（保证 GF 引用不进栈）
        GCHeapUtilities::GetGCHeap()->WaitUntilGCComplete()   # 事件阻塞，等 GC 结束
        m_fPreemptiveGCDisabled = 1                # 重新进入 coop
        ResetThreadState(TS_SuspensionTrapped)
        continue                                    # 出 loop 前再查一次（可能又有新 GC）
    break
```

**这里的物理挂起交集**：`SuspendAllThreads` 在**另一个线程**上观察 `m_fPreemptiveGCDisabled` 来决定要不要 `Hijack()`。若某线程恰好正处于 `RareDisablePreemptiveGC` 的空窗（已标 coop 但还没撤），`Hijack()` 会把它挂起、读到 `m_fPreemptiveGCDisabled` 仍为 1、然后 redirect 它。redirect 放行后它继续执行 `RareDisablePreemptiveGC`，最终撞到 `WaitUntilGCComplete()`。**所以物理挂起只解决"谁先到 rendezvous"的竞争，不解决"谁来阻止进栈"——那由 cooperative 协议保证**。

### 4.4 GCX_COOP / GCX_PREEMP 与模式切换宏

`threadsuspend.cpp` 中 `GCX_COOP()` / `GCX_COOP_NO_DTOR()` / `GCX_PREEMP_NO_DTOR()` 等 RAII：进/出一个临时模式区，函数回来时用 holder 恢复原模式。`RedirectedHandledJITCase` 内部用 `GCX_PREEMP_NO_DTOR()` 在 redirect 例程里退到 preemptive 再进入 `WaitUntilGCComplete`。

---

## 5. LEAF barrier 的定位（它在 store+barrier 原子性中扮演什么角色）

### 5.1 弄清概念：CoreCLR 的 "LEAF" / "GCsafe" 指什么

在 CoreCLR 语境里，**"LEAF 保证"（GCSafeLevel）不是写屏障**，而是**托管/VM 函数注解系统**——它约束函数体在 **safepoint 竞态一致性**上的义务。对应机制有若干形态：

1. **`FCALL` 的 GC 触发约束（fcall.h:1-12）**
   > "Causing GC or EH in an FCALL is illegal. QCalls should be used instead. Compile time errors occur if you try to violate either of these rules."
   `FCIMPL*` 宏的 prologue 在合同开启（`ENABLE_CONTRACTS`）时注入 `ForbidGC __fCallCheck(...)`——**禁止 FCall 内部触发 GC**。这样 GC 侧**能安全假设 FCall 执行的指令区间不会进入一个可能被 suspend 判定的安全点**，从而保证"FCall 正在执行时，不会在它内部发生 GC 参考竞态"。

2. **contract 的 `GC_TRIGGERS` / `GC_NOTRIGGER` / `LEAF` 注解**
   - `GC_TRIGGERS`：函数可能触发 GC（会切 preemptive / 分配 / 等待事件）。
   - `GC_NOTRIGGER`：函数**不得**触发 GC（禁用对手持引用区域切换模式）。
   - `LEAF` / `LIMITED_METHOD_CONTRACT`：函数**不调用任何其它函数**（叶函数），隐含"无分配、无 GC 点、无异常"。`EnsureThreadIsSuspended`、`GetThreadContext` 包装、`ResumeAllThreads` 等标 `LEAF`/`GC_NOTRIGGER` 的，是**在物理挂起他人期间可安全执行的代码**的证明——挂起方此刻不可 alloc/不可触发 GC/不可碰被停线程持有的锁。

3. **`LEAF` 与 safepoint poll 的关系（对本主题的关键）**：因为**FEATURE_SPECIAL_USER_MODE_APC / GCPOLL** 等的写屏障/GC 检查是**在"所有被挂起线程已 rendezvous"之后**才安全且仅由**运行中的线程**执行。一个**标为 LEAF 的写屏障函数**（例如内联的 `ErectWriteBarrier` 变体）保证其**指令序列极短且不含可被挂起判定的安全点**，因此当 GC 正 suspend 时，别的线程在这几纳秒里执行 store+barrier 不会被中途截断成"store 已做但 barrier 未做"的撕裂状态。

### 5.2 在"store+barrier 原子性"中的确切角色（针对 CRAG 关心的问题）

**直接回答"LEAF barrier 扮演什么角色"**：CoreCLR 的裸 `store → write barrier` 两指令**不是原子对**——一个被 suspend 的线程完全可以卡在"store 落子、barrier 未发"之间。但 CoreCLR **不需要**让这段原子：
- 因为 GC 启动时线程要么已在 cooperative mode（此时它尚未开始一次新的 store），要么被驱赶到 preemptive mode（WriteBarrier 不会在 preemptive 下执行）。
- **WriteBarrier 触发只在 cooperative mode**；一旦线程被判定 preemptive，它不再执行 barrier。
- 所以"store+barrier"的一致性**由模式切换（mode switch）保证，而非由 barrier 本身 LEAF/原子保证**。LEAF 化的 barrier 仅取性能 + 可证明"不触发二次 GC / 不在挂起他人期间可执行"。

**结论：LEAF barrier 是"可证明安全的原子代码片段"的编译期契约，不是"总线级原子化"的手段。** 对 CRAG：若 A3 想照 CoreCLR 复制这条，应复制"**进入 barrier 前必须已处于 coop 且已完成 rendezvous 检测**"，而非尝试把 store+barrier 做成原子指令。

### 5.3 CoreCLR 的写屏障全景（来自 gchelpers.h / GcData）

CoreCLR 通过 `StompWriteBarrier*` 在**运行时已完成 GC suspend 后**（`isRuntimeSuspended` 参数）才 stomp 切换写屏障实现（ephemeral/size/resize/watch 变体）。这同样说明：**写屏障实现的切换 / 触发严格与 UE 挂起状态绑定**，barrier 本身不做状态检查——状态检查由调用它的模式保证。

---

## 6. 硬 STW 的捕获：对线程栈、寄存器窗口（CONTEXT）如何用于 GC root 枚举

### 6.1 CONTEXT 捕获确实存在，但用途是"定位安全点 / 改写 EIP"，不是"扫描根"

Three 处捕获点：
1. `EnsureThreadIsSuspended`（GetThreadContext 当 fence）—— 确认线程真正停住。
2. `Thread::HandledJITCase()` —— 拿 `CONTEXT` 里的 IP 判断是否在 **fully interruptible 的指令处（polling 安全点）**。若是，`CheckForAndDoRedirectForGC()` 用 `SetThreadContext` **改写 IP 指向 `RedirectedHandledJITCaseForGCThreadControl_Stub`**，然后 `ResumeThread`。线程 resume 后执行 redirect stub → `RedirectedHandledJITCase(RedirectReason_GCSuspension)` → `GCX_PREEMP` 退 preemptive → `WaitUntilGCComplete()`。
3. `EEGetThreadContext`（threadsuspend.cpp:526）：包装 `pThread->GetThreadContext(pContext)`，断言 `CheckSuspended(pThread)`——**只在线程确已被 OS 挂起时**允许读，用于调试 / hijack 前的检查，**不用于根扫描**。

### 6.2 根扫描真正的入口：`StackWalkFrames`（栈上 m_pFrame 链）

GC 扫描线程栈根用的是 `Thread::StackWalkFrames` / `StackWalkFramesEx`（threadsuspend.cpp:840/1013），基于 `m_pFrame`（cooperative 协议在 preemptive 进入点设置的 Frame 链）从注册窗口向下走。**这要求线程处于 preemptive 且已稳定**——所以扫描发生在 rendezvous（`WaitUntilGCComplete` 前的 preemptive 状态）里，**而非 `SuspendThread` 挂起状态**。这是整个设计区别于"全线程物理挂起扫描"的根本点。

> **对 CRAG 的提示**：CoreCLR 也**会**在挂起态读寄存器（debugger、hijack 判定），但生产 GC 根枚举刻意避开挂起态寄存器窗口，因为 OS 无法保证其一致性（Cutler/Clift 引文）。CRAG 若主路径采用"挂起+直接扫寄存器根"，必须自行解决与 OS `GetThreadContext` 一致性的博弈；CoreCLR 的选择是**宁可在安全点放行后走软件栈遍历**。

---

## 7. 与 per-thread handshake 的关系（厘清"何时用哪个"）

### 7.1 顶层答案：CoreCLR 是混合，hard STW 只是兜底/驱赶，不是主路径

`threadsuspend.h` 的 `SuspendStatistics` 字段集印证分层：
```
MinMaxTot osSuspend, crawl, wait;          # OS 级挂起 vs 栈遍历请求 vs 等完成
cntOSSuspendResume; cntHijackCrawl; cntHijackTrap;
cntRedirections; cntFailedRedirections; cntFailedSuspends; cntWaitTimeouts;
```
- `osSuspend`（OS 硬挂起）是**一条独立可计量的路径**，与 `crawl`（逻辑栈遍历）并列。正常场景 `cntOSSuspendResume` 小 —— 大多数线程在 `g_TrapReturningThreads` 置位后**自己**在 `RareDisablePreemptiveGC` 排队，不需要 OS 挂起。
- **`osSuspend` 触发条件**：某个 cooperative 线程**卡在比较长的纯 JIT'd 代码区间里**（不做模式切换、不撞安全点），GC 发起方不能无限等它可以自撤——于是用 `SuspendThread` 强制截停并 redirect。这就是"hard STW 用于无法用软 handshake 及时收编的线程"。

### 7.2 何时用 handshake / 何时用 hard STW

| 情形 | 采用机制 |
|---|---|
| 线程此刻正在 preemptive（或已标 coop 正走 `RareDisablePreemptiveGC`） | **软 handshake**：它自己撞 `WaitUntilGCComplete` 排队，零 OS syscall |
| 线程在 cooperative 且停在 fully-interruptible JIT 安全点 | **hard STW 快路径**：suspend→判安全点→redirect→resume |
| 线程在 cooperative 且停在 non-interruptible（return 地址处） | **hard hijack**：suspend→改写 return address→resume，方法返回时兜入 hijack→rendezvous |
| 线程正持 `m_dwForbidSuspendThread`（持某些锁） | **放弃本次挂起**（`STR_Forbidden`），靠它自己撤 |
| 非 Windows / 无 CONTEXT 改写能力 | **`InjectActivation`**（信号/APC）替代 hard suspend 做同一驱赶 |

### 7.3 结论：硬 STW 是否真主路径？

**不是。** 它就是"把不合作线程逼到 rendezvous"的最后一个手段，并且逼到就放行。真正让所有线程"停对位置"的，是 **ThreadStore 全局锁 + `g_TrapReturningThreads` 标志 + 每个线程自己的 cooperative→preemptive 模式转换 + 全局 GC 完成事件** 这套软协调。**"让全部线程物理挂起并停留到 GC 完成"在整个 CoreCLR 里都不存在。**

---

## 8. 对 CRAG A3 设计的启示（哪些可直接复制，哪些因 CRAG 无 SVR/handshake 结构需改制）

CRAG 当前 safepoint 为 per-thread handshake 软协调（`suspend_seq/suspend_ack/suspend_event`，见 MEMORY：gc 域）。A3 想用硬 STW 替代它作主机制。对照 CoreCLR：

### 8.1 可直接复制 / 被 CoreCLR 证实的做法（高置信）

1. **hold-lock + 置全局"trap/suspend"标志 + process-wide memory barrier**，再开始逐个收编线程，最后等"剩余=0"。`SuspendAllThreads` 的 `while(remaining>0){ … ; barrier microdelay … }` 结构可直接采纳。
2. **`m_dwForbidSuspendThread` 互斥护栏**：挂起他人前先自增自己的 forbid 计数，再查目标；**挂起后重读目标 forbid**，非零则 resume 重试。防止 A、B 两线程互相挂。**这条必须有**，否则并发 GC 双触发/多 GC 线程互挂死锁。
3. **挂起前预分配 OSCONTEXT**（`ThreadStore::AllocateOSContext()`），挂起期间置 `IncCantAllocCount()`——因为被停线程可能正持分配器/堆锁。**CRAG 若在挂起他人时 alloc，存在隐藏死锁**。
4. **挂起后不直接扫寄存器根**，而是**把寄存器捕获定位"安全点位置"**，用改 IP / 改返回地址把它驱赶到一个干净事件（`WaitUntilGCComplete`）上排队。CRAG 若保留精确扫描，应照此在"安全点 rendezvous"里扫，而非挂起态直接扫。这同时绕开 OS `GetThreadContext` 一致性问题（Cutler 引文）。
5. **WriteBarrier stomp / 实现切换只在"已完成 suspend"后做**（`isRuntimeSuspended`）：切换写屏障与挂起状态绑定，barrier 本身不带状态检查。

### 8.2 必须改制的部分（CRAG 缺 CoreCLR 结构处）

1. **CRAG 无 `m_pFrame` 链 / `StackWalkFrames` 托管栈遍历基础设施**（CRAG 是 codegen 原生翻译，栈遍历依赖 `Chaos.TestFramework`/JIT root map）。CoreCLR 软 rendezvous 的根扫描依赖 `m_pFrame` 软件栈；CRAG 若走"软 rendezvous 后扫栈"需**先有等价的帧链或依赖 JIT GC-slot-map**，否则退到保守扫描。
2. **CRAG 无 ThreadStore 锁 + 全局事件驱动的 `WaitUntilGCComplete` 双层结构**。A3 若引入物理挂起态停留扫描，须自建"全局 GC 完成事件"，否则每线程只能 spin 等 GC 结束（CoreCLR 明确不用 spin）。
3. **"硬 STW 全挂起并停留到 GC 完成"是 CoreCLR 刻意回避的**（OS CONTEXT 不可靠 + 被停线程持锁风险）。CRAG 若坚持此路线，等于走 CoreCLR 证伪路径，需额外解决：挂起态 root 枚举的寄存器一致性问题 + 被停线程持锁导致的挂起死锁面（需 forbid-suspend 全漏数），否则 A3 会引入比 handshake 更高的死锁/误扫风险。
4. **平台裁剪**：CoreCLR 把 `SuspendThread` 在非 Windows 裁掉（`DISABLE_THREADSUSPEND`），Unix 用 `InjectActivation`。CRAG A3 若主打跨平台（含 Unix），硬 STW 主路径在 Unix 上要么不可用、要么改用信号注入，架构上应预留 PAL 抽象而不是硬绑 Win32 `SuspendThread`。

### 8.3 建议的 A3 取向（一句话推荐）

**不要用"全线程物理挂起停留"替代 handshake，而是把硬 STW 定位为 handshake 的强制收编兜底**，拷贝 CoreCLR 三层：(i) 全局 trap 标志 + 事件排队（软，主路径）、(ii) `SuspendThread`+`GetThreadContext` 仅用于把"久久不撤的 cooperative 线程"拽到安全点并放行、(iii) `m_dwForbidSuspendThread` 互斥护栏 + 挂起期禁 alloc。这样既拿到硬 STW 的确定性/低延迟，又不复制 CoreCLR 已证伪的"挂起态扫描寄存器根"坑，且天然兼容 CRAG 当前 handshake 结构（只需在其上叠加强制收编层）。

---

## 附：关键源码定位索引（dotnet/runtime main）

| 机制 | 文件:符号 |
|---|---|
| 全局编排 `SuspendEE` | `src/coreclr/vm/threadsuspend.cpp:5515` |
| 主循环 `SuspendAllThreads` | `src/coreclr/vm/threadsuspend.cpp:3215` |
| 单线程硬驱赶 `Thread::Hijack` | `src/coreclr/vm/threadsuspend.cpp:~3370` |
| OS 挂起 `Thread::SuspendThread` | `src/coreclr/vm/threadsuspend.cpp:~230` |
| 挂起 fence `EnsureThreadIsSuspended` | `src/coreclr/vm/threadsuspend.cpp:108` |
| 恢复 `RestartEE` / `ResumeAllThreads` | `src/coreclr/vm/threadsuspend.cpp:5375` |
| cooperative 进入自检 `RareDisablePreemptiveGC` | `src/coreclr/vm/threadsuspend.cpp:2052` |
| 模式字段 `m_fPreemptiveGCDisabled` / 宏 | `src/coreclr/vm/threads.h:881,1228,1273` |
| 重定向例程 `RedirectedHandledJITCase` | `src/coreclr/vm/threadsuspend.cpp:2617` |
| 根扫描入口 `StackWalkFrames` | `src/coreclr/vm/threadsuspend.cpp:840,1013` |
| FCall GC 约束 / ForbidGC | `src/coreclr/vm/fcall.h:1-12, FC_COMMON_PROLOG` |
| 写屏障 stomp（仅 suspend 后） | `src/coreclr/vm/gchelpers.h` |
| 全局堆 / GcInProgress | `src/coreclr/vm/gcheaputilities.h`, `gcruntimecontext` 逻辑在 `threadsuspend.cpp` `SetGCInProgress` |
| Suspend 统计（osSuspend/hijack/crawl 分层） | `src/coreclr/vm/threadsuspend.h: SuspendStatistics` |
