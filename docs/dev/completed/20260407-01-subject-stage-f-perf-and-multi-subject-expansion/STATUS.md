---
task_id: 20260407-01-subject-stage-f-perf-and-multi-subject-expansion
title: Subject Stage F Perf And Multi-Subject Expansion
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-07 00:17:57 +08:00
updated_at: 2026-04-07 01:42:14 +08:00
current_dir: docs/dev/completed/20260407-01-subject-stage-f-perf-and-multi-subject-expansion
parent_task_id: 20260406-03-subject-test-framework-rollout-roadmap
source_task_id: 20260406-03-subject-test-framework-rollout-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260407-01-subject-stage-f-perf-and-multi-subject-expansion/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260407-01-subject-stage-f-perf-and-multi-subject-expansion/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260407-01-subject-stage-f-perf-and-multi-subject-expansion/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260407-01-subject-stage-f-perf-and-multi-subject-expansion/notes/progress-v1-03.md`

## Scope Update

- user_correction: `tests/smoke/input/*` projects all need canonical homes under `subjects/`; the task is not limited to `GenericEcho`.
- completed_now: Added canonical subject/source trees for `HelloWorld`, `GenericEcho`, `ReflectionLite`, `PInvokeLite`, and `HostEmbeddingLite`.
- compatibility_rule: Legacy `tests/smoke/input/*` directories remain in-tree for compatibility and baseline purposes, but smoke suites and run-manifest paths now resolve to `subjects/*/source`.
- verification_now: Passed targeted Python test coverage, `dotnet build` for all five migrated subjects, and direct DLL execution with expected smoke output.
- completed_follow_up: Subject-aware perf baseline/reporting work has now been completed on top of the smoke-input migration correction.

## 当前判断

- current_focus: 本任务已完成归档；`Stage F` 已把 smoke canonical subject 迁移、第二个正式 subject `GenericEcho`、subject-aware `perf.dev` / `perf.release` 以及长期基线规则一并收口。
- why_now: 用户要求持续推进直到全部任务完成；在修复剩余 `run` 套件失败并完成全量验证后，本 child plan 已满足完成条件。
- done_definition: 已满足；`GenericEcho` 已成为第二个正式 subject，`perf.dev` / `perf.release` 已在 subject 模型下落地，subject-aware perf baseline/report 与多 smoke subject canonical source 规则已有代码、测试、架构文档与 wiki 闭环。

## 最近摘要

- 2026-04-07 00:17:57 +08:00: 用户要求继续 `subject` 主线；据此从父 roadmap `20260406-03-subject-test-framework-rollout-roadmap` 派生出当前 `Stage F` child plan。
- 2026-04-07 00:17:57 +08:00: 已完成 `Stage F` brainstorm / design / plan 定稿，范围固定为 `windows` 首批 `GenericEcho` benchmark subject 与 `perf.dev` / `perf.release`。
- 2026-04-07 00:17:57 +08:00: 当前进入执行态，首个 chunk 收敛为第二个正式 subject 的 manifest / planner / registry 骨架，不先把 perf runtime 带进半成品状态。
- 2026-04-07 11:25:00 +08:00: 已把 `tests/smoke/input/*` 下的五个项目迁移为 `subjects/*/source` canonical source，并完成 smoke manifest / run-manifest 切换与实机 `dotnet build` + DLL smoke 验证。
- 2026-04-07 01:42:14 +08:00: 已补齐 subject-aware perf baseline / reporting / command surface，修复剩余 `run` 套件失败，并通过完整 `tests/unit/run`、`tests/integration/run`、`tests/tooling/run` 验证后归档完成。

## 下一步

- next_action: 无。若后续继续扩展更多 subject、更多 perf host 或新的 public perf surface，应从新的 follow-up 任务重新派生。
- owner: codex
- trigger: 仅当出现新的 subject/perf/platform 范围扩展时，才需要继续派生后续任务。

## 风险 / 阻塞

### risks

- 如果把第二个 subject 继续强绑到 `HelloWorldObject` 的 native proof worker，会把 `Stage F` 错做成 `HelloWorldObject` 特例扩写，而不是稳定的第二个 subject onboarding pattern。
- 如果 perf baseline 继续伪装成旧 suite perf，而不是 subject-aware 路径，后续 subject perf 与旧 `tests/perf` 契约会长期冲突。
- 如果把“长期基线整理”误做成删除测试工程，会直接违反用户已经确认的“全部保留、全部转长期基线”约束。

### blockers

- 无。

## 关联任务

### parent

- `20260406-03-subject-test-framework-rollout-roadmap`

### source

- `20260406-03-subject-test-framework-rollout-roadmap`（roadmap-child）

### children

- 无
