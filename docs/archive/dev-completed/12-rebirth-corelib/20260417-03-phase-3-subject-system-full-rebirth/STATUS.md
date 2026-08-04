---
task_id: 20260417-03-phase-3-subject-system-full-rebirth
title: Phase 3 Subject System Full Rebirth
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-17 15:40:38 +08:00
updated_at: 2026-04-17 23:40:00 +08:00
latest_stop_point: Phase 3 completed; three canonical subjects now live on the reborn directory model, legacy `validation/proof/native-reference` has been removed, and Phase 3 regression evidence is green
current_dir: docs/dev/completed/20260417-03-phase-3-subject-system-full-rebirth
parent_task_id: 20260417-03-subject-feature-matrix-rebaseline-plan
source_task_id: 20260417-03-subject-feature-matrix-rebaseline-plan
source_relation: roadmap-child-phase-3
design_doc: docs/dev/completed/20260417-03-phase-3-subject-system-full-rebirth/design-v1-01.md
plan_doc: docs/dev/completed/20260417-03-phase-3-subject-system-full-rebirth/plan-v1-01.md
total_tasks: 5
current_task: completed
active: false
---

## 完成结论

- 三个 canonical subject 已全部切到新结构：
  - `SolutionCorePack`
  - `MixedExecutionFeaturePack`
  - `HotUpdateHostPack`
- 正式 subject source 中已经移除旧命名与旧旁路：
  - `Archetypes`
  - `FeatureSlices`
  - `PatchModules`
  - `Launcher`
  - `validation/proof/native-reference`
- Windows native proof CMake source 已切到 workspace 物化模板，不再依赖 subject 内嵌 proof host。
- 与 subject 结构重生直接相关的 compatibility / planner / registry / worker 测试已同步对齐到新模型。

## 本轮收口

- `HotUpdateHostPack`
  - `Archetypes -> EngineeringScenarios`
  - `PatchModules -> Patch`
  - solution / csproj / manifest / tests 对齐
- `SolutionCorePack`
  - `Launcher -> Host`
  - `FeatureSlices -> Proofs`
  - `Archetypes -> EngineeringScenarios`
  - 删除 `validation/proof/native-reference`
- `MixedExecutionFeaturePack`
  - `ManagedBridge / Lowering / Archetypes` 收口到 `Host / Proofs / Benchmarks / EngineeringScenarios`
  - manifest / tests 对齐
- `subject_workers.py`
  - 改为物化 generic native proof workspace 模板
  - root `CMakeLists.txt` 不再路由 `CHAOS_SUBJECT_PROOF_ROOT`

## 已验证

- `python -m pytest tests/unit/compatibility/test_mainline_feature_pack_onboarding.py tests/unit/compatibility/test_interface_dispatch_aot_runtime_completeness.py tests/unit/compatibility/test_linker_stripping_proof_subject.py -q`
  - `12 passed`
- `python -m pytest tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py tests/unit/compatibility/test_subject_manifest_schema.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/compatibility/test_mainline_feature_pack_onboarding.py tests/unit/compatibility/test_native_reference_bootstrap_support.py tests/unit/compatibility/test_async_await_proof_subject.py tests/unit/compatibility/test_interface_dispatch_aot_runtime_completeness.py tests/unit/compatibility/test_linker_stripping_proof_subject.py tests/unit/compatibility/test_marshaling_proof_subject.py tests/unit/compatibility/test_nested_exception_proof_subject.py tests/unit/compatibility/test_threading_gc_proof_subject.py tests/unit/execution/test_subject_workers.py tests/unit/run/test_repo_layout.py -q`
  - `126 passed`
- `python -m pytest tests/integration/registry/test_registry_scan.py tests/integration/registry/test_declared_metadata_discovery.py tests/tooling/run/test_subject_command.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py tests/unit/compatibility/test_loader_switch_opcode.py tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/compatibility/test_subject_manifest_schema.py tests/unit/performance/test_benchmark_subject_sources.py tests/unit/planning/test_solution_core_pack_planner.py -q`
  - `98 passed`

## 风险 / 余留

- Phase 4 仍要把 `subject.features.json` authority 与正式 proof / benchmark / hotupdate 资产逐条重新挂接。
- 仓库历史文档中仍会出现旧命名，这是历史记录，不属于正式 product/test 结构残留。

## 下一步

- 切入 `20260417-03-phase-4-completed-feature-remigration-and-verification-rebuild`
- 先做 completed feature obligation 审计基线，再按 `SolutionCorePack / MixedExecutionFeaturePack / HotUpdateHostPack` 逐个收口

## wiki

- 本阶段暂不直接写入 wiki。
- 原因：Phase 4~6 还要继续收口 feature obligation、legacy purge 和 coverage automation，待主 roadmap 结构稳定后统一沉淀。
