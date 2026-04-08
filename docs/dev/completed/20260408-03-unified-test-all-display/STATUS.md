---
task_id: 20260408-03-unified-test-all-display
title: 统一入口 test all 展示增强
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-08 18:10:00 +08:00
updated_at: 2026-04-08 19:15:45 +08:00
current_dir: docs/dev/completed/20260408-03-unified-test-all-display
parent_task_id:
source_task_id:
source_relation:
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260408-03-unified-test-all-display/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260408-03-unified-test-all-display/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260408-03-unified-test-all-display/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260408-03-unified-test-all-display/notes/progress-v1-01.md`

## 当前判断

- current_focus: Task 1，先补失败测试，锁定 `test all` 入口文案、预览页、progress 汇总和 summary 摘要的目标行为。
- why_now: 当前已经有设计文档，可以切换到 TDD 执行；先锁行为，再补聚合字段和渲染实现。
- done_definition: `test all` 在菜单、启动前预览、运行中 progress、完成后 summary 都输出更完整信息，相关测试和回归通过。

## 执行概况

- total_tasks: 4
- current_task: Task 1，先补失败测试，覆盖菜单预览、progress 汇总和 summary 摘要。

## 最近摘要

- 2026-04-08 18:10:00 +08:00: 已确认当前问题不在执行链路，而在入口标题、progress 页面和 summary 页面展示过薄。
- 2026-04-08 18:10:00 +08:00: 已比较轻量补文案、批次可视化、面板化 dashboard 三条路径，推荐“批次可视化”方案。
- 2026-04-08 18:10:00 +08:00: 用户已同意按推荐方案继续收口正式设计，不进入实现。
- 2026-04-08 19:05:00 +08:00: 已将设计转为实现计划，接下来按 TDD 顺序补失败测试、再补聚合字段与渲染层。

## 下一步

- next_action: 修改 `tests/tooling/run/test_tui.py` 和 `tests/tooling/run/test_unified_test_command.py`，先让针对新展示行为的测试失败。
- owner: codex
- trigger: 失败测试落下后进入数据聚合和渲染实现。

## 风险 / 边界

### risks

- 如果只改文案不补聚合字段，运行中和完成后页面仍然无法回答“这次到底跑了什么、失败在哪里”。
- 如果现在直接做 dashboard 级复杂交互，会把统一入口的学习成本抬高，超出当前需求。

### blockers

- 无。

## Completion Notes

- 2026-04-08 19:15:45 +08:00: 完成 `test all` 入口标题、启动前 preview、运行中 batch/count/current/failure 展示，以及 summary 的 overall/breakdown/failure digest 增强。
- 2026-04-08 19:15:45 +08:00: 验证通过 `python -m unittest tests.tooling.run.test_tui tests.tooling.run.test_unified_test_command`。
- 2026-04-08 19:15:45 +08:00: 验证通过 `python -m unittest tests.tooling.run.test_command_manifest tests.tooling.run.test_registry_command tests.tooling.run.test_tui tests.tooling.run.test_unified_test_command tests.unit.run.test_repo_layout`。
- follow_up: 无。后续若要继续扩展 dashboard 或交互层级，另开 follow-up。
