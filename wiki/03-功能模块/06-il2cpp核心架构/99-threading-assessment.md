# Threading 实现综合评估报告

> 基于项目 threading 源码全面 survey 与 CoreCLR 横向对比，覆盖 managed thread、thread pool、GC safepoint、monitor/synchronization、execution context、timer、async/task、GC worker pool 八大模块。
>
> 日期：2026-05-22 | 范围：`src/native/runtime-core/` + `runtime_stubs/` + `src/native/common/chaos/`

---

## 总览：实现完整性矩阵

| 模块 | 实现状态 | CoreCLR 对齐度 | 差距等级 | 风险评估 |
|------|----------|---------------|---------|---------|
| Managed Thread 生命周期 | ~85% | 中 | L2 | 缺少 thread pool 集成、abort 未完整 |
| Thread Pool | ~70% | 中-低 | L2 | HillClimbing 简化版、缺少 CPU/histogram 反馈 |
| GC Safepoint Protocol | ~75% | 中 | L1 | 基础结构完整，预抢占和超时恢复有差距 |
| Monitor / SyncBlock | ~65% | 中 | L2 | 无唤醒风暴抑制、不公平竞争、lazy initialization 缺失 |
| ReaderWriterLock / Barrier | ~60% | 中 | L2 | 基于 std 低配封装，无 CoreCLR 级优化 |
| WaitHandle (MRE/ARE) | ~80% | 中 | L1 | 功能完整但缺少 named mutex 等 |
| ExecutionContext / AsyncLocal | ~50% | 低 | L3 | 仅支持 4 slot 内联，无完整 capture/restore |
| Timer Queue | ~60% | 低 | L3 | gate thread 500ms tick，精度受限 |
| TaskRunner (Task.Run) | ~70% | 中 | L2 | 基本 Task.Run 可用，缺少 await 状态机集成 |
| GC Worker Pool | ~80% | 高 | L1 | 专用 worker，创建后不销毁 |
| ThreadPool ETW | ~40% | 低 | L3 | 仅 Windows，事件种类很少 |

**综合评估**：整体 threading 实现处于 **functional but not production-ready** 状态。核心骨架完整（thread lifecycle、safepoint、thread pool、monitor），但在精细控制、性能优化、诊断能力上与 CoreCLR 有明显差距。生产环境压力下可能在 thread pool 波动、monitor 竞争、safepoint 超时、ExecutionContext 丢失四个方向出现问题。

---

## 1. Managed Thread — 生命周期管理

### 当前项目实现

- **ManagedThread** (`thread_state.h:34-142`): 完整结构体，包含 TLS identity (thread_local ManagedThread*)、OS thread handle、TLAB backup、safepoint 协议字段
- **线程创建** (`thread_state.cpp:94-145`): `CreateManagedThread()` — 分配 ManagedThread、分配线程 ID、创建 suspend_event (Windows auto-reset / POSIX condvar)
- **Attach/Detach** (`thread_state.cpp:147-258`): `AttachManagedThread()` / `DetachManagedThread()` — 注册到 `s_thread_registry` (lock-free linked list)、TLS 读写
- **ABI Stubs** (`threading_stubs.cpp`): `chaos_thread_ctor` / `_start` / `_join` / `_interrupt` / `_abort` / `_yield` / `_sleep` — OS 抽象层
- **最大线程数控制**: `__thread_max_ = 2047`
- **线程注册表**: lock-free singly linked list (`s_thread_registry`)，支持稳定遍历用于 safepoint

### CoreCLR 对比

| 维度 | 项目当前 | CoreCLR | 差距 |
|------|---------|---------|------|
| TLS 访问 | `thread_local ManagedThread*` | `ThreadStore::GetCurrentThread()` + `ThreadLocal` | 等价 |
| 线程注册表 | lock-free linked list | `ThreadStore` singleton + lock-free list | 等价的实现选择 |
| 线程 ID 管理 | 递增分配，上限 2047 | `m_ThreadId` + OS TID，无硬上限 | 项目有限制 |
| Abort/Interrupt | 基础 ABI stub | `ThreadAbortProvider` + CER (Constrained Execution Regions) | 项目无 CER，abort 不安全 |
| ThreadPool 集成 | 无 — thread pool worker 不 attach 为 managed thread | worker 走 `ManagedThreadBase::ThreadPool()` 统一生命周期 | **缺失** |
| 最大线程数控制 | 硬编码 2047 | 动态，受进程和资源约束 | 不够灵活 |

