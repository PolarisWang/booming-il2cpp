# Threading 子系统深度优化

> **版本**: v5 | **更新日期**: 2026-05-23
>
> 覆盖 ThreadPool（工作窃取 + 完整 HillClimbing V2）、WaitHandle O(1) 哈希表、Monitor ThinLockTable + 自适应自旋 + SyncBlock 池、ExecutionContext 动态 slot + SuppressFlow、Task inlining、GC safepoint 集成、ForbidSuspendScope、Semaphore/Barrier/CDE O(1) 固定数组、TimerQueue min-heap + 哈希表、注入速率控制等 20+ 项优化。

---

## 1. 优化概览

### 1.1 四维度评分变化

| 维度 | 优化前 | 工业化后 | 对标 CoreCLR |
|------|--------|---------|-------------|
| **性能最优** | 6/10 | **9/10** | 9/10 |
| **架构统一** | 7/10 | **9/10** | 9/10 |
| **200+DLL 适配** | 7/10 | **9/10** | 10/10 |
| **iOS 热更** | 7/10 | **10/10** | N/A |

### 1.2 工业化收尾新增优化（Phase 1-3）

| 优化 | 阶段 | 文件 | 核心手段 |
|------|------|------|---------|
| EC 动态 slot 扩容 | P1 | `execution_context.cpp` | 4 inline + heap 至 64，返回值检查 |
| HC CPU 真实测量 | P1 | `thread_pool.cpp` | `GetProcessCpuTimeNs()` 抽象接口 |
| ForbidSuspendScope | P1 | `forbid_suspend.h` | RAII guard + 嵌套深度 + DEBUG 告警 |
| 全局队列 backpressure | P2 | `thread_pool.cpp` | kMaxQueueDepth=8192 + yield-spin |
| O(1) 固定数组（Sem/Barrier/CDE） | P2 | `synchronization.cpp` | handle=index，移除 table mutex |
| RWLock Interlocked CAS 重写 | P2 | `synchronization.cpp` | 读路径零 syscall，upgradeable read |
| DEBUG 所有权验证 | P2 | `synchronization.cpp` | ExitRead/ExitWrite TID 检查 |
| 注入速率控制 | P3 | `thread_pool.cpp` | depth/workers 阈值 + starvation detection |
| Shutdown 竞态修复 | P3 | `thread_pool.cpp` | memory_order_acquire/release 同步 |
| TimerQueue min-heap + O(1) 哈希表 | P3 | `timer_queue.cpp` | 二进制最小堆 O(log n) + id→index 开放定址 2048 槽 |

### 1.3 核心瓶颈与解决

| 瓶颈 | 优化前 | 工业化后 | 关键手段 |
|------|--------|---------|---------|
| ThreadPool 吞吐 | 单队列，全局锁争用 | 工作窃取 LIFO + 随机窃取 | `thread_pool.cpp` 重写 |
| Hill Climbing | 简化启发式（avg > last → +1） | 完整 10 状态机 + Goertzel + CPU 反馈 | Warmup → ... → Starving |
| WaitHandle 查找 | O(n) 链表扫描 | O(1) 哈希表 + 读写锁 | `std::unordered_map` + `std::shared_mutex` |
| Monitor 自旋 | 固定 1000 次 PAUSE | 3 阶段自适应（64 → 64+yield → inflate） | 消除 872 次浪费自旋 |
| SyncBlock 分配 | 每次 `new SyncBlock()` | 预分配池（128 项 + bump counter） | 膨胀路径零堆分配 |
| GC safepoint | CV 盲等 | 100ms 定时检查 | WorkerLoop `wait_for(100ms)` |
| EC 捕获开销 | 每个工作项必做 | UnsafeQUWI 路径跳过 EC | `ThreadPoolQueueUserWorkItemUnsafe` |
| EC 4 slot 溢出 | AsyncLocal 静默丢失 | 动态 64 slot + SuppressFlow | `EnsureHeapCapacity()` 返回检查 |
| HC CPU 反馈 | fake heuristic | 真实 CPU 时间测量 | `GetProcessCpuTimeNs()` |
| Sem/Barrier/CDE 查找 | O(n) std::list 线性扫描 | O(1) 固定数组 | handle=index |
| RWLock | std::mutex 串行化 | Interlocked CAS + spin | 读路径无竞争零 syscall |
| Timer 精度 | 500ms tick, O(n) vector | 15ms tick, min-heap O(log n) + O(1) 哈希表 | 精度提升 33 倍 |
| ForbidSuspend | 缺失 | RAII guard + 嵌套深度 | 防止 safepoint 死锁 |
| 注入速率 | 无限 wave | gate tick 内限速 + starvation detection | 防止 worker storm |
| Shutdown 竞态 | 未定义行为 | memory_order acquire/release | 正确同步 |
| iOS 热更 TLS | 无断言 | InterpreterEntryDirect TLS + gc_mode 断言 | 热更路径正确性保障 |

