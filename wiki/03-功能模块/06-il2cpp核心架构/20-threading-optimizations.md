# Threading 子系统深度优化

> **版本**: v3 | **更新日期**: 2026-05-16
>
> 覆盖 ThreadPool（工作窃取 + 完整 Hill Climbing）、WaitHandle O(1) 哈希表、Monitor 自适应自旋 + SyncBlock 池、ExecutionContext bump arena、Task inlining、GC safepoint 集成等 14 项优化。

---

## 1. 优化概览

### 1.1 四维度评分变化

| 维度 | 优化前 | 优化后 | 对标 CoreCLR |
|------|--------|--------|-------------|
| **性能最优** | 6/10 | **9/10** | 9/10 |
| **架构统一** | 7/10 | **9/10** | 9/10 |
| **200+DLL 适配** | 7/10 | **9/10** | 10/10 |
| **iOS 热更** | 7/10 | **10/10** | N/A |

### 1.2 核心瓶颈与解决

| 瓶颈 | 优化前 | 优化后 | 关键手段 |
|------|--------|--------|---------|
| ThreadPool 吞吐 | 单队列，全局锁争用 | 工作窃取 LIFO + 随机窃取 | `thread_pool.cpp` 重写 |
| Hill Climbing | 简化启发式（avg > last → +1） | 完整状态机（CoreCLR 兼容） | Warmup → ClimbExplore → Climbing → Stabilizing → Steady |
| WaitHandle 查找 | O(n) 链表扫描 | O(1) 哈希表 + 读写锁 | `std::unordered_map` + `std::shared_mutex` |
| Monitor 自旋 | 固定 1000 次 PAUSE | 3 阶段自适应（64 → 64+yield → inflate） | 消除 872 次浪费自旋 |
| SyncBlock 分配 | 每次 `new SyncBlock()` | 预分配池（128 项 + bump counter） | 膨胀路径零堆分配 |
| GC safepoint | CV 盲等 | 100ms 定时检查 | WorkerLoop `wait_for(100ms)` |
| EC 捕获开销 | 每个工作项必做 | UnsafeQUWI 路径跳过 EC | `ThreadPoolQueueUserWorkItemUnsafe` |
| iOS 热更 TLS | 无断言 | InterpreterEntryDirect TLS + gc_mode 断言 | 热更路径正确性保障 |

---

## 2. 架构设计决策

### 2.1 ThreadPool: 工作窃取 + 完整 Hill Climbing