### 差距分析 (L2)

ThreadPool worker 不 attach 为 managed thread 是最关键的缺失。这意味着：
1. Thread pool worker 在 GC safepoint 中不被跟踪（suspend_seq 不作用于它们）
2. `Thread.CurrentThread` 在 thread pool 上下文中不可用
3. `ExecutionContext` 流转 chain 中断

---

## 2. Thread Pool

### 当前项目实现

- **WorkerLocalQueue** (`thread_pool.h:51-72`): lock-free LIFO work-stealing queue
  - 本地 LIFO push/pop（cache friendly）
  - 从随机 victim 的 local queue 底部 steal（FIFO 语义）
  - `TrySteal()` → CAS-based lock-free 实现
- **Gate Thread** (`thread_pool.cpp:498-544`): 500ms tick
  - Drain IOCP completions
  - 调用 `HillClimbingController::OnGateTick(completed, current_workers)`
  - 根据 suggestion 增减 worker：`AdjustWorkerCount(suggestion)`
  - Worker 增减逻辑：AddWorker (new std::thread + WorkerLoop) / RemoveWorker (设置退出标记)
- **HillClimbingController** (`thread_pool.h:31-45`): 5-state 简化版
  - 状态机: `Warmup → ClimbExplore → Climbing → Stabilizing → Steady`
  - 8-sample 滑动窗口计算平均吞吐
  - 每个 gate tick 比较 `avg_throughput / last_throughput_` 决定方向
  - `kHillClimbingMaxWorker = 32767`, `kHillClimbingMinWorker = 1`
- **IOCP** (`thread_pool.cpp:64-65, 242-260`): 专用的 `s_iocp_thread` 处理 IO completion
- **ETW** (`threadpool_events.h`): 仅 Windows，最基本的事件（Create/Unregister）

### CoreCLR 对比

| 维度 | 项目当前 | CoreCLR | 差距 |
|------|---------|---------|------|
| 本地队列 | LIFO + work-stealing | LIFO + work-stealing | 等价的实现选择 |
| HillClimbing 算法 | 5-state, 8-sample 均值比较, 无频域分析 | **Goertzel 算法**单频 DFT, 频域分析吞吐信号, 9-state 状态机, 非线形增益, 方波抖动 | **核心差距** |
| HillClimbing 状态数 | 5 (Warmup/ClimbExplore/Climbing/Stabilizing/Steady) | 9 (Warmup/ClimbExplore/Climbing/ClimbFix/Steady/SteadyFix/Saturating/Random/Stuck) | 缺少 ClimbFix/SteadyFix/Saturating/Random/Stuck |
| 窗口大小 | 8 固定 | 自适应，含方向跟踪 | 粗粒度 |
| 反馈信号 | 仅 completed_count (吞吐) | throughput + CPU utilization + worker count 多维 | **缺失 CPU 反馈** |
| 抖动抑制 | 无 | 方波抖动 + 非线形 gain | 缺少 |
| 线程注入 | 直接 +1 或 -1 | 批量 wave + elidelimited | 可能过度波动 |
| ThreadPoolBoundHandle | 无 | 完整实现 (IOCP bound handle) | 缺少 |
| CPU 检测 | 无 | `CpuUtilization` 实时监测 | **严重缺失** |
| HillClimbing .NET 9/10 | — | ~20年演进, 热路径 inline, Portable HillClimbing | 代差 |

### 差距分析 (L2 — 中风险)

HillClimbing 是 thread pool 最关键的性能组件。项目当前的实现相当于 CoreCLR ~2005 年水平的简化版本：

- **仅有吞吐反馈**: 只用 `completed_count` 估算吞吐，缺少 CPU utilization 耦合 → 在 IO 密集和 CPU 密集混合负载下可能错误调整
- **无频域分析**: CoreCLR 使用 Goertzel 算法从吞吐时间序列中提取主导频率成分，可以区分短暂波动和真实趋势；项目只用简单均值比较
- **无方波抖动**: CoreCLR 在稳态时注入方波抖动来探测系统是否还有余量，项目 Steady 状态下不做任何探测
- **注入/回收策略简单**: +1/-1 的线性调整在快速变化的负载下响应过慢，wave 注入量也不会随状态自适应

