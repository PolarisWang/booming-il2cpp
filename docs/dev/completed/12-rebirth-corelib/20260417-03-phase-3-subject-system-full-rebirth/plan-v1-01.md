# Phase 3 Subject System Full Rebirth 实施计划 v1.01

Date: 2026-04-17 15:40:38 +08:00
Status: completed

## 计划清单

- [x] Batch 1: `HotUpdateHostPack` 目录与路径重生
- [x] Batch 2: `SolutionCorePack` 的 `EngineeringScenarios / Proofs / Benchmarks / Host` 结构切换
- [x] Batch 3: `MixedExecutionFeaturePack` 的 `EngineeringScenarios / Host / Proofs / Benchmarks` 结构切换
- [x] Batch 4: 删除 `validation/*` 与旧 subject 路径兼容依赖
- [x] Batch 5: 跑 Phase 3 回归并准备 Phase 4 输入

## 完成摘要

- canonical subject 结构重生完成，正式 source 中不再保留旧命名。
- native proof host 已切到 generic workspace 物化模板。
- Phase 3 回归证据：
  - `12 passed`
  - `126 passed`
  - `98 passed`
