---
task_id: 20260417-03-phase-4-completed-feature-remigration-and-verification-rebuild
title: Phase 4 Completed Feature Remigration And Verification Rebuild
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-17 23:35:00 +08:00
updated_at: 2026-04-17 17:12:13 +08:00
latest_stop_point: Feature obligation audit, canonical asset remigration, legacy cutover contracts, and Phase 4 regression suite are complete
current_dir: docs/dev/completed/20260417-03-phase-4-completed-feature-remigration-and-verification-rebuild
parent_task_id: 20260417-03-subject-feature-matrix-rebaseline-plan
source_task_id: 20260417-03-subject-feature-matrix-rebaseline-plan
source_relation: roadmap-child-phase-4
design_doc: docs/dev/completed/20260417-03-phase-4-completed-feature-remigration-and-verification-rebuild/design-v1-01.md
plan_doc: docs/dev/completed/20260417-03-phase-4-completed-feature-remigration-and-verification-rebuild/plan-v1-01.md
total_tasks: 5
current_task: completed
active: false
---

## 当前结论

- `subject.features.json -> declared proof/benchmark assets` 的 obligation 基线已经补齐，新增缺口 `ArithmeticOpsProofEntry`、`GenericInterfaceDispatchBenchmarkEntry`、`PatchCallbackFlowProofEntry`、`MethodReplacementBenchmarkEntry` 已进入正式 subject 主线。
- `tests/contracts/shared/test_feature_obligation_coverage.py` 现在可以直接审计三大 canonical subjects 的 `proofRequired / benchmarkRequired` 覆盖。
- `tests/contracts/shared/test_phase5_legacy_cutover_contract.py` 已补上，锁住 canonical verification 不回退到 stdout 判定，也不回退到旧入口协议和旧目录语义。
- `SolutionCorePack`、`MixedExecutionFeaturePack`、`HotUpdateHostPack` 的 declared metadata / registry / planner / worker / benchmark source 主干回归已通过。

## 执行结果

- Batch 1: 完成 feature obligation RED 审计基线与缺口补齐。
- Batch 2: 完成 `SolutionCorePack` remigration 收口，并把新增 proof/benchmark 纳入 discovery 与 benchmark source contract。
- Batch 3: 完成 `MixedExecutionFeaturePack` remigration 回归确认，formal proof/benchmark/host 主干保持新结构。
- Batch 4: 完成 `HotUpdateHostPack` remigration 收口，并补齐 patch callback / method replacement 正式资产。
- Batch 5: 完成 Phase 4 回归、legacy cutover contract 与 Phase 5/6 输入整理。

## 验证

- `python -m pytest tests/contracts/shared/test_feature_obligation_coverage.py tests/integration/registry/test_declared_metadata_discovery.py tests/unit/performance/test_benchmark_subject_sources.py tests/unit/compatibility/test_hot_update_skeleton_subject.py -q`
  - `21 passed`
- `python -m pytest tests/contracts/shared/test_feature_obligation_coverage.py tests/integration/registry/test_declared_metadata_discovery.py tests/integration/registry/test_registry_scan.py tests/tooling/run/test_subject_command.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/performance/test_benchmark_subject_sources.py tests/unit/planning/test_solution_core_pack_planner.py tests/unit/execution/test_subject_workers.py tests/unit/run/test_repo_layout.py -q`
  - `146 passed`
- `python -m pytest tests/contracts/shared/test_feature_obligation_coverage.py tests/contracts/shared/test_phase5_legacy_cutover_contract.py tests/contracts/shared/test_subject_authority_freeze.py tests/contracts/shared/test_testframework_collection_contract.py tests/integration/registry/test_declared_metadata_discovery.py tests/integration/registry/test_registry_scan.py tests/tooling/run/test_subject_command.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/performance/test_benchmark_subject_sources.py tests/unit/planning/test_solution_core_pack_planner.py tests/unit/execution/test_subject_workers.py tests/unit/run/test_repo_layout.py -q`
  - `156 passed`

## 风险 / 阻塞

### risks

- 本轮没有跑全仓 `pytest`，完成判断基于 subject/test framework 主干与 Phase 4/5/6 相关回归集。

### blockers

- 当前无硬阻塞。

## 下一步

- 由 parent roadmap 统一收口并归档。

## wiki

- 已更新 `docs/architecture/managed-native-hotupdate-test-pipeline.md`
- 已更新 `wiki/04-工具与集成/统一测试框架.md`
- 已更新 `wiki/06-测试验证/新增测试接入规范.md`
- 已更新 `wiki/06-测试验证/INDEX.md`
- 已更新 `wiki/02-Skill体系/04-质量保障/project-test-governance.md`
