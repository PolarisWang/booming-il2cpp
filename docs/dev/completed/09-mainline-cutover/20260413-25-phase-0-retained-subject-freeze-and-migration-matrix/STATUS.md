---
task_id: 20260413-25-phase-0-retained-subject-freeze-and-migration-matrix
title: Phase 0 Retained Subject Freeze And Migration Matrix
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 08:57:38 +08:00
updated_at: 2026-04-13 09:08:24 +08:00
current_dir: docs/dev/completed/20260413-25-phase-0-retained-subject-freeze-and-migration-matrix
parent_task_id: 20260413-24-solution-core-pack-subject-consolidation
source_task_id: 20260413-24-solution-core-pack-subject-consolidation
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/design-v1-01.md`
- parent_roadmap: `docs/dev/in-progress/20260413-24-solution-core-pack-subject-consolidation/roadmap-v1-01.md`
- plan: `docs/dev/completed/20260413-25-phase-0-retained-subject-freeze-and-migration-matrix/plan-v1-01.md`

## 当前判断

- current_focus: Phase 0 已完成 retained subject 清单、准入规则和 legacy-to-retained 迁移矩阵冻结，后续阶段将以这批文档作为唯一迁移输入。
- why_done: 所有现有顶层 subject 都已经获得明确的长期归宿，retained subject 只保留三类，且“compatibility alias 最终一次性清零”已冻结为后续硬约束。
- done_definition: inventory、rules、mapping 三类文档已落地，父任务和索引已同步，下一步明确切换到 Phase 1 显式 `.sln` 升级。

## 最近摘要

- 2026-04-13 08:57:38 +08:00: 创建 Phase 0 子任务目录、状态文件与执行计划，准备切换 `ACTIVE`。
- 2026-04-13 09:08:24 +08:00: 完成 retained subject freeze、准入规则和 legacy-to-retained 迁移矩阵；子任务归档到 `completed/`，父任务切换到 Phase 1。

## 下一步

- next_action: 返回父 roadmap，执行 `20260413-26-phase-1-explicit-solution-upgrade`，先打通显式 `.sln` source 的 host-input build 与 declared catalog 解析。
- owner: codex
- trigger: 需要让 retained subject 从 project-set 升级为显式 solution 时立即恢复。

## 风险 / 阻塞

### risks

- `GenericEcho`、`CompatibilityMatrixProof`、`MobileHelloWorldProof` 的最终落点需要逐条重判，不能只按目录名推断。
- Phase 0 虽已冻结边界，但真正的显式 solution 通路和物理迁移仍未开始，后续阶段如果实现不到位仍可能把仓库留在半切换状态。

### blockers

- 当前无外部 blocker。
