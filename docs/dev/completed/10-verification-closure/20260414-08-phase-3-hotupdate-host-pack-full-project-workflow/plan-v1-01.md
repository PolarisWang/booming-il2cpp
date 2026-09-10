# Phase 3 HotUpdateHostPack Full-Project Workflow 实现计划

> 面向执行 Agent：继续使用 `dev:executing-plans`、`dev:test-driven-development` 和 `dev:verification-before-completion`。
> 目标：把 `HotUpdateHostPack` 收口为完整热更 workflow 的 canonical retained subject，补齐 capability metadata、proof/benchmark 覆盖与 `FullProjectHotUpdateSolution` 的工程表达。

## 目标文件

- `subjects/HotUpdateHostPack/subject.manifest.json`
- `subjects/HotUpdateHostPack/source/HotUpdateHostPack.sln`
- `subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj`
- `subjects/HotUpdateHostPack/source/Host/Program.cs`
- `subjects/HotUpdateHostPack/source/Host/Proofs/**`
- `subjects/HotUpdateHostPack/source/Host/Benchmarks/**`
- `subjects/HotUpdateHostPack/source/Archetypes/FullProjectHotUpdateSolution/**`
- `tests/integration/registry/test_declared_metadata_discovery.py`
- `tests/unit/compatibility/test_hot_update_skeleton_subject.py`
- `tests/unit/compatibility/test_hot_update_e2e_flows.py`
- `tests/unit/performance/test_benchmark_subject_sources.py`

## 执行步骤

- [x] 先扩展 HotUpdate declared metadata discovery RED，要求 proof / benchmark 条目暴露 capability family / capability item / archetype / hot-update capability
- [x] 为 `HotUpdateHostPack` proof 条目补齐 capability metadata，并审视是否需要补入显式 `metadata supplement` proof
- [x] 为 `HotUpdateHostPack` benchmark 条目补齐 capability metadata、execution modes 与 archetype 映射
- [x] 对齐 `FullProjectHotUpdateSolution`、subject manifest、host 入口和默认 proof 行为
- [x] 跑 HotUpdate 相关 compatibility / registry / benchmark-source 回归
- [x] 收口状态文档并返回父 roadmap

## 验证

- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`
- `python -m pytest tests/unit/compatibility/test_hot_update_skeleton_subject.py -q`
- `python -m pytest tests/unit/compatibility/test_hot_update_e2e_flows.py -q`
- `python -m pytest tests/unit/performance/test_benchmark_subject_sources.py -q`

## 执行备注

- 本阶段优先把 `HotUpdateHostPack` 对齐到 capability taxonomy，不重新引入以字符串为核心的外部调度面。
- proof 继续使用 `ChaosUnitTest` + `Assert`；benchmark 继续使用 `ChaosBenchmark`。
- 如需新增条目，优先放进现有 `Host/Proofs` / `Host/Benchmarks` 与 `Archetypes/FullProjectHotUpdateSolution` 结构，不拆新的顶层 subject。
