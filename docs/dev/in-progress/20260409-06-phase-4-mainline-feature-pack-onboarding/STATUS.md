---
task_id: 20260409-06-phase-4-mainline-feature-pack-onboarding
title: Phase 4 MainlineFeaturePack Onboarding
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-09 13:13:08 +08:00
updated_at: 2026-04-09 15:20:01 +08:00
current_dir: docs/dev/completed/20260409-06-phase-4-mainline-feature-pack-onboarding
parent_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260409-06-phase-4-mainline-feature-pack-onboarding/brainstorm-v1-02.md`
- design: `docs/dev/completed/20260409-06-phase-4-mainline-feature-pack-onboarding/design-v1-02.md`
- roadmap_or_plan: `docs/dev/completed/20260409-06-phase-4-mainline-feature-pack-onboarding/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260409-06-phase-4-mainline-feature-pack-onboarding/notes/progress-v1-03.md`

## 执行概况

- total_tasks: 4
- current_task: 已完成归档。

## 当前判断

- current_focus: `MainlineFeaturePack` 已从 planning-only manifest 接成真实完整 subject；proof entry 与 runtime managed `Main` 已拆开，后续 capability batch 可以继续挂在同一个 full subject 上推进。
- why_now: `Phase 4` 的目标不是再造一个大样例，而是把 `native output / trace / perf` 三条主线真正收口到一个主宿主上，为 `Phase 5/6/7` 提供稳定入口。
- done_definition: 真实 `source / validation / baselines` 目录已落地；subject-scoped proof build 已落地；`proof entry override` 已落地；`correctness.dev` / `correctness.platform` / `perf.profile` 三条统一入口均已验证。

## 最近摘要

- 2026-04-09 13:13:08 +08:00: 父 roadmap 派生 `Phase 4` child，开始把 planning-only `MainlineFeaturePack` 接成真实完整 subject。
- 2026-04-09 14:05:00 +08:00: 识别出 runtime `Main` 与 native proof entry 的结构冲突，确认采用 `proof entry override`，避免 trace CLI 入口与 proof lowering shape 互相污染。
- 2026-04-09 15:20:01 +08:00: 已落地 `proof entry override`、reachable closure 过滤、subject-scoped proof build，以及 `MainlineFeaturePack` 的真实 `source / unit / proof / perf` 目录结构。
- 2026-04-09 15:20:01 +08:00: 用户已确认 `contract` / `subject` 边界清理延后为单独 follow-up，不插入当前主任务。

## 下一步

- next_action: 无。后续回到父 roadmap，启动 `20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing` 的 design review，并在评审后进入实现。
- owner: codex
- trigger: `Phase 4` focused verification 全部通过，完整 subject 主线已经稳定。

## 风险 / 阻塞

### risks

- `contract` / `subject` 边界清理仍未执行；当前主线只是显式记录为 follow-up，不代表边界已经最终稳定。
- `trace` / `perf` 目前仍以 Windows 宿主为第一落点；这不是跨平台闭环，只是 `Phase A` 主线的首宿主收口。

### blockers

- 无。

## Completion Notes

- 2026-04-09 15:20:01 +08:00: `MainlineFeaturePack/ProofEntry::Run()` 作为 native proof entry，`Program.Main(string[] args)` 继续保留为 managed trace export 入口；两者职责已拆开。
- 2026-04-09 15:20:01 +08:00: `ManagedClosureContracts`、`DriverEntry`、`LoaderStage`、`subject_workers.py` 已支持 `proof entry override`，`LinkerStage` 已按 entry reachable closure 过滤。
- 2026-04-09 15:20:01 +08:00: `CMakeLists.txt` 与 `_windows_subject_build(...)` 已支持 `CHAOS_SUBJECT_PROOF_ROOT`，proof build 不再硬编码 `HelloWorldObject`。
- 2026-04-09 15:20:01 +08:00: 验证通过：
  - `python -m unittest tests.unit.run.test_phase4_mainline_feature_pack_onboarding`
  - `python -m unittest tests.unit.run.test_phase4_mainline_feature_pack_onboarding tests.unit.run.test_subject_workers.SubjectWorkersTests.test_windows_build_target_overrides_generator_and_records_non_repo_cmake_binary_dir tests.unit.run.test_subject_workers.SubjectWorkersTests.test_frontend_pipeline_worker_records_analysis_bundle_under_subject_artifacts`
  - `dotnet test subjects/MainlineFeaturePack/validation/unit/MainlineFeaturePack.Subject.UnitTests/MainlineFeaturePack.Subject.UnitTests.csproj -c Release`
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --validation-profile proof-dev --variant CHECK --json`，run id `20260409-145631-windows-475e`
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.platform --validation-profile trace-platform --variant CHECK --json`，run id `20260409-145903-windows-3667`
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json`，run id `20260409-145944-windows-472f`

## 关联任务

### parent

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`

### source

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`（roadmap-child）

### children

- 无。
