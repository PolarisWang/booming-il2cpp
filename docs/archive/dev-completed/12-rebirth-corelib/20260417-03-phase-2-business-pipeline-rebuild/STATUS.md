---
task_id: 20260417-03-phase-2-business-pipeline-rebuild
title: Phase 2 Business Pipeline Rebuild
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-17 14:20:00 +08:00
updated_at: 2026-04-17 15:40:38 +08:00
latest_stop_point: Phase 2 completed; generated bucket has dropped legacy `workloadEntry`, runtime/interpreter benchmark manifests now classify resolution kind explicitly, and the Phase 2 regression suite is green
current_dir: docs/dev/completed/20260417-03-phase-2-business-pipeline-rebuild
parent_task_id: 20260417-03-subject-feature-matrix-rebaseline-plan
source_task_id: 20260417-03-subject-feature-matrix-rebaseline-plan
source_relation: roadmap-child-phase-2
design_doc: docs/dev/completed/20260417-03-phase-2-business-pipeline-rebuild/design-v1-01.md
plan_doc: docs/dev/completed/20260417-03-phase-2-business-pipeline-rebuild/plan-v1-01.md
total_tasks: 5
current_task: completed
active: false
---

## 完成结论

- planner / registry / workspace / report / dashboard 的业务消费面已经收口到 declared metadata、collection 和精简 manifest。
- `subjects/*/validation/proof/native-reference/main.cpp` 不再是 workspace native proof host 的生成前置。
- generated bucket 不再携带无用的 `workloadEntry`。
- managed / interpreter benchmark 结果 manifest 显式写入 `benchmarkResolutionKind`，把剩余 `workloadEntry` 明确降级为 fallback / trace 语义，而不是隐式业务真源。

## 本轮收口

- `subject_workers.py`
  - 删除 generated bucket 的 `workloadEntry`
  - 为 managed / interpreter benchmark 结果 manifest 增加 `benchmarkResolutionKind`
  - 维持 `legacy-workload-entry` 仅作为反射 fallback，不再让 structured host 路由依赖它
- `tests/unit/execution/test_subject_workers.py`
  - 锁定 generated manifest 不再写 `workloadEntry`
  - 锁定 runtime / interpreter benchmark manifest 的 resolution kind 合同
- Phase 2 台账完成归档，准备切入 Phase 3

## 已验证

- `python -m pytest tests/unit/execution/test_subject_workers.py -k "native_proof_emitter or runtime_perf_collect_passes_workload_entry_and_assembly_path_to_perf_harness or interpreter_runtime_perf_passes_workload_entry_and_assembly_path_to_harness or runtime_perf_collect_resolves_declared_benchmark_by_entry_index_for_structured_harness_execution or runtime_perf_collect_uses_collection_arguments_for_workspace_benchmark_host or interpreter_runtime_perf_uses_collection_arguments_for_workspace_benchmark_host" -q`
  - `6 passed`
- `python -m pytest tests/unit/planning/test_subject_planner.py tests/unit/registry/test_declared_registry_matrix_selection.py tests/integration/registry/test_registry_scan.py tests/unit/planning/test_project_workspace.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_perf_dashboard.py tests/unit/reporting/test_subject_reporting.py tests/unit/performance/test_benchmark_command.py tests/tooling/run/test_subject_command.py tests/unit/compatibility/test_hot_update_productization_gates.py tests/unit/execution/test_subject_workers.py tests/integration/run/test_perf_harness_hotupdate_benchmark_host.py -q`
  - `167 passed`

## 风险 / 余留

- Phase 3 仍要处理 subject 目录的旧命名、旧 solution path 与 `validation/` 残留。
- `workloadEntry` 在旧 manifest / 旧 fixture 里仍有兼容字段，彻底移除要等 Phase 5 一次性清扫。

## 下一步

- 切入 `20260417-03-phase-3-subject-system-full-rebirth`
- 先做 canonical subject 的目录与命名重生，开始清理 `Archetypes / FeatureSlices / PatchModules / Launcher / validation`

## wiki

- 本阶段暂不直接写入 wiki。
- 原因：主 roadmap 仍在继续，subject 目录结构和正式路径还会在 Phase 3~5 大幅收敛，等结构稳定后统一写入架构与治理文档。