```
┌─────────────────────────┐
│     GlobalQueue         │  ← 锁保护 FIFO（外部入队）
│   std::deque<WorkItem>  │
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

**Hill Climbing 状态机**:

```
Warmup → ClimbExplore → Climbing → Stabilizing → Steady
```

| 状态 | 行为 |
|------|------|
| Warmup | 初始斜坡，每 500ms +1 线程到 min 配置 |
| ClimbExplore | 注入 2 线程波，测 gain（throughput diff / thread delta） |
| Climbing | gain > 5 时每周期 +1 线程 |
| Stabilizing | gain < -5 时回退 wave 线程数 |
| Steady | 保持线程数，throughput 下降 50% 时重新 ClimbExplore |

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

### 2.3 Monitor: 自适应自旋 + SyncBlock 池

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

**文件**: `src/native/runtime-core/core/monitor.cpp`, `sync_mutex.cpp`

### 2.4 ExecutionContext: bump arena

- 每个线程维护 TLS bump arena（~1KB）
- 小型 EC（≤4 AsyncLocal）→ bump arena（~1-2ns）
- 溢出 → 堆分配（~50-100ns）
- Task 开始/结束时 arena reset

**文件**: `src/native/runtime-core/execution_context.cpp`

### 2.5 Task inlining

- `TaskWait()` 检测 task 是否已调度
- 未调度 → `TryInlineTask(task)` 在当前线程执行
- 已调度/失败 → fallback blocking wait
- 递归内联检测防止栈溢出

**文件**: `src/native/runtime-core/task_runner.cpp`

### 2.6 平台特定等待基元

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

| 场景 | 优化前 | 优化后 | 提升 |
|------|--------|--------|------|
| 单线程入队 | 33 K ops/s | ~80 K ops/s | **2.4x** |
| 8 worker, 100K tasks | — | ~130 K ops/s | — |
| 16 worker stress | — | 无退化 | — |

### 3.2 WaitHandle 延迟

| 操作 | 优化前 | 优化后 | 提升 |
|------|--------|--------|------|
| WaitOne (2048 句柄) | O(n)~6 K/s | O(1)~40 K/s | **6.7x** |
| WaitAny (8 句柄) | — | 1.55x | — |
| WaitAll (8 句柄) | — | 1.38x | — |
| Set (并发) | 全局锁 | 句柄级锁 | **无锁争用** |

### 3.3 Monitor 竞争

| 场景 | 优化前 | 优化后 | 提升 |
|------|--------|--------|------|
| 递归进入 | CAS~15 cycles | load~2 cycles | **7.5x** |
| 中等竞争 | 固定 1000 次自旋 | 自适应 128 次 | **节省 ~3us** |
| SyncBlock 膨胀 | 堆分配 | bump 池分配 | **零分配** |

### 3.4 futex/ulock（平台相关）

| 基元 | 等待延迟 | 提升对比 |
|------|---------|---------|
| `std::condition_variable` (pthread) | ~2-5 us | baseline |
| Linux `futex` | ~100 ns | **20-50x** |
| iOS `__ulock_wait` | ~100 ns | **20-50x** |

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
| EC bump arena | 减少热更路径堆分配 | 低 |
| GC safepoint + TP | GC 在 interpreter 线程也能及时暂停 | 低 |

---

## 5. 测试与验证

### 5.1 基线通过率

| 测试 | 结果 |
|------|------|
| threading-tasks-primitives fact mode | **16/16** ✅ |
| threading-monitor-interlocked fact mode | **15/15** ✅ |
| stress 4 threads 5000 iters | **16/16** ✅ |
| stress 8 threads 5000 iters | **16/16** ✅ |
| stress 16 threads 5000 iters | **16/16** ✅ |

### 5.2 验收标准

| 项目 | 标准 | 当前状态 |
|------|------|---------|
| AOT fact tests (15/15 + 16/16) | 全部通过 | ✅ |
| WaitHandle storm ≥ 80K ops/sec | 通过 | ✅ |
| ThreadPool 8 worker, 100K tasks | ≤ 1/3 原时间 | ✅ |
| Benchmark 模式无崩溃 | 安全处理异常 | ✅ |
| GC 暂停 ≤ 100ms | 通过 | ✅ |

---

## 6. 关键 API 与文件映射

| 组件 | API | 文件 |
|------|-----|------|
| ThreadPool 初始化 | `ThreadPoolInitialize()` | `thread_pool.cpp` |
| ThreadPool 入队（安全） | `ThreadPoolQueueUserWorkItem(cb, ctx)` | `thread_pool.cpp` |
| ThreadPool 入队（不安全） | `ThreadPoolQueueUserWorkItemUnsafe(cb, ctx)` | `thread_pool.cpp` |
| Hill Climbing 状态机 | `HillClimbingController::OnGateTick()` | `thread_pool.cpp` |
| WaitHandle 创建 | `WaitHandleCreate(initial_state, type)` | `wait_handle.cpp` |
| WaitHandle 等待 | `WaitHandleWaitOne(id, timeout_ms)` | `wait_handle.cpp` |
| WaitHandle 多等待 | `WaitHandleWaitAny/WaitAll(ids, count, timeout)` | `wait_handle.cpp` |
| Monitor 进入 | `MonitorEnter(obj, tid)` | `core/monitor.cpp` |
| Monitor 退出 | `MonitorExit(obj, tid)` | `core/monitor.cpp` |
| SyncBlock 分配 | `AllocateSyncBlockFromPool()` | `core/sync_mutex.cpp` |
| Task.Run | `TaskRun(delegate_fn)` | `task_runner.cpp` |
| ExecutionContext 捕获 | `ExecutionContextCapture()` | `execution_context.cpp` |
| GC safepoint 检查 | `SafepointPoll()` | `thread_state.cpp` |

### 6.1 ThreadPool WorkerLoop 优先级策略

Worker 线程获取工作的优先级：

1. **Local Queue (LIFO)** — 当前 worker 的本地队列顶部，无锁操作
2. **Steal from Random Victim** — 从随机 worker 的队列底部窃取
3. **Global Queue** — 锁保护的全局 FIFO
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
| 1D | Monitor 递归 CAS 跳过 | `core/monitor.cpp` | ✅ |
| 1E | SyncBlock 预分配池 | `core/sync_mutex.cpp` | ✅ |
| 2A | ThreadPool 工作窃取 | `thread_pool.cpp` | ✅ |
| 2B | Hill Climbing 完整状态机 | `thread_pool.cpp` | ✅ |
| 2C | UnsafeQueueUserWorkItem | `thread_pool.cpp` | ✅ |
| 2D | ThreadPool 空闲回收 | `thread_pool.cpp` | ✅ |
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
| ReaderWriterLock 改造 | 非瓶颈，使用极少 |
| SyncBlock deflation | CoreCLR 实际不执行 |
| 跨进程 WaitHandle | 超出本次范围 |
