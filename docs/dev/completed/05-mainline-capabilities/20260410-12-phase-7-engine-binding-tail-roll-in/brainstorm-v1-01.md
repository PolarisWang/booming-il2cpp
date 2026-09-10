# Phase 7 Engine Binding Tail Roll In Brainstorm v1.01

Date: 2026-04-10 00:45:00 +08:00

## 目标

把 `engine binding` 从 discuss 文档正式收口成 roadmap 最后一个 child task，同时保持前六阶段已经稳定的 performance-first core mainline 不被反向污染。

## 已有结论

- `engine binding` 是正式 contract 边界，不等价于泛化 `interop`，也不应退化成 `RuntimeRoles` 杂项集合。
- `LoweringPlan` 应增量承载 `EngineBindings` 与 `HostBindings`。
- 首批 engine proof 不应扩写回 `MainlineFeaturePack`，而应使用三个最小 subject：
  - `EngineLogWriteLite`
  - `EngineObjectHandleLite`
  - `EngineLifecycleCallbackLite`
- 首批只做 Windows，两条 matrix：
  - `windows-dev-output`
  - `windows-reference-trace`
- 继续复用现有 `subject + matrix + artifact-chain` 主线，不为 engine binding 新造平行系统。

## 方案对比

### 方案 A：继续把 engine binding 混进 `MainlineFeaturePack`

- 优点：复用现有完整 subject，新增目录最少。
- 缺点：service call、object handle、lifecycle callback 三类语义会再次混成大烟囱；失败定位会退化；也会把完整 subject 重新拖回 mega smoke。

### 方案 B：三个最小 engine subject，沿用现有 artifact-chain

- 优点：与 `20260407-10` 到 `20260407-15` 的讨论结论一致；每类 engine 语义单独验证；对 reporting/trace/diagnostics 最友好；不会污染 `MainlineFeaturePack` 的 Phase A 完整主线。
- 缺点：需要补三套 subject manifest/source/validation surface，以及最小 engine-aware planning/reporting 增量。

### 方案 C：先做一个 `EngineBindingSmoke`，以后再拆

- 优点：短期起步快。
- 缺点：会把当前最需要冻结的 contract 边界推迟到以后，后续几乎必然返工。

## 决策

采用方案 B。

## 设计边界

- 不把当前主线描述成完整 `C# -> C++ total solution`。
- `engine binding` 只做 Phase 7 的最小正式收口，不提前引入移动端、完整引擎 SDK 或新的并行报告体系。
- 所有 runtime/codegen 决策继续服从 generated native `C++` 热路径性能优先。

## 下一步

- 基于本结论写 `design-v1-01.md`。
- 随后写 `plan-v1-01.md`，并直接进入 TDD 实现。
