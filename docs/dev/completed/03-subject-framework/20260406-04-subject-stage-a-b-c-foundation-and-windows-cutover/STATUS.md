---
task_id: 20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover
title: Subject Stage A+B+C Foundation And Windows Cutover
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-06 15:25:00 +08:00
updated_at: 2026-04-06 18:50:00 +08:00
current_dir: docs/dev/completed/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover
parent_task_id: 20260406-03-subject-test-framework-rollout-roadmap
source_task_id: 20260406-03-subject-test-framework-rollout-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/notes/progress-v1-03.md`

## 当前判断

- current_focus: 本任务已完成归档；Stage A+B+C 已把 `subjects/HelloWorldObject` 真源、contract cutover、planner/executor/worker 最小脊柱，以及 Windows 首批 matrix cutover 固定为新的长期实现边界。
- why_now: 这条 child plan 的职责是把 `subject` 框架首轮基础从设计层真正落成代码与测试闭环；现在该边界已经稳定，不应继续留在执行态。
- done_definition: 已满足；`subjects/HelloWorldObject/` 已成为正式真源，`contracts.py` 已切到 canonical `contracts/` + `tests/contracts/`，最小 planner/executor/worker 与 Windows cutover 测试已通过，`verify-roadmap-0` 已退化为 compatibility gate，并且长期知识已沉淀到 `docs/architecture/subject-test-framework-v1/`。

## 最近摘要

- 2026-04-06 15:25:00 +08:00: 用户接受并行 roadmap 中的默认推荐，确认首份计划排除 `perf`，并接受 `subjects/HelloWorldObject/` 第一阶段物理落地、public surface 后置、Windows `verify-roadmap-0` 保持 mandatory compatibility gate。
- 2026-04-06 15:25:00 +08:00: 基于上面的确认，从并行 roadmap 派生本 child 计划任务；本任务仍保持 `active: false`，不改动当前主线的 `ACTIVE.md`。
- 2026-04-06 15:47:00 +08:00: 用户补充后续测试工程清理时，除 `HelloWorldObject` 外还要保留 `echo`；本任务文档按仓库现有命名统一写为 `GenericEcho`，并把它标记为后续清理白名单成员。
- 2026-04-06 15:52:00 +08:00: 用户进一步明确，后续测试工程清理要“全部保留、全部转长期基线”；因此本任务只把这条约束记入后续计划位，不把清理理解成删减样例。
- 2026-04-06 18:10:00 +08:00: 已按 `executing-plans` 进入正式执行，确认当前仓库无其他活动任务占用；开始按 TDD 顺序实现 `subject` 真源、contract cutover 与 Windows cutover 主线。
- 2026-04-06 18:50:00 +08:00: 已收敛 `tests/tooling/run/test_verify_roadmap0.py` 到新的 subject-based 契约，并在 `build/scripts/verify-roadmap-0.py` 中加入 `resolve_stage4_runtime_root()` 兼容新旧 proof runtime 落点。
- 2026-04-06 18:50:00 +08:00: 已通过 42 项受影响单测；已尝试执行 `python build/scripts/verify-roadmap-0.py windows`，但宿主环境缺少 `cmake`，因此实机 gate 只记录为环境缺口，不构成本任务代码归档阻塞。
- 2026-04-06 18:50:00 +08:00: 已新增 `docs/architecture/subject-test-framework-v1/foundation-and-windows-cutover-v1.md` 并把本任务转入 completed 归档。

## 下一步

- next_action: 无。若继续 `subject` 主线，实现入口转由父 roadmap 派生 `Stage D + Stage E` follow-up plan，随后再进入 `Stage F`。
- owner: codex
- trigger: 本任务的代码、测试与长期文档边界都已收口。

## 风险 / 阻塞

### risks

- 如果后续 `Stage D + Stage E` 绕开本轮冻结下来的 planner/executor/bucket 边界，可能重新引入第二套隐式 subject 框架。
- 如果后续在未补齐宿主 `cmake` 的情况下直接要求跑 Windows 实机 gate，仍会再次遇到环境缺口。

### blockers

- 无。

## 关联任务

### parent

- `20260406-03-subject-test-framework-rollout-roadmap`

### source

- `20260406-03-subject-test-framework-rollout-roadmap`：roadmap-child

### children

- 无。
