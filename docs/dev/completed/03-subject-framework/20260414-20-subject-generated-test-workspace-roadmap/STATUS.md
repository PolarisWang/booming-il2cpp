---
task_id: 20260414-20-subject-generated-test-workspace-roadmap
title: Subject Generated Test Workspace And Unified Test Entry
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 16:17:43 +08:00
updated_at: 2026-04-14 18:37:13 +08:00
current_dir: docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap
parent_task_id:
source_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_relation: follow-up-after-capability-closure
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/roadmap-v1-01.md`

## 当前结论

- current_focus: 本 roadmap 已完成，subject workspace、generated managed/native test host、unified test entry、benchmark catalog/dashboard closure 与 legacy deletion 已统一收口到 workspace manifest v2 主链。
- why_now: 这是 capability closure 之后的结构性清理路线，目标是把 subject 新增成本降到“只新增 solution source + `ChaosUnitTest` / `ChaosBenchmark` attribute”。
- success_definition:
  - 新增 subject 只需要 source solution 与 `ChaosUnitTest` / `ChaosBenchmark` 声明。
  - `run generate/build/test` 共享同一份 workspace manifest v2。
  - benchmark catalog / dashboard 与 unified test entry 消费同一套 declared catalog / workspace host 解析结果。
  - compatibility alias、legacy run shim、旧 registry-object dispatch 与历史 proof/benchmark 手工注册入口已被清理。

## 最近摘要

- 2026-04-14 16:17:43 +08:00: 创建 roadmap 任务目录，完成 brainstorm、design 与 roadmap 初稿。
- 2026-04-14 17:32:13 +08:00: 完成 Phase 0、Phase 1、Phase 2，并创建 `20260414-33-phase-3-unified-test-entry-and-registry-cutover` child task。
- 2026-04-14 18:37:13 +08:00: 完成 Phase 3 unified test entry cutover，并内联收口 Phase 4 benchmark closure 与 Phase 5 legacy deletion；修复 loader 缺失 `conv.i` / `conv.u` 导致的 native-aot workload bundle 失败；全量 `python -m pytest` 通过。

## 下一步

- next_action: 无。本 roadmap 已完成并归档；如需继续 generic 化 `tests/unit/planning/test_subject_planner.py` 与 retained-subject 规划逻辑，请另起 follow-up。
- owner: codex
- trigger: 代码、验证与 `docs/dev` 状态均已闭环。

## 风险 / 阻塞

### risks

- 当前无阻塞；残余的 tests/planner generic 化问题不影响本路线已定义的收口目标，但仍值得后续单独清理。

### blockers

- 无。

## 验证

- `python -m pytest`

## wiki

- 本任务只在 `docs/dev` 记录执行状态，未单独更新 `wiki/`。