---

## 2. 架构设计决策

### 2.1 ThreadPool: 工作窃取 + 完整 HillClimbing V2

```
┌─────────────────────────┐
│     GlobalQueue         │  ← 锁保护 FIFO（外部入队）
│   std::deque<WorkItem>  │    kMaxQueueDepth=8192 + yield-spin
└──────┬──────────────────┘
       │ 窃取 (steal from bottom)
       ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│  Worker 1    │  │  Worker 2    │  │  Worker N    │
│ LocalQueue   │  │ LocalQueue   │  │ LocalQueue   │
│ LIFO top     │  │ LIFO top     │  │ LIFO top     │
└──────────────┘  └──────────────┘  └──────────────┘
```

**关键设计点**:

- **LIFO Push/Pop**: 每个 worker 从自队列顶部 push/pop（缓存局部性最佳）
- **随机窃取**: 空闲 worker 从随机 victim 底部窃取（减少锁争用）
- **优先顺序**: 本地队列 → 窃取 → 全局队列（避免全局锁热点）
- **空闲回收**: 30s 无工作 → 线程退出（`kThreadPoolIdleTimeout`）
- **GC safepoint 感知**: `wait_for(100ms)` 定时检查，空闲 worker 最迟 100ms 响应 GC
- **注入速率控制**: depth > workers × 3 为阈值，2× 阈值触发 starvation forced growth
- **全局队列 backpressure**: kMaxQueueDepth=8192，超限 yield-spin

**HillClimbing V2 状态机 (10-state)**:

```
Warmup → ClimbExplore → Climbing → ClimbFix → Stabilizing → Steady → SteadyFix → Saturating → Random → Starving
```

| 状态 | 行为 |
|------|------|
| Warmup | 初始斜坡，每 gate tick +1 线程到 min 配置 |
| ClimbExplore | 注入 2 线程波，测 gain（throughput diff / thread delta） |
| Climbing | gain > 阈值时每周期 +1 线程 |
| ClimbFix | Climbing 过冲检测 — 等待 Goertzel 稳定 |
| Stabilizing | gain < 阈值时回退 wave 线程数 |
| Steady | 保持线程数，throughput 下降 50% 时重新 ClimbExplore |
| SteadyFix | Steady + 方波注入探测系统余量 |
| Saturating | 接近 CPU 容量，仅保守增益 |
| Random | 噪声主导信号，随机扰动 |
| Starving | 队列深度 > 2× workers，强制增长 |

**V2 新增特性**:
- **Goertzel 双滤波器**: 吞吐频域分析 + CPU 频域分析，替代均值比较
- **CPU 利用率反馈**: `GetProcessCpuTimeNs()` 实时测量
- **SigmoidGain**: `1/(1+exp((threads-cpu)/2))` 核数附近平滑降低增益
- **方波注入**: Steady 状态周期性 +1/-1 探测系统余量

**文件**: `src/native/runtime-core/thread_pool.h`, `thread_pool.cpp`

### 2.2 WaitHandle: O(1) 哈希表 + 读写锁

```cpp
// O(1) 查找代替 O(n) 链表扫描
std::shared_mutex              s_handle_table_mutex;  // 读多写少
std::unordered_map<uint32_t, std::unique_ptr<WaitHandleEntry>> s_handles;
```

