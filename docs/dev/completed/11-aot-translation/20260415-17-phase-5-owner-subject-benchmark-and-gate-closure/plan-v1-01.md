# Phase 5 Owner Subject Benchmark And Gate Closure 实现计划

> 面向执行 Agent：本阶段以“外层证据闭环”为目标，先确认 contract 单源，再完成项目级验证并归档父 roadmap。

## 目标

- 固化 capability -> owner subject / support state / requirement contract
- 让 benchmark/dashboard、unsupported/reporting、release gate 全部基于同一 contract 工作
- 用 targeted suites 与项目级回归证明 Phase 1-5 已形成完整闭环

## 设计文档

- `docs/dev/completed/20260415-17-phase-5-owner-subject-benchmark-and-gate-closure/design-v1-01.md`

## 涉及文件与职责

- `build/toolchains/run/testing/capability_coverage.py`
- `build/toolchains/run/testing/release_evidence_contracts.py`
- `build/toolchains/run/testing/registry.py`
- `build/toolchains/run/testing/benchmark_dashboard_generator.py`
- `build/toolchains/run/testing/perf_dashboard.py`
- `build/toolchains/run/testing/subject_reporting.py`
- `build/toolchains/run/testing/unsupported_feature_report.py`
- `tests/unit/compatibility/test_phase5_owner_subject_coverage.py`
- `tests/unit/compatibility/test_hot_update_productization_gates.py`
- `tests/unit/reporting/test_subject_reporting.py`
- `tests/unit/performance/test_benchmark_dashboard_generator.py`

## 执行步骤

- [x] Task 1: 对齐 owner subject / support state / proof-benchmark requirement contract
  - 审计 capability family/item 到 owner subject 的映射
  - 固化 support state 与 proof/benchmark requirement 规则
- [x] Task 2: 接通 benchmark/dashboard、unsupported/reporting 与 release gate
  - registry 注入 owner/support metadata
  - dashboard 与 reporting 统一消费 release evidence contract
- [x] Task 3: 跑验证并确认外层证据闭环
  - targeted suites
  - 项目级 `pytest -q`
- [x] Task 4: 更新父 roadmap / index 并归档
  - 归档 `20260415-17`
  - 关闭 `20260415-11`

## 本轮验证

- `python -m pytest tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/compatibility/test_hot_update_productization_gates.py tests/unit/reporting/test_subject_reporting.py tests/unit/performance/test_benchmark_dashboard_generator.py -q` -> `35 passed`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q` -> `8 passed`
- `python -m pytest -q` -> `713 passed, 40 skipped`
