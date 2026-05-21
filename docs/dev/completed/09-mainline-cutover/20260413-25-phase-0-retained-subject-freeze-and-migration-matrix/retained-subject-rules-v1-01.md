# Retained Subject Rules v1.01

Date: 2026-04-13 08:57:38 +08:00

## 1. Retained Subject 清单

方案 A 在本仓库中的 retained subject 只保留三类：

1. `SolutionCorePack`
2. `HotUpdateHostPack`
3. `MixedExecutionFeaturePack`

除此之外，不再默认长期保留新的顶层 subject。

## 2. 三类 retained subject 的职责

### 2.1 `SolutionCorePack`

负责承接：

- archetype solution
- mainline correctness
- engine-facing host/interop slice
- onboarding / smoke slice
- core runtime benchmark
- 绝大多数 managed/native solution ingestion 场景

### 2.2 `HotUpdateHostPack`

负责承接：

- host-plus-patch executable plan
- patch load / roundtrip / rollback / replacement
- hot-update 相关 runtime proof 与 benchmark

### 2.3 `MixedExecutionFeaturePack`

负责承接：

- mixed execution executable plan
- interpreter / managed / native 混合路径
- mixed execution proof 与 benchmark

## 3. 顶层 subject 新增准则

只有满足以下条件，才允许新增新的顶层 subject：

- 它拥有新的 executable plan
- 它不能被现有三类 retained subject 清晰吸收
- 它需要长期独立的 artifact lineage
- 它需要长期独立的 owner 边界

如果只是以下情况，一律不允许新建顶层 subject：

- 新增一个 correctness proof
- 新增一个 runtime benchmark
- 新增一个 interop slice
- 新增一个 engine host slice
- 新增一个 archetype 变体
- 新增一个 onboarding / smoke 变体

这些都必须并入已有 retained subject。

## 4. 命名规则

### 顶层 retained subject

- `SolutionCorePack`
- `HotUpdateHostPack`
- `MixedExecutionFeaturePack`

### 禁止新增的历史命名模式

- `*Proof`
- `Bench*`
- `*Lite`

这些名字可以作为迁移源出现，但不能作为新的 canonical 目标出现。

## 5. compatibility alias 规则

本次方案 A 下，compatibility alias 只允许作为迁移中的短期过渡。

冻结规则如下：

- 兼容 alias 不是长期结构组成部分
- 兼容 alias 不允许成为默认入口
- 最终阶段必须一次性全部删除
- 不允许留下长期 “历史名仍可直接使用” 的常驻兼容层

## 6. retained subject 的最低能力要求

每个 retained subject 都必须具备：

- 显式 `.sln`
- 统一 `Launcher`
- 至少一条可运行 `proof`
- 至少一条可运行 `benchmark`

## 7. Phase 0 结论

Phase 0 对 retained subject 的冻结结论如下：

- 三类 retained subject 已定，不再摇摆
- `SolutionCorePack` 是主 retained subject
- compatibility alias 最终必须一次性清零
- 顶层 subject 的新增门槛已经提升为 executable-plan 级别，而不是测试点级别
