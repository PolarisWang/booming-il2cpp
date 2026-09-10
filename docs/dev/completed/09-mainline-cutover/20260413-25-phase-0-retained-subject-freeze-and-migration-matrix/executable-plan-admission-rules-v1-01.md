# Executable Plan Admission Rules v1.01

Date: 2026-04-13 08:57:38 +08:00

## 1. 目标

明确什么样的变化足以新建顶层 subject，什么样的变化只能并入现有 retained subject。

这是方案 A 的核心治理规则。

## 2. 可接受的新 subject 理由

只有当一个候选对象满足 “新的 executable plan” 时，才可以讨论是否新建顶层 subject。

这里的 executable plan 指的是以下层面的稳定差异：

- 运行模型不同
- 产物链路不同
- 部署方式不同
- 核心 artifact lineage 不同

## 3. 在当前仓库中已经承认的三类 executable plan

### 3.1 core solution executable plan

代表对象：

- `SolutionCorePack`

覆盖：

- managed solution ingestion
- generated native executable
- mainline correctness
- archetype solution
- interop / engine-facing host slice
- core runtime benchmark

### 3.2 host-plus-patch executable plan

代表对象：

- `HotUpdateHostPack`

覆盖：

- host build
- patch generation
- patch load
- rollback / replacement

### 3.3 mixed runtime executable plan

代表对象：

- `MixedExecutionFeaturePack`

覆盖：

- managed / interpreter / native 混合执行
- mixed proof / benchmark

## 4. 不能触发新 subject 的变化

以下变化一律不能新建顶层 subject：

- 新增一个 `ChaosUnitTest`
- 新增一个 `ChaosBenchmark`
- 新增一个 archetype 变体
- 新增一个 engine host slice
- 新增一个 interop 场景
- 新增一个 bridge 场景
- 新增一个 mobile host proof
- 新增一个 benchmark workload

这些变化都只能并入现有 retained subject。

## 5. engineering workload 与 declared benchmark 的边界

以下对象继续属于 engineering workload，不得包装成新 subject，也不应误建成 `ChaosBenchmark`：

- convert
- codegen
- generated native AOT
- native link
- package
- patch generation

`ChaosBenchmark` 只负责 subject 内部 runtime benchmark。

## 6. 边界样例

### `GenericEcho`

结论：

- 不能因为它是边界项就新建 retained subject
- 只能在 `SolutionCorePack` 内被重判为：
  - runtime benchmark
  - 或 engineering workload

### `CompatibilityMatrixProof`

结论：

- 不能继续占用顶层 subject
- 只能并回 `SolutionCorePack`
- 形式优先是 engineering validation

### `MobileHelloWorldProof`

结论：

- 不能单独保留为长期 mobile subject
- 只能并回 `SolutionCorePack` 的 mobile/device slice

## 7. Phase 0 结论

从 Phase 0 起，顶层 subject 的准入门槛正式冻结为：

- executable plan 级别

而不再是：

- proof 级别
- benchmark 级别
- slice 级别
