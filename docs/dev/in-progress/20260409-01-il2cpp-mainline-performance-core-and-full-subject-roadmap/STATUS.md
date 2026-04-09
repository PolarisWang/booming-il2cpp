---
task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
title: IL2CPP Mainline 性能优先核心与完整 Subject 路线图
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-09 00:05:09 +08:00
updated_at: 2026-04-09 15:20:01 +08:00
current_dir: docs/dev/in-progress/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/brainstorm-v1-02.md`
- design: `docs/dev/in-progress/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/design-v1-02.md`
- roadmap_or_plan: `docs/dev/in-progress/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/roadmap-v1-02.md`
- latest_progress: `docs/dev/in-progress/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/notes/progress-v1-09.md`

## 当前判断

- current_focus: `20260409-06-phase-4-mainline-feature-pack-onboarding` 已完成；roadmap 当前切到 `20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing` 的 spec review。
- why_now: 完整 subject 主线已经稳定，接下来最影响 generated native 工程质量上限的缺口集中在 `dispatch`、`closed generics/layout`、`arrays/boxing`。
- done_definition: 当前 roadmap 仍然不是“完整 `C# -> C++ total solution`”；它只是 `Phase A performance-first core mainline roadmap`，只有在 `Phase 5/6/7` 收口后才能谈 `Phase A` 闭环。

## 最近摘要

- 2026-04-09 00:55:36 +08:00: `Phase 0` 完成，`DefinitionOfComplete` 与 `PerformanceGovernance` 已冻结。
- 2026-04-09 10:22:00 +08:00: `Phase 1` 完成，`OptimizationFacts`、native perf contract 与 `MainlineFeaturePack` 第一版 contract 已冻结。
- 2026-04-09 11:54:10 +08:00: `Phase 2` 完成，`SemanticWorld -> Linker -> CodeGen` 已切到 performance-first core contract。
- 2026-04-09 13:13:08 +08:00: `Phase 3` 完成，lowering family 选择与 arbitration 已从 sample-driven emitter 中迁出。
- 2026-04-09 15:20:01 +08:00: `Phase 4` 完成，`MainlineFeaturePack` 已成为真实完整 subject，并完成 `output / trace / perf` 三条统一入口验证。
- 2026-04-09 15:20:01 +08:00: 新增 `Phase 5` child `20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing`，当前停在 brainstorm/design review gate。

## 下一步

- next_action: 评审 `20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing/design-v1-01.md`；确认后再进入 `Phase 5` plan 和实现。
- owner: codex
- trigger: 用户要求继续推进主任务，但根据 brainstorming gate，`Phase 5` 实施前需要先过设计评审。

## 风险 / 阻塞

### risks

- 当前路线仍不能表述成完整 `C# -> C++` 总方案；`exceptions` 还在 `Phase 6`，`engine binding` 还在 `Phase 7` 尾阶段。
- `arrays/boxing` 目前是最硬的技术缺口：loader opcode、lowering family 与 runtime helper 需要一起补齐。
- 如果 `Phase 5` 重新退回多 subject 或 mega proof entry 方案，会破坏 `Phase 4` 刚建立的完整 subject 主宿主定位。

### blockers

- `Phase 5` implementation 被 brainstorm/design review gate 卡住；这不是技术阻塞，而是故意保留的设计确认点。

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
- 进行中：
  - `20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing`
- 待派生：
  - `phase-6-capability-batch-b-delegate-exception-and-phase-a-closure`
  - `phase-7-engine-binding-tail-roll-in`
