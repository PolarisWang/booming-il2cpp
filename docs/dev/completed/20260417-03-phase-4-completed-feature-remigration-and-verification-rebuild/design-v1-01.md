# Phase 4 Completed Feature Remigration And Verification Rebuild 设计 v1.01

Date: 2026-04-17 23:35:00 +08:00
Status: completed

## 目标

- 以 `subject.features.json` 为 authority，重建三大 canonical subject 的 completed feature 验证闭环。
- 把每条 feature 的 obligation 明确绑定到正式验证资产：
  - `unit/contracts`
  - `managed proof`
  - `native proof`
  - `hotupdate proof/smoke`
  - `benchmark`
- 删除仍然漂浮在旧结构、旧命名、旧 correctness 路径上的重复验证与失效验证。

## 输入真源

- `subjects/SolutionCorePack/subject.features.json`
- `subjects/MixedExecutionFeaturePack/subject.features.json`
- `subjects/HotUpdateHostPack/subject.features.json`
- 当前 canonical subject 的 `Proofs / Benchmarks / EngineeringScenarios / Host / Patch / SharedContracts`
- 已存在的 `tests/unit/**`、`tests/contracts/**`、`tests/integration/**`

## 范围边界

- 负责把“已完成能力”重新挂到新 subject/test framework 主线上。
- 不负责新增新的 AOT Core IR feature。
- 不负责新增新的 HotUpdate 语义能力。
- 不把测试框架逻辑混进 IL2CPP core；审计与映射留在 subject/test pipeline 侧。

## 批次策略

1. 先做 authority 与现有验证资产的审计基线，找出缺口、重复项和漂移项。
2. 再按 subject 逐个收口：
   - `SolutionCorePack`
   - `MixedExecutionFeaturePack`
   - `HotUpdateHostPack`
3. 每个 subject 都遵循：
   - 先补 RED 的 audit / contract 测试
   - 再调整 proof / benchmark / host 资产
   - 最后跑 targeted regression
4. 整个阶段收尾时输出可进入 Phase 5 的 legacy purge 清单。

## 验收口径

- 每条 `proofRequired = true` 的 feature 都能追到正式 proof 资产。
- 每条 `benchmarkRequired = true` 的 feature 都能追到正式 benchmark 资产。
- `supportStates` 触及 hotupdate / mixed execution 的 feature，必须追到对应 host 验证资产。
- 不再存在“feature authority 已声明，但验证资产仍靠旧路径或历史兼容约定兜底”的情况。
