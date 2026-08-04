---
task_id: 20260406-03-subject-test-framework-rollout-roadmap
title: Subject 测试框架重构落地路线与风险收口
task_type: roadmap
lifecycle_status: completed
phase: archived
created_at: 2026-04-06 15:10:00 +08:00
updated_at: 2026-04-07 01:42:14 +08:00
current_dir: docs/dev/completed/20260406-03-subject-test-framework-rollout-roadmap
parent_task_id:
source_task_id: 20260405-01-il2cpp-reboot-after-abandoned-roadmap
source_relation: parallel-design-follow-up
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260406-03-subject-test-framework-rollout-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260406-03-subject-test-framework-rollout-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260406-03-subject-test-framework-rollout-roadmap/roadmap-v1-01.md`
- latest_progress: `docs/dev/completed/20260406-03-subject-test-framework-rollout-roadmap/notes/progress-v1-06.md`

## 当前判断

- current_focus: 本 roadmap 已完成归档；`Stage A+B+C`、`Stage D+E`、`Stage F` 三份 child plan 均已完成，subject 测试框架 rollout 主线不再保留活动入口。
- why_now: `Stage F` 已在本轮完成 smoke canonical source、多 subject onboarding、subject-aware perf baseline/reporting 与全量 `run` 验证，因此父 roadmap 的最后一项跟踪职责已经结束。
- done_definition: 已满足；三段式实施顺序全部完成，长期边界已沉淀到架构文档与 wiki，本 roadmap 可以整体归档。

## 最近摘要

- 2026-04-06 15:10:00 +08:00: 用户明确要求“并行执行，不影响当前 active 任务”，因此本任务以 `active: false` 的并行设计任务形式创建，不修改 `docs/dev/ACTIVE.md`。
- 2026-04-06 15:10:00 +08:00: 本任务只读 `docs/discuss/` 既有设计结论，不再向 `docs/discuss/` 追加同步文档；正式输出集中在本任务目录。
- 2026-04-06 15:25:00 +08:00: 基于用户已确认的边界，派生 child plan `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover`，只覆盖 `Stage A + Stage B + Stage C`。
- 2026-04-06 15:40:00 +08:00: 父 roadmap 改为跟踪“首份 child plan 已创建”的状态，并把后续 `Stage D + Stage E` 合并计划与 `Stage F` 单独计划标记为 roadmap 中的置后计划位。
- 2026-04-06 15:47:00 +08:00: 用户补充后续清理测试工程时，除 `HelloWorldObject` 外还要保留 `echo`；文档统一按仓库现有命名写为 `GenericEcho`，并作为后续清理阶段的保留白名单。
- 2026-04-06 15:52:00 +08:00: 用户进一步明确，后续测试工程清理不再以删减为目标，而是“全部保留、全部转长期基线”；因此清理语义改为结构收敛、归档分层、命名统一与框架接线，不再讨论删除样例。
- 2026-04-06 18:50:00 +08:00: `20260406-04-subject-stage-a-b-c-foundation-and-windows-cutover` 已完成归档，`subjects/HelloWorldObject` 真源、contract cutover、最小 execution spine 与 Windows cutover 已正式收口。
- 2026-04-06 18:50:00 +08:00: 已新增 `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/subject-test-framework-v1/foundation-and-windows-cutover-v1.md`，把 Stage A+B+C 的长期边界从执行文档沉淀到架构文档。
- 2026-04-06 22:15:00 +08:00: 用户明确要求继续 `subject` 主线；据此派生 `20260406-08-subject-stage-d-e-reporting-and-public-cutover`，把 `Stage D + Stage E` 从 queued-later plan 升级为真实 child plan。
- 2026-04-06 22:15:00 +08:00: 新 child plan 已固定 reporting 采用 `B2`、events 采用方案 `C`，并把 public cutover 继续限制在 `HelloWorldObject` 与既有四个 Windows matrix。
- 2026-04-06 23:49:54 +08:00: `20260406-08-subject-stage-d-e-reporting-and-public-cutover` 在重载 Windows `PATH`、恢复 `cmake` 后，通过组合 unittest 与 `python build/scripts/verify-roadmap-0.py windows`，完成 Windows compatibility gate。
- 2026-04-07 00:00:58 +08:00: `20260406-08-subject-stage-d-e-reporting-and-public-cutover` 已归档到 `docs/dev/completed/`；当前 active 入口回到父 roadmap，后续只保留 `Stage F` follow-up 计划位。
- 2026-04-07 00:17:57 +08:00: 用户再次要求继续任务；据此派生 `20260407-01-subject-stage-f-perf-and-multi-subject-expansion`，把 `Stage F` 从 queued-later-plan 升级为真实 child plan。
- 2026-04-07 00:17:57 +08:00: 新 child plan 已固定为 `windows` 首批 `GenericEcho` benchmark subject 与 subject-aware `perf.dev` / `perf.release`，父 roadmap 不再作为 active 执行入口。
- 2026-04-07 01:42:14 +08:00: `20260407-01-subject-stage-f-perf-and-multi-subject-expansion` 已完成 canonical smoke subjects、subject-aware perf baseline/reporting、剩余 run-suite 修复与全量验证；父 roadmap 至此整体完成并归档。

## 下一步

- next_action: 无。若未来继续扩展 subject 测试框架，应基于已归档的架构边界另开新的 follow-up。
- owner: codex
- trigger: 仅当出现新的 subject framework follow-up 需求时，才需要重新打开后续任务。

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
- `20260406-08-subject-stage-d-e-reporting-and-public-cutover`：roadmap-child-plan
- `20260407-01-subject-stage-f-perf-and-multi-subject-expansion`：roadmap-child-plan
