---
task_id: 20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover
title: Subject Stage A+B+C Foundation And Windows Cutover
task_type: plan
lifecycle_status: in_progress
phase: planning
created_at: 2026-04-06 15:25:00 +08:00
updated_at: 2026-04-06 15:52:00 +08:00
current_dir: docs/dev/in-progress/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover
parent_task_id: 20260406-03-subject-test-framework-rollout-roadmap
source_task_id: 20260406-03-subject-test-framework-rollout-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档
- brainstorm: `docs/dev/in-progress/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/plan-v1-01.md`
- latest_progress: `docs/dev/in-progress/20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/notes/progress-v1-01.md`

## 当前判断
- current_focus: 只为 `Stage A + Stage B + Stage C` 生成首份实现计划，覆盖 `subjects/HelloWorldObject` 真源落地、planner/executor/worker 最小脊柱，以及 Windows 首批 matrix 的内部 cutover。
- why_now: 父并行 roadmap 已完成关键边界确认；如果继续停留在路线讨论层，后续实现会缺少可执行的 chunk 顺序和明确的验证锚点。
- done_definition: `plan-v1-01.md` 足够具体，能指导后续执行任务分块实现，并且明确排除 `perf`、public CLI / registry / TUI、subject/session 聚合 reporting 等后置项。

## 最近摘要
- 2026-04-06 15:25:00 +08:00: 用户接受并行 roadmap 中的默认推荐，确认首份计划排除 `perf`，并接受 `subjects/HelloWorldObject/` 第一阶段物理落地、public surface 后置、Windows `verify-roadmap-0` 保持 mandatory compatibility gate。
- 2026-04-06 15:25:00 +08:00: 基于上面的确认，从并行 roadmap 派生本 child 计划任务；本任务仍保持 `active: false`，不改动当前主线的 `ACTIVE.md`。
- 2026-04-06 15:47:00 +08:00: 用户补充后续测试工程清理时，除 `HelloWorldObject` 外还要保留 `echo`；本任务文档按仓库现有命名统一写为 `GenericEcho`，并把它标记为后续清理白名单成员。
- 2026-04-06 15:52:00 +08:00: 用户进一步明确，后续测试工程清理要“全部保留、全部转长期基线”；因此本任务只把这条约束记入后续计划位，不把清理理解成删减样例。

## 下一步
- next_action: 审查 `plan-v1-01.md` 是否满足后续执行要求；如无调整，再决定是否进入执行阶段。
- owner: codex
- trigger: 用户确认计划内容后，才进入执行阶段。

## 风险/阻塞
### risks
- 计划如果把 Stage D 的 reporting / events 全链路提前纳入，会再次扩张到超大任务。
- 当前代码仍强依赖旧 `suite/registry/session`，如果计划对 bridge 切口描述不清，后续执行时很容易出现双模型混用。

### blockers
- 无。

## 关联任务
### parent
- `20260406-03-subject-test-framework-rollout-roadmap`

### source
- `20260406-03-subject-test-framework-rollout-roadmap`：roadmap-child

### children
- 无。
