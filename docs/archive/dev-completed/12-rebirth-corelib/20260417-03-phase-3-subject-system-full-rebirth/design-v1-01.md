# Phase 3 Subject System Full Rebirth 设计 v1.01

Date: 2026-04-17 15:40:38 +08:00
Status: in-progress

## 目标

- 把三个 canonical subject 的内部结构直接推进到 roadmap 约束的新形态。
- 不保留长期双轨目录命名。
- 让 planner / registry / tests / docs 消费的新路径与新命名一致。

## 目标结构

- `EngineeringScenarios/`
- `Proofs/`
- `Benchmarks/`
- `Host/`
- `Patch/`
- `SharedContracts/`

## 本批范围

- `HotUpdateHostPack`
  - `source/Archetypes/FullProjectHotUpdateSolution` -> `source/EngineeringScenarios/FullProjectHotUpdateSolution`
  - `source/PatchModules` -> `source/Patch`
  - scenario solution 内部 `PatchModules/` -> `Patch/`

## 批次策略

1. 先改测试合同，锁住新路径。
2. 再改 subject manifest / solution / project / 文件路径。
3. 跑 subject 相关 targeted regression。
4. 再进入 `SolutionCorePack` 和 `MixedExecutionFeaturePack`。
