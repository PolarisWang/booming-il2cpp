---
task_id: 20260407-05-phase-3-run-scoped-artifact-spine
title: Phase 3 Run-Scoped Artifact Spine
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-07 16:47:13 +08:00
updated_at: 2026-04-07 16:47:13 +08:00
current_dir: docs/dev/completed/20260407-05-phase-3-run-scoped-artifact-spine
parent_task_id: 20260407-02-subject-layout-and-variant-traceability-roadmap
source_task_id: 20260407-02-subject-layout-and-variant-traceability-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260407-05-phase-3-run-scoped-artifact-spine/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260407-05-phase-3-run-scoped-artifact-spine/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260407-05-phase-3-run-scoped-artifact-spine/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260407-05-phase-3-run-scoped-artifact-spine/notes/progress-v1-01.md`

## 当前判断

- current_focus: 已完成 Phase 3 收口，把 subject 的 run ledger、matrix 报告、validation 侧产物与 run 级日志统一回收到 `artifacts/subjects/<id>/runs/<run-id>`。
- why_now: 如果不先冻结 run-scoped artifact spine，后续 `CHECK|PROFILE|SHIP` 变体无法做到稳定追踪，也无法把 report/perf 与 codegen/native 一起对齐。
- done_definition: 已满足；subject-local run 根、兼容指针、reused stage 路径重写与 focused verification 均已闭环。

## 最近摘要

- 2026-04-07 16:47:13 +08:00: 已把 matrix report、perf validation、subject summary、run report 全部切到 run-scoped 目录。
- 2026-04-07 16:47:13 +08:00: 已修复 reused stage 结果回放，保留 `primaryEvidencePaths`、runtime diagnostics 与 perf 报告路径。
- 2026-04-07 16:47:13 +08:00: 已验证 `run test summary --run <run-id>` 能从 subject-local run report 读取新布局。

## 下一步

- next_action: 无。后续由 `20260407-06-phase-4-variant-aware-codegen-and-native` 继续接入 `CHECK|PROFILE|SHIP` 变体链路。
- owner: codex
- trigger: 仅当 `variant` 已贯穿 planner/codegen/native/report 并稳定后，才继续派生 `Phase 5`。

## 风险 / 阻塞

### risks

- 如果复用旧 run 时继续只 copy bucket 而不修正 manifest 元数据，traceability 会再次断裂。
- 如果让全局 `artifacts/logs/tests` 继续承担 canonical ledger，subject-local 产物约束会再次失效。

### blockers

- 无。

## 关联任务

### parent

- `20260407-02-subject-layout-and-variant-traceability-roadmap`

### source

- `20260407-02-subject-layout-and-variant-traceability-roadmap`（roadmap-child）

### children

- 无。