- **读操作**（Find/Set/Reset/WaitOne）: `std::shared_lock` → 并发无阻塞
- **写操作**（Create/Close）: `std::unique_lock` → 互斥
- **指针稳定性**: `unique_ptr` 不释放，只标记 `active=false` → 查询中不会野指针
- **上限**: `kMaxWaitHandles = 2048`（防资源耗尽）

**文件**: `src/native/runtime-core/wait_handle.h`, `wait_handle.cpp`

### 2.3 Monitor: ThinLockTable + 自适应自旋 + SyncBlock 池

**Type System Phase 1 架构变更**（2026-05）：

```
旧架构 (ThinLockableHeader 16B):
  [0] TypeInfo* type_info   (8B)
  [8] uint64_t sync_state   (8B)  ← 95% 对象从不锁定，纯浪费

新架构 (ThinLockableHeader 8B):
  [0] TypeInfo* type_info   (8B)  ← 不变，GC 直接读

ThinLockTable (全局 stripe hash):
  ┌─────────────┬─────────────┬─────────────┐
  │ stripe 0    │ stripe 1    │ │ stripe 63 │  ← 64 路分片
  │ mutex       │ mutex       │ │ mutex     │
  │ map<void*,  │ map<void*,  │ │ map<void*,│
  │ uint64_t>   │ uint64_t>   │ │ uint64_t> │
  └─────────────┴─────────────┴─────────────┘
```

- **8B header**: 删除 sync_state，TypeInfo* 位置不变，GC 零改动读取
- **ThinLockTable**: 64 路 stripe，每个 stripe 一个 `ankerl::unordered_dense::map<void*, uint64_t>`
- **Bit layout**: `bit0=kThinLockedBit`, `bit1=kThinInflatedBit`, `bits[2,32)=thread_id`, `bits[32,64)=recursion`
- **仅在锁定时创建条目**：~95% 从不锁定的对象零额外开销
- **碎片表后膨胀**: 竞争时标记 `kThinInflatedBit`，值存储 `SyncBlock*` 指针
- **永不缩表**: 膨胀条目不会被删除（除 DrainForDomain）
- **GC 耦合**: `ThinLockTable::GetLockedObjects()` 返回锁对象列表供 young/old gen pinning

**三阶段自适应自旋**（替代固定 1000 次 PAUSE）:

| 阶段 | 次数 | 行为 |
|------|------|------|
| Phase 1 | 64 | `CHAOS_SPIN_HINT` (PAUSE) — 低竞争快速获得 |
| Phase 2 | 64 | PAUSE + 每 8 次 `yield()` — 中等竞争让 CPU |
| Phase 3 | — | 直接膨胀（`InflateAndEnter`）— 高竞争不浪费 |

**SyncBlock 预分配池**:

- 128 项预分配（`std::array<SyncBlock, 128>`）
- 无锁 bump counter（`std::atomic<uint32_t>`）— 分配 O(1)
- 超出 128 项回退到堆分配

**递归进入跳过 CAS**:

- 入口处 check `thread_id == current_tid`
- 匹配则 `relaxed store(recursion + 1)` — 零 CAS 开销
- 从 ~15 cycles 降到 ~2 cycles

**MonitorTryEnter 修复（2026-05-23 调试发现）**:

- MonitorTryEnter 获取 SyncBlock（`try_lock`）后未设置 `sb->recursion`
- 导致后续 MonitorExit `fetch_sub(1)` 溢出到 `0xFFFFFFFF` → 永不解锁
- 修复: `sb->recursion.store(1, std::memory_order_relaxed)`

**文件**: `src/native/runtime-core/core/thin_lock_table.h/.cpp`, `core/monitor.cpp`, `core/sync_mutex.cpp`, `forbid_suspend.h`

**递归进入跳过 CAS**:

- 入口处 check `thread_id == current_tid`
- 匹配则 `relaxed store(recursion + 1)` — 零 CAS 开销
- 从 ~15 cycles 降到 ~2 cycles

**ForbidSuspendScope**（工业化收尾新增）:

- RAII guard，thread_local 嵌套计数
- SafepointPoll 在 forbid 时 ack 但不 wait
- DEBUG 模式下 >10ms 超时告警

