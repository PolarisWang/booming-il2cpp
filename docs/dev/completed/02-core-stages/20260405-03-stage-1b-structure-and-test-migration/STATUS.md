---
task_id: 20260405-03-stage-1b-structure-and-test-migration
title: Stage 1B Structure And Test Migration
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-05 22:40:00 +08:00
updated_at: 2026-04-05 23:32:04 +08:00
current_dir: docs/dev/completed/20260405-03-stage-1b-structure-and-test-migration
parent_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档
- brainstorm: `docs/dev/completed/20260405-03-stage-1b-structure-and-test-migration/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260405-03-stage-1b-structure-and-test-migration/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260405-03-stage-1b-structure-and-test-migration/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260405-03-stage-1b-structure-and-test-migration/notes/progress-v1-03.md`

## 当前判断
- current_focus: 承接 reboot roadmap 的 Stage 1B，把 managed skeleton、tests family 与 tooling canonical path 迁移落地。
- why_now: Stage 1A 已完成 contract surface freeze，需要立即把结构层与测试层的 canonical 方向固化到工程入口，避免后续 proof 阶段继续踩旧路径。
- done_definition: repo 的目录结构、tests family 和 tooling 路径与 reboot 设计保持一致。

## 最近摘要
- 2026-04-05 22:40:00 +08:00: 从 reboot roadmap 派生为 Stage 1B 子任务，目前保持非 active，等待 Stage 1A 输出进一步冻结。
- 2026-04-05 23:20:00 +08:00: Stage 1A 已归档，Stage 1B 正式切换为 active 执行；当前先同步任务状态，再进入 TDD，锁定 canonical tests path 与 managed skeleton 期望。
- 2026-04-05 23:32:04 +08:00: 已完成 Stage 1B 落地：`src/managed/Chaos.IL2CPP.*` skeleton 建立、`tests/proof` 输入与 suite 骨架建立、run/build/verify/tooling 全部切到 `tests/contracts` canonical 路径，并补齐 wiki 与任务状态同步。

## 下一步
- next_action: 无。若继续 reboot 主线，下一步从父 roadmap 派生 Stage 2 `HelloWorld` proof spec 子任务。
- owner: codex
- trigger: 无。

## 风险/阻塞
### risks
- 兼容层 `tests/contract/` 仍保留在仓库中，后续阶段若继续新增内容，必须明确禁止写入该路径。
- 对外公开命令族目前仍保持 `contract` 命名，而源码树 canonical 根目录已经切到 `tests/contracts`；后续如继续扩展 test family，需要继续保持命名与目录映射的一致性说明。

### blockers
- 无。

## 关联任务
### parent
- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`

### source
- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`：roadmap-child

### children
- 无。

