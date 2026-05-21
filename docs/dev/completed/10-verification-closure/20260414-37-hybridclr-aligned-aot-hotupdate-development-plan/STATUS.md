---
task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
title: HybridCLR 对齐的 AOT 与热更开发计划
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 23:34:39 +08:00
updated_at: 2026-04-15 19:15:12 +08:00
current_dir: docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
parent_task_id:
source_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_relation: follow-up-after-core-capability-roadmap
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md`
- phase_0_status: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/STATUS.md`
- phase_1_status: `docs/dev/completed/20260415-02-phase-1-subject-source-reshape-and-test-metadata-cutover/STATUS.md`
- phase_2_status: `docs/dev/completed/20260415-03-phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation/STATUS.md`
- phase_3_status: `docs/dev/completed/20260415-04-phase-3-hotupdate-runtime-and-supplemental-metadata-foundation/STATUS.md`
- phase_4a_status: `docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/STATUS.md`
- phase_4b_status: `docs/dev/completed/20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface/STATUS.md`
- phase_4c_status: `docs/dev/completed/20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure/STATUS.md`
- phase_4d_status: `docs/dev/completed/20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening/STATUS.md`
- phase_5_status: `docs/dev/completed/20260415-09-phase-5-feature-coverage-expansion-on-owner-subjects/STATUS.md`
- phase_6_status: `docs/dev/completed/20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates/STATUS.md`

## 当前结论

- 父 roadmap 的 Phase 0-6 已全部完成，并全部收口到 completed。
- Phase 6 已完成 benchmark / dashboard / unsupported report / release evidence 对 capability truth 的统一收口，并同步清除了遗留的 Phase 4 / registry / workload bundle blocker。
- 项目级 `python -m pytest -q` 已恢复为 `699 passed, 40 skipped`，父 roadmap 满足归档条件。

## 最近摘要

- 2026-04-15 20:25:00 +08:00：激活 `20260415-10 Phase 6`，开始收口 benchmark / dashboard / unsupported report / release evidence。
- 2026-04-15 18:26:05 +08:00：Phase 6 已完成 `proofOptional` / `missingProof` / `missingBenchmark` contract summary 收口。
- 2026-04-15 18:26:05 +08:00：Phase 6 相关回归 `85 passed`，但全量 `python -m pytest -q` 仍为 `21 failed, 678 passed, 40 skipped`。
- 2026-04-15 19:15:12 +08:00：补齐 `Conv_ovf_u1` loader/native-aot lowering 缺口，更新陈旧的 registry / phase4 / workload bundle 断言，并重新跑通全量 `python -m pytest -q`，结果为 `699 passed, 40 skipped`。

## 下一步

- next_action: 无。本 roadmap 已完成并归档；如需继续扩展 HybridCLR 对齐的 AOT / hotupdate 能力，请另起 follow-up。
- next_action_detail: 无。
- owner: codex
- trigger: 无。

## 风险 / 阻塞

### risks

- 当前无阻塞归档的新风险；后续若继续扩展 typed-il / AOT / hotupdate 能力，仍需保持 `AotCoreIr -> planner -> emitter + Scriban` 的单向收口。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py tests/unit/registry/test_declared_registry_matrix_selection.py tests/unit/compatibility/test_declared_metadata_discovery_contract.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/compatibility/test_metadata_closure_proof_subject.py tests/unit/compatibility/test_async_await_proof_subject.py tests/unit/compatibility/test_threading_gc_proof_subject.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_hot_update_productization_gates.py tests/unit/reporting/test_subject_reporting.py tests/unit/selection/test_selectors.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_benchmark_command.py -q`
  - 结果：`85 passed`
- `python -m pytest -q`
  - 结果：`699 passed, 40 skipped`

## wiki

- 当前仅更新 `docs/dev` 执行状态；未新增必须沉淀到长期 wiki 的项目规则。
