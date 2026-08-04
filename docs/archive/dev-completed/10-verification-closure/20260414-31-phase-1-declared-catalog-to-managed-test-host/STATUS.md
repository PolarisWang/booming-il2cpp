---
task_id: 20260414-31-phase-1-declared-catalog-to-managed-test-host
title: Phase 1 Declared Catalog To Managed Test Host
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 16:49:05 +08:00
updated_at: 2026-04-14 17:04:51 +08:00
current_dir: docs/dev/completed/20260414-31-phase-1-declared-catalog-to-managed-test-host
parent_task_id: 20260414-20-subject-generated-test-workspace-roadmap
source_task_id: 20260414-20-subject-generated-test-workspace-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/roadmap-v1-01.md`
- phase_0_contract_freeze: `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/contract-freeze-v1-01.md`
- phase_0_legacy_cleanup_inventory: `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/legacy-cleanup-inventory-v1-01.md`
- plan: `docs/dev/completed/20260414-31-phase-1-declared-catalog-to-managed-test-host/plan-v1-01.md`

## 当前结论

- current_focus: Phase 1 已完成，declared catalog 现已输出稳定的 `entryIndex`，并且 managed proof host / benchmark host 的最小生成链路已经建立。
- why_now: Phase 2 需要以 Phase 1 的 generated managed host 作为 workspace v2 与 native test project 的输入骨架，否则还会回退到手写 host 或字符串分发。
- success_definition:
  - catalog 输出同时包含 `stableId` 与按 family 排序后的 `entryIndex`
  - `build/toolchains/run/testing/generated_managed_hosts.py` 可生成 proof/benchmark managed host 源码与 `.csproj`
  - host 生成对空 catalog 与缺失关键元数据的输入会直接失败，不再静默吞掉坏数据

## 最近摘要

- 2026-04-14 16:49:05 +08:00: 创建 Phase 1 child task，准备从 RED 测试开始实现 generated managed host 主链。
- 2026-04-14 17:04:51 +08:00: 完成 `entryIndex` catalog 接线、managed host 生成器与边界校验，并通过 Phase 1 相关测试。

## 下一步

- next_action: 返回父 roadmap，创建并执行 `20260414-32-phase-2-workspace-v2-and-native-test-project-generation`。
- owner: codex
- trigger: Phase 1 代码、测试与文档状态已经闭环。

## 风险 / 阻塞

### risks

- Phase 2 若不直接消费 generated managed host，而是重新拼一套 native test 列表，会重新引入双轨 identity。
- `workspace.manifest.json` v2 若继续写回旧字段作为主输出，Phase 5 的 cleanup 会被拖慢。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/planning/test_generated_managed_hosts.py`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py`
- `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py`

## wiki

- 本子任务只在 `docs/dev` 记录执行状态，不单独更新 `wiki/`。
