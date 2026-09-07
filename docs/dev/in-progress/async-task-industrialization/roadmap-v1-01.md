# System.Threading.Tasks 生产级工程 — Roadmap

> 版本：v1-01 | 创建：2026-09-07
> 上游：brainstorm（手工状态机方案确认、C++20 coroutine 排除）
> 定位：跨 4 域（运行时 + codegen 翻译 + 翻译管线 + 验证覆盖）的多阶段工程

## 一、目标

将 C# `System.Threading.Tasks`（Task、async/await、ThreadPool、组合子、Parallel）在 chaos-il2cpp 的 AOT 翻译管线中达到**可生产级质量**，具体包括：

1. 所有 `async Task` 状态机的 IL 被正确翻译为 **native C++ 手工状态机**（非 C++20 coroutine 占位）
2. `AsyncTaskMethodBuilder`、`AsyncValueTaskMethodBuilder` 的 builder 契约完整实现
3. `ThreadPool` 的 native 线程池（`thread_pool.cpp`）投入真实使用，生命周期由 `RuntimeInit` 管理
4. `ExecutionContext`/`SynchronizationContext` 跨 await 流正确
5. `TaskCompletionSource`/`Task.Delay`/`Task.WhenAll`/`Task.WhenAny` 等组合子完整
6. 热更新路径支持状态机方法替换
7. 性能：AOT vs .NET8 差距 < 2×
8. foundation-dll fact 验证覆盖 async/Task 核心方法

## 二、范围边界

### 包含
- `Task` / `Task<T>` / `ValueTask` / `ValueTask<T>` 的完整生命周期
- `async/await` 状态机 → native C++ 手工状态机翻译
- `AsyncTaskMethodBuilder` / `AsyncValueTaskMethodBuilder` builder 契约
- `ThreadPool.QueueUserWorkItem` + 线程池原生实现
- `Task.Run` / `Task.Factory.StartNew`
- `Task.Delay`（timer 集成）
- `TaskCompletionSource<T>`
- `Task.WhenAll` / `Task.WhenAny`
- `ConfigureAwait(false)` / `ConfigureAwait(true)`
- `ExecutionContext` 跨 await 传播
- `SynchronizationContext` 调度
- `Parallel.For` / `Parallel.ForEach` / `Parallel.Invoke`
- `AsyncLocal<T>`
- 热更新：状态机方法替换支持
- foundation-dll fact 验证

### 不含（非目标）
- `IAsyncEnumerable<T>` / `await foreach`（Phase 2 后考虑）
- `IAsyncDisposable` / `await using`（Phase 2 后考虑）
- `Pipelines` / `System.IO.Pipelines`（独立域）
- `Channel<T>` / `System.Threading.Channels`（独立域）
- `TaskScheduler` 自定义调度器（仅默认 TaskScheduler）
- `ManagedThreadPool` 替代（保持 native 线程池唯一）

## 三、三优先级权衡

| 优先级 | 本任务的权衡结论 |
|--------|----------------|
| **P1 性能最优** | 手工状态机翻译（非 C++20 coroutine）—— 精确控制 Frame 分配、可复用 GC 对象、无 pinned 碎片 |
| **P2 架构完美** | 忠实于 Roslyn 的 IL 状态机结构，零信息损失；运行时组件职责清晰（state machine / builder / awaiter / thread pool） |
| **P3 热更适配** | 手工状态机方案天然支持方法替换（`MoveNext` 函数指针可重定向、状态机 struct 可替换） |

## 四、阶段列表

### Phase 1：运行时基础设施闭环（4 周）

