---
task_id: 20260412-09-phase-2-chaos-testing-contract-foundation-batch-1
title: Phase 2 Chaos Testing Contract Foundation Batch 1
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-12 22:22:28 +08:00
updated_at: 2026-04-12 22:32:38 +08:00
current_dir: docs/dev/completed/20260412-09-phase-2-chaos-testing-contract-foundation-batch-1
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- plan: `docs/dev/completed/20260412-09-phase-2-chaos-testing-contract-foundation-batch-1/plan-v1-01.md`

## 当前判断

- current_focus: Phase 2 第一批已完成 `Chaos.TestFramework` 最小合同、`testDeclarationMode` 默认行为与 declaration typed foundation 的落地。
- why_done: reference project、Python declaration helper、manifest capability 默认值与验证链路都已经进入仓库并被自动化测试覆盖。
- done_definition: 本批已满足计划内 4 个任务，并通过 `dotnet build` 与全量 `pytest`。

## 最近摘要

- 2026-04-12 22:22:28 +08:00: 从父 roadmap 恢复执行，创建 Phase 2 Batch 1 子任务，准备先冻结 `Chaos.TestFramework` 最小合同与 manifest 声明模式支撑。
- 2026-04-12 22:32:38 +08:00: 已新增 `src/reference/Chaos.TestFramework`、`build/toolchains/run/testing/declarations.py` 与 `testDeclarationMode` capability 默认值，验证结果为 `688 passed, 40 skipped` 与 `dotnet build` 成功。

## 下一步

- next_action: 返回父 roadmap，继续 Phase 2 Batch 2 的 metadata discovery 与 catalog/registry 接线。
- owner: codex
- trigger: 当需要把 declaration contract 接入真实程序集 metadata 扫描与统一 catalog 时恢复。

## 风险 / 阻塞

### risks

- declaration discovery 的真正程序集 metadata 扫描仍未落地，下一批需要在不破坏当前合同边界的前提下接到 catalog/registry 主链。
- 当前 `Chaos.TestFramework` 仍只覆盖最小 attribute/enum 合同；helper API 扩展应等实际 subject 迁移时按需推进。

### blockers

- 当前无外部 blocker。
