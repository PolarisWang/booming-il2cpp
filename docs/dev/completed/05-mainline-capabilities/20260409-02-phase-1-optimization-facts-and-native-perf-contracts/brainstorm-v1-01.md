# Phase 1 OptimizationFacts And Native Perf Contracts Brainstorm v1.01

Date: 2026-04-09
Status: design-approved-enter-plan

## 1. 目标

本轮 child brainstorm 不进入实现，而是先把 `Phase 1` 的边界收紧：

- `OptimizationFacts` 到底作为哪一层的正式 contract 出现。
- native perf evidence/report 到底如何落在既有 `subject + matrix + artifact-chain` 主线内。
- `MainlineFeaturePack` 在 `Phase 1` 里只冻结什么 contract，不提前做什么实现。

## 2. 已确认前提

- 完整 subject 采用独立宿主 `MainlineFeaturePack`。
- 首批性能导向 capability 继续做，且 `exceptions` 必须纳入本 roadmap。
- `engine binding` 放到整个 roadmap 最后，因此不属于本阶段范围。

## 3. 方案比较

### 方案 A：把 `OptimizationFacts` 塞成 `SemanticWorld` 第七块 output

优点：

- 表面上对象更少。

缺点：

- 会直接破坏前面讨论已经冻结的 semantic layer 边界。
- 会把 optimization-oriented facts 重新混进 canonical semantic layer。

判断：

- 不采用。

### 方案 B：把 `OptimizationFacts` 作为 `Linker -> CodeGen` 的独立 contract

优点：

- 最符合 `2026-04-08` 讨论结论。
- 能把 definition-level semantic facts 与 closed-world/performance facts 分离。
- 更适合后续做 legality filter + performance arbitration。

缺点：

- 需要新增 contract 类型、artifact 落点与 manifest 引用。

判断：

- 采用。

### 方案 C：先不引入正式 contract，只在 emitter/codegen 里做临时字段

优点：

- 改动看起来最小。

缺点：

- 会继续维持 sample-driven 与 ad-hoc logic。
- 会让 `Phase 2/3` 缺少稳定入口。

判断：

- 不采用。

## 4. native perf 证据策略

### 方案 A：只在 `report.json` 放汇总，不保存运行证据

- 优点：实现最轻。
- 缺点：无法支撑 baseline compare、样本复盘和 perf 异常定位。

### 方案 B：运行证据在 `runtime`，汇总与 compare 在 `report + report/` 附件

- 优点：沿用既有 bucket，不新造平行体系。
- 缺点：需要冻结附件文件名和 summary 结构。

判断：

- 采用。

### 方案 C：新造顶层 `perf/` bucket

- 优点：概念上看起来直接。
- 缺点：直接破坏现有 artifact-chain 模型。

判断：

- 不采用。

## 5. `MainlineFeaturePack` 在本阶段的边界

本阶段只冻结：

- manifest/profile/matrix/pipeline 契约
- native perf 主线需要的 evidence/report 结构
- 为后续 capability slices 预留的 contract 命名

本阶段不做：

- 真正的 subject source tree onboarding
- 完整 native host 与 proof/perf 实现
- capability slice 的实际代码补全

## 6. 结论

`Phase 1` 进入 plan 的正式结论是：

1. `OptimizationFacts` 作为 `Linker -> CodeGen` 的独立 contract 落地。
2. native perf 继续复用既有 artifact buckets，运行证据落在 `runtime`，汇总与 compare 落在 `report`。
3. `MainlineFeaturePack` 在本阶段只冻结 planning/contract surface，不提前膨胀为真实实现任务。