.NET 9/10 的 HillClimbing 又做了以下演进（项目差距更大）：
- 热路径 inline 避免函数调用开销
- Portable HillClimbing 移除对 Windows ETW timer 的依赖
- 更激进的线程回收策略适应 cloud-native 场景

---

## 3. GC Safepoint Protocol

### 当前项目实现

- **Generation-based handshake** (`thread_state.cpp:261-631`):
  - `RequestGlobalSafepoint()`: CAS 获取 s_safepoint_owner → bump epoch → 遍历线程注册表设置 `suspend_seq`
  - `ReleaseGlobalSafepoint()`: 清除所有线程 `suspend_seq` → signal suspend_event → 恢复 preemptive suspended → 释放 owner
  - `SafepointPoll()`: 检查 `suspend_seq` (fast path ~1 cycle) → 非零则设置 `suspend_ack` → wait on event (零 CPU)
- **Cooperative/Preemptive 模式切换** (`thread_state.h:198-210`):
  - `GcTransitionToCooperative()`: 切换后立即 SafepointPoll
  - `GcTransitionToPreemptive()`: 标记 preemptive → SafepointPoll(notify_only)
- **超时与预抢占** (`thread_state.cpp:478-585`):
  - 100ms 软超时 (`kSafepointTimeoutNs`)
  - 5s 硬超时 (`kSafepointHardTimeoutNs`)
  - Windows: `QueueUserAPC` fallback
  - POSIX: `SIGUSR1` signal handler 强制 ack
  - iOS: 不支持预抢占（无信号）
- **嵌套支持**: `s_safepoint_depth` (thread_local) 跟踪重入
- **GcTransition 宏** (`gc_transition.h`): `GcTransitionToPreemptive()` / `GcTransitionToCooperative()` 宏封装
- **SafepointOwner guard** (`thread_state.cpp:432-448`): CAS 确保单 owner

### CoreCLR 对比

| 维度 | 项目当前 | CoreCLR | 差距 |
|------|---------|---------|------|
| 协议类型 | generation-based per-thread atomic handshake | `g_TrapReturningThreads` + hijack + signal | 不同的实现策略 |
| 响应机制 | thread loop 中 SafepointPoll | Poll + return-address hijack + signal handler | 全面性差距 |
| 非托管线程 | 不做强制暂停（hard timeout 后 force release） | `g_TrapReturningThreads` 标记 + hijack return address | CoreCLR 更积极 |
| 超时恢复 | 软超时 100ms → 预抢占 → 硬超时 5s → force release | 类似的 2-level timeout + `Thread.Resume` fallback | 基本等价 |
| 死锁安全 | 无 `ForbidSuspend` 区域 | `ForbidSuspend` 区域避免死锁 | **缺失** |
| SuspendThread (Windows) | 仅 APC 队列，不调用 SuspendThread | `SuspendThread` + `GetThreadContext` 完整恢复 | 项目恢复路径简单 |
| Signal 处理 (POSIX) | SIGUSR1 handler，spin-wait | `SIGUSR1` / `SIGUSR2` + `sgen_signal_handler` | 基本等价 |
| Profiler 集成 | 无 | GCNotification + ETW + profiling API | **缺失** |

### 差距分析 (L1 — 功能基本完整)

项目 safepoint 协议的基础设计是合理的。generation-based handshake + event wait 与 CoreCLR 的 `g_TrapReturningThreads` + hijack 是两种不同策略，各有优劣：

**项目的优势**：
- SafepointPoll 在 interpreter loop 中拥有明确检查点，确定性好
- Event-based wait 在 safepoint 期间零 CPU 占用
- 嵌套支持设计良好

**关键缺失**：
1. **`ForbidSuspend` 区域**: CoreCLR 在持有 critical lock 的路径中标记 `ForbidSuspend`，防止 GC 请求 safepoint 时死锁。项目没有类似机制，在高竞争场景下可能发生 `safepoint_owner` 等待线程 A，而线程 A 等待线程 B 释放锁，线程 B 被 safepoint 暂停的循环死锁。**这是最值得关注的风险点**。
2. **非托管代码暂停**: CoreCLR 通过 hijack return address 主动暂停在 native 代码中未定期 poll 的线程；项目仅依赖超时 + 预抢占 fallback，超时窗口内延长了 STW pause
3. **Profiling/Diagnostic hooks**: 缺少 safepoint duration 的 ETW 事件输出

