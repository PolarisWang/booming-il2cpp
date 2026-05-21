---
task_id: 20260414-32-phase-2-workspace-v2-and-native-test-project-generation
title: Phase 2 Workspace v2 And Native Test Project Generation
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 17:04:51 +08:00
updated_at: 2026-04-14 17:32:13 +08:00
current_dir: docs/dev/completed/20260414-32-phase-2-workspace-v2-and-native-test-project-generation
parent_task_id: 20260414-20-subject-generated-test-workspace-roadmap
source_task_id: 20260414-20-subject-generated-test-workspace-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/completed/20260414-20-subject-generated-test-workspace-roadmap/roadmap-v1-01.md`
- phase_0_contract_freeze: `docs/dev/completed/20260414-30-phase-0-generated-workspace-contract-and-legacy-inventory/contract-freeze-v1-01.md`
- phase_1_completion: `docs/dev/completed/20260414-31-phase-1-declared-catalog-to-managed-test-host/STATUS.md`
- plan: `docs/dev/completed/20260414-32-phase-2-workspace-v2-and-native-test-project-generation/plan-v1-01.md`

## 当前结论

- current_focus: Phase 2 已完成，`project_workspace.py` 现在会生成 workspace manifest v2，并把 managed projects、managed test projects、native projects、native test projects 全部收敛到同一份工作区描述。
- why_now: Phase 3 需要直接消费这份 workspace v2 做 unified test entry cutover；Phase 4 的 benchmark runner 和 dashboard 也依赖它作为唯一工程清单来源。
- success_definition:
  - `workspace.manifest.json` 显式写出 v2 根字段和四类工程记录
  - generated managed proof/benchmark host 被纳入 workspace，并写入 `.sln`
  - native test project 默认构建路由回到 proof host，不再把 generated static library 当成默认 native build 目标
  - 新 manifest 不再写入 `generatedRoot`、`defaultBuildNativeProject`、`nativeProjectPath` 等 legacy 字段

## 最近摘要

- 2026-04-14 17:04:51 +08:00: Phase 1 完成并归档；创建 Phase 2 child task，准备接线 workspace v2 与 native test project 生成。
- 2026-04-14 17:32:13 +08:00: 完成 workspace manifest v2、managed test project/native test project 生成、默认 proof-host native build 路由、以及 solution 顶层 native project 收口；相关 Phase 2 测试全部通过。

## 下一步

- next_action: 返回父 roadmap，激活并执行 `20260414-33-phase-3-unified-test-entry-and-registry-cutover`。
- owner: codex
- trigger: Phase 2 代码、测试与文档状态已经闭环。

## 风险 / 阻塞

### risks

- Phase 3 如果仍然绕开 workspace v2，CLI/registry 会继续保留第二条执行主链。
- Phase 4 如果不直接复用 Phase 2 的 managed/native test host 路由，benchmark identity 仍可能再次分叉。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/planning/test_project_workspace.py`
- `python -m pytest tests/unit/planning/test_generated_managed_hosts.py tests/integration/registry/test_declared_metadata_discovery.py tests/unit/compatibility/test_compiled_subject_catalog.py`

## wiki

- 本子任务只在 `docs/dev` 记录执行状态，不单独更新 `wiki/`。
