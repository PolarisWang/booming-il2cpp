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
- 采用手工状态机翻译，不做 C++20 coroutine reverse-engineering
- 目标：async Task 全家桶（Task/Task<T>/ValueTask 等）+ performance < .NET8 2× + hotupdate 支持
- 非目标：await foreach、await using、Pipelines、Channel、自定义 TaskScheduler（后置）

### 架构结论
- native 侧 ThreadPool（thread_pool.cpp）是孤儿，需 `ThreadPoolInitialize` 接到 RuntimeInit
- AsyncStateMachineBox native 等价物需用 GC 堆对象（非 pinned）
- 状态机是按 IL `MoveNext` 直译（switch + goto），不多做一次变换

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
dispatch_model: sequential
active_batches: []
completed_batches: []
```