---

## 4. Monitor & Synchronization

### 当前项目实现

- **Monitor** (`core/monitor.cpp`): 3-phase adaptive spin lock
  - Phase 1: 64 PAUSE only (超快路径)
  - Phase 2: 64 PAUSE + periodic yield (每8次)
  - Phase 3: 立即 inflate，不再自旋
  - **Thin lock**: 2-bit 标记 + thread ID（低 30bit），无锁路径在 `EnterMonitor` 开始时检查
  - **Inflated lock**: Allocation `SyncBlock` + std::mutex + condition_variable
  - **SyncBlock 池**: 128 项预分配，`AllocateSyncBlockFromPool()` / `FreeSyncBlockToPool()`，stripe hash 减少竞争
  - **TryEnter / Exit / Pulse / PulseAll / Wait** 均有实现
- **SpinLock** (`core/monitor.cpp`): `SpinLockEnter` / `SpinLockExit` / `SpinLockIsHeld` — 简单 TAS spinlock，用于内部保护
- **Synchronization primitives** (`synchronization.h/cpp`): SemaphoreSlim、ReaderWriterLockSlim、Barrier、CountdownEvent
  - 全部基于 `std::mutex` + `std::condition_variable` 封装
  - Barrier: 参与线程数跟踪 + phase 信号
  - ReaderWriterLockSlim: 读计数 + 写标志 + 等待队列
- **WaitHandle** (`wait_handle.h/cpp`): ManualResetEvent / AutoResetEvent
  - O(1) unordered_map 存储，max 2048 handles
  - 内部 `std::atomic<bool>` + `std::condition_variable`

### CoreCLR 对比

| 维度 | 项目当前 | CoreCLR | 差距 |
|------|---------|---------|------|
| Monitor thin lock | 2-bit + thread ID | `OBJHEADER::Bits` + `SyncBlock::m_Header` | 等价设计 |
| SyncBlock 池 | 128 预分配，malloc 扩展 | `g_pSyncBlock` 数组动态增长 | 项目预分配有限 |
| Spin strategy | 3-phase (64+64+inflate) | Adaptive, 根据 CPU/核心数动态 | 项目固定迭代次数 |
| 唤醒风暴 | 无保护 | `PulseAll` 不放大 (mach-wide broadcast) | 可能唤醒风暴 |
| 公平性 | 不公平 (std::mutex) | 交替公平 + 饥饿避免 | 可能线程饥饿 |
| ReaderWriterLockSlim | std::mutex 封装 | 专门的自旋 + 等待链实现 | 大幅差距 |
| Barrier | std::condition_variable | 专门 phase 感知实现 | 可用但低效 |
| Lock recursion | EnterMonitor 检查 recursion | `m_Recursion` 计数 | 基本等价 |
| Contention monitoring | 无 (无 ETW) | `MONITOR_CONTENTION_EVENT` | **缺失** |

### 差距分析 (L2 — 中风险)

Monitor 的 3-phase spin lock + thin lock/inflation 结构完整，但在高竞争场景下表现需要验证：

**主要问题**：
1. **ReaderWriterLockSlim** 基于 std::mutex + condition_variable 实现，在高读负载下性能差（所有读操作串行化到同一个 mutex）。CoreCLR 使用 Interlocked 操作 + spin waiter chain，完全不依赖 OS 同步原语
2. **唤醒风暴**: `PulseAll` 在多个等待线程间广播时，没有抑制机制，可能导致 thundering herd
3. **竞争监测缺失**: 无法知道哪些 lock 是热点，无法诊断死锁或过度竞争
4. **公平性**: std::mutex 默认不公平，可能导致写线程饿死

---

## 5. ExecutionContext / AsyncLocal

### 当前项目实现

- **ExecutionContext** (`execution_context.h`): 极小实现
  - 4 个 inline `AsyncLocalValue` slot (`slots_[4]`)
  - `AsyncLocalValue`: 基础 struct 含 `void*` data + destructor
  - `Capture()` / `Run()` 基础 flow
  - Small-value optimization（直接存储小值）
