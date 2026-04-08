---
task_id: 20260408-02-runtime-baseline-namespace-migration
title: runtime-baseline 命名空间迁移
task_type: plan
lifecycle_status: in_progress
phase: executing
created_at: 2026-04-08 13:40:17 +08:00
updated_at: 2026-04-08 13:40:17 +08:00
current_dir: docs/dev/in-progress/20260408-02-runtime-baseline-namespace-migration
parent_task_id:
source_task_id:
source_relation:
active: true
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260408-02-runtime-baseline-namespace-migration/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260408-02-runtime-baseline-namespace-migration/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260408-02-runtime-baseline-namespace-migration/plan-v1-01.md`
- latest_progress: `docs/dev/in-progress/20260408-02-runtime-baseline-namespace-migration/notes/progress-v1-01.md`

## 执行概况

- total_tasks: 5
- current_task: Chunk 1，先锁定新 canonical 命名与旧入口兼容断言。

## 当前判断

- current_focus: live surface 需要从 `roadmap-0` 迁到 `runtime-baseline` / `platform-gates`，历史归档与讨论文档保持历史名不动。
- why_now: 用户已确认按该方案执行迁移，当前仓库的正式输出与统一入口已经具备收口命名的条件。
- done_definition: 新 canonical 命名完成切换，旧入口保持兼容，聚焦回归通过，live wiki/docs 与 registry 语义一致。

## 最近摘要

- 2026-04-08 13:40:17 +08:00: 已扫描 `roadmap-0` 在命令、registry、脚本、artifact、文档与测试中的 live 引用面。
- 2026-04-08 13:40:17 +08:00: 已冻结 canonical 方案：runtime 主线用 `runtime-baseline`，gate 文档用 `platform-gates`，旧入口保兼容。
- 2026-04-08 13:40:17 +08:00: 已写入 brainstorm、design 与 plan，准备按 TDD 先改失败测试。

## 下一步

- next_action: 修改受影响测试，先让新 canonical 命名和旧兼容入口形成明确 RED。
- owner: codex
- trigger: 完成测试改动后进入命令、registry 与低层脚本实现迁移。

## 风险 / 阻塞

### risks

- `system` object id 变更会影响 registry、pipeline 解析和命令示例，必须同步迁移。
- `docs/architecture/roadmap-0/` 的 live 重命名会影响路径断言和 overview 文档。
- 旧 wrapper 如果只保文件名不保行为，用户现有脚本仍会断。

### blockers

- 无。