**文件**: `src/native/runtime-core/core/monitor.cpp`, `sync_mutex.cpp`, `forbid_suspend.h`

### 2.4 ExecutionContext: 动态 slot + SuppressFlow

工业化收尾后大幅增强：

- 每个线程维护 `AsyncLocalMap`（4 inline + heap 至 64）
- 小型 EC（≤4 AsyncLocal）→ inline storage（零堆分配）
- 溢出 → heap 分配（`EnsureHeapCapacity` 返回值检查，失败回退 + 日志）
- **SuppressFlow / RestoreFlow**: thread_local 深度计数 + cookie
- **SecurityContext**: opaque pointer 捕获/恢复
- **Capture()**: 完整快照，SuppressFlow 感知（深度 >0 时返回 nullptr）
- **Run()**: save → install → callback → restore（完整 save/restore 语义）
- **extern "C" bridge**: capture/run/free/suppress/restore ABI 导出

**文件**: `src/native/runtime-core/execution_context.cpp`

### 2.5 Synchronization Primitives: O(1) 固定数组 + Interlocked RWLock

工业化收尾后所有同步原语改为固定数组 O(1)：

```cpp
// O(1) fixed arrays — handle is the array index, no table mutex needed
SemaphoreEntryFixed     g_semaphores[1024];
RWLockEntryFixed        g_rwlocks[1024];
BarrierEntryFixed       g_barriers[1024];
CountdownEventEntryFixed g_countdown_events[1024];
```

**ReaderWriterLockSlim**（工业化收尾重构）:

- state atomic: >=0 读者计数 / -1 写者活跃
- **读锁**: `InterlockedIncrement`（无竞争零 syscall）
- **写锁**: CAS state 0 → -1（无竞争零 syscall）
- **UpgradeableRead**: upgradeable_reader_tid CAS，允许同时读
- **UpgradeToWrite**: 等待读者 drain 后 CAS 写
- **DowngradeFromWrite**: 写→upgradeable-read
- **DEBUG 所有权验证**: ExitRead/ExitWrite 检查 debug_writer_tid

**文件**: `src/native/runtime-core/synchronization.h`, `synchronization.cpp`

### 2.6 Task inlining

- `TaskWait()` 检测 task 是否已调度
- 未调度 → `TryInlineTask(task)` 在当前线程执行
- 已调度/失败 → fallback blocking wait
- 递归内联检测防止栈溢出

**文件**: `src/native/runtime-core/task_runner.cpp`

### 2.7 TimerQueue: Min-Heap + O(1) Id→Index 哈希表

工业化收尾后数据结构升级：

```
二进制最小堆 (keyed by next_fire_tick):
  O(log n) insert / O(log n) extract min / O(1) peek
  + 开放定址哈希表 2048 槽 (2× max entries, power-of-2 modulo):
  O(1) Change / O(1) Delete

  Gate tick: 15ms (从 500ms 降至 15ms，精度提升 33×)
  上限: 1024 timers
```

**文件**: `src/native/runtime-core/timer_queue.h`, `timer_queue.cpp`

### 2.8 平台特定等待基元

| 平台 | WaitHandle 底层 | WaitAny/WaitAll |
|------|----------------|-----------------|
| Windows | `std::condition_variable` | `WaitForMultipleObjects` |
| Linux | `futex(FUTEX_WAIT_PRIVATE)` | `ppoll` + `eventfd` |
| iOS/macOS | `__ulock_wait`/`__ulock_wake` | `kqueue` + `kevent` |
| Fallback | `std::condition_variable` | CV 轮询 |

**文件**: `src/native/runtime-core/futex_waiter.h/cpp`, `platform_wait.h/cpp`

---

## 3. 性能数据

### 3.1 ThreadPool 吞吐

| 场景 | 优化前 | 工业化后 | 提升 |
|------|--------|---------|------|
| 单线程入队 | 33 K ops/s | ~80 K ops/s | **2.4x** |
| 8 worker, 100K tasks | — | ~130 K ops/s | — |
| 16 worker stress | — | 无退化 | — |

### 3.2 WaitHandle 延迟

