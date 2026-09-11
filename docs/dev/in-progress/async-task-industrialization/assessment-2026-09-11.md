# System.Threading.Tasks 评估报告

> 报告日期：2026-09-11 | 对比基线：.NET 8 production Task 完整语义

---

## 一、执行摘要

**总体结论：async/Task AOT 翻译处于"基础设施就绪、语义严重不完整"状态。**
- **已完成**：运行时基础设施（线程池、EC、续列协议、TaskSource、CancellationToken）、codegen 状态机结构化发射（含 EH）、ShapeRegistry 接线（~20 native→managed API）
- **未完成**：~85% Task API surface 仍走 `ChaosExternalRuntimeFallback` → 0 解释器 stub；**已实现的部分，tier-2 quality（有"能跑通"的证据，但无真值正确性验证）**
- **红线阻塞**：commit `1d77bbc5e`（ci-r7-clean 恢复）**静默抹除**了 P3-3（WhenAll/WhenAny）、P3-2（timer-backed Task.Delay）、P3-4（EC auto-flow）的 native 实现。STATUS.md 列为"landed"的代码在 HEAD **不存在**。
- **生产级合格度估值**：~25%（基础设施质量好，但覆盖太少 + 回归未察觉）

---

## 二、已实现的——真正生产的（可信部分）

### 2.1 运行时基础设施（Q: A-）
| 组件 | 文件 | 评估 |
|------|------|------|
| AsyncTask 原子语义 | `async.h:42-60` | `std::atomic<bool>` 正确；lost-wakeup 防止(两次有序检查)；Re-check after publish |
| finish_async_task | `async.h:81-104` | 正确的 acquire-release 序，内存序推理符合 C++ memory model |
| ThreadPool | `thread_pool.cpp` | work-stealing、hill-climbing、gate thread 架构完整；由 `RuntimeInit:80` 调用 |
| ExecutionContext | `execution_context.cpp:318` | Capture/Run/Free/SuppressFlow 完整；smoke 11/11 |
| Task.Run (native) | `task_runner.cpp:28-47` | 端到端实现（AsyncTask+EC capture+ThreadPool+完成发布+续列），注册于 RuntimeInit |
| CancellationToken | `cancellation_token.cpp:222` | 表驱动(256 sources/1024 registrations)，timer-backed auto-cancel, first-cancel-wins |
| TimerQueue | `timer_queue.cpp:335` | min-heap + gate thread (~15ms tick) |
| TaskSource (TCS native) | `async.h:391-486` | set_result/set_exception/try_set_{result,exception,canceled}，CTest 4/4 |

### 2.2 Codegen 状态机发射（Q: B+）
| 特性 | 文件/commit | 评估 |
|------|------------|------|
| MoveNext 结构化发射 | `MethodEmission.cs:232-249` | 复用 TryBuildStructuredMethodBody + EH 5-shape；不 C++20 coroutine |
| Suspend return | `StructuredIR.Emit.cs:1685` | AwaitUnsafeOnCompleted 后插 IRReturn，阻止 fall-through SetResult(0) |
| ABI box pointer 修复 | `1e52a419d` + `90bc27f86` | 传 `chaos_locals[N]`（box 持久指针），不是 `&chaos_locals[N]`（栈槽地址）；9 单元测试验证 |
| builder/awaiter native 接线 | `S16.cs:182-380` | 6 ops → `async_task_builder_*` |
| self-call 假阳性修复 | uncommitted diff `LinearEmission.Advanced.cs:345-358` | async Main 双 dispatch entry 不触发 CHAOS_IL2CPP_FAIL |

### 2.3 Native 冒烟测试（Q: A-）
- 22 CTest，覆盖：TaskRun 多线程/高并发、Builder SetResult/SetException 跨线程、Awaiter poll、Continuation exactly-once、HandCrafted 状态机往返、TaskSource 三态、Yield 跨线程往返
- **关键缺失**：无 Task.Delay 真实计时测试、无 CancellationToken 测试、无 EC/AsyncLocal 测试、无 GC stress 测试

---

## 三、关键回归（STATUS.md 不实）

commit `1d77bbc5e` ("fix(ci): restore full src/cmake/contracts trees from ci-r7-clean") 用 ci-r7-clean 分支的内容覆盖了 src/，**抹除了已经合入 main 的 P3 实现**。`9765888f9` WIP commit 补回了简化 stub 但未恢复完整实现。

