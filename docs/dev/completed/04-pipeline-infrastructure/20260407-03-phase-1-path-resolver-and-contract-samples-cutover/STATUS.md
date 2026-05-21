---
task_id: 20260407-03-phase-1-path-resolver-and-contract-samples-cutover
title: Phase 1 Path Resolver And Contract Samples Cutover
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-07 13:11:07 +08:00
updated_at: 2026-04-07 16:47:13 +08:00
current_dir: docs/dev/completed/20260407-03-phase-1-path-resolver-and-contract-samples-cutover
parent_task_id: 20260407-02-subject-layout-and-variant-traceability-roadmap
source_task_id: 20260407-02-subject-layout-and-variant-traceability-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260407-03-phase-1-path-resolver-and-contract-samples-cutover/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260407-03-phase-1-path-resolver-and-contract-samples-cutover/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260407-03-phase-1-path-resolver-and-contract-samples-cutover/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260407-03-phase-1-path-resolver-and-contract-samples-cutover/notes/progress-v1-01.md`

## 当前判断

- current_focus: 承接父 roadmap 的 `Phase 1`，先冻结路径解析真源，再把 contract canonical sample 命名从 `examples` 收口到 `samples`，同时保留兼容读取。
- why_now: 若继续让 `contracts.py`、测试和文档直接硬编码旧物理路径，后续 manifest、artifact spine 和 variant 引入都会在错误路径上叠加复杂度。
- done_definition: 存在统一 path resolver；`contracts/artifacts/v0/samples` 与 `contracts/native/v0/samples` 成为新的 canonical 样例根；核心 tooling/tests 已通过 resolver 或新路径消费；旧 `examples` 仅保留兼容语义。

## 最近摘要

- 2026-04-07 13:11:07 +08:00: 由父 roadmap `20260407-02-subject-layout-and-variant-traceability-roadmap` 派生 `Phase 1` child plan，并立即进入执行。
- 2026-04-07 13:11:07 +08:00: 本任务范围刻意收窄为路径解析与 contract samples 命名收口，不提前进入 subject manifest 扩展、run-scoped artifact spine 或 variant 实现。
- 2026-04-07 13:11:07 +08:00: 当前无 wiki 更新；待 Phase 1 稳定落地且形成长期知识后，再在后续阶段统一沉淀到 `wiki/`。

## 下一步

- next_action: 先按 TDD 扩展路径测试，锁定 resolver 与 `samples` canonical 路径，再实现对应代码和目录迁移。
- owner: codex
- trigger: 一旦 Phase 1 验证通过，就回写父 roadmap，并继续派生 `Phase 2` child plan。

## 风险 / 阻塞

### risks

- 如果本阶段直接物理删除旧 `examples` 路径，会让尚未迁移的历史文档和 compatibility mirror 失效。
- 如果 resolver 只服务于 `contracts.py`，而不纳入 `subjects.py` 一类现有 helper，后续还会继续长出第二套路径拼装逻辑。

### blockers

- 无。

## 关联任务

### parent

- `20260407-02-subject-layout-and-variant-traceability-roadmap`

### source

- `20260407-02-subject-layout-and-variant-traceability-roadmap`（roadmap-child）

### children

- 无。
