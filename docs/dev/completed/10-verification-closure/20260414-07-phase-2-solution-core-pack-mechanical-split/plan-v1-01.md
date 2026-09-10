# Phase 2 SolutionCorePack Mechanical Split 实现计划

> 面向执行 Agent：继续使用 `dev:executing-plans`、`dev:test-driven-development` 和 `dev:verification-before-completion`。
> 目标：把 `SolutionCorePack` 真正收敛为 capability taxonomy 下的核心 retained subject，完成机械拆分、metadata 对齐、legacy 结构清理和关键回归闭环。

## 目标文件

- `subjects/SolutionCorePack/subject.manifest.json`
- `subjects/SolutionCorePack/source/SolutionCorePack.sln`
- `subjects/SolutionCorePack/source/Launcher/**`
- `subjects/SolutionCorePack/source/FeatureSlices/**`
- `subjects/SolutionCorePack/source/Benchmarks/**`
- `subjects/SolutionCorePack/source/Archetypes/**`
- `tests/unit/compatibility/test_solution_core_pack_subject.py`
- `tests/unit/planning/test_solution_core_pack_planner.py`
- `tests/unit/run/test_benchmark_subject_sources.py`
- `tests/integration/registry/test_solution_core_pack_registry.py`

## 执行步骤

- [x] 跑 `SolutionCorePack` 相关 RED，确认当前机械拆分和入口切换仍有缺口
- [x] 对齐 `subject.manifest.json`、solution、launcher 与 `FeatureSlices/Benchmarks/Archetypes` 的真实结构
- [x] 为 `CoreRuntimeFeatures` / `CoreRuntimeBenchmarks` 条目补齐 capability family / capability item metadata
- [x] 清理 `SolutionCorePack` 内部残留的 legacy `Slices/` / `MainlineFeaturePack` / `PerformanceFeaturePack` 路径耦合
- [x] 跑 `SolutionCorePack` 相关 compatibility / planner / registry / benchmark source 回归并收口状态文档

## 验证

- `python -m pytest tests/unit/compatibility/test_solution_core_pack_subject.py -q`
- `python -m pytest tests/unit/planning/test_solution_core_pack_planner.py -q`
- `python -m pytest tests/unit/run/test_benchmark_subject_sources.py -q`
- `python -m pytest tests/integration/registry/test_solution_core_pack_registry.py -q`

## 执行备注

- 本阶段优先让 `SolutionCorePack` 成为 capability taxonomy 的真实承载体，不回退到新增顶层 subject 的路线。
- 如需调整 benchmark 入口或 proof entry，优先收敛到 `ChaosBenchmark` / `ChaosUnitTest` 元数据，而不是扩张字符串协议面。
- 本阶段不处理 `HotUpdateHostPack` / `MixedExecutionFeaturePack` 的主体迁移，那部分属于后续子任务。