| 丢失内容 | STATUS 声称 | HEAD 事实 | 证据 |
|---------|-----------|----------|------|
| `async_stubs.cpp` 中的 WhenAll/WhenAny 实现 | "P3-3 landed on main (822933575)" | **不存在**。159 lines (HEAD) vs 333 lines (feccdb166 已合入) | `grep chaos_task_when_all async_stubs.cpp` → 0 hits |
| `async_stubs.h` 中的 WhenAll/WhenAny/TimerDelay 声明 | "P3-3/2 landed" | **不存在**。46 lines (HEAD) vs 53 lines (feccdb166) | Header 不含 `when_all`/`when_any`/`delay_stub` |
| TimerQueue-backed Task.Delay | "P3-2 landed (4bb252875)" | **不存在**。`ChaosAsyncTaskDelay` 立即 `finish_async_task`，不做定时延迟 | 注：WIP commit 754f 加回了 stub 形式的 Delay |
| `S16.cs` WhenAll/WhenAny 注册 | "P3-5a landed (feccdb166)" | **不存在**。`grep -c WhenAll` → 0 (feccdb166: 4) | S16 799 lines (HEAD) vs 805 lines |
| `task_runner.cpp` ExecutionContext 自动流 | "P3-4 landed (5c540b3a9)" | **不存在**。`AsyncContinuationDispatch` 无 EC capture/run/free | 98 lines (HEAD) vs 121 lines |
| 4 个 WhenAll/WhenAny CTest | "native smoke 26/26" | **不存在**。22 tests at HEAD (feccdb166: 26+) | `grep WhenAll smoke_test.cpp` → 0 hits |

### 修复建议
恢复被覆盖的内容：`git checkout feccdb166 -- src/native/runtime-core/runtime_stubs/` `src/native/runtime-core/task_runner.cpp` `src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs` `tests/unit/runtime-native/runtime-core/threading/async_integration_smoke_test.cpp`，然后叠加 `9765888f9` 的新增（ChaosAsyncTaskDelay stub 带参数验证）。需要先确认 feccdb166 中的内容与 `9765888f9` 兼容。

> ⚠️ **本轮不修上述回归**——此为事实报告。回归修复是独立的下一轮工作。

---

## 四、未实现的 Task API surface

### 4.1 已有 native 实现但 codegen 未注册（低成本高收益）

| API | Native 状态 | 修复成本 |
|-----|-----------|---------|
| `Task.Run(Func<Task>)` × 4 overload | `task_runner.cpp` 完整实现，`RuntimeInit` 已注册 `g_async_task_run_fn` | **小** — 加 1 条 registry entry |
| `CancellationToken`/`CancellationTokenSource` 相关 API | `cancellation_token.cpp` 表驱动实现完整 | **中** — 对象模型 + registry |
| `ThreadPool.QueueUserWorkItem` × 2 overload | `thread_pool.cpp` 已暴露 `ThreadPoolQueueUserWorkItemUnsafe` | **小** — 加 registry wrapper |

### 4.2 无 native 实现（需运行时侧开发）

| API | 优先级 | 估计工作量 |
|-----|--------|-----------|
| `Task.WhenAll(Task[])` / `WhenAny(Task[])` | 🔴 P0 | **中** — 原实现被抹除，恢复即可 |
| `Task.Wait()` / `WaitAll()` / `WaitAny()` | 🔴 P0 | **大** — 需 SpinWait + 阻塞续列 |
| `Task<T>.Result` | 🔴 P0 | **大** — 同 Wait + InlineIfPossible |
| `Task.ConfigureAwait(bool)` | 🟡 P1 | **大** — SyncCtx.Post/Send 需 native |
| `Task.FromResult<T>()` / `FromException<T>()` | 🟡 P1 | **小** — AsyncTask wrap |
| `Task.ContinueWith()` | 🟡 P1 | **大** — 续列组合子 |
| `TaskScheduler` / `SynchronizationContext` | 🟡 P1 | **很大** — 新架构组件 |
| `ValueTask<T>` 本身（非 builder ops） | 🟡 P1 | **中** — 对象模型 + 快速路径 |
| `Task.Factory.StartNew()` | 🟢 P2 | **小** — Task.Run 包装 |
| `Parallel.For/ForEach/Invoke` | 🟢 P2 | **大** — partitioner + native 实现 |
| `IAsyncEnumerable<T>` | 🔵 P3 | **很大** — 新编译器变换 |
| `IAsyncDisposable` / `await using` | 🔵 P3 | **中** — 新语言模式 |
| `AsyncLocal<T>` 跨 await 自动流 | 🟡 P1 | **中** — EC 已实现，需 `ConfigureAwait` 联动 |

### 4.3 异常语义缺口