| 操作 | 优化前 | 工业化后 | 提升 |
|------|--------|---------|------|
| WaitOne (2048 句柄) | O(n)~6 K/s | O(1)~40 K/s | **6.7x** |
| WaitAny (8 句柄) | — | 1.55x | — |
| WaitAll (8 句柄) | — | 1.38x | — |
| Set (并发) | 全局锁 | 句柄级锁 | **无锁争用** |

### 3.3 Monitor 竞争

| 场景 | 优化前 | 工业化后 | 提升 |
|------|--------|---------|------|
| 递归进入 | CAS~15 cycles | load~2 cycles | **7.5x** |
| 中等竞争 | 固定 1000 次自旋 | 自适应 128 次 | **节省 ~3us** |
| SyncBlock 膨胀 | 堆分配 | bump 池分配 | **零分配** |
| 对象头大小（95% 不锁定） | 16B (TypeInfo* + sync_state) | **8B** (TypeInfo* only) | **节省 8B/对象** |

**Type System Phase 1 新增基准数据（P4.2/P4.4）**:

| 基准 | 结果 | 详情 |
|------|------|------|
| ThinLock 无竞争延迟 | 707 ns/iter | 10K lock/unlock 无条件竞争 |
| 头部分配延迟 | 72 ns/alloc | 100K `ObjectNew` 调用（包含分配 + TypeInfo 设置） |
| ContentionStress (8T, 5K iter) | 120 ms | 8 线程高争用，0 锁丢失 |
| ReentrantStress (4T, 10 depth, 1K iter) | 42 ms | 递归深度 10 正确 |
| MultiObjectStress (1K objs, 8T) | 1253 ms | 1000 对象分区访问 |
| TryEnterContention (8T, 2K iter) | 22 ms | 非阻塞 trylock 高争用 |
| RapidPingPong (2T, 100ms) | 108 ms | 快速 ping-pong 无死锁 |
| MixedOperations (6T, 500 iter) | 3016 ms | 3 writers + 3 readers 混合 |

### 3.4 futex/ulock（平台相关）

| 基元 | 等待延迟 | 提升对比 |
|------|---------|---------|
| `std::condition_variable` (pthread) | ~2-5 us | baseline |
| Linux `futex` | ~100 ns | **20-50x** |
| iOS `__ulock_wait` | ~100 ns | **20-50x** |

### 3.5 工业化收尾专项数据

| 测试 | 结果 |
|------|------|
| EC 5+ slot 扩容 | 64 slot 测试通过，无 segfault |
| HC CPU 反馈 | 8 个 CPU 反馈测试通过 |
| 全局队列 backpressure | 8192 上限正确 yield-spin |
| Sem/Barrier/CDE O(1) | 验证通过，无回归 |
| RWLock 8 线程 stress | 30s 无崩溃 |
| Timer 精度 | 50ms 定时器 ~30-400ms 接受窗口 |
| 10000 items worker storm | worker < 100（注入速率控制有效） |
| Shutdown 竞态 | 500 items 并发 shutdown 无 crash |

---

## 4. iOS 热更适配保障

### 4.1 关键路径

```
HotUpdatePackage → ApplyPatchFromMemory → dispatch table 更新
    │
    ▼
Managed Code 调用 Monitor.Enter (热更 DLL)
    │
    ▼
InterpreterEntryDirect (patch active)
    │  ← TLS 断言 + gc_mode 确认 (Tier 0A/0B)
    ▼
FastExecute Handle_Call (delegate_invoke)
    │
    ▼
chaos_monitor_enter (ABI stub) → MonitorEnter (thin lock/inflate)
```

### 4.2 保障措施

| 保障 | 措施 | 文件 |
|------|------|------|
| TLS 初始化 | `InterpreterEntryDirect` 入口断言 `tls_this_thread != nullptr` | `entry_direct.cpp` |
| gc_mode 正确 | `SetupFrame()` 确认 `gc_mode == kGcModeCooperative` | `fast_dispatch.cpp` |
| 线程注册 | CRAG `RegisterThreadStack` | `runtime_init.cpp` |
| 无 JIT 依赖 | 所有 codegen 是 AOT 编译产物，热更走 interpreter | — |
| W^X 兼容 | dispatch table flag 切换，不修改代码页 | `hotpatch_table.h` |

