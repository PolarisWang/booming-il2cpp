# Phase 1 OptimizationFacts And Native Perf Contracts 设计 v1.01

Date: 2026-04-09
Status: plan-ready

## 1. 设计目标

把 `2026-04-08` 已冻结的 performance-first 结论，变成可以在后续实现中直接消费的正式 contract：

- `OptimizationFacts` 独立于 `SemanticWorld`
- native perf 证据与报告沿用现有 artifact-chain
- `MainlineFeaturePack` 的 profile/matrix/pipeline 第一版 contract 提前冻结

## 2. `OptimizationFacts` 设计

### 2.1 归属边界

- 上游来源：`Linker`
- 下游主要消费者：`CodeGen`
- canonical 语义：closed-world / optimization-oriented facts

它不是：

- `SemanticWorld` 第七块 canonical output
- stable metadata lookup 的扩展查询
- emitter 私有推理缓存

### 2.2 第一版最小内容

第一版 contract 至少承接四类事实：

- `closedWorldSpecializations`
  - retained concrete type/method instantiations
- `dispatchFacts`
  - virtual/interface dispatch 的目标约束、可去虚拟化候选
- `layoutFacts`
  - layout/storage、boxing、array element kind 等与 native code quality 直接相关的结构事实
- `exceptionFacts`
  - 与 `throw`、handler shape、required helper/landing path 相关的第一版 optimization-facing facts

说明：

- 这里把 `exceptions` 提前预留为正式字段，是因为用户已明确要求它必须纳入本 roadmap。
- 这不代表 `Phase 1` 立即完成 exception 语义实现，只代表 contract 不允许缺位。

### 2.3 artifact 落点

推荐第一版 canonical artifact：

- `analysis/analysis/optimization-facts.json`

原因：

- 它属于 `Linker -> CodeGen` 之间的分析层事实。
- 不应落入 `generated/`，因为它不是 backend output。
- 不应新造 bucket。

## 3. native perf evidence/report 设计

### 3.1 evidence 落点

运行证据继续落在：

- `matrices/<matrix-id>/runtime/`

第一版可冻结的文件形态：

- `perf.runtime.json`
- `perf.samples.json` 或等价样本文件

### 3.2 report 落点

机器入口继续保持：

- `matrices/<matrix-id>/report.json`

同时允许 `report/` 附件目录承接：

- `perf-summary.json`
- `perf-baseline-compare.json`
- `perf-metrics.json`

设计要求：

- `report.json` 必须能摘要 perf 结论与附件路径。
- compare/baseline 不能只存在 UI 侧临时拼装结果。

## 4. `MainlineFeaturePack` contract 设计

### 4.1 第一版 profile

- `proof-dev`
- `trace-platform`
- `perf-profile`

### 4.2 第一版 matrix

- `windows-native-check`
- `windows-reference-trace`
- `windows-native-profile`

### 4.3 第一版 pipeline

- `proof-runtime-output`
- `proof-runtime-trace`
- `native-runtime-perf`

设计原则：

- 只冻结 planning surface，不在本阶段真正接入 source tree。
- `CHECK` 与 `PROFILE` 是第一版必须覆盖的变体。
- `SHIP` 暂不进入 `Phase 1` 交付面。

## 5. 受影响模块

### 5.1 managed contracts

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
  - 新增 `OptimizationFacts`、native perf evidence/report 相关 contract

### 5.2 linker / codegen

- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
  - 预留并输出第一版 `OptimizationFacts`
- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
  - 接入 `OptimizationFacts` 输入边界

### 5.3 subject planning / reporting / worker surface

- `build/toolchains/run/runtime_manifest.json`
- `build/toolchains/run/manifest.py`
- `build/toolchains/run/operation_reporting.py`
- `build/toolchains/run/commands/test.py`
  - 冻结 `native-runtime-perf` planning/reporting surface

### 5.4 tests

- `tests/unit/run/test_subject_manifest_schema.py`
- `tests/unit/run/test_subject_planner.py`
- `tests/unit/run/test_subject_reporting.py`
- `tests/unit/run/test_subject_workers.py`
- `tests/tooling/run/test_subject_command.py`
  - 先用 RED 锁定新的 contract surface

## 6. 非目标

本阶段不包含：

- 真正把 `MainlineFeaturePack` source/unit/proof/perf 工程接入仓库
- 真正完成 dispatch / arrays / generics / exceptions 的功能实现
- 真正推进 engine binding

## 7. 结论

`Phase 1` 采用如下设计：

1. `OptimizationFacts` 独立落在 `Linker -> CodeGen` 边界。
2. 其 canonical artifact 进入 `analysis/analysis/optimization-facts.json`。
3. native perf 运行证据落在 `runtime/`，汇总与 compare 落在 `report.json + report/` 附件。
4. `MainlineFeaturePack` 本阶段只冻结 planning/contract surface，不做实际 onboarding。