| 语义 | 现状 | 影响 |
|------|------|------|
| `AggregateException` unwrap | 仅有独立 subject 测试，不接入 Task fault → await throws | await 一个 failed task 不会抛异常 |
| `Task.Exception` property | 无 registry + 无 native | 无法从外部检查异常 |
| `UnobservedTaskException` | 无 | 沉默吞异常 |
| `OperationCanceledException` | cancellation_token.cpp 不被 wire 到 Task fault | `CancellationToken` 参与的操作不会正确 fault |
| `async void` 异常传播 | 未验证（仅有 AsyncVoidMethodBuilder ops） | 行为未定义 |
| Task fault 的 exception 类型 | native 侧只存 intptr，无类型区分 | 无法区分"cancelled" vs "faulted" vs "result available" |

### 4.4 Verification 缺口（foundation-dll 覆盖）

| Chunk | 状态 |
|-------|------|
| `threading-tasks` | **build broken**（`chaos_tcs_*` 1-arg vs 2-arg C2660）；AOT 456/456 passed 含 321 UNVERIFIED smoke |
| `System.Threading.Tasks.Parallel` | 107/107 "passed" 但 全 42L stub，验证了零 |
| `Task.Run` / `WhenAll` / `Task.Yield` / `TCS` 等 | **无** subject |

---

## 五、生产级合格度差距

| 维度 | 等级 | 说明 |
|------|------|------|
| **语义完整性** | **20%** | ~85% production Task API 不可达；可达的 yield/delay 是 stub（IsCompleted 恒 1，GetResult 恒 0） |
| **正确性验证** | **25%** | 22 CTest 质量高，但无一验证真实 async/await round-trip 的返回值正确性；codegen 测试只做文本断言 |
| **性能** | **0%** | 无 benchmark；`ChaosAsyncTaskAwaiterGetIsCompleted→1` 使状态机永不真挂起，性能数据不具实际意义 |
| **稳定性** | **20%** | ThreadPoolWorkerCount data race；`s_initialized` 不重置；AsyncTask 内存泄漏 |
| **GC 安全性** | **15%** | AsyncTask plain `new` 永不释放；GA 无法追踪；跨线程 UAF 可能 |
| **异常路径** | **5%** | 无 AggregateException、无 OperationCanceledException→Task fault 接线、无 UnobservedTaskException |
| **HotUpdate** | **10%** | slot mapping bug 未解决（`ApplyPatchFromMemoryEx` 返回 null，无 context root cause） |
| **验证覆盖** | **15%** | foundation-dll threading-tasks chunk build broken；无真实 Task API 的 fact |

### 到 production 还需要的工作量（估计）

| 阶段 | 工作量 | 产出 |
|------|--------|------|
| **P0 fix** — 恢复 clobbered P3 代码 + 修复 ThreadPool+s_initialized+TCS C2660 | 1-2天 | HEAD baseline 回到 STATUS 声称状态 |
| **P0 extend** — `Task.Run` registry + `CancellationToken` registry + `Task.Delay` 真 timer | 2-3天 | 5 个最常用 Task API 可用 |
| **P1 core** — `Task.Wait/Result` + `ConfigureAwait(false)` + `Task.WhenAll/WhenAny` + `Task.FromResult` | 1-2周 | ~40% production API surface |
| **P1 exception** — 异常传播到 await throws + AggregateException 包装 | 1周 | 异常行为正确 |
| **P1 performance** — AsyncTask GC ownership + Frame 池化 + benchmark baseline | 2周 | 可开始测量性能 |
| **P2 full** — SyncCtx + Parallel + ValueTask + ContinueWith | 3-4周 | ~80% surface |
| **P3 long tail** — AsyncLocal 自动流 + IAsyncEnumerable + hotupdate 验证 | 4-6周 | ~95% surface |
| **P3 verification** — foundation-dll fact × 20+ + GC stress + have benchmark < 2× .NET 8 | 3周 | 可生产声明 |
| **合计** | ~14-18 周 | production quality |

主风险是并行约束：P1/P2 的大部分依赖于恢复 P3-3/2/4 的 clobbered 代码（那个是 2 天的工作，不应卡其他项）。

---

## 六、原生运行时质量审计（独立于覆盖范围）

### 6.1 好的设计决策
1. **Atomic ordering 正确** — `finish_async_task` 的 lost-wakeup 防止是 industry standard double-checked locking；
2. **EC 零成本 fast path** — 无需分配 heap 时返回 nullptr；
3. **CancellationToken 回调在锁外运行** — 正确防止回调重入死锁；
4. **TaskSource 统一 TCS 模型** — `try_set_*` 用 `compare_exchange_strong` 保证 exactly-once 胜利；
5. **Finish_async_task 两次检查** — has_continuation 检查在 claim continuation_fired 之前，正确消除竞态窗。

### 6.2 真实缺陷（与覆盖无关）

