# Design v1-01

## Batch 2 最小实现设计

### 目标

为 `Phase 2 / Batch 2` 建立一个可验证的 async/await proof slice，使主线第一次具备“识别 async state machine、保留其闭包、为其选择独立 lowering family、拥有独立 subject proof”的能力。

### 设计范围

#### Loader

- 取消“仅顶层类型方法进入 `methodOwners`”的限制。
- 让嵌套类型上的 `MoveNext` / `SetStateMachine` / 编译器生成字段进入 `LoadedWorldModel`。

#### SemanticWorld

- 基于以下信号识别 async slice：
  - `System.Runtime.CompilerServices.IAsyncStateMachine`
  - `AsyncTaskMethodBuilder`
  - `TaskAwaiter`
  - 编译器生成的 `MoveNext`
- 产出最小 capability：
  - `requires-async-state-machine`
  - `requires-task-awaiter`

#### Linker

- 当可达方法命中 async builder/awaiter surface 时：
  - 额外保留对应 compiler-generated state machine type
  - 保留其 `MoveNext` / `SetStateMachine`
- 产出专门的 dispatch / optimization 标记，供 codegen 选择 async lowering family。

#### CodeGen

- 新增 lowering family：`managed-async.awaitable-int.minimal`
- 选择条件：
  - entry 为同步 `Run()`
  - world capabilities 命中 async state machine slice
  - proof 形态满足“单 `async Task<int>` + 单 await + 取结果输出”
- 模板策略：
  - 本轮不实现真实 Task runtime
  - 在 native proof 中把 proof slice 同步化，直接产出与 async proof 一致的可观察输出

#### Subject

- 新增 `subjects/AsyncAwaitProof/`
- `source/Program.cs` 包含：
  - `async Task<int> ComputeAsync()`
  - `await Task.FromResult(...)`
  - `Run()` 同步输出结果

### 验证策略

- 先写新的 RED 测试锁定：
  - subject 树存在
  - loader/semantic/linker/codegen 表面存在
  - lowering family/template/subject 名称冻结
- 再补实现并运行：
  - 新增 Batch 2 测试
  - 现有阶段测试回归
  - core solution build

### 风险

- 仅靠 proof-level lowering 不能代表通用 async runtime。
- 如果不同时处理 loader 的嵌套方法闭包，后续任何 async proof 都会在 linker/codegen 前就丢信息。