- **AsyncLocal**: 无独立实现，融合在 ExecutionContext 中
- **EC flow**: `TaskRunCallback` 中 `ExecutionContext::Capture()` + `Run()` 手动管理

### CoreCLR 对比

| 维度 | 项目当前 | CoreCLR | 差距 |
|------|---------|---------|------|
| Slot 数量 | 4 固定 inline | `LogicalCallContext` + `IllogicalCallContext` 动态 | 严重不足 |
| Capture/Restore | 基础捕获 + 恢复 | `ExecutionContext.Capture()` + `Run()` + `CreateCopy()` | 基础等价 |
| SecurityContext | 无 | `SecurityContext` / `HostExecutionContext` | **缺失** |
| SuppressFlow | 无 | `AsyncFlowControl` / `ExecutionContext.SuppressFlow()` | **缺失** |
| 动态扩容 | 无 (固定 4 slot) | `IAsyncLocalValueMap` 动态 | **缺失** |
| IL 集成 | 无 | `AsyncLocalValue` 从 `CallContext` 类型化存储 | **缺失** |

### 差距分析 (L3 — 高风险)

ExecutionContext 是 async/await chain 正确性的关键。当前实现过于简化：

- **4 slot 硬限制**意味着任何超过 4 个 AsyncLocal 的场景都会静默丢失数据
- **缺少 SecurityContext/HostExecutionContext** 导致模拟（impersonation）和安全上下文在 async 中丢失
- **缺少 SuppressFlow** 意味着 library 代码无法避免不必要的上下文流动（影响性能）
- Task.Run 能工作但 async/await 链中 EC 传播不可靠

**这是个运行时正确性问题而不仅是性能问题**：上层业务代码依赖 AsyncLocal 做逻辑上下文传递，4 slot 限制在复杂场景下必然溢出。

---

## 6. Timer Queue

### 当前项目实现

- **TimerQueue** (`timer_queue.h`):
  - 最大 1024 个 timer
  - 由 gate thread 500ms tick 驱动，扫描到期 timer
  - `CreateTimer` / `ChangeTimer` / `DeleteTimer`
  - 内部 `std::vector` 存储

### CoreCLR 对比

| 维度 | 项目当前 | CoreCLR | 差距 |
|------|---------|---------|------|
| 精度 | 500ms tick → 最差 ~500ms 延迟 | 1ms 级精度 (高分辨率 timer) | **精度差距大** |
| 容量 | 1024 硬限制 | 动态 | 有限 |
| 数据结构 | vector 线性扫描 | 多层时间轮 (TimingWheel) | O(n) vs O(1) |
| IO timer 集成 | 无 | `ThreadPoolBoundHandle` + timer | 无 |
| 高精度需求 | 不支持 sub-100ms | 支持 | 不支持 |

### 差距分析 (L3 — 功能受限)

500ms tick + vector 线性扫描在小负载下可用，但：
- `System.Threading.Timer` 的短间隔（如 100ms）精度无法保证
- 大量 timer 时线性扫描 O(n) 性能差
- 1024 硬限制在高负载下可能溢出
- `Task.Delay` 依赖 timer，500ms tick 意味着 Delay(50ms) 实际延迟 ~500ms

---

## 7. TaskRunner (Task.Run / async)

### 当前项目实现

- **task_runner.cpp**: `TaskRun()` → `ThreadPoolQueueUserWorkItemUnsafe` 将 task 投递到 thread pool
- **async.h** (`common/chaos/`): `AsyncTask` struct + `async_task_run()` 函数指针转发
- **RegisterAsyncTaskRun()**: 在初始化时注册回调

### CoreCLR 对比

| 维度 | 项目当前 | CoreCLR | 差距 |
|------|---------|---------|------|
| Task.Run | 基础实现 | 完整 `Task<T>` + 状态机 | 功能基础 |
| async/await 状态机 | 无 (依赖前端编译器) | `AsyncTaskMethodBuilder` + `AsyncValueTaskMethodBuilder` | **缺失运行时支持** |
| TaskScheduler | 无 | 可插拔，含默认 ThreadPoolTaskScheduler | **缺失** |
| TaskCompletionSource | 无 | 完整 | 缺失 |
| ValueTask | 无 | 完整 | 缺失 |
| Awaitable 协议 | 无 | `INotifyCompletion` + `ICriticalNotifyCompletion` | **缺失** |
| Yield | 无 | `Task.Yield()` / `SwitchToThreadPool` | 缺失 |