| 目标 | 内容 |
|------|------|
| **goal** | 补齐 native 运行时 Runable 所需的全部桥梁，使 ThreadPool 活起来、状态机在 native 侧可执行 |
| **exit_criteria** | · `ThreadPoolInitialize` 由 `RuntimeInit` 调用，worker 线程存活<br>· `AsyncStateMachineBox` native 等价物实现（Task 子类 + continuation 容器 + EC 保存）<br>· `TaskAwaiter.UnsafeOnCompleted` → native 回调路径接通<br>· `ExecutionContext.Capture/Run` 的 native 实现<br>· 单元测试覆盖各组件 |
| **dependencies** | 无（独立，只依赖现有 runtime-core） |
| **resolved_decisions** | · thread_pool.cpp 的 hill-climbing 和 work-stealing 全部保留，只加启动点<br>· ASyncStateMachineBox 用 GC 堆对象（不用 pinned）<br>· awaiter 回调用函数指针 + 上下文指针，不用 `std::function` |
| **watch_items** | · ThreadPool 首次启动时的线程配置<br>· EC 捕获的性能开销 |

### Phase 2：状态机翻译引擎（6 周）

| 目标 | 内容 |
|------|------|
| **goal** | 替换 `AsyncCoroutineEmitter.cs` 的占位 coroutine 发射，实现真正的 IL 状态机 → native C++ 翻译 |
| **exit_criteria** | · `MoveNext()` IL → C++ 状态机代码翻译器（`BuildAsyncStructuredBody` 的 IR 被嵌入生成）<br>· `<>t__builder.Start` 翻译为 `AsyncTaskMethodBuilder.Start` 的 native 调用<br>· `builder.AwaitUnsafeOnCompleted` 翻译为 awaiter 注册 + task 续列<br>· `builder.SetResult/SetException` 翻译正确<br>· `ConfigureAwait(true/false)` 分支处理<br>· 首条端到端 async/await 在 AOT 中执行通过 |
| **dependencies** | Phase 1（运行时桥接就绪后才能测） |
| **resolved_decisions** | · 不反推 await 链，直接翻译 IL 状态机 switch + goto<br>· `MoveNext` 作为普通成员函数，可被 hotupdate 替换<br>· awaiter 结构体保持 IL 布局 |
| **watch_items** | · 异常传播路径正确性<br>· 栈深度 vs 递归续列 |

### Phase 3：组合子 + 完整 async 语义（4 周）

| 目标 | 内容 |
|------|------|
| **goal** | 补齐 `TaskCompletionSource`、`Task.Delay`、`WhenAll`/`WhenAny`、`ExecutionContext`/`SynchronizationContext` 流、`AsyncLocal` |
| **exit_criteria** | · `TaskCompletionSource<T>` 完整（SetResult/SetException/TrySet*/Task 属性）<br>· `Task.Delay` 基于 timer_queue 的 native 实现<br>· `Task.WhenAll` / `Task.WhenAny` 正确（任意 task 数）<br>· `ExecutionContext` 跨 await 自动捕获/恢复<br>· `SynchronizationContext` 调度（Post/Send 到上下文）<br>· `ConfigureAwait(true)` 走 SyncCtx，`ConfigureAwait(false)` 直入线程池<br>· `AsyncLocal<T>` 值跨 await 保持 |
| **dependencies** | Phase 2（状态机翻译就绪后才有"跨 await"概念） |
| **resolved_decisions** | · EC 用 thread-local 栈保存，不分配 heap（fast path）<br>· SyncCtx 检测用 `SynchronizationContext.Current != null`，不为 null 且 not default 才 Post |
| **watch_items** | · AsyncLocal 的 undo 机制<br>· EC 嵌套深度 |

### Phase 4：Parallel / 高级并发（3 周）

| 目标 | 内容 |
|------|------|
| **goal** | `Parallel.For/ForEach/Invoke` 的完整 AOT 翻译 + ThreadPool 的 hill-climbing 调优 |
| **exit_criteria** | · `Parallel.For/ForEach/Invoke` 的 AOT 执行正确<br>· `ParallelLoopState.Break/Stop` 正确<br>· `ParallelOptions` 支持<br>· ThreadPool hill-climbing 在本机负载下调优验证 |
| **dependencies** | Phase 1（ThreadPool 就绪） |
| **resolved_decisions** | · Parallel 用 native 线程池 + 本地 work-stealing<br>· 不实现 `ParallelLinqOptions`（PLINQ 独立域） |
| **watch_items** | · Parallel 的 partitioner 策略<br>· 与 BGC 的交互性能 |

