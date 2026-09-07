# STATUS — System.Threading.Tasks 生产级工程

## 元信息

```yaml
task_id: async-task-industrialization
task_type: roadmap
phase: roadmap
roadmap_or_plan: docs/dev/in-progress/async-task-industrialization/roadmap-v1-01.md
created: 2026-09-07
created_by: user
source: 用户要求 — System.Task 到可生产级质量；brainstorm 确认手工状态机方案，排除 C++20 coroutine
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
```

## Scope

将 C# `System.Threading.Tasks`（Task、async/await、ThreadPool、组合子、Parallel）在 chaos-il2cpp AOT 翻译管线中达到可生产级质量。
核心方案：**IL 状态机 → native C++ 手工状态机**（替换当前 AsyncCoroutineEmitter 的占位 C++20 coroutine）。

## Inputs

- 深度调研：CLR async 无 coroutine，JIT 自建状态机变换（`jit/async.cpp`）
- 现有 native 地基：`src/native/common/chaos/async.h`、`task_runner.cpp`、`thread_pool.cpp`、`monitor.cpp`
- 现有 codegen 占位：`AsyncCoroutineEmitter.cs`（占位 coroutine）、`AsyncAwaiterCatalog.cs`
- Roslyn 生成的状态机 IL：`>d__` struct + `MoveNext()`（switch on state）
- CoreCLR AsyncTaskMethodBuilder 契约：Start/AwaitUnsafeOnCompleted/SetResult/SetException

## 设计摘要（权威输入）

### 边界拍板
- 采用手工状态机翻译，不做 C++20 coroutine reverse-engineering— **决策来源**：brainstorm 深挖 C++20 coroutine 语义，判定其不能满足 P1(perf)+P2(arch)+P3(hotupdate)，详见 `docs/dev/archive/discuss/`（brainstorm 记录）
- 目标：async Task 全家桶（Task/Task<T>/ValueTask 等）+ **performance < .NET8 2×（roadmap 计划目标，非已测量的性能数据；具体 baseline 待 Phase 5 时才产出）** + hotupdate 支持
- 非目标：await foreach、await using、Pipelines、Channel、自定义 TaskScheduler（后置）

### 架构结论
- native 侧 ThreadPool（thread_pool.cpp）是孤儿，需 `ThreadPoolInitialize` 接到 RuntimeInit — **证据**：`async.h`/`thread_pool.cpp` 中的 ThreadPool 类型已独立存在，但既不被 RuntimeInit 调用也不被任何 GC 路径引用，属于"有代码无接入"的孤儿状态；详见 `src/native/runtime-core/thread_pool.cpp` + `src/native/runtime-core/async.h`
- AsyncStateMachineBox native 等价物需用 GC 堆对象（非 pinned）— **证据**：CoreCLR 的 `AsyncStateMachineBox<T>` 是 `Task<T>` 子类、分配在 GC 堆上；pinned 违背 GC 设计（产生碎片、阻止晋升）；详见 `src/coreclr/System.Private.CoreLib/src/System/Runtime/CompilerServices/AsyncTaskMethodBuilder.cs`（CoreCLR 源码）
- 状态机是按 IL `MoveNext` 直译（switch + goto），不多做一次变换 — **证据**：Roslyn 生成的 `>d__` struct 已含完整 switch(state)/goto 状态机，直接翻译即可复用；见 Inputs 段的 Roslyn 状态机 IL

### 阶段切分
Phase 1（运行时基础设施闭环）→ Phase 2（状态机翻译引擎）→ Phase 3（组合子语义）→ Phase 4（Parallel）→ Phase 5（性能优化）→ Phase 6（验证+hotupdate）

## 下一步

- 从 **Phase 1 的 ASYNC-P1-1（ThreadPool 生命周期接入 RuntimeInit）** 开始
- 串行推进。Phase 2 完成后评估是否切 hybrid

## 最近摘要

- 2026-09-07：完成深度调研。确认 CLR async 无 coroutine、JIT 自建状态机。并行了 agent 深挖 C++20 coroutine 语义，判定其不能满足 P1(perf)+P2(arch)+P3(hotupdate)，采用手工状态机方案。

## latest_stop_point

创建 roadmap-v1-01.md + STATUS.md。下一步执行 ASYNC-P1-1。

## 关键文档

- roadmap-v1-01.md

## 调度状态

```yaml
# dispatch_model 权威声明在顶部元信息 yaml（本文件 L19）；此处不重复维护以避免漂移。
active_batches: []
completed_batches: []
```
## 执行进度

### ASYNC-P1-1 ✅ completed (commit a5f6ec141)
ThreadPool 生命周期接入 RuntimeInit：RuntimeInit 调 ThreadPoolInitialize()、RuntimeShutdown 调 ThreadPoolShutdown()、加 s_initialized 单次守卫。GC 6/6 + threading 12/13 绿。

### ASYNC-P1-2 in-progress
AsyncStateMachineBox native 等价物。
`async.h` AsyncTask 是 primitive：非 GC 堆 (`CHAOS_IL2CPP_NEW`)、无 continuation dispatch、无 MoveNext 续列机制。需补：Task 的 GC 承载 + continuation 注册/派发 + EC 保存。

## Phase 1 收尾（实际进度）

* ASYNC-P1-1 ✅ a5f6ec141: ThreadPool→RuntimeInit
* ASYNC-P1-2 ✅ ef440e3de: AsyncTask 续列(box resumption)契约 + 5测试
* ASYNC-P1-3 ✅ a46d4be49: 续列经 ThreadPool 派发 + 桥接注册 + 2测试
* ASYNC-P1-4: EC infra **已存在且已消费**(execution_context.cpp 318行 + task_runner/thread_pool 已用), execution_context_smoke 11/11 绿 → 视为 pre-existing 满足, 不再重复建。跨 await 自动 EC 流属 P3-4。
* ASYNC-P1-5: async_integration_smoke + execution_context_smoke 已覆盖 → 满足

**Phase 1 net**: ThreadPool 活、Task 能续列派发、EC 贯穿。Phase 2 是翻译引擎(接 AsyncCoroutineEmitter 占位 → 真状态机翻译)。

## Phase 2 on-ramp（ASYNC-P2-1, 下一会话入口）
Async codegen 现状(MethodEmission.cs:206-263)：
- Complex async → interpreter-dispatch stub(不执行状态机)
- 非 Complex async → GenPromise+GenCoro 占位(co_await std::suspend_always{} 不做任何事) →
  NativeSymbol wrapper 返回 handle
⇒ 两态都不真正执行 MoveNext 的 IL。

P2-1 正确入口(最小可验证翻译器)：
1. 在 translator 用一个手工迷你 async Task subject(如 async Task<int> One(){await Task.Yield();return 1;})，
   锁机其 >d__ IL：switch(state)+builder+awaiter。
2. 新增实现："MoveNext IL → C++ 状态机 struct+成员函数" (非 C++20 coroutine)，绕过现 GenCoro 占位。
3. 复用 Phase 1 的 native 续列契约(async_task_on_completed/builder) 接线 MoveNext 重入。
4. 跑通端到端(entry AOT 态)后，再扩通用正常方法 lowering(translateNormalBody 复用+state 处理)。

警告：AsyncCoroutineEmitter/MethodEmission 是 Multi-session 增量built，
替换须在清醒完整会话谨慎做，避免在状态机 IL → C++ 翻译误埋 latent 错。
Phase 1 infra (ThreadPool/续列/EC) 已真落地可用，是 P2-1 的稳固地基。
