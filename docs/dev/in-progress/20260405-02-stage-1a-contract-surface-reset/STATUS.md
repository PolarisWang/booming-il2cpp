---
task_id: 20260405-02-stage-1a-contract-surface-reset
title: Stage 1A Contract Surface Reset
task_type: plan
lifecycle_status: in_progress
phase: planning
created_at: 2026-04-05 22:40:00 +08:00
updated_at: 2026-04-05 22:40:00 +08:00
current_dir: docs/dev/in-progress/20260405-02-stage-1a-contract-surface-reset
parent_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_relation: roadmap-child
active: true
---

## 关键文档
- brainstorm: `docs/dev/in-progress/20260405-02-stage-1a-contract-surface-reset/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260405-02-stage-1a-contract-surface-reset/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260405-02-stage-1a-contract-surface-reset/plan-v1-01.md`
- latest_progress: `docs/dev/in-progress/20260405-02-stage-1a-contract-surface-reset/notes/progress-v1-01.md`

## 当前判断
- current_focus: 先冻结 proof-facing contract surface，包括 `contracts/` 单根语义、artifact schema/example、canonical `subjectId`、ABI/bridge proof-required subset 与 direct-call landing 约束。
- why_now: parent reboot 任务已升级为 roadmap；当前最小可执行子任务应先锁 contract surface，避免后续结构迁移反复变更 canonical path。
- done_definition: Stage 1B 与后续 proof-spec 子任务都能以本任务产出的 contract 边界为稳定上游。

## 最近摘要
- 2026-04-05 22:40:00 +08:00: 从 reboot roadmap 派生为第一个 active 子任务，承接 Stage 1A。

## 下一步
- next_action: 从 `plan-v1-01.md` 的 Chunk 1 开始，先统一 `contracts/` 根目录语义与 shared `subjectId` 落点。
- owner: codex
- trigger: parent roadmap 已确认 “先 1A，后 1B” 的执行顺序。

## 风险/阻塞
### risks
- 如果 contract 冻结和结构迁移同时大面积推进，canonical path 与 compatibility path 容易打架。
- 如果 `subjectId` 规则在这一层没有写死，Stage 2 以后会重新引入隐式映射。

### blockers
- 无。

## 关联任务
### parent
- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`

### source
- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`：roadmap-child

### children
- 无。

