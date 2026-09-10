# Phase 5 Owner Subject Benchmark And Gate Closure Design v1.01

Date: 2026-04-15 23:18:43 +08:00
Status: active

## 1. 设计目标

把 Phase 1-4 的 widened capability 收口到外层工程证据，确保：

- owner subject 有稳定归属
- benchmark/dashboard 能正确展示 capability contract
- unsupported/reporting/release gate 复用同一份 evidence contract

## 2. 核心设计

### 2.1 Capability Contract

新增 `build/toolchains/run/testing/capability_coverage.py`，作为 owner / support state / proof requirement / benchmark requirement 的单一来源。

### 2.2 Release Evidence Contract

新增 `build/toolchains/run/testing/release_evidence_contracts.py`，把 declared proof / benchmark 与 capability contract 汇总成：

- `ok`
- `missingProof`
- `missingBenchmark`
- `proofOptional`
- `unsupported`

### 2.3 外层消费面

以下链路都必须消费同一 contract：

- `registry.py`
- `benchmark_dashboard_generator.py`
- `perf_dashboard.py`
- `subject_reporting.py`
- `unsupported_feature_report.py`

### 2.4 Owner Driver

- `DispatchProofEntry` / `InterfaceDispatchProofEntry` 作为 widened generic dispatch proof owner
- `DispatchBenchmarkEntry` 作为 generic virtual dispatch benchmark owner
- `tests/unit/compatibility/test_phase5_owner_subject_coverage.py`
- `tests/unit/compatibility/test_hot_update_productization_gates.py`
- `tests/unit/reporting/test_subject_reporting.py`
- `tests/unit/performance/test_benchmark_dashboard_generator.py`

## 3. 非目标

- 不新增新的 typed-il/AOT core IR capability batch
- 不修改 public workload ABI
- 不把测试框架逻辑下沉进 AOT core / planner / emitter

## 4. 验证策略

- 先跑 Phase 5 targeted suites，确认 contract / dashboard / gate 行为闭环
- 再跑项目级 `pytest -q`
- 通过后归档 Phase 5，并关闭父 roadmap