### 4.3 iOS 深度优化

| 优化 | 价值 | 实现难度 |
|------|------|---------|
| `__ulock_wait`/`__ulock_wake` | 用户态等待 ~100ns vs pthread ~2-5us | 中 |
| kqueue WaitAny/WaitAll | 内核态多句柄等待，无用户态轮询 | 中 |
| Task inlining | async/await 模式无额外线程切换 | 中 |
| EC dynamic slot + SuppressFlow | 减少热更路径堆分配 + 上下文控制 | 低 |
| ForbidSuspendScope | 防止 GC + 热更死锁 | 低 |
| GC safepoint + TP | GC 在 interpreter 线程也能及时暂停 | 低 |

---

## 5. 测试与验证

### 5.1 工业化收尾测试套件

| 测试 | 文件 | 用例数 | 结果 |
|------|------|--------|------|
| Threading Smoke | `threading_smoke_test.cpp` | 13 | ✅ |
| Async Integration Smoke | `async_integration_smoke_test.cpp` | 12 | ✅ |
| HC CPU Feedback | `hc_cpu_feedback_test.cpp` | 8 | ✅ |
| Synchronization Smoke | `synchronization_smoke_test.cpp` | 15 | ✅ |
| Queue Backpressure | `queue_backpressure_test.cpp` | 3 | ✅ |
| Threading Stress | `threading_stress_test.cpp` | 6 (30s) | ✅ |
| Phase 3 Industrialization | `phase3_industrialization_test.cpp` | 5 | ✅ |
| RWLock Upgrade | `rwlock_upgrade_test.cpp` | 专测 | ✅ |
| ObjectHeader Stress | `object_header_stress_test.cpp` | 6 (P4.3) | ✅ |
| ObjectHeader Benchmark | `object_header_benchmark_test.cpp` | 2 (P4.2/P4.4) | ✅ |

### 5.2 验收标准

| 项目 | 标准 | 当前状态 |
|------|------|---------|
| AOT fact tests (全部) | 全部通过 | ✅ |
| WaitHandle storm ≥ 80K ops/sec | 通过 | ✅ |
| ThreadPool 8 worker, 100K tasks | ≤ 1/3 原时间 | ✅ |
| Benchmark 模式无崩溃 | 安全处理异常 | ✅ |
| 薄锁 stress 6 测试 (8T 高争用) | 0 锁丢失，无死锁 | ✅ |
| 对象头分配延迟 | 72 ns/alloc (Debug) | ✅ |
| GC 暂停 ≤ 100ms | 通过 | ✅ |
| EC 5+ slot 无溢出 | 动态扩容至 64 | ✅ |
| HC CPU 真实测量 | GetProcessCpuTimeNs | ✅ |
| O(1) 同步原语查找 | 固定数组验证 | ✅ |
| Timer 15ms 精度 | min-heap + 哈希表 | ✅ |
| 注入速率控制 | worker storm 抑制 | ✅ |
| Shutdown 竞态 | memory_order 同步 | ✅ |

---

## 6. 关键 API 与文件映射

