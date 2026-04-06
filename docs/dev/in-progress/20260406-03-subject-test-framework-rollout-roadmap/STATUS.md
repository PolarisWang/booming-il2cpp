---
task_id: 20260406-03-subject-test-framework-rollout-roadmap
title: Subject 测试框架重构落地路线与风险收口
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-06 15:10:00 +08:00
updated_at: 2026-04-06 18:50:00 +08:00
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
- latest_progress: `docs/dev/in-progress/20260406-03-subject-test-framework-rollout-roadmap/notes/progress-v1-02.md`

## 当前判断

- current_focus: 维护并行父 roadmap，当前 `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover` 已完成归档；父 roadmap 只继续保留 `Stage D + Stage E` 与 `Stage F` 的 follow-up 派生入口。
- why_now: 首份 child plan 已经真正落地完成，现在需要把父 roadmap 的状态从“等待首份 plan 执行”切换成“等待后续 plan 派生”，防止主线已经闭环却仍显示为待执行。
- done_definition: 已满足当前阶段目标；父 roadmap 已完成对 `20260406-04` 的跟踪闭环，并保留 `Stage D + Stage E` 合并计划与 `Stage F` 单独计划的位置，同时不影响当前 active 主线。

## 最近摘要

- 2026-04-06 15:10:00 +08:00: 用户明确要求“并行执行，不影响当前 active 任务”，因此本任务以 `active: false` 的并行设计任务形式创建，不修改 `docs/dev/ACTIVE.md`。
- 2026-04-06 15:10:00 +08:00: 本任务只读 `docs/discuss/` 既有设计结论，不再向 `docs/discuss/` 追加同步文档；正式输出集中在本任务目录。
- 2026-04-06 15:25:00 +08:00: 基于用户已确认的边界，派生 child plan `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover`，只覆盖 `Stage A + Stage B + Stage C`。
- 2026-04-06 15:40:00 +08:00: 父 roadmap 改为跟踪“首份 child plan 已创建”的状态，并把后续 `Stage D + Stage E` 合并计划与 `Stage F` 单独计划标记为 roadmap 中的置后计划位。
- 2026-04-06 15:47:00 +08:00: 用户补充后续清理测试工程时，除 `HelloWorldObject` 外还要保留 `echo`；文档统一按仓库现有命名写为 `GenericEcho`，并作为后续清理阶段的保留白名单。
- 2026-04-06 15:52:00 +08:00: 用户进一步明确，后续测试工程清理不再以删减为目标，而是“全部保留、全部转长期基线”；因此清理语义改为结构收敛、归档分层、命名统一与框架接线，不再讨论删除样例。
- 2026-04-06 18:50:00 +08:00: `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover` 已完成归档，`subjects/HelloWorldObject` 真源、contract cutover、最小 execution spine 与 Windows cutover 已正式收口。
- 2026-04-06 18:50:00 +08:00: 已新增 `docs/architecture/subject-test-framework-v1/foundation-and-windows-cutover-v1.md`，把 Stage A+B+C 的长期边界从执行文档沉淀到架构文档。

## 下一步

- next_action: 等待用户要求继续 `subject` 主线；届时按既定顺序派生 `Stage D + Stage E` 合并 plan，再在其后保留 `Stage F`。
- owner: codex
- trigger: 只有在用户明确要求继续 `subject` 主线或派生 follow-up plan 时，父 roadmap 才继续向下推进。

## 风险 / 阻塞

### risks

- 如果后续不持续把工作压成 `Stage D + Stage E` 与 `Stage F` 两个 follow-up plan，`perf`、多平台、public CLI/TUI 等维度仍可能重新混入下一份实现计划。
- 如果后续测试工程清理再次按“删减工程”理解推进，会直接违反“全部保留并转长期基线”的新约束。

### blockers

- 无。

## 关联任务

### parent

- 无。

### source

- `20260405-01-il2cpp-reboot-after-abandoned-roadmap`：parallel-design-follow-up

### children

- `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover`：roadmap-child-plan
