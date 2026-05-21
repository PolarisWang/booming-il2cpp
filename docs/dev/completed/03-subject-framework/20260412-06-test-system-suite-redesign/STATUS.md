---
task_id: 20260412-06-test-system-suite-redesign
title: 测试体系 Solution-First Subject 重设计与能力覆盖模型
task_type: roadmap
lifecycle_status: completed
phase: archived
created_at: 2026-04-12 15:22:24 +08:00
updated_at: 2026-04-13 07:24:06 +08:00
current_dir: docs/dev/completed/20260412-06-test-system-suite-redesign
parent_task_id:
source_task_id: 20260412-05-phase-b-generic-native-aot-benchmark-backend
source_relation: paused-current-task-for-broader-redesign
active: false
---

## 关键文档
- brainstorm: `docs/dev/completed/20260412-06-test-system-suite-redesign/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260412-06-test-system-suite-redesign/design-v1-03.md`
- roadmap_or_plan: `docs/dev/completed/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- latest_progress: `docs/dev/completed/20260412-06-test-system-suite-redesign/notes/progress-v1-01.md`
- completed_child_phase0: `docs/dev/completed/20260412-07-phase-0-hybrid-model-freeze-and-migration-inventory/plan-v1-01.md`
- completed_child_phase1: `docs/dev/completed/20260412-08-phase-1-generic-test-taxonomy-cleanup/plan-v1-01.md`
- completed_child_phase2_batch1: `docs/dev/completed/20260412-09-phase-2-chaos-testing-contract-foundation-batch-1/plan-v1-01.md`
- completed_child_phase2_batch2: `docs/dev/completed/20260412-10-phase-2-declaration-metadata-discovery-batch-2/plan-v1-01.md`
- completed_child_phase3_batch1: `docs/dev/completed/20260412-11-phase-3-thin-manifest-foundation-batch-1/plan-v1-01.md`
- completed_child_phase3_batch2: `docs/dev/completed/20260412-12-phase-3-orchestration-shared-profiles-batch-2/plan-v1-01.md`
- completed_child_phase3_batch3: `docs/dev/completed/20260412-13-phase-3-engineering-core-compiled-catalog-batch-3/plan-v1-01.md`
- completed_child_phase4_batch1: `docs/dev/completed/20260412-14-phase-4-archetype-subject-cutover-batch-1/plan-v1-01.md`
- completed_child_phase5_batch1: `docs/dev/completed/20260413-15-phase-5-mainline-declared-unit-cutover-batch-1/plan-v1-01.md`
- completed_child_phase5_batch2: `docs/dev/completed/20260413-16-phase-5-mainline-legacy-proof-shell-cutover-batch-2/plan-v1-01.md`
- completed_child_phase5_batch3: `docs/dev/completed/20260413-17-phase-5-mainline-runtime-proof-shell-cutover-batch-3/plan-v1-01.md`
- completed_child_phase5_batch4: `docs/dev/completed/20260413-18-phase-5-mainline-advanced-proof-shell-cutover-batch-4/plan-v1-01.md`
- completed_child_phase6_batch1: `docs/dev/completed/20260413-19-phase-6-benchmark-hotupdate-cutover-batch-1/plan-v1-01.md`
- completed_child_phase6_batch2: `docs/dev/completed/20260413-20-phase-6-hot-update-host-pack-cutover-batch-2/plan-v1-01.md`
- completed_child_phase6_batch3: `docs/dev/completed/20260413-21-phase-6-mixed-execution-feature-pack-cutover-batch-3/plan-v1-01.md`
- completed_child_phase7_batch1: `docs/dev/completed/20260413-22-phase-7-registry-selector-artifact-pipeline-cutover-batch-1/plan-v1-01.md`
- completed_child_phase8_batch1: `docs/dev/completed/20260413-23-phase-8-compatibility-closure-and-legacy-cleanup-batch-1/plan-v1-01.md`

## 当前判断
- current_focus: 本 roadmap 已完成归档，v1.03 的 solution-first + attribute-declared hybrid 模型已经成为测试体系唯一 canonical 主线。
- why_done: Phase 0-Phase 8 全部子任务均已落地归档；`Solution*` archetype、`MainlineFeaturePack`、`PerformanceFeaturePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack`、统一 registry/selector/artifact/pipeline，以及 Phase 8 legacy cleanup 均已完成闭环验证。
- done_definition: 已满足。proof/benchmark 顶层 shell 不再是主路径，`tests/` 与 `subjects/` 职责已重新对齐，且 `python -m pytest -q` 全量结果为 `781 passed, 40 skipped`。

## 最近摘要
- 2026-04-12 15:22:24 +08:00: 创建 roadmap，冻结“subject = canonical solution source bundle”的改造主线，并开始拆分 Phase 0-Phase 8 子任务。
- 2026-04-13 05:58:37 +08:00: Phase 7 Batch 1 完成归档，统一 registry / selector / CLI / reporting / artifact 入口切换完成，正式进入 Phase 8。
- 2026-04-13 07:24:06 +08:00: Phase 8 Batch 1 完成归档，planning 层 legacy shell 回归用例全部切到 canonical packs；roadmap 全量验证结果为 `781 passed, 40 skipped`，整体收口完成。

## 下一步
- next_action: 无。后续若恢复 `20260412-05-phase-b-generic-native-aot-benchmark-backend` 或 `20260411-10-post-phase7-development-roadmap`，需以前述 roadmap 结论为新的前置基线重新规划。
- trigger: 仅在需要开启新的后续能力建设任务时再创建新 plan / roadmap。

## 风险 / 阻塞

### risks

- `20260412-05-phase-b-generic-native-aot-benchmark-backend` 与 `20260411-10-post-phase7-development-roadmap` 仍处于 hanging，需要在恢复前重新对齐本路线已完成的 engineering/declaration 边界。
- 未来若要进一步按版本管理 compatibility alias 的退役周期，应新开独立任务处理，不应回写本已归档 roadmap。

### blockers

- 无。
