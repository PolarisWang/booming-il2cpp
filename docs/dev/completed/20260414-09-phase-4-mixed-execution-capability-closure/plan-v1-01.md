# Phase 4 MixedExecution Capability Closure 实现计划

> 面向执行 Agent：继续使用 `dev:executing-plans`、`dev:test-driven-development` 和 `dev:verification-before-completion`。  
> 目标：把 `MixedExecutionFeaturePack` 收口为 mixed/interpreter 能力族的 canonical retained subject，补齐 proof/benchmark capability metadata，并为缺失的显式能力项提供 proof 承载。

## 目标文件

- `subjects/MixedExecutionFeaturePack/subject.manifest.json`
- `subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.sln`
- `subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj`
- `subjects/MixedExecutionFeaturePack/source/ManagedBridge/Program.cs`
- `subjects/MixedExecutionFeaturePack/source/ManagedBridge/Proofs/**`
- `subjects/MixedExecutionFeaturePack/source/ManagedBridge/Benchmarks/**`
- `subjects/MixedExecutionFeaturePack/source/Lowering/**`
- `subjects/MixedExecutionFeaturePack/source/Interpreter/**`
- `subjects/MixedExecutionFeaturePack/source/Archetypes/MixedBridgeSolution/**`
- `tests/integration/registry/test_declared_metadata_discovery.py`
- `tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py`
- `tests/unit/performance/test_benchmark_subject_sources.py`

## 执行步骤

- [x] 先扩展 MixedExecution declared metadata discovery RED，要求 proof / benchmark 条目暴露 capability family / capability item / archetype
- [x] 为现有 mixed/interpreter proof / benchmark 条目补齐 capability metadata
- [x] 为缺失的 mixed/interpreter level-2 capability item 增加少量显式 proof 条目
- [x] 对齐 `ManagedBridge/Program.cs` 默认 proof 集合中的 canonical archetype 表达
- [x] 跑 MixedExecution 相关 compatibility / registry / benchmark-source 回归
- [x] 收口状态文档并返回父 roadmap

## 验证

- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`
  - 结果：`8 passed`
- `python -m pytest tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py -q`
  - 结果：`11 passed`
- `python -m pytest tests/unit/performance/test_benchmark_subject_sources.py -q`
  - 结果：`4 passed`

## 执行备注

- benchmark 继续遵循 selective benchmark 原则，不为每个能力项机械新增 benchmark。
- 缺失能力项优先通过 proof 单文件条目补齐，避免重新拆出新的顶层 subject。
- mixed/interpreter 条目统一使用 `ChaosCapabilityFamily.MixedExecution` 与对应 `ChaosCapabilityItem`，不再借用 hot-update 口径表达 mixed 能力。
