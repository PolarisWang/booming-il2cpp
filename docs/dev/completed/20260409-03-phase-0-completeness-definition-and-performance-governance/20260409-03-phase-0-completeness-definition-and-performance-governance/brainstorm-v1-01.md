# Phase 0 完整性定义与性能治理 Brainstorm v1.01

Date: 2026-04-09
Status: ready-for-plan

## 1. 本轮目标

本轮不是实现新的 runtime/codegen 功能，而是把父 roadmap 新增的 `Phase 0` 真正冻结成可执行前置条件：

- 定义当前主线的 `DefinitionOfComplete`
- 定义当前主线的 `PerformanceGovernance`
- 正式解除 `20260409-02-phase-1-optimization-facts-and-native-perf-contracts` 的前提歧义

## 2. 为什么必须先做这一层

如果直接继续原 `Phase 1`，会有两个根本问题：

1. 后续任何人都无法准确回答“当前是否已经完成完整 `C# -> C++` 方案”
2. 后续任何人都可能继续用不同口径理解“C++ 性能最优”

这两个问题不先冻结，后续的 `OptimizationFacts`、`MainlineFeaturePack`、capability batch 都会建立在不稳定前提上。

## 3. 可选路径

### 方案 A：只在父 roadmap 里写一句原则，不再单独派生子任务

- 优点：最快。
- 缺点：没有单独的冻结文档，后续 child task 仍会回到口头理解。
- 结论：不采用。

### 方案 B：派生专门的 `Phase 0` 子任务，单独冻结两份文档

- 优点：边界最清晰，能直接变成后续 child task 的正式前提。
- 缺点：当前会多一个 planning/documentation 子任务。
- 结论：采用。

## 4. 本轮直接结论

本轮采用 **方案 B**，并默认冻结以下方向：

- 当前“complete”默认指向 `Phase A Mainline Complete`，而不是 total solution complete
- 当前性能治理默认优先级按：
  - runtime throughput / allocation
  - startup
  - code size / binary size
  - compile time
- `exceptions` 是本轮 roadmap 内强制交付项
- `engine binding` 保持在 roadmap 最后阶段

## 5. 输出物

本轮需要产出：

- `definition-of-complete-v1-01.md`
- `performance-governance-v1-01.md`
- `plan-v1-01.md`
- 同步后的父 roadmap / status / index

## 6. 进入计划的判断

本轮已经不需要继续扩散 brainstorm。

原因是：

- 目标明确
- 边界明确
- 输出物明确
- 不涉及新的代码实现不确定性

因此下一步直接进入 `plan`。
