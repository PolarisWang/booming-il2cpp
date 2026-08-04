---
task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
title: IL2CPP Mainline 性能优先核心与完整 Subject 路线图
task_type: roadmap
lifecycle_status: completed
phase: archived
created_at: 2026-04-09 00:05:09 +08:00
updated_at: 2026-04-10 02:18:40 +08:00
current_dir: docs/dev/completed/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/brainstorm-v1-02.md`
- design: `docs/dev/completed/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/design-v1-02.md`
- roadmap_or_plan: `docs/dev/completed/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/roadmap-v1-02.md`
- latest_progress: `docs/dev/completed/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/notes/progress-v1-12.md`

## 当前判断

- current_focus: 父 roadmap 已完成归档，`Phase A performance-first core mainline roadmap` 已闭环。
- why_now: `Phase 0` 到 `Phase 7` 全部 child 已完成，其中 `Phase 7 engine binding` 也已用正式 subject / matrix / artifact-chain 收口。
- done_definition: 当前可以表述“performance-first core mainline roadmap 已完成”，但仍然不能表述“完整 `C# -> C++ total solution` 已成立”。

## 最近摘要

- 2026-04-10 00:35:24 +08:00: `Phase 6` focused correctness 与 unified `perf.profile` 全部通过，native perf baseline compare 恢复为热态 `ok`。
- 2026-04-10 00:45:00 +08:00: 正式派生 `Phase 7` child `20260410-12-phase-7-engine-binding-tail-roll-in`。
- 2026-04-10 02:14:51 +08:00: `Phase 7` 三个最小 engine subject 六条 Windows run 全部通过。
- 2026-04-10 02:15:54 +08:00: unified registry 已刷新并纳入三个 engine subject。
- 2026-04-10 02:18:40 +08:00: 父 roadmap 归档为 `completed`。

## 下一步

- next_action: 无。后续如需推进 total solution、contract root 清理、非 Windows engine binding 或更大 engine API 面，请另开 follow-up。
- owner: codex
- trigger: `Phase 7` 已完成，roadmap 不再有未完成 child。

## 风险 / 阻塞

### risks

- 当前完成态仍只覆盖 `Phase A performance-first core mainline roadmap`，不覆盖完整 `.NET/BCL/platform` 总方案。
- `contract root` 与 subject 内容拆分清理仍是单独 follow-up，不在本 roadmap 内。
- engine binding 当前是最小 Windows-first 闭环；后续平台扩展与 richer engine API 仍需新任务。

### blockers

- 当前无阻塞。

## 关联任务

### parent

- 无。

### source

- 无。

### children

- 已完成：
  - `20260409-03-phase-0-completeness-definition-and-performance-governance`
  - `20260409-02-phase-1-optimization-facts-and-native-perf-contracts`
  - `20260409-04-phase-2-core-contract-cutover`
  - `20260409-05-phase-3-capability-driven-lowering-and-arbitration`
  - `20260409-06-phase-4-mainline-feature-pack-onboarding`
  - `20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing`
  - `20260409-08-phase-6-capability-batch-b-delegate-exception-and-phase-a-closure`
  - `20260410-12-phase-7-engine-binding-tail-roll-in`
