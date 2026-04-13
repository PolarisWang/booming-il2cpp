---
task_id: 20260412-13-phase-3-engineering-core-compiled-catalog-batch-3
title: Phase 3 Engineering Core Compiled Catalog Batch 3
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 00:22:36 +08:00
updated_at: 2026-04-13 00:34:29 +08:00
current_dir: docs/dev/completed/20260412-13-phase-3-engineering-core-compiled-catalog-batch-3
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- plan: `docs/dev/completed/20260412-13-phase-3-engineering-core-compiled-catalog-batch-3/plan-v1-01.md`

## 当前判断

- current_focus: Phase 3 Batch 3 已完成 engineering-core compiled catalog foundation，`engineeringProfile` 已能稳定投影为工程级 catalog 条目。
- why_done: `engineeringValidations[]`、`engineeringWorkloads[]` 与已声明条目已经合并进统一 compiled subject catalog，后续 Phase 4~7 可以在稳定 schema 上继续切换。
- done_definition: compiled subject catalog 已稳定包含 `engineeringValidations[]`、`engineeringWorkloads[]`、`declaredUnitTests[]`、`declaredBenchmarks[]` 四类条目，并通过定向与全量验证。

## 最近摘要

- 2026-04-13 00:22:36 +08:00: 归档 Batch 2 并激活子任务 `20260412-13-phase-3-engineering-core-compiled-catalog-batch-3`，准备冻结 engineering-core compiled catalog 最小 schema。
- 2026-04-13 00:34:29 +08:00: 完成 engineering-core projection 与 compiled subject catalog merge，定向测试和 `python -m pytest -q` 均通过，结果为 `708 passed, 40 skipped`。

## 下一步

- next_action: 返回父 roadmap，进入 Phase 4 Batch 1，开始 `golden` 到 `Solution*` archetype subject cutover。
- owner: codex
- trigger: 需要把 archetype solution subjects 变成新的 canonical 工程级样例时立即恢复。

## 风险 / 阻塞

### risks

- engineering validation/workload 的命名虽然已经冻结，但 Phase 7 selector/report 切换时仍要避免再次泄漏旧 `run/phase` 语义。
- archetype subjects 还未切换到新的工程级 catalog 消费路径，Batch 4 之前仍存在 canonical 样例目录与新 schema 脱节的风险。

### blockers

- 当前无外部 blocker。
