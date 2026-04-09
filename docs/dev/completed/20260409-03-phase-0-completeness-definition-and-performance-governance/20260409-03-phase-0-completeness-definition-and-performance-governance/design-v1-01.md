# Phase 0 完整性定义与性能治理设计 v1.01

Date: 2026-04-09
Status: design-frozen-for-plan

## 1. 设计目标

本设计负责把父 roadmap 中新增的 `Phase 0` 变成正式、可引用、可执行的前置规范。

本轮不做：

- runtime/codegen 代码改造
- `OptimizationFacts` contract 实现
- `MainlineFeaturePack` source tree 接入

本轮只做两件事：

1. 冻结当前主线的 `DefinitionOfComplete`
2. 冻结当前主线的 `PerformanceGovernance`

## 2. 交付物设计

### 2.1 `definition-of-complete-v1-01.md`

这份文档要明确三层完成定义：

- 语义完整度
- 平台完整度
- 工程完整度

它需要回答：

- 当前主线到底覆盖哪些语义
- 哪些能力是 roadmap 内强制交付
- 哪些平台属于当前主线，哪些属于后续 total-solution follow-up
- 工程上什么状态才算“这条主线完成”

### 2.2 `performance-governance-v1-01.md`

这份文档要明确：

- `generated native performance-first` 的目标函数
- 允许牺牲哪些次级指标
- 需要哪些证据才能声称某个决策更优
- 当 `exceptions`、`generics specialization`、`devirtualization`、layout/code size 冲突时怎么裁决

## 3. `DefinitionOfComplete` 的推荐结构

### 3.1 语义完整度

当前 `Phase A Mainline Complete` 至少要求以下能力进入正式交付范围：

- `virtual/interface dispatch`
- `closed generics specialization + layout`
- `arrays + boxing/unboxing`
- `delegates/callbacks`
- `exceptions`
- 必要的 `reflection/interop` 补位

当前不纳入本轮完成定义的：

- 全量高阶 `BCL` 语义兼容
- `.NET 10` 输入兼容
- `macOS parity`
- Android / iOS runtime subset
- `engine binding`

### 3.2 平台完整度

当前主线的完成定义只要求：

- `Windows reference`
- `Windows native`

换句话说：

- 当前主线先以 Windows host 收口
- parity / mobile / engine 是外层 follow-up，不是当前 done definition 的一部分

### 3.3 工程完整度

工程上要同时满足：

- 保留 `HelloWorldObject`、`GenericEcho`、`ReflectionLite`、`PInvokeLite`
- 新增 `MainlineFeaturePack`
- 存在统一的 `native output / trace / perf` 验证主线
- 存在可复用的 perf baseline 与 report gate
- `engine binding` 不新造平行 artifact/system

## 4. `PerformanceGovernance` 的推荐结构

### 4.1 目标函数

默认优先级冻结为：

1. generated native 热路径 runtime throughput / allocation
2. startup
3. code size / binary size
4. compile time

### 4.2 裁决原则

- correctness 先成立
- legality filter 先于 performance arbitration
- performance arbitration 先于 deterministic tie-breaker
- managed-only benchmark 不能单独推翻 native evidence

### 4.3 证据层级

能够支持裁决的证据应按以下顺序组织：

- native perf baseline
- generated C++ inspection
- runtime metrics / trace
- correctness proof
- managed benchmark 只作为辅助信息

### 4.4 冲突默认拍板

当多个合法 lowering/path 同时存在时：

- 默认选择更能保留 specialization / devirtualization / layout-aware codegen 空间的方案
- 但不能以破坏 required `exceptions` 语义为代价
- 只有在性能裁决无法区分时，才回退到 deterministic order

## 5. 设计结论

本轮 `Phase 0` 完成后，后续 child task 将得到两个稳定前提：

1. “complete”指的是哪一层 complete
2. “性能最优”到底按什么证据和优先级拍板

这意味着 `20260409-02` 可以从“等待父 roadmap 重规划”切换为“等待 `Phase 0` 完成后恢复”。
