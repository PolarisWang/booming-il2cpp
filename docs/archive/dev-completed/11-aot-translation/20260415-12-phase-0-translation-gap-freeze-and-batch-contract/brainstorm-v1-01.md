# Phase 0 Translation Gap Freeze And Batch Contract Brainstorm v1.01

Date: 2026-04-15 20:32:24 +08:00

## 问题定义

新的 Phase 7 roadmap 已经建立，但还没有把“当前真实缺口”正式冻结成 child-task 可执行台账。

如果直接开始 Phase 1 代码实现，会立即遇到三个问题：

- 不知道哪些未支持点属于同一 capability batch。
- 不知道哪些缺口应该归到 loader，哪些应该归到 planner / emitter / owner subject 收口。
- 不知道哪些能力应该先做，哪些应该继续 deferred。

## 现状信号

- `LoaderStage.cs` 仍对 opcode、type/method/field handle、method specification、exception region kind 有硬失败。
- `TypeProviders.cs` 仍明确不支持 function pointer signature。
- `NativeAotLoweringPlanner.cs` 仍对 opcode、entry ABI、dispatch kind、target reference metadata、ABI carrier、structured EH shape 设了大量 `NotSupportedException`。
- `NativeAotEmitter.cs` 当前只接受 `generic-managed-entry` 与 `int(void)` workload ABI。

## 结论

本阶段必须先做 capability-first 的 gap freeze，输出：

- gap inventory
- batch matrix
- child-task mapping
- priority / deferred 判断

这样后续 Phase 1-4 才能按清晰边界推进，而不是临时救火。