### 差距分析 (L2 — 功能不足)

当前 `TaskRun()` 只能支持 `Task.Run(Action)` 这一种用法：

- `async/await` 在 managed C# 层通过编译器生成状态机工作，但运行时需要 `AsyncTaskMethodBuilder`、`INotifyCompletion`、`TaskAwaiter` 等支持
- `Task.Delay` 不可用（需要 timer 集成）
- `Task.Yield()` 不可用
- `TaskCompletionSource` 不可用于手动信号

这意味着当前基于 C# async/await 的代码在项目上无法正常运行。

---

## 8. GC Worker Pool

### 当前项目实现

- **GCWorkerPool** (`gc/gc_worker_pool.h/cpp`):
  - 专用 non-managed-thread worker（max 7）
  - 创建后永不销毁
  - 用于 GC 并行标记/清理
  - 通过 `GCToOSInterface::StartAdditionalThreads` 创建
  - 与 GC 层通过 `GcWorkEntry` / `WorkEntryList` 通信

### CoreCLR 对比

| 维度 | 项目当前 | CoreCLR | 差距 |
|------|---------|---------|------|
| Worker 生命周期 | 创建后永不销毁 | 动态管理，idle 可回收 | 项目可能浪费线程 |
| 最大数量 | 硬编码 7 | 动态 (CPU 核心数) | 固定不够灵活 |
| 与 GC 协作 | GcWorkEntry 通信 | `GCScan::GcScanRoots` + `GCHeap::GcCount` | 等价 |

### 差距分析 (L1 — 低风险)

GC worker pool 是当前实现最接近 CoreCLR 的模块，功能基本完整。主要问题是：
- 7 hard limit 在 >8 核机器上可能浪费并行能力
- 永不销毁的策略在低内存场景下不友好（但 GC 场景下合理）

---

## 风险优先级矩阵

```
高风险 ┼───────────────────────────────────●───
       │                                   │
       │                                   │
       │                    ●              │
 中风险 ┼────────────●──────┼──────────────│───
       │             │      │              │
       │       ●     │      │              │
 低风险 ┼───────┼─────┼──────┼──────●───────┼───
       │       │     │      │      │       │
       └───────┴─────┴──────┴──────┴───────┴───
       低影响   │    中影响  │    高影响
                │           │
                ▼           ▼
     L3: ExecutionContext  L2: Monitor (竞争/公平性)
     L3: Timer (精度)     L2: Thread Pool (HillClimbing)
                           L2: Managed Thread (TP attach)
                           L1: Safepoint (ForbidSuspend)
                           L1: GC Worker Pool
```

### 按影响排序

| 优先级 | 模块 | 风险 | 影响场景 |
|--------|------|------|---------|
| P0 | ExecutionContext | AsyncLocal 丢失 / 4 slot 溢出 | 复杂 async 业务逻辑错误 |
| P0 | Safepoint ForbidSuspend | 死锁 | 高竞争 + GC 时 hang |
| P1 | Thread Pool HillClimbing | 吞吐波动 / CPU 浪费 | IO + CPU 混合负载 |
| P1 | Timer 精度 | Delay 不精准 | 超时 / 定时任务 |
| P2 | Monitor 唤醒风暴 | 性能抖动 | 高竞争 Monitor.Wait |
| P2 | ReaderWriterLockSlim | 读路径串行化 | 高频读缓存场景 |
| P2 | Thread 未 attach ThreadPool | EC 链断裂 | 业务代码 Thread.CurrentThread |
| P3 | GC Worker Pool 硬上限 | CPU 浪费 | >8 核机器 |
| P3 | ETW 事件 | 诊断困难 | 性能分析 |

---

## 改进建议

### P0 — 必须修复（影响正确性）

**1.1 ExecutionContext 动态 slot 扩容**
- 移除 4 slot 硬限制，改为 `IAsyncLocalValueMap` 类似的动态映射
- 实现 `SuppressFlow()` / `RestoreFlow()` 接口
- `Capture()` 返回浅拷贝，`Run()` 执行 restore 语义
- 参考 CoreCLL `ExecutionContext.cs` 的处理链

