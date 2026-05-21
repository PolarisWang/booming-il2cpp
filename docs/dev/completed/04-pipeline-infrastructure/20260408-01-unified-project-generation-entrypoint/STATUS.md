---
task_id: 20260408-01-unified-project-generation-entrypoint
title: 统一入口工程生成、核心宿主与部署输出
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-08 11:25:00 +0800
updated_at: 2026-04-08 13:13:54 +08:00
current_dir: docs/dev/completed/20260408-01-unified-project-generation-entrypoint
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260408-01-unified-project-generation-entrypoint/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260408-01-unified-project-generation-entrypoint/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260408-01-unified-project-generation-entrypoint/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260408-01-unified-project-generation-entrypoint/notes/progress-v1-02.md`

## 执行概况

- total_tasks: 5
- current_task: 全部 5 个 chunk 已完成，任务已归档

## 当前判断

- current_focus: 本任务已归档；统一入口已正式承载 `generate/build project subject|core` 与 `deploy core`，`artifacts/projects/**` 与 `deploy/core/**` 边界、`subject-exec` 手动刷新规则以及 core deploy 目标矩阵都已落地。
- why_now: 计划中的 5 个 chunk 已全部完成，相关 wiki 已补齐，focused Python 回归已经重新通过。
- done_definition: 已满足；`generate project subject|core`、`build project subject|core`、`deploy core` 与相关自动化验证全部落地。

## 最近摘要

- 2026-04-08 11:25:00 +0800: 已完成统一入口、subject/native/core 现状扫描，确认当前仓库缺少正式的 project/workspace 生成能力。
- 2026-04-08 11:25:00 +0800: 已形成方案对比，推荐“引用仓库真源的工作区生成器”而不是源码快照导出。
- 2026-04-08 11:25:00 +0800: 已判断本任务适合进入 roadmap，而不是直接写单阶段 plan。
- 2026-04-08 11:42:00 +0800: 用户确认 `subject-exec` 采用手动刷新，并要求统一入口下一步优先加入 `generate project subject`。
- 2026-04-08 11:42:00 +0800: 已生成旧版父 roadmap，原先阶段顺序为 subject generation -> subject build -> core/target expansion。
- 2026-04-08 13:05:00 +0800: 用户新增核心范围：IL2CPP 核心层需要输出原生 exe，公共语义类似 `dotnet <dll>`，并要求正式产物进入 `deploy/`。
- 2026-04-08 13:05:00 +0800: 已据此重写 brainstorm/design/roadmap，把任务改成“核心 deploy 主线 + 调试工作区主线”的双轨模型。
- 2026-04-08 13:20:00 +0800: 已创建 `plan-v1-01.md`、`docs/dev/ACTIVE.md` 与首份 `notes/progress-v1-01.md`，任务正式切到执行态。
- 2026-04-08 11:57:49 +08:00: 已实现 `build/toolchains/run/project_workspace.py` 与 `build/toolchains/run/deploy_layout.py`，并接入 `commands/project.py`、`commands/deploy.py`、`commands/build.py`、`run.py`。
- 2026-04-08 11:57:49 +08:00: 已补齐 `tests.tooling.run.test_project_command`、`tests.unit.run.test_project_workspace`、`tests.unit.run.test_deploy_core`，并更新 `wiki/04-工具与集成/unified-project-workspaces-and-deploy-core.md` 及索引。
- 2026-04-08 13:13:54 +08:00: 已再次运行 focused Python 回归：`python -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_project_command tests.unit.run.test_project_workspace tests.unit.run.test_deploy_core tests.tooling.run.test_registry_command tests.tooling.run.test_tui tests.tooling.run.test_build tests.tooling.run.test_operation_reporting`，结果为 `Ran 70 tests in 3.150s`、`OK (skipped=25)`。

## 下一步

- next_action: 无。后续若扩展更多 target delivery kind 或新的 core 产品入口，再从本任务冻结的工作区 / 部署边界派生 follow-up。
- owner: codex
- trigger: 仅当出现新的平台交付语义或新的统一入口能力需求时，才需要新开 follow-up。

## 风险 / 阻塞

### risks

- 如果后续把 `artifacts/projects/**` 与 `deploy/core/**` 混用，工作区与正式交付物的边界会再次失焦。
- subject native 调试工程与 canonical run-scoped artifact 的边界必须写清，否则后续容易把 `artifacts/projects/**` 误当正式证据。
- 如果未来继续扩展 direct-run host 语义，仍需坚持“正式产物进入 `deploy/`、调试工作区留在 `artifacts/projects/`”这一分层。

### blockers

- 无
