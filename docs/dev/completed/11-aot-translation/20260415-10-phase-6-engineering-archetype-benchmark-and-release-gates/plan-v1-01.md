# Phase 6 Engineering Archetype Benchmark And Release Gates 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 执行本计划，并持续维护 `STATUS.md`、`docs/dev/ACTIVE.md` 与 `notes/progress-*.md`。

## 目标

- 基于 Phase 5 冻结后的 capability truth，重做 benchmark / dashboard / unsupported report / release evidence。

## 执行原则

- 所有 contract 真值统一来自 `capability_coverage.py` 与 declared metadata。
- 不新增新的 string protocol。
- 不在 reporting / dashboard 层重复硬编码 owner / support-state 真值。

## 设计文档

- `docs/dev/completed/20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates/design-v1-01.md`

## 涉及文件与职责

- `build/toolchains/run/testing/benchmark_dashboard_generator.py`
  - benchmark dashboard 数据 contract 与 missing/unsupported reason 语义
- `build/toolchains/run/testing/templates/benchmark-dashboard.html`
  - dashboard 重点信息布局与 reason copy
- `build/toolchains/run/testing/perf_dashboard.py`
  - productization / perf gate 使用的 benchmark contract 汇总
- `build/toolchains/run/testing/release_evidence_contracts.py`
  - declared proof / benchmark contract classification 汇总
- `build/toolchains/run/testing/unsupported_feature_report.py`
  - unsupported / proof-optional / reporting-owned 语义收口
- `build/toolchains/run/testing/subject_reporting.py`
  - release evidence / reporting 汇总链路
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
  - NativeAOT blocker 所需的 opcode loader 支持
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
  - NativeAOT conversion lowering 与 planner-side ABI 对齐
- `tests/unit/performance/*`
  - benchmark dashboard / benchmark command / workload bundle 回归
- `tests/unit/reporting/*`
  - reporting / release evidence 回归
- `tests/unit/compatibility/test_hot_update_productization_gates.py`
  - productization / perf gate / unsupported report contract 回归
- `tests/unit/compatibility/test_phase4*_*.py`
  - NativeAOT / planner / registry 相关 blocker 回归
- `tests/integration/registry/test_declared_metadata_discovery.py`
  - real subject declared discovery 对齐

## 执行步骤

- [x] Task 1：建立 Phase 6 第一轮 RED 审计
  - 锁定 dashboard 当前缺少 owner subject / capability item / support states / missing reason
  - 锁定 productization / perf gate 仍停留在旧版 summary benchmark contract
- [x] Task 2：收口 reporting contract
  - 统一 reporting 消费 `proofRequired` / `benchmarkRequired` / `supportStates`
  - 引入 diagnostics `47/48/49/50` 的 reporting-owned `proofOptional` machine-readable reason
- [x] Task 3：重做 benchmark dashboard / perf dashboard contract
  - `benchmark_dashboard_generator.py` 暴露 owner/capability/support-state/proofRequired/benchmarkRequired
  - subject/case mode status 暴露 `reasonCode` / `reasonLabel`
  - HTML 布局突出 Managed baseline、case contract、missing reason
  - `perf_dashboard.py` 同步消费 capability truth
- [x] Task 4：重做 release evidence / unsupported report
  - 将 full-solution / engineering archetype gate 汇总到统一 release evidence
  - 区分 `unsupported` / `missingBenchmark` / `proofOptional` / `reportingOwned`
- [x] Task 5：回归、文档与归档
  - 相关 compatibility / reporting / performance suite 已通过
  - 通过补齐 `Conv_ovf_u1` loader/native-aot lowering 缺口，并更新陈旧的 registry / phase4 / workload bundle 断言，项目级 `python -m pytest -q` 已恢复为 `699 passed, 40 skipped`

## 本轮已完成验证

- `python -m pytest tests/unit/compatibility/test_hot_update_productization_gates.py::Phase8ProductizationGatesTests::test_declared_contract_report_distinguishes_reporting_owned_missing_proof_and_missing_benchmark tests/unit/reporting/test_subject_reporting.py::SubjectReportingTests::test_build_subject_summary_surfaces_release_evidence_contract_summary -q`
  - 结果：`2 passed`
- `python -m pytest tests/unit/compatibility/test_hot_update_productization_gates.py tests/unit/reporting/test_subject_reporting.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_benchmark_command.py tests/unit/selection/test_selectors.py -q`
  - 结果：`46 passed`
- `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py tests/unit/registry/test_declared_registry_matrix_selection.py tests/unit/compatibility/test_declared_metadata_discovery_contract.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/compatibility/test_metadata_closure_proof_subject.py tests/unit/compatibility/test_async_await_proof_subject.py tests/unit/compatibility/test_threading_gc_proof_subject.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_hot_update_productization_gates.py tests/unit/reporting/test_subject_reporting.py tests/unit/selection/test_selectors.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_benchmark_command.py -q`
  - 结果：`85 passed`
- `python -m pytest tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/performance/test_native_aot_workload_entry_bundle.py tests/integration/registry/test_declared_metadata_discovery.py -q`
  - 结果：`14 passed`
- `python -m pytest tests/unit/compatibility -k "phase4" -q`
  - 结果：`66 passed, 162 deselected`
- `python -m pytest -q`
  - 结果：`699 passed, 40 skipped`

## 状态维护要求

- 每完成一个 Task，都要更新：
  - `docs/dev/completed/20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates/STATUS.md`
  - `docs/dev/ACTIVE.md`
  - `docs/dev/completed/20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates/notes/progress-*.md`
- 本 task 已满足相关 target suite 与项目级 `pytest` 的归档条件，可归档到 `docs/dev/completed/`。