### Phase 5：性能优化（3 周）

| 目标 | 内容 |
|------|------|
| **goal** | 达到 < 2× .NET8 的 AOT 性能目标 |
| **exit_criteria** | · 状态机 Frame 分配优化（复用池化）<br>· `Task` 对象分配优化（size 优化 + 复用）<br>· `MoveNext` 内联策略<br>· awaiter 链式续列优化（InlineIfPossible 路径）<br>· end-to-end benchmark 达到 < 2× .NET8 |
| **dependencies** | Phase 2-3（语义完整后才能优化） |
| **resolved_decisions** | · 先保语义，再优化<br>· 优化不改语义，只改分配/内联/路径 |
| **watch_items** | · 优化是否损害 hotupdate 能力 |

### Phase 6：验证覆盖 + 热更新验证（3 周）

| 目标 | 内容 |
|------|------|
| **goal** | foundation-dll fact 验证 + hotupdate 场景验证 |
| **exit_criteria** | · `System.Threading.Tasks` 全 family 在 foundation-dll 中建立（含 `threading-tasks` chunk 的 fact 通过）<br>· 核心 async 模式（Task.Run、await、ConfigureAwait、WhenAll、Delay、Tcs）各有 fact 覆盖<br>· hotupdate 替换 async 状态机方法的测试通过 |
| **dependencies** | Phase 2-4（语义完整后才能验证） |
| **resolved_decisions** | · 用 `System.Private.CoreLib` 的现有 `threading-tasks` chunk 不作新 family<br>· hotupdate 测试用已有的 patch 框架 |
| **watch_items** | · 验证数据的长期稳定性 |

## 五、子任务映射

| task_id | phase | status | purpose | depends_on | estimated_effort |
|---------|-------|--------|---------|------------|-----------------|
| ASYNC-P1-1 | Phase 1 | planned | ThreadPool 生命周期接入 RuntimeInit | — | 小 |
| ASYNC-P1-2 | Phase 1 | planned | AsyncStateMachineBox 的 native 等价物 | — | 中 |
| ASYNC-P1-3 | Phase 1 | planned | awaiter 回调桥接（UnsafeOnCompleted → native） | — | 中 |
| ASYNC-P1-4 | Phase 1 | planned | ExecutionContext capture/restore native 实现 | — | 中 |
| ASYNC-P1-5 | Phase 1 | planned | 单元测试（Phase 1 各组件） | ASYNC-P1-1~4 | 小 |
| ASYNC-P2-1 | Phase 2 | planned | 替换 AsyncCoroutineEmitter 为真实状态机翻译 | ASYNC-P1-3 | 大 |
| ASYNC-P2-2 | Phase 2 | planned | builder 契约翻译（Start/AwaitUnsafeOnCompleted/SetResult） | ASYNC-P2-1 | 中 |
| ASYNC-P2-3 | Phase 2 | planned | ConfigureAwait 分支处理 | ASYNC-P2-1 | 中 |
| ASYNC-P2-4 | Phase 2 | planned | 端到端 async/await 验证 | ASYNC-P2-1~3 | 小 |
| ASYNC-P3-1 | Phase 3 | planned | TaskCompletionSource 实现 | ASYNC-P2-4 | 中 |
| ASYNC-P3-2 | Phase 3 | planned | Task.Delay + timer 集成 | ASYNC-P2-4 | 中 |
| ASYNC-P3-3 | Phase 3 | planned | Task.WhenAll/WhenAny | ASYNC-P2-4 | 中 |
| ASYNC-P3-4 | Phase 3 | planned | ExecutionContext 自动流 | ASYNC-P1-4 | 中 |
| ASYNC-P3-5 | Phase 3 | planned | SynchronizationContext 调度 | ASYNC-P3-4 | 中 |
| ASYNC-P3-6 | Phase 3 | planned | AsyncLocal 实现 | ASYNC-P3-4 | 中 |
| ASYNC-P3-7 | Phase 3 | planned | 组合子集成测试 | ASYNC-P3-1~6 | 小 |
| ASYNC-P4-1 | Phase 4 | planned | Parallel.For/ForEach/Invoke 翻译 | ASYNC-P1-1 | 中 |
| ASYNC-P4-2 | Phase 4 | planned | ThreadPool hill-climbing 调优验证 | ASYNC-P1-1 | 中 |
| ASYNC-P5-1 | Phase 5 | planned | 状态机 Frame 分配优化 | ASYNC-P2-4 | 中 |
| ASYNC-P5-2 | Phase 5 | planned | Task 对象分配优化 | ASYNC-P3-7 | 中 |
| ASYNC-P5-3 | Phase 5 | planned | end-to-end 性能基准 | ASYNC-P5-1~2 | 小 |
| ASYNC-P6-1 | Phase 6 | planned | foundation-dll fact 覆盖 | ASYNC-P2-4 | 中 |
| ASYNC-P6-2 | Phase 6 | planned | hotupdate 场景验证 | ASYNC-P2-4 | 中 |

