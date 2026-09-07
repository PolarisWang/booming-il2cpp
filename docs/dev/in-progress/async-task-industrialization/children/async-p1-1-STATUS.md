# STATUS — Async Task Phase1 子任务 1：ThreadPool 生命周期接入 RuntimeInit

## 元信息

```yaml
task_id: asycp1-1
task_type: plan
phase: implementation
parent_task_id: async-task-industrialization
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-executing-plans
child_execution_mode: auto
```

## Scope

让 native ThreadPool（`src/native/runtime-core/thread_pool.cpp`）由 `RuntimeInit` 真正启动与关闭。当前 `ThreadPoolInitialize` 零调用者：
- 在 `runtime_init.cpp` 的初始化序列加 `ThreadPoolInitialize()`
- 在 shutdown / 进程退出加 `ThreadPoolShutdown()`（幂等）
- 确保重复调用安全（`ThreadPoolInitialize` 已有 `s_shutdown` reset，需确认 init 只跑一次）
- 确认 gate/wakeable/hill-climbing 线程真正拉起

用 `RuntimeInit` 里已有 `RegisterAsyncTaskRun`（task_runner 挂 async_task_run）作为模式参照。

### 边界
- 只动 `thread_pool.cpp/h` + `runtime_init.cpp`（或其 lifecycle 钩子所在文件）
- **不**改 AsyncTaskModelBuilder / MoveNext / codegen —— 那是后续子任务
- **不**改生产 GC 分配路径

## Inputs

- `src/native/runtime-core/thread_pool.cpp`（781 行，已有 ThreadPoolInitialize/Shutdown/QueueUserWorkItem/WorkerLoop/GateThreadLoop/WakeableWorkerLoop/hill-climb）
- `src/native/runtime-core/thread_pool.h`
- `src/native/runtime-core/core/runtime_init.cpp`（有 RegisterAsyncTaskRun 于 :77）
- `src/native/runtime-core/task_runner.cpp`（TaskRun 用 ThreadPoolQueueUserWorkItemUnsafe）
- 已知：当前没有谁调用 ThreadPoolInitialize，池是"未接线孤儿"

## Expected Outputs

- `runtime_init.cpp` 在合适时机调用 `ThreadPoolInitialize()`，销毁路径调 `ThreadPoolShutdown()`
- `ThreadPoolInitialize` 有单次守卫（避免二次 init 起双 gate 线程）
- 一个验证：初始化后 `ThreadPoolWorkerCount() >= 1`，`ThreadPoolQueueUserWorkItem` 能让 worker 执行回调

## Exit Criteria

- `ThreadPoolInitialize` 有调用者，且单次执行（幂等）
- 构建通过；相关 native unit test 用真实线程池跑通一次 QueueUserWorkItem→worker 执行
- 无回归（GC / runtime 测试绿）

## Terminal Notes

(待执行后补)

## Scope 边界（roadmap 参照）

- roadmap-v1-01.md §四 Phase 1 ASYNC-P1-1
- Phase 1 exit_criteria 首条：ThreadPoolInitialize 由 RuntimeInit 调用，worker 存活
