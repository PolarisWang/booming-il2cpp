---
task_id: 20260409-02-phase-1-optimization-facts-and-native-perf-contracts
title: Phase 1 OptimizationFacts 与 Native Perf Contracts
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-09 00:21:12 +08:00
updated_at: 2026-04-09 10:22:00 +08:00
current_dir: docs/dev/completed/20260409-02-phase-1-optimization-facts-and-native-perf-contracts
parent_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260409-02-phase-1-optimization-facts-and-native-perf-contracts/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260409-02-phase-1-optimization-facts-and-native-perf-contracts/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260409-02-phase-1-optimization-facts-and-native-perf-contracts/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260409-02-phase-1-optimization-facts-and-native-perf-contracts/notes/progress-v1-04.md`

## 执行概况

- total_tasks: 5
- current_task: 已完成归档

## 当前判断

- current_focus: 本任务已归档；`OptimizationFacts`、native perf evidence/report 与 `MainlineFeaturePack` 第一版 planning surface 已冻结，但这不等价于完整 `C# -> C++` total solution 已完成。
- why_now: 用户要求先把 performance-first core contract 收紧，再为后续 `Phase 2/3/4` 提供稳定入口。
- done_definition: 已满足；first-class `OptimizationFacts` artifact、native perf report surface、planning-only full-subject contract 与聚焦验证均已完成。

## 最近摘要

- 2026-04-09 00:21:12 +08:00: 父 roadmap 已确认新增独立完整 subject `MainlineFeaturePack`。
- 2026-04-09 00:21:12 +08:00: 已确认首批性能导向 capability 继续推进，且 `exceptions` 不能被排除在主线外。
- 2026-04-09 00:21:12 +08:00: 已确认 `engine binding` 放到整个 roadmap 最后，因此本阶段只冻结 core/native-perf/full-subject contract，不提前扩 engine 维度。
- 2026-04-09 10:22:00 +08:00: 已把 `OptimizationFacts` 作为 managed closure 正式 artifact 接入 `Linker -> CodeGen -> Driver`，并补齐 schema/sample/snapshot。
- 2026-04-09 10:22:00 +08:00: 已冻结 `MainlineFeaturePack` 的 planning-only manifest，以及 `native-runtime-perf` 的 worker/report/command surface。
- 2026-04-09 10:22:00 +08:00: 已通过本任务 required suites；更宽一层的 `tests.integration.run.test_registry_scan` 仍有 2 个既有 alias 断言失败，但不属于本任务改动面。

## 下一步

- next_action: 无。后续回到父 roadmap，派生 `phase-2-core-contract-cutover`。
- owner: codex
- trigger: 用户要求继续推进父 roadmap。

## 风险 / 边界

### risks

- 本任务不声明 `MainlineFeaturePack` 已完成真实 source/unit/proof/perf 工程接入；那属于 `Phase 4`。
- 本任务不声明 generated native 的全局性能最优已被全量证明；这里只冻结了 `Phase 0/1` 的治理规则与 contract 入口。
- 本任务按原计划暂不写 wiki；待 `Phase 2/3/4` 真正落地后，再统一回写长期知识页。

### blockers

- 无。

## 关联任务

### parent

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`

### source

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`（roadmap-child）

### children

- 无。

## Completion Notes

- 2026-04-09 10:22:00 +08:00: 完成 `OptimizationFacts` artifact、native perf report attachments 和 `MainlineFeaturePack` planning surface 的 Phase 1 冻结。
- 2026-04-09 10:22:00 +08:00: 验证通过 `python -m unittest tests.unit.run.test_subject_manifest_schema tests.unit.run.test_subject_planner tests.unit.run.test_subject_reporting tests.unit.run.test_subject_workers`。
- 2026-04-09 10:22:00 +08:00: 验证通过 `python -m unittest tests.unit.run.test_subject_workers_perf`。
- 2026-04-09 10:22:00 +08:00: 验证通过 `python -m unittest tests.tooling.run.test_subject_command`。
- 2026-04-09 10:22:00 +08:00: 验证通过 `python -m unittest tests.unit.run.test_managed_closure_contract_bundle`。
- 2026-04-09 10:22:00 +08:00: 组合回归通过 `python -m unittest tests.unit.run.test_subject_manifest_schema tests.unit.run.test_subject_planner tests.unit.run.test_subject_reporting tests.unit.run.test_subject_workers tests.unit.run.test_subject_workers_perf tests.tooling.run.test_subject_command tests.unit.run.test_managed_closure_contract_bundle`（38 tests）。
- 2026-04-09 10:22:00 +08:00: 边界检查 `python -m unittest tests.integration.run.test_registry_scan` 仍有 2 个既有失败；失败点为旧 alias `system/roadmap-0-android-startup-gate` / `system/roadmap-0-ios-packaging-gate`，而当前 registry 导出 `system/android-startup-gate` / `system/ios-packaging-gate`。
- follow_up: 回到父 roadmap，派生 `phase-2-core-contract-cutover`。