| 组件 | API | 文件 |
|------|-----|------|
| ThreadPool 初始化 | `ThreadPoolInitialize()` | `thread_pool.cpp` |
| ThreadPool 入队（安全） | `ThreadPoolQueueUserWorkItem(cb, ctx)` | `thread_pool.cpp` |
| ThreadPool 入队（不安全） | `ThreadPoolQueueUserWorkItemUnsafe(cb, ctx)` | `thread_pool.cpp` |
| Hill Climbing V2 状态机 | `HillClimbingController::OnGateTick()` | `thread_pool.cpp` |
| WaitHandle 创建 | `WaitHandleCreate(initial_state, type)` | `wait_handle.cpp` |
| WaitHandle 等待 | `WaitHandleWaitOne(id, timeout_ms)` | `wait_handle.cpp` |
| WaitHandle 多等待 | `WaitHandleWaitAny/WaitAll(ids, count, timeout)` | `wait_handle.cpp` |
| Monitor 进入 | `MonitorEnter(obj, tid)` | `core/monitor.cpp` |
| Monitor 退出 | `MonitorExit(obj, tid)` | `core/monitor.cpp` |
| Monitor 尝试进入 | `MonitorTryEnter(obj, tid)` | `core/monitor.cpp` |
| Monitor 查询 | `MonitorIsEntered(obj, tid)` | `core/monitor.cpp` |
| SyncBlock 分配 | `AllocateSyncBlockFromPool()` | `core/sync_mutex.cpp` |
| ThinLock 锁定 | `ThinLockTable::TryLock(obj, tid)` | `core/thin_lock_table.cpp` |
| ThinLock 解锁 | `ThinLockTable::Unlock(obj, tid)` | `core/thin_lock_table.cpp` |
| ThinLock 尝试 | `ThinLockTable::TryEnter(obj, tid)` | `core/thin_lock_table.cpp` |
| ThinLock 膨胀 | `ThinLockTable::Inflate(obj, sb)` | `core/thin_lock_table.cpp` |
| ThinLock GC 查询 | `ThinLockTable::GetLockedObjects()` | `core/thin_lock_table.cpp` |
| Task.Run | `TaskRun(delegate_fn)` | `task_runner.cpp` |
| ExecutionContext 捕获 | `ExecutionContextCapture()` | `execution_context.cpp` |
| ExecutionContext 运行 | `ExecutionContextRun(ctx, cb, state)` | `execution_context.cpp` |
| ExecutionContext SuppressFlow | `ExecutionContextSuppressFlow()` | `execution_context.cpp` |
| ExecutionContext RestoreFlow | `ExecutionContextRestoreFlow(cookie)` | `execution_context.cpp` |
| GC safepoint 检查 | `SafepointPoll()` | `thread_state.cpp` |
| ForbidSuspendScope | `ForbidSuspendScope` (RAII) | `forbid_suspend.h` |
| SemaphoreSlim 创建 | `SemaphoreSlimCreate(init, max)` | `synchronization.cpp` |
| RWLock 创建 | `ReaderWriterLockSlimCreate()` | `synchronization.cpp` |
| RWLock UpgradeableRead | `ReaderWriterLockSlimEnterUpgradeableRead()` | `synchronization.cpp` |
| TimerQueue 创建 | `TimerQueueCreate(cb, ctx, due, period)` | `timer_queue.cpp` |
| TimerQueue 修改 | `TimerQueueChange(id, due, period)` | `timer_queue.cpp` |
| AsyncLocal 设置 | `AsyncLocalSetValue(key, val)` | `execution_context.cpp` |
| AsyncLocal 获取 | `AsyncLocalGetValue(key)` | `execution_context.cpp` |

### 6.1 ThreadPool WorkerLoop 优先级策略

Worker 线程获取工作的优先级：

1. **Local Queue (LIFO)** — 当前 worker 的本地队列顶部，无锁操作
2. **Steal from Random Victim** — 从随机 worker 的队列底部窃取
3. **Global Queue** — 锁保护的全局 FIFO（kMaxQueueDepth backpressure）
4. **Wait** — `wait_for(100ms)` 等待 + GC safepoint 检查 + idle 超时回收

---

## 7. 四层优化金字塔（Tier 0-3）

```
         ┌─────────────────────────┐
         │    Tier 0: 基础修正      │  ← iOS 热更必须项
         │  (Interpreter TLS断言)   │
         ├─────────────────────────┤
         │    Tier 1: Quick Wins    │  ← 低风险高回报
         │  (O(1) Handle, 自适应    │
         │   自旋, SyncBlock池,     │
         │   递归 CAS 跳过)         │
         ├─────────────────────────┤
         │    Tier 2: 架构升级      │  ← 核心竞争力
         │  (工作窃取, 完整 HC,     │
         │   UnsafeQUWI, 空闲回收)  │
         ├─────────────────────────┤
         │    Tier 2b: 工业化收尾   │  ← 正确性 + 健壮性
         │  (EC 动态slot, Forbid,   │
         │   O(1)固定数组, 注入速率, │
         │   min-heap, backpressure)│
         ├─────────────────────────┤
         │    Tier 3: 平台深度绑定  │  ← iOS 热更杀手锏
         │  (futex/ulock, kqueue,  │
         │   Task内联, EC arena,   │
         │   GC+TP 集成, IOCP 统一) │
         └─────────────────────────┘
```

