---
task_id: 20260405-03-stage-1b-structure-and-test-migration
title: Stage 1B Structure And Test Migration
task_type: plan
lifecycle_status: in_progress
phase: planning
created_at: 2026-04-05 22:40:00 +08:00
updated_at: 2026-04-05 22:40:00 +08:00
current_dir: docs/dev/in-progress/20260405-03-stage-1b-structure-and-test-migration
parent_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档
- brainstorm: `docs/dev/in-progress/20260405-03-stage-1b-structure-and-test-migration/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260405-03-stage-1b-structure-and-test-migration/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260405-03-stage-1b-structure-and-test-migration/plan-v1-01.md`
- latest_progress: `docs/dev/in-progress/20260405-03-stage-1b-structure-and-test-migration/notes/progress-v1-01.md`

## 当前判断
- current_focus: 承接 reboot roadmap 的 Stage 1B，把 managed skeleton、tests family 与 tooling canonical path 迁移落地。
- why_now: parent roadmap 已确定该子任务应在 Stage 1A 之后推进，以避免 contract canonical path 尚未冻结时就开始大面积迁移。
- done_definition: repo 的目录结构、tests family 和 tooling 路径与 reboot 设计保持一致。

## 最近摘要
- 2026-04-05 22:40:00 +08:00: 从 reboot roadmap 派生为 Stage 1B 子任务，目前保持非 active，等待 Stage 1A 输出进一步冻结。

## 下一步
- next_action: 以 Stage 1A 的 contract 输出为前提，准备执行 `plan-v1-01.md` 的 Chunk 1 和 Chunk 2。
- owner: codex
- trigger: `20260405-02-stage-1a-contract-surface-reset` 完成 contract surface freeze 或至少锁定 canonical path。

## 风险/阻塞
### risks
- 如果过早执行，容易在旧路径和新路径之间反复返工。
- 如果 tooling path 切换与 tests family 调整不同步，容易造成大量脆弱失败。

### blockers
- 需要等待 `20260405-02-stage-1a-contract-surface-reset` 锁定关键 canonical 路径与 shared identity 落点。

## 关联任务
### parent
- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`

### source
- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`：roadmap-child

### children
- 无。

