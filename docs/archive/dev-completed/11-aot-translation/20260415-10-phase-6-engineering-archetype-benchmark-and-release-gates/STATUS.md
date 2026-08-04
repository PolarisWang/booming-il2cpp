---
task_id: 20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates
title: Phase 6 Engineering Archetype Benchmark And Release Gates
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 20:25:00 +08:00
updated_at: 2026-04-15 19:15:12 +08:00
current_dir: docs/dev/completed/20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates
parent_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_status: `docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md`
- parent_roadmap: `docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md`
- phase_5_status: `docs/dev/completed/20260415-09-phase-5-feature-coverage-expansion-on-owner-subjects/STATUS.md`
- brainstorm: `docs/dev/completed/20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates/design-v1-01.md`
- plan: `docs/dev/completed/20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates/plan-v1-01.md`

## 当前结论

- Phase 6 已完成 benchmark dashboard、perf dashboard、unsupported report 与 release evidence summary 对 Phase 5 capability truth 的统一收口。
- 通过补齐 `LoaderStage` 的 `Conv_ovf_u1` 识别，以及 `NativeAotLoweringPlanner` 的 conversion lowering，收口了剩余 Phase 4 NativeAOT blocker。
- 已同步更新 registry discovery、AOT contract 与 workload bundle 中已经落后的断言，使其与当前 planner + Scriban 架构和扩展后的 declared subject catalog 对齐。
- 项目级 `python -m pytest -q` 已恢复为 `699 passed, 40 skipped`，本 child task 满足归档条件。

## 最近摘要

- 2026-04-15 20:25:00 +08:00：创建并激活 `20260415-10`，承接父 roadmap 的 `Phase 6`。
- 2026-04-15 18:02:00 +08:00：完成 benchmark dashboard / perf dashboard capability truth 收口。
- 2026-04-15 18:26:05 +08:00：新增 `release_evidence_contracts.py` 共享 contract summary helper，同时接入 `unsupported_feature_report.py` 与 `subject_reporting.py`。
- 2026-04-15 18:26:05 +08:00：新增 RED/GREEN 回归，冻结 diagnostics `47/48/49/50` 的 reporting-owned `proofOptional` contract，并验证 subject summary 会输出 `releaseEvidenceSummary`。
- 2026-04-15 18:26:05 +08:00：Phase 6 相关回归 `85 passed`；全量 `python -m pytest -q` 结果为 `21 failed, 678 passed, 40 skipped`，失败集中在 integration registry discovery 与 Phase 4 / NativeAot suites。
- 2026-04-15 19:15:12 +08:00：补齐 `Conv_ovf_u1` loader/native-aot lowering 缺口，更新陈旧的 registry / phase4 / workload bundle 断言，并重新跑通项目级 `python -m pytest -q`，结果为 `699 passed, 40 skipped`。

## 下一步

- next_action: 无。本 Phase 6 child task 已完成并归档；父 roadmap 同步完成归档。
- owner: codex
- trigger: 无。

## 风险 / 阻塞

### risks

- 当前无阻塞归档的新风险；后续若继续扩展 release gate / reporting contract，仍应坚持从 `capability_coverage.py` 派生单一真值。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py tests/unit/registry/test_declared_registry_matrix_selection.py tests/unit/compatibility/test_declared_metadata_discovery_contract.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/compatibility/test_metadata_closure_proof_subject.py tests/unit/compatibility/test_async_await_proof_subject.py tests/unit/compatibility/test_threading_gc_proof_subject.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_hot_update_productization_gates.py tests/unit/reporting/test_subject_reporting.py tests/unit/selection/test_selectors.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_benchmark_command.py -q`
  - 结果：`85 passed`
- `python -m pytest tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/performance/test_native_aot_workload_entry_bundle.py tests/integration/registry/test_declared_metadata_discovery.py -q`
  - 结果：`14 passed`
- `python -m pytest tests/unit/compatibility -k "phase4" -q`
  - 结果：`66 passed, 162 deselected`
- `python -m pytest -q`
  - 结果：`699 passed, 40 skipped`

## wiki

- 本轮仅更新 `docs/dev` 执行状态；未新增需要沉淀到长期 wiki 的项目规则。
