---
task_id: 20260414-36-planning-tests-genericization-followup
title: Planning Tests Genericization Follow-up
task_type: design
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 19:28:33 +08:00
updated_at: 2026-04-14 19:54:36 +08:00
current_dir: docs/dev/completed/20260414-36-planning-tests-genericization-followup
parent_task_id:
source_task_id: 20260414-20-subject-generated-test-workspace-roadmap
source_relation: follow-up-after-roadmap-closure
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260414-36-planning-tests-genericization-followup/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260414-36-planning-tests-genericization-followup/design-v1-01.md`
- plan: `docs/dev/completed/20260414-36-planning-tests-genericization-followup/plan-v1-01.md`

## 当前结论

- current_focus: 本 follow-up 已完成，planning 层测试已经按 `generic contract`、`fixture repo smoke`、`retained real-subject integration` 三层拆开。
- why_now: 父 roadmap 已完成 subject workspace / unified entry 主链，这一轮用于清理残留的 planner 测试耦合，避免 generic suite 继续绑定真实 subject。
- success_definition:
  - `tests/unit/planning/test_subject_planner.py` 只保留 synthetic manifest contract tests。
  - `tests/unit/planning/test_fixture_subject_planner.py` 独立承载 fixture repo smoke。
  - `tests/unit/planning/test_solution_core_pack_planner.py` 集中承载 retained real-subject planning 回归。
  - 全量 `python -m pytest` 通过。

## 最近摘要

- 2026-04-14 19:28:33 +08:00: 创建 follow-up 任务，冻结三层拆分设计与执行边界。
- 2026-04-14 19:34:30 +08:00: 完成实现计划，开始按 generic / fixture / retained 三层迁移 planning tests。
- 2026-04-14 19:54:36 +08:00: 完成测试迁移、fixture test 清理和 3 个 stale compatibility tests 的 legacy core solution cutover 对齐；`python -m pytest tests/unit/planning` 与全量 `python -m pytest` 均通过。

## 下一步

- next_action: 无。本 follow-up 已完成并归档；后续如需继续扩大 planner / registry / dashboard 的 generic 化边界，请另起新任务。
- owner: codex
- trigger: 代码、验证和 `docs/dev` 状态均已闭环。

## 风险 / 阻塞

### risks

- 当前无阻塞。后续若要继续泛化 retained real-subject 相关测试，应继续保持“generic contract / fixture smoke / retained integration”三层边界，不要再回退到混合文件。

### blockers

- 无。

## 验证

- `python -m pytest tests/unit/planning/test_subject_planner.py`
- `python -m pytest tests/unit/planning/test_fixture_subject_planner.py`
- `python -m pytest tests/unit/planning/test_solution_core_pack_planner.py`
- `python -m pytest tests/unit/planning`
- `python -m pytest`

## wiki

- 本任务只在 `docs/dev` 记录执行状态，未单独更新 `wiki/`。
