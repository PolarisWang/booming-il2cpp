---
task_id: 20260406-03-subject-test-framework-rollout-roadmap
title: Subject 测试框架重构落地路线与风险收口
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-06 15:10:00 +08:00
updated_at: 2026-04-06 15:52:00 +08:00
current_dir: docs/dev/in-progress/20260406-03-subject-test-framework-rollout-roadmap
parent_task_id:
source_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_relation: parallel-design-follow-up
active: false
---

## 关键文档
- brainstorm: `docs/dev/in-progress/20260406-03-subject-test-framework-rollout-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260406-03-subject-test-framework-rollout-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260406-03-subject-test-framework-rollout-roadmap/roadmap-v1-01.md`
- latest_progress: `docs/dev/in-progress/20260406-03-subject-test-framework-rollout-roadmap/notes/progress-v1-01.md`

## 当前判断
- current_focus: 维护并行父 roadmap，确保 `docs/discuss/` 收敛出的 `subject / goal / environment-matrix / artifact-chain / planner / executor / reporting / events` 设计，已经落成可执行分阶段路线，并且首个 child plan 与后续置后计划位保持一致。
- why_now: `Stage A + Stage B + Stage C` 的首份 child plan 已经创建；现在需要把父 roadmap、子计划和 `docs/dev/INDEX.md` 对齐，避免并行任务再次退回到“边讨论边实现”的状态。
- done_definition: 父 roadmap 明确跟踪 `20260406-04` 这一份 child plan，且已经为后续 `Stage D + Stage E` 合并计划和 `Stage F` 单独计划保留位置；同时不影响当前 active 主线。

## 最近摘要
- 2026-04-06 15:10:00 +08:00: 用户明确要求“并行执行，不影响当前 active 任务”，因此本任务以 `active: false` 的并行设计任务形式创建，不修改 `docs/dev/ACTIVE.md`。
- 2026-04-06 15:10:00 +08:00: 本任务读取 `docs/discuss/` 既有设计结论，不再向 `docs/discuss/` 追加同步文档；正式输出集中在本任务目录。
- 2026-04-06 15:25:00 +08:00: 基于用户已确认的边界，派生 child plan `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover`，只覆盖 `Stage A + Stage B + Stage C`。
- 2026-04-06 15:40:00 +08:00: 父 roadmap 改为跟踪“首份 child plan 已创建”的状态，并把后续 `Stage D + Stage E` 合并计划与 `Stage F` 单独计划标记为 roadmap 中的置后计划位。
- 2026-04-06 15:47:00 +08:00: 用户补充后续清理测试工程时，除 `HelloWorldObject` 外还要保留 `echo`；文档统一按仓库现有命名写为 `GenericEcho`，并作为后续清理阶段的保留白名单。
- 2026-04-06 15:52:00 +08:00: 用户进一步明确，后续测试工程清理不再以删减为目标，而是“全部保留、全部转长期基线”；因此清理语义改为结构收敛、归档分层、命名统一与框架接线，不再讨论删除样例。

## 下一步
- next_action: 等待用户审阅 `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover/plan-v1-01.md`；如无边界调整，再按父 roadmap 置后派生 `Stage D + Stage E` 合并计划与 `Stage F` 单独计划。
- owner: codex
- trigger: 用户确认首份 child plan 可作为后续执行依据后，本任务继续作为父 roadmap 存在；后续只在需要派生新的 follow-up plan 时继续推进。

## 风险/阻塞
### risks
- 当前代码与 `docs/discuss/` 设计之间存在明显代差：仓库里还没有 `subjects/`，核心代码仍基于 `suite/registry/session`，如果直接进入实现，边改边定边界的风险很高。
- 并行任务如果长期不回收回主 roadmap，会和当前 active reboot 主线形成双重决策源。
- 如果不持续把后续工作压成 `Stage D + E` 与 `Stage F` 两个置后计划位，`perf`、多平台、public CLI/TUI 等维度仍可能重新在首份实施计划里扩张。
- 如果后续测试工程清理仍按“删减工程”理解推进，会和“全部保留并转长期基线”的新约束直接冲突，导致后续 plan 语义失真。

### blockers
- 无。当前只需等待用户审阅首份 child plan，再决定何时派生后续置后计划。

## 关联任务
### parent
- 无。

### source
- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`：parallel-design-follow-up

### children
- `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover`：roadmap-child-plan