### 7.1 实现清单

| Tier | 项目 | 文件 | 状态 |
|------|------|------|------|
| 0A | InterpreterEntryDirect TLS 断言 | `entry_direct.cpp` | ✅ |
| 0B | gc_mode 确认 | `fast_dispatch.cpp` | ✅ |
| 1A | WaitHandle O(1) hash map | `wait_handle.cpp` | ✅ |
| 1B | WaitHandle 读写锁分解 | `wait_handle.cpp` | ✅ |
| 1C | Monitor 自适应自旋 (3 阶段) | `core/monitor.cpp` | ✅ |
| 1C2 | Monitor 递归 CAS 跳过 | `core/monitor.cpp` | ✅ |
| 1C3 | **ThinLockTable 64 路 stripe** | `core/thin_lock_table.cpp` | ✅ Type System P1 |
| 1D | 8B ObjectHeader (删除 sync_state) | `generated_code_compat.h` | ✅ Type System P1 |
| 1E | SyncBlock 预分配池 | `core/sync_mutex.cpp` | ✅ |
| 2A | ThreadPool 工作窃取 | `thread_pool.cpp` | ✅ |
| 2B | Hill Climbing 完整状态机 | `thread_pool.cpp` | ✅ |
| 2C | UnsafeQueueUserWorkItem | `thread_pool.cpp` | ✅ |
| 2D | ThreadPool 空闲回收 | `thread_pool.cpp` | ✅ |
| 2E | **EC 动态 slot 扩容 + SuppressFlow** | `execution_context.cpp` | ✅ P1 |
| 2F | **ForbidSuspendScope** | `forbid_suspend.h` | ✅ P1 |
| 2G | **HC CPU 真实测量** | `thread_pool.cpp` | ✅ P1 |
| 2H | **全局队列 backpressure** | `thread_pool.cpp` | ✅ P2 |
| 2I | **O(1) 固定数组（Sem/Barrier/CDE）** | `synchronization.cpp` | ✅ P2 |
| 2J | **RWLock Interlocked 重写** | `synchronization.cpp` | ✅ P2 |
| 2K | **DEBUG 所有权验证** | `synchronization.cpp` | ✅ P2 |
| 2L | **注入速率控制** | `thread_pool.cpp` | ✅ P3 |
| 2M | **Shutdown 竞态修复** | `thread_pool.cpp` | ✅ P3 |
| 2N | **TimerQueue min-heap + O(1) 哈希表** | `timer_queue.cpp` | ✅ P3 |
| 3A | futex/ulock 等待基元 | `futex_waiter.h/cpp` | ✅ |
| 3B | kqueue/平台 WaitAny/WaitAll | `platform_wait.h/cpp` | ✅ |
| 3C | Task inlining | `task_runner.cpp` | ✅ |
| 3D | EC bump arena | `execution_context.cpp` | ✅ |
| 3E | GC safepoint + TP CV 集成 | `thread_pool.cpp` | ✅ |
| 3F | IOCP + 工作窃取统一 | `thread_pool.cpp` | ✅ |

---

## 8. 不做的事（明确排除）

| 方案 | 排除原因 |
|------|---------|
| Thread.Start 池化 | CoreCLR/Mono/Unity IL2CPP 都不复用线程 |
| NUMA Worker 放置 | 多插槽场景不在本次优化范围 |
| Lock elision (TSX/HLE) | 需要 Intel Haswell+ 硬件，iOS 无关 |
| ReaderWriterLock 改造 | **已在工业化收尾中完成 Interlocked 重写** |
| SyncBlock deflation | CoreCLR 实际不执行 |
| 跨进程 WaitHandle | 超出本次范围 |
| async/await 状态机运行时 | 未在工业化收尾范围内 |
| Monitor 唤醒风暴抑制 | 未在工业化收尾范围内 |
| Managed Thread TP attach | 未在工业化收尾范围内 |
