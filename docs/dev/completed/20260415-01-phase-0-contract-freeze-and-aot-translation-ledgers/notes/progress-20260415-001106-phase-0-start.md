# Phase 0 Progress - 2026-04-15 00:11:06 +08:00

## 本轮执行

- 创建 `20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers` child task。
- 基于现有 retained subjects、`Chaos.TestFramework` 枚举、reference bundle 和 architecture 文档，开始冻结：
  - reference bundle 边界
  - 两级 capability taxonomy
  - stable capability identity
  - `body availability` 口径
  - AOT translation / unsupported 双台账

## 已确认事实

- canonical base bundle 为 `assets/reference-bundles/dotnet-foundation/net8.0`
- controlled supplement 当前基线为 `assets/reference-bundles/dotnet-foundation/lib/ReferenceGreeter.dll`
- retained subject 仍为：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- `ChaosUnitTest` / `ChaosBenchmark` 的 family / item / archetype / modes / metrics 等公开字段已存在于 `src/reference/Chaos.TestFramework`

## 下一步

- 完成初稿后逐份对照 subject / bundle / enum 现状做自检，再回写父 roadmap 状态。