| # | 缺陷 | 文件:行 | 严重度 |
|---|------|---------|--------|
| D1 | **AsyncTask 生命周期泄漏** — `async_task_create` 用 plain `new`，永不 `delete`。每个 `Task.Run/Delay/TCS` 分配随进程常驻。GC 堆不追踪此分配。 | `async.h:139` | 🔴 HIGH |
| D2 | **ThreadPoolWorkerCount data race** — `s_workers.size()` 在无锁下读取(`thread_pool.cpp:790,186`)，而被 `s_mutex` 守卫写入 | `thread_pool.cpp:790` | 🟡 MED |
| D3 | **s_initialized 不重置** — `ThreadPoolShutdown` 后无法重新初始化；同进程热更场景必崩 | `thread_pool.cpp:641` | 🟡 MED |
| D4 | **EC partial-install on OOM** — `EnsureHeapCapacity` 分配失败时静默跳过上下文安装 | `execution_context.cpp:220-227` | 🟡 MED |
| D5 | **TaskAwaiter.GetResult 恒 0** — 无异常传播；`await Task<T>` 返回 0 | `async_stubs.cpp:91,100` | 🟡 MED |
| D6 | **无 Cancel→Task fault 接线** — `CancellationTokenSourceCancel` 不 fault 关联的 AsyncTask | `cancellation_token.cpp:156-160` | 🟢 LOW |
| D7 | **Yield 恒同步完成** — 注册 dispatcher 后 0（正确），但 stub 写死 1（错） | `async_stubs.cpp:29` | 🟡 MED |
| D8 | **ContinuationData heap 递** — `AsyncStateMachineContinuationData` 每个 await 分配，无池化 | `async.h:291-294` | 🟢 LOW |

---

## 七、附录：关键文件索引

| 角色 | 路径 | 说明 |
|------|------|------|
| 原生 async 并发基石 | `src/native/common/chaos/async.h:488` | AsyncTask + builder + awaiter + combinator + TCS |
| 线程池 | `src/native/runtime-core/thread_pool.cpp:793` | work-stealing + hill-climbing + gate thread |
| 任务运行器 | `src/native/runtime-core/task_runner.cpp` | Task.Run + ECM 续列派发（HEAD 缺 EC flow） |
| EC + AsyncLocal | `src/native/runtime-core/execution_context.cpp:318` | Capture/Run/Free/SuppressFlow/AsyncLocal |
| CancellationToken | `src/native/runtime-core/cancellation_token.cpp:222` | 表驱动 CTS |
| TimerQueue | `src/native/runtime-core/timer_queue.cpp:335` | min-heap + gate thread |
| async extern stubs | `src/native/runtime-core/runtime_stubs/async_stubs.cpp` | registry entry points（HEAD 缺 WhenAll/WhenAny/Delay） |
| ShapeRegistry async 接线 | `src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs` | ~20 entry points |
| codegen 状态机发射 | `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.MethodEmission.cs:206-249` | MoveNext → structured IR |
| codegen ABI 修复 | `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.Helpers.cs:82-198` | `IdentifyAsyncBoxPointerLocalSlots` |
| Rollback 残留死代码 | `src/managed/Chaos.IL2CPP.Generator/Emission/AsyncCoroutineEmitter.cs` | 仅 `IsAsyncStateMachineMoveNext` + `ClassifyAsyncMethod` 存活 |
| 全死代码 | `src/managed/Chaos.IL2CPP.Generator/Emission/AsyncAwaiterCatalog.cs` | 零引用 |
| 测试：codegen 文本断言 | `tests/unit/managed/codegen/AsyncPipelineTests.cs` | 9 tests（0 编译 C++） |
| 测试：async native 冒烟 | `tests/unit/runtime-native/runtime-core/threading/async_integration_smoke_test.cpp` | 22 CTest（HEAD 缺 WhenAll/WhenAny/Delay tests） |
| 状态文档（不准确） | `docs/dev/in-progress/async-task-industrialization/STATUS.md` | 声称 P3-3/4/5a land，HEAD 无对应实现 |
| Roadmap | `docs/dev/in-progress/async-task-industrialization/roadmap-v1-01.md` | 6 Phase 架构设计（仍权威） |
| Hotupdate 交接 | `docs/dev/in-progress/async-task-industrialization/handoff-2026-09-10-hotupdate-slot-mapping.md` | slot mapping bug |
| 当前 nightly threading-tasks 结果 | `tests/e2e/nightly-build-report/logs/20260909_082404-0148c576b/System.Private.CoreLib/threading-tasks/run.log` | build broken (C2660), fact 456/456 (321 UNVERIFIED) |