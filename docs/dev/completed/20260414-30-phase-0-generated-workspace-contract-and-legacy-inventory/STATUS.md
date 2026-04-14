---
task_id: 20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory
title: Phase 0 Generated Workspace Contract And Legacy Inventory
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 16:34:34 +08:00
updated_at: 2026-04-14 16:49:05 +08:00
current_dir: docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory
parent_task_id: 20260414-20-subject-generated-test-workspace-roadmap
source_task_id: 20260414-20-subject-generated-test-workspace-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/roadmap-v1-01.md`
- plan: `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/plan-v1-01.md`
- contract_freeze: `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/contract-freeze-v1-01.md`
- legacy_cleanup_inventory: `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/legacy-cleanup-inventory-v1-01.md`
- migration_order: `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/migration-order-v1-01.md`

## 当前结论

- current_focus: Phase 0 已完成，generated workspace v2、generated host、legacy cleanup inventory 与 migration order 的硬边界已经冻结。
- why_now: Phase 1-5 后续实现不再需要回头重谈 contract，可以直接围绕此冻结基线推进。
- success_definition:
  - `workspace.manifest.json` v2 的根字段与四类工程职责已固定
  - declared catalog v2 与 `entryIndex` 调度规则已固定
  - legacy inventory 已覆盖 workspace、runner、subject host、tests shim 四条线

## 最近摘要

- 2026-04-14 16:34:34 +08:00: 创建 Phase 0 child task，准备冻结 contract 并审计 legacy surface。
- 2026-04-14 16:49:05 +08:00: 完成 contract freeze、legacy cleanup inventory 与 migration order 三份文档，并通过自检。

## 下一步

- next_action: 返回父 roadmap，创建并执行 `20260414-31-phase-1-declared-catalog-to-managed-test-host`。
- owner: codex
- trigger: Phase 0 文档已完成并归档。

## 风险 / 阻塞

### risks

- 如果 Phase 1 实现时绕开 `entryIndex`，Phase 0 的“禁止继续扩张字符串调度面”会失效。
- 如果 Phase 2 又回写旧 manifest 字段，后续 cleanup 会再次分叉。

### blockers

- 当前无外部 blocker。

## wiki

- 本子任务只在 `docs/dev` 冻结 contract，不单独更新 `wiki/`。