**1.2 ForbidSuspend 区域标记**
- 在以下路径添加 `ForbidSuspendRegion` guard：
  - `s_safepoint_owner` CAS 竞争路径（已有部分保护但不完整）
  - `SyncBlock` 分配/释放（`AllocateSyncBlockFromPool`）
  - `thread_registry` 遍历（`stable_freeze` + 遍历窗口）
  - `ThreadPool::AddWorker` / `RemoveWorker`（创建线程 + 注册）
- 实现 `ForbidSuspendScope` RAII guard，内部用 thread_local 嵌套计数
- `SafepointPoll` 检查 forbid 嵌套计数，如果 forbid 中则跳过 suspend 等待

### P1 — 高优先级（影响性能）

**2.1 HillClimbing 增强**
- 引入 **CPU utilization 反馈**: 在 `GateTick` 中加入 getrusage 或 QueryProcessCycleTime 检测 CPU 利用率
- 引入 **Goertzel 算法**: 从吞吐时间序列提取频域特征，替代均值比较
- 增加状态: 至少添加 `ClimbFix`（矫正过度）和 `Saturating`（CPU 100% 时暂停添加）
- 方波抖动: 在 Steady 状态周期性注入 small wave 探测系统余量
- `.NET 9/10` 的热路径 inline 和 portable HillClimbing 可作为后续目标

**2.2 Timer Queue 精度提升**
- 改用高分辨率 timer (Windows: `CreateThreadpoolTimer` / POSIX: `timerfd_create`)
- gate thread tick 从 500ms 降到 10-15ms
- 引入最小堆(binary min-heap)替代 vector 线性扫描，O(log n) 插入/删除/到期检查
- 如果必须保持 500ms gate tick，短 timer 独立走 dedicated 高精度线程

### P2 — 中优先级（功能补强）

**3.1 Managed Thread — Thread Pool attach**
- Thread pool worker `WorkerLoop()` 入口调用 `AttachManagedThread()`，出口调用 `DetachManagedThread()`
- 附加 GC 模式切换: worker 在托管代码运行时切 cooperative，等待工作时切 preemptive
- `Thread.CurrentThread` 可访问 thread pool worker

**3.2 ReaderWriterLockSlim 重构**
- 放弃基于 std::mutex 的实现
- 改用 Interlocked 操作: `m_readers` 计数 + spin waiter list
- 写锁: `m_writeOwner` CAS + 等待队列
- 读锁: `m_readCount` InterlockedIncrement (无竞争路径零 syscall)

**3.3 Monitor 唤醒风暴抑制**
- `PulseAll` 替换为 chain-signal: 只唤醒一个等待者，由它接力唤醒下一个
- 或者引入 `m_expectWakeCount` 语义控制 broadcast 放大
- 公平性: 在 SyncBlock 中嵌入等待队列顺序

### P3 — 低优先级（工具/诊断）

**4.1 ETW 事件丰富**
- safepoint start/end/duration
- thread pool worker add/remove
- monitor contention
- thread create/attach/detach

**4.2 GC Worker Pool 动态化**
- 根据 CPU 核数自动调整 max workers
- idle worker 超时回收

---

## 结论

| 方面 | 评价 |
|------|------|
| **架构设计** | 正确。generation-based safepoint、thin lock/inflation、work-stealing thread pool 的方向都对 |
| **实现完整性** | 约 65-70%。核心路径工作，边缘路径和非功能需求差距大 |
| **CoreCLR 对齐度** | 中。有架构自信（部分设计优于 CoreCLR），但精细度差距明显 |
| **生产就绪度** | ⚠️ 不可直接用于生产。需先修复 P0/P1 的 4 个问题 |
| **最大单点风险** | **ExecutionContext 4-slot 溢出** — 静默数据丢失最难排查 |
| **最大性能风险** | **HillClimbing 缺少 CPU 反馈** — 混合负载下 thread 数量剧烈波动 |
| **最大稳定性风险** | **缺少 ForbidSuspend** — 特定竞争模式下的死锁可能 |

**建议路线**:
1. 短期（1-2 周）: ExecutionContext 动态扩容 + ForbidSuspend 区域
2. 中期（3-4 周）: HillClimbing + CPU 反馈 + Goertzel 频域分析
3. 长期: Timer 精度提升 + ReaderWriterLockSlim 重写 + Monitor 唤醒抑制
