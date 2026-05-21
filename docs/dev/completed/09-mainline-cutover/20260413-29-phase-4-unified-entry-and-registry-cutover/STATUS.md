---
task_id: 20260413-29-phase-4-unified-entry-and-registry-cutover
title: Phase 4 Unified Entry And Registry Cutover
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 12:14:06 +08:00
updated_at: 2026-04-13 13:39:09 +08:00
current_dir: docs/dev/completed/20260413-29-phase-4-unified-entry-and-registry-cutover
parent_task_id: 20260413-24-solution-core-pack-subject-consolidation
source_task_id: 20260413-24-solution-core-pack-subject-consolidation
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/completed/20260413-24-solution-core-pack-subject-consolidation/design-v1-01.md`
- parent_roadmap: `docs/dev/completed/20260413-24-solution-core-pack-subject-consolidation/roadmap-v1-01.md`
- plan: `docs/dev/completed/20260413-29-phase-4-unified-entry-and-registry-cutover/plan-v1-01.md`

## 当前判断

- current_focus: Phase 4 child 已完成，并在统一 retained entry / registry cutover 后继续承接了 retained subject verification 与 Phase 6 第一批 cleanup 收口。
- why_done: `SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack` 的 public path 与 canonical dispatch 已对齐；`SolutionCorePack` 的 3 个 archetype managed-output matrix、3 个 retained subject 的 proof / benchmark 真实运行全部通过；compatibility alias 已从 `path_resolver.py` 一次性删除；已迁入 `SolutionCorePack` 的 `SolutionSimpleLib`、`SolutionMultiProject`、`SolutionPackageReference` legacy roots 已物理删除。
- done_definition: 三个 retained subject 使用统一 retained subject public path；manifest / planner / resolver 不再依赖 alias 回退；registry / selector / CLI 默认面向 retained subject；相关回归、registry refresh 与真实 subject runs 全部通过。

## 最近摘要

- 2026-04-13 12:14:06 +08:00: Phase 4 子任务激活，准备先审计 retained subject 的 launcher、manifest、matrix entry 与 registry 暴露差异，再决定最小 cutover 方案。
- 2026-04-13 13:26:39 +08:00: 完成 unified retained cutover 收口，并直接连带完成 verification + cleanup closure；`SolutionCorePack` archetype roots 已迁入并删除旧根，compatibility alias 已清零，所有 retained solution-mode subject 的 proof / benchmark 与 archetype managed-output matrix 均已真实跑通。
- 2026-04-13 13:39:09 +08:00: 归档后补跑最终验证：`142` 个回归测试再次通过，`registry refresh` 成功，`SolutionCorePack` 3 个 archetype matrix + proof/perf、`HotUpdateHostPack` proof/perf、`MixedExecutionFeaturePack` proof/perf 全部再次真实跑通。

## 下一步

- next_action: 无。本 child task 已完成并归档。
- owner: codex
- trigger: 如需继续展开更大范围的 `tests/` taxonomy 重构，另起 follow-up。

## 风险 / 阻塞

### risks

- 本 child 内只做了与 legacy `Solution*` roots 解耦直接相关的最小 `tests/` 清理：把对真实 top-level subject 根的依赖改为 synthetic fixture。更大范围的 `tests/` taxonomy 重构未在本 child 展开，但这不影响当前 done definition。

### blockers

- 当前无外部 blocker。
