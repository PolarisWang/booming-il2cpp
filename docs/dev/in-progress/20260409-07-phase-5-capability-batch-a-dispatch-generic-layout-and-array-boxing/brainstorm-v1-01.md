# Phase 5 Capability Batch A Brainstorm v1.01

Date: 2026-04-09

## 用户目标

- 在 `Phase 4` 已完成的前提下，继续推进 `IL2CPP mainline` 主任务。
- 决策继续以 generated native 工程的 `C++` 性能为第一目标。
- `exceptions` 仍留在 roadmap 内，`engine binding` 放在最后，不提前插入。

## 当前项目上下文

- `Phase 4` 已经把 `MainlineFeaturePack` 接成真实完整 subject。
- 当前完整 subject 已具备：
  - managed source tree
  - subject-owned unit/proof
  - trace export
  - native perf baseline
- 当前 loader / runtime / codegen 的已知现状：
  - loader 已支持 `callvirt`、`ldtoken`、`ldelem.ref`
  - loader 还缺 `newarr`、`box`，并且大概率还缺 `stelem.ref`
  - runtime / bootstrap 已有 `ArrayNew`、`BoxValue`、`UnboxValue`、`ResolveVirtualMethod`、`InvokeVirtual` stub
  - 现有 lowering family 仍主要覆盖 object/generic/reflection/interop 的最小形状

## 方案对比

### 方案 A：继续拆多个 probe subject

- 做法：为 `dispatch`、`generic/layout`、`arrays/boxing` 各自新建独立 subject。
- 优点：每个 capability 很纯，局部验证简单。
- 缺点：直接违背 `Phase 4` 建立完整 subject 主宿主的意图，后续 perf evidence 会再次碎片化。
- 结论：不采用。

### 方案 B：在 `MainlineFeaturePack` 里塞一个 mega proof entry

- 做法：只保留一个 proof entry，把所有 Batch A 能力都揉进同一条 proof 路径。
- 优点：不需要扩展 subject planner / manifest。
- 缺点：proof correctness、能力边界、故障定位、perf evidence 会重新耦合；后续 Batch B 只会更糟。
- 结论：不采用。

### 方案 C：一个完整 subject + 多个 proof slice + matrix-scoped entry override

- 做法：保留一个 `MainlineFeaturePack`，但为 capability correctness 增加多个 proof slice；通过 matrix 级 `source.entry` override 让不同 matrix 选到不同 proof entry。
- 优点：
  - 保持 full subject 主宿主定位
  - capability correctness 可以按 slice 独立验证
  - perf evidence 仍能留在统一的 `MainlineFeaturePack` 主线下
  - 最小化对当前 subject/planner 架构的扰动
- 缺点：需要扩展 manifest / planner / tests，并在 `Phase 5` 里同步梳理 capability slice 命名与矩阵布局。
- 结论：推荐采用。

## 推荐方案

采用方案 C，并补充两条执行原则：

1. `dispatch` 采用双路径思路：能在 closed world 下去虚的调用优先直接 call，不能去虚时才走 runtime virtual dispatch helper。
2. `arrays/boxing` 作为单独难点处理，首批只补齐支撑 `MainlineFeaturePack` Batch A slice 所必需的最小 opcode / helper 集，不扩展成通用数组语义大全。

## 需要用户重点确认的点

- 是否接受 `Phase 5` 继续保持“一个完整 subject，不分裂回多个 probe subject”。
- 是否接受 `matrix-scoped entry override` 作为 capability slice 的主要选择机制。
- 是否接受 `dispatch` 的首批策略为“优先去虚，保留 runtime virtual fallback”，而不是先只做纯动态虚调用或只做纯静态直呼。
