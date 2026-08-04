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

## 褰撳墠缁撹

- `subject.features.json -> declared proof/benchmark assets` 鐨?obligation 鍩虹嚎宸茬粡琛ラ綈锛屾柊澧炵己鍙?`ArithmeticOpsProofEntry`銆乣GenericInterfaceDispatchBenchmarkEntry`銆乣PatchCallbackFlowProofEntry`銆乣MethodReplacementBenchmarkEntry` 宸茶繘鍏ユ寮?subject 涓荤嚎銆?
- `tests/contracts/shared/test_feature_obligation_coverage.py` 鐜板湪鍙互鐩存帴瀹¤涓夊ぇ canonical subjects 鐨?`proofRequired / benchmarkRequired` 瑕嗙洊銆?
- `tests/contracts/shared/test_phase5_legacy_cutover_contract.py` 宸茶ˉ涓婏紝閿佷綇 canonical verification 涓嶅洖閫€鍒?stdout 鍒ゅ畾锛屼篃涓嶅洖閫€鍒版棫鍏ュ彛鍗忚鍜屾棫鐩綍璇箟銆?
- `SolutionCorePack`銆乣MixedExecutionFeaturePack`銆乣HotUpdateHostPack` 鐨?declared metadata / registry / planner / worker / benchmark source 涓诲共鍥炲綊宸查€氳繃銆?

## 鎵ц缁撴灉

- Batch 1: 瀹屾垚 feature obligation RED 瀹¤鍩虹嚎涓庣己鍙ｈˉ榻愩€?
- Batch 2: 瀹屾垚 `SolutionCorePack` remigration 鏀跺彛锛屽苟鎶婃柊澧?proof/benchmark 绾冲叆 discovery 涓?benchmark source contract銆?
- Batch 3: 瀹屾垚 `MixedExecutionFeaturePack` remigration 鍥炲綊纭锛宖ormal proof/benchmark/host 涓诲共淇濇寔鏂扮粨鏋勩€?
- Batch 4: 瀹屾垚 `HotUpdateHostPack` remigration 鏀跺彛锛屽苟琛ラ綈 patch callback / method replacement 姝ｅ紡璧勪骇銆?
- Batch 5: 瀹屾垚 Phase 4 鍥炲綊銆乴egacy cutover contract 涓?Phase 5/6 杈撳叆鏁寸悊銆?

## 楠岃瘉

- `python -m pytest tests/contracts/shared/test_feature_obligation_coverage.py tests/integration/registry/test_declared_metadata_discovery.py tests/unit/performance/test_benchmark_subject_sources.py tests/unit/compatibility/test_hot_update_skeleton_subject.py -q`
  - `21 passed`
- `python -m pytest tests/contracts/shared/test_feature_obligation_coverage.py tests/integration/registry/test_declared_metadata_discovery.py tests/integration/registry/test_registry_scan.py tests/tooling/run/test_subject_command.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/performance/test_benchmark_subject_sources.py tests/unit/planning/test_solution_core_pack_planner.py tests/unit/execution/test_subject_workers.py tests/unit/run/test_repo_layout.py -q`
  - `146 passed`
- `python -m pytest tests/contracts/shared/test_feature_obligation_coverage.py tests/contracts/shared/test_phase5_legacy_cutover_contract.py tests/contracts/shared/test_subject_authority_freeze.py tests/contracts/shared/test_testframework_collection_contract.py tests/integration/registry/test_declared_metadata_discovery.py tests/integration/registry/test_registry_scan.py tests/tooling/run/test_subject_command.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/performance/test_benchmark_subject_sources.py tests/unit/planning/test_solution_core_pack_planner.py tests/unit/execution/test_subject_workers.py tests/unit/run/test_repo_layout.py -q`
  - `156 passed`

## 椋庨櫓 / 闃诲

### risks

- 鏈疆娌℃湁璺戝叏浠?`pytest`锛屽畬鎴愬垽鏂熀浜?subject/test framework 涓诲共涓?Phase 4/5/6 鐩稿叧鍥炲綊闆嗐€?

### blockers

- 褰撳墠鏃犵‖闃诲銆?

## 涓嬩竴姝?

- 鐢?parent roadmap 缁熶竴鏀跺彛骞跺綊妗ｃ€?

## wiki

- 已更新 `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/subject-test-framework-v1/INDEX.md`
- 已更新 `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
- 宸叉洿鏂?`wiki/04-宸ュ叿涓庨泦鎴?缁熶竴娴嬭瘯妗嗘灦.md`
- 宸叉洿鏂?`wiki/06-娴嬭瘯楠岃瘉/鏂板娴嬭瘯鎺ュ叆瑙勮寖.md`
- 宸叉洿鏂?`wiki/06-娴嬭瘯楠岃瘉/INDEX.md`
- 宸叉洿鏂?`wiki/02-Skill浣撶郴/04-璐ㄩ噺淇濋殰/project-test-governance.md`
