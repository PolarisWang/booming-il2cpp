---
task_id: 20260409-09-unified-entrypoint-solution-menu
title: 统一入口工程菜单与 Solution 入口优化
task_type: plan
lifecycle_status: in_progress
phase: planning
created_at: 2026-04-09 22:30:00 +08:00
updated_at: 2026-04-09 22:38:00 +08:00
current_dir: docs/dev/in-progress/20260409-09-unified-entrypoint-solution-menu
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260409-09-unified-entrypoint-solution-menu/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260409-09-unified-entrypoint-solution-menu/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260409-09-unified-entrypoint-solution-menu/plan-v1-01.md`
- latest_progress: `docs/dev/in-progress/20260409-09-unified-entrypoint-solution-menu/notes/progress-v1-01.md`

## 执行概况

- total_tasks: 6
- current_task: 设计已确认；当前冻结根目录 `solutions/**` 布局、聚合生成入口、subject solution/native project 关系与菜单参数流，等待进入实现。

## 当前判断

- current_focus: 将现有 `generate/build project subject|core` 从“Build 子项”提升为“工程 / IDE”入口；把输出统一迁移到仓库根下的 `solutions/**`；新增“生成所有 Solution（当前宿主）”；并要求 subject solution 引用同次生成出的 native project。
- why_now: 用户进一步要求 solution 不再放在 `artifacts/`，而是直接作为根目录可见的 IDE 入口；同时希望菜单中存在“一键生成所有 solution”的聚合选项。
- done_definition: 根目录布局、菜单位置、聚合入口、说明区、参数流和 subject solution/native project 关系都已冻结，后续实现无需再回到交互层重新讨论。

## 最近摘要

- 2026-04-09 22:20:00 +08:00: 扫描统一入口现状，确认 `generate/build project subject|core` 已经存在于底层命令与 workspace 生成链路中。
- 2026-04-09 22:24:00 +08:00: 确认文本菜单走 `run_manifest.json` + `menu.py`，全屏 TUI 走 `tui.py`，单改 manifest 不能完成菜单改造。
- 2026-04-09 22:28:00 +08:00: 用户确认新增菜单入口的总体方向；当前方案冻结为新增 `工程 / IDE` 一级入口，并在菜单内收集必填参数。
- 2026-04-09 22:38:00 +08:00: 用户新增三项约束：solution 统一输出到仓库根、菜单需要有“生成所有 solution”选项、subject solution 生成时必须顺带生成 native project 并引用它。

## 下一步

- next_action: 按 `plan-v1-01.md` 进入实现，优先迁移输出根到 `solutions/**`，补 `generate project all`，并让 subject solution 引用 native project。
- owner: codex
- trigger: 用户确认开始实现，或后续会话直接进入执行。

## 风险 / 阻塞

### risks

- 如果只改菜单而不改 `project_workspace.py`，输出根仍会停留在 `artifacts/projects/**`，与新要求冲突。
- 如果只把命令露出来而不补参数选择流，`subject/core` 菜单入口依旧不可直接使用。
- 如果 subject solution 不引用 native project，用户仍需要手动分别打开 solution 与 native workspace，达不到这轮目标。

### blockers

- 无代码阻塞；当前只是等待是否进入实现。
