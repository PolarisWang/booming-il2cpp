---
task_id: 20260409-09-unified-entrypoint-solution-menu
title: 统一入口工程菜单与 Solution 入口优化
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-09 22:30:00 +08:00
updated_at: 2026-04-10 09:10:28 +08:00
current_dir: docs/dev/completed/20260409-09-unified-entrypoint-solution-menu
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260409-09-unified-entrypoint-solution-menu/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260409-09-unified-entrypoint-solution-menu/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260409-09-unified-entrypoint-solution-menu/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260409-09-unified-entrypoint-solution-menu/notes/progress-v1-03.md`

## 执行概况

- total_tasks: 6
- current_task: 全部 6 个任务已完成；当前状态已归档。

## 当前判断

- current_focus: `generate/build project subject|core` 已提升为 `Project And IDE` / `工程 / IDE` 入口；工作区根已迁移到 `solutions/**`；`generate project all` 已接入；Windows subject solution 已引用同次生成的 native project。
- why_now: 用户进一步要求 solution 不再放在 `artifacts/`，而是直接作为根目录可见的 IDE 入口；同时希望菜单中存在“一键生成所有 solution”的聚合选项。
- done_definition: 根目录布局、菜单位置、聚合入口、说明区、参数流和 subject solution/native project 关系都已落地，并通过计划内自动化回归验证。

## 最近摘要

- 2026-04-09 22:20:00 +08:00: 扫描统一入口现状，确认 `generate/build project subject|core` 已经存在于底层命令与 workspace 生成链路中。
- 2026-04-09 22:24:00 +08:00: 确认文本菜单走 `run_manifest.json` + `menu.py`，全屏 TUI 走 `tui.py`，单改 manifest 不能完成菜单改造。
- 2026-04-09 22:28:00 +08:00: 用户确认新增菜单入口的总体方向；当前方案冻结为新增 `工程 / IDE` 一级入口，并在菜单内收集必填参数。
- 2026-04-09 22:38:00 +08:00: 用户新增三项约束：solution 统一输出到仓库根、菜单需要有“生成所有 solution”选项、subject solution 生成时必须顺带生成 native project 并引用它。
- 2026-04-10 08:54:49 +08:00: 已切换到执行态；当前从 Task 1 开始，用 TDD 先补 manifest/project workspace/project command 的红测，再进入实现。
- 2026-04-10 09:10:28 +08:00: `run_manifest.json`、`project_workspace.py`、`commands/project.py`、`menu.py`、`tui.py` 与对应测试已完成改造；项目 wiki 已同步到 `solutions/**` 与 `Project And IDE` 新语义。
- 2026-04-10 09:10:28 +08:00: 已运行 `python -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_tui tests.tooling.run.test_tui_routing tests.tooling.run.test_project_command tests.unit.run.test_project_workspace`，结果通过。

## 下一步

- next_action: 无。若后续需要扩展“直接打开 VS / Xcode”或补 macOS/Linux native IDE 特化，再另开 follow-up。
- owner: codex
- trigger: 无。

## 风险 / 阻塞

### risks

- 如果只改菜单而不改 `project_workspace.py`，输出根仍会停留在 `artifacts/projects/**`，与新要求冲突。
- 如果只把命令露出来而不补参数选择流，`subject/core` 菜单入口依旧不可直接使用。
- 如果 subject solution 不引用 native project，用户仍需要手动分别打开 solution 与 native workspace，达不到这轮目标。

### blockers

- 无。
