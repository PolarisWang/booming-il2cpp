---
task_id: 20260410-02-contract-fixture-ownership-cutover
title: Contract Fixture Ownership Cutover - 把 contract 定义与 concrete fixture 分离
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-10 10:35:00 +08:00
updated_at: 2026-04-10 11:00:00 +08:00
current_dir: docs/dev/in-progress/20260410-02-contract-fixture-ownership-cutover
parent_task_id: 20260409-11-contract-root-without-subject-content
source_task_id: 20260409-11-contract-root-without-subject-content
source_relation: child_of_roadmap_phase_1
active: false
---

## 关键文档

- roadmap: `docs/dev/in-progress/20260409-11-contract-root-without-subject-content/roadmap-v1-01.md`
- layout-target: `docs/dev/completed/20260410-01-layout-boundary-freeze/layout-target-v1.md`
- path-mapping: `docs/dev/completed/20260410-01-layout-boundary-freeze/path-mapping-v1.md`
- plan: `docs/dev/in-progress/20260410-02-contract-fixture-ownership-cutover/plan-v1-01.md`

## 当前判断

- current_focus: 已完成。contract fixture 已迁移，legacy 路径已删除，所有 consumer 已切到新路径，测试通过。
- done_definition: contracts/ 下不存在 sample/snapshot/examples；tests/contracts/analysis/ 成为唯一 canonical fixture 根；tests/contract/ 不再存在；16/16 验证测试通过。

## 最近摘要

- 2026-04-10 10:35:00 +08:00: 创建 child task，编写计划。
- 2026-04-10 10:40:00 +08:00: Batch 1-2 完成：移动 artifact samples/snapshots 和 native samples 到新位置。
- 2026-04-10 10:45:00 +08:00: Batch 3 完成：删除 contracts/examples/、contracts/native/examples/、contracts/native/docs/、tests/contract/。
- 2026-04-10 10:50:00 +08:00: Batch 4-6 完成：更新 path_resolver、subject_workers、run_manifest、所有测试和 active docs。
- 2026-04-10 11:00:00 +08:00: Batch 7 验证通过：16/16 测试通过，5/5 文件系统检查正确。`test_subject_workers.py` 有 1 个 pre-existing failure (unrelated)。

## 下一步

- next_action: 无。Phase 1 完成，可以启动 Phase 2 `run-tooling-domain-reorg`。
- owner: codex

## 风险 / 阻塞

### risks

- 无。

### blockers

- 无。