## 六、依赖拓扑

```
Phase 1（运行时基础设施）— 独立，可优先启动
  │
  ▼
Phase 2（状态机翻译引擎）— 依赖 Phase 1
  │
  ├──▶ Phase 3（组合子语义）— 依赖 Phase 2
  │       │
  │       └──▶ Phase 5（性能优化）— 依赖 Phase 2+3
  │
  └──▶ Phase 4（Parallel）— 依赖 Phase 1，与 Phase 2 并行
  │
  └──▶ Phase 6（验证覆盖）— 依赖 Phase 2+3+4
```

## 七、风险

| 风险 | 等级 | 缓解措施 |
|------|------|---------|
| ThreadPool 的 hill-climbing 参数在当前硬件上不合适 | 中 | Phase 1 添加可 env 覆盖的默认配置，Phase 4 实机调优 |
| `ExecutionContext` 的捕获性能超预期（每次 awaiter 注册都 capture） | 中 | Phase 1 先做简单 capture，Phase 5 优化（脏检查 + 缓存） |
| 状态机翻译器的 IL→C++ 转换复杂度高于预期（异常边界、try/catch 中的 await） | 高 | Phase 2 先从最简模式（无 try/catch 的线性 await）开始，逐步迭代 |
| Hotupdate 替换状态机时，正在运行的 `MoveNext` 如何安全替换 | 中 | hotupdate 只在 safepoint 进行，参考已有的 patch 框架 |
| foundation-dll 的 `threading-tasks` chunk 中某些方法 lowering 失败 | 中 | 逐方法 fallback 到解释器，先建立可 work 的 subset |

## 八、备选路径

- 若某个 Phase 2 的 IL 模式无法翻译：回退到当前 `ChaosExternalRuntimeFallback` 解释器路径（但不标记为完成）
- 若 EC 性能达不到要求：提供 `ExecutionContext.SuppressFlow()` 在 hot path 上跳过 capture
- 若 ThreadPool hill-climbing 不稳定：退化为固定线程数（`CHAOS_GC_MaxThreadPoolThreads` 环境变量）

## 九、当前建议推进顺序

**从 Phase 1 开始，串行推进。** Phase 1 无前置依赖且独立于 codegen，可立即启动。

## 十、子任务执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
recommended_next_child: ASYNC-P1-1
```

## 十一、调度策略

`sequential` — 所有子任务串行推进，每次一个。Phase 2 完成后，Phase 3/4 可并行，届时再评估是否需要切换 `hybrid`。

## 十二、关键文档

- `docs/dev/in-progress/async-task-industrialization/roadmap-v1-01.md`（本文件）
- `docs/dev/in-progress/async-task-industrialization/STATUS.md`