---
task_id: 20260412-14-phase-4-archetype-subject-cutover-batch-1
title: Phase 4 Archetype Subject Cutover Batch 1
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 00:34:29 +08:00
updated_at: 2026-04-13 00:59:47 +08:00
current_dir: docs/dev/completed/20260412-14-phase-4-archetype-subject-cutover-batch-1
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- plan: `docs/dev/completed/20260412-14-phase-4-archetype-subject-cutover-batch-1/plan-v1-01.md`

## 当前判断

- current_focus: Phase 4 Batch 1 已完成 archetype subject cutover，`SolutionSimpleLib`、`SolutionMultiProject`、`SolutionPackageReference` 已成为新的 canonical solution subjects。
- why_done: `subjects/golden/*` 已退化为 compatibility shell，`path_resolver` 与相关 registry/planner tests 已对齐新 canonical 路径，且 legacy subject id alias 可继续解析。
- done_definition: archetype solution subjects 成为工程级主链入口，`golden` 不再承担 canonical source 职责，相关定向验证与全量 `pytest` 均已通过。

## 最近摘要

- 2026-04-13 00:34:29 +08:00: 归档 Phase 3 Batch 3 后激活子任务 `20260412-14-phase-4-archetype-subject-cutover-batch-1`，开始 `golden -> Solution*` archetype cutover。
- 2026-04-13 00:59:47 +08:00: 完成 `Solution*` archetype subjects 落地、legacy `golden` compatibility shell 对齐、repo layout 契约补齐，并通过 `python -m pytest -q`，结果为 `712 passed, 40 skipped`。

## 下一步

- next_action: 返回父 roadmap，切入 Phase 5 Batch 1，先把 `MainlineFeaturePack` 这类已合并 proof subject 切到 `ChaosUnitTest` 声明式发现主线。
- owner: codex
- trigger: 需要让真实 subject 首次落地 `Chaos.TestFramework` 引用与 declared unit test catalog 时立即执行。

## 风险 / 阻塞

### risks

- `subjects/golden/*` 仍会保留一段时间作为 compatibility shell，Phase 8 之前需要持续避免团队把它重新当成 canonical source 使用。
- `convert-golden-multi-project` 目前只完成 canonical subject 重定向，suite/workload 命名的最终收口仍需在后续 engineering workload 清理阶段完成。

### blockers

- 当前无外部 blocker。
