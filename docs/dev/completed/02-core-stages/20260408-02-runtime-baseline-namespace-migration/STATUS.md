---
task_id: 20260408-02-runtime-baseline-namespace-migration
title: runtime-baseline 命名空间迁移
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-08 13:40:17 +08:00
updated_at: 2026-04-08 14:32:41 +08:00
current_dir: docs/dev/completed/20260408-02-runtime-baseline-namespace-migration
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260408-02-runtime-baseline-namespace-migration/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260408-02-runtime-baseline-namespace-migration/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260408-02-runtime-baseline-namespace-migration/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260408-02-runtime-baseline-namespace-migration/notes/progress-v1-01.md`

## 执行概况

- total_tasks: 5
- current_task: 已完成归档

## 当前判断

- current_focus: 统一入口、registry、low-level verify 脚本、live docs/wiki 已全部切到 `runtime-baseline` / `platform-gates` canonical surface。
- why_now: 用户要求直接完成迁移并清除 `roadmap-0` 作为 live canonical 命名。
- done_definition: 新 canonical 命名生效、旧入口保持兼容、聚焦回归通过、registry 自检无 warning。

## 最近摘要

- 2026-04-08 13:40:17 +08:00: 完成受影响面扫描，冻结 `runtime-baseline` / `platform-gates` 迁移方案。
- 2026-04-08 14:32:41 +08:00: 完成 run manifest、prepare/test/verify、registry alias、low-level wrapper、system/pipeline manifest、live docs/wiki 迁移。
- 2026-04-08 14:32:41 +08:00: 通过聚焦 unittest 批次、`run.py test registry list --json` 与 `run.py test registry check-consistency --json`。

## 下一步

- next_action: 无。后续仅在需要继续清理未索引历史副本或做更大范围命名清洗时另开 follow-up。
- owner: codex
- trigger: 用户提出新的 follow-up。

## 风险 / 阻塞

### risks

- 由于当前环境限制，任务目录未做原地目录移动，而是复制归档到 `completed/` 并从索引移除 `in-progress` 条目；这不影响仓库的 live 任务状态。

### blockers

- 无。
