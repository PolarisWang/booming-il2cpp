---
task_id: 20260413-17-phase-5-mainline-runtime-proof-shell-cutover-batch-3
title: Phase 5 Mainline Runtime Proof Shell Cutover Batch 3
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 01:43:29 +08:00
updated_at: 2026-04-13 01:59:08 +08:00
current_dir: docs/dev/completed/20260413-17-phase-5-mainline-runtime-proof-shell-cutover-batch-3
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- plan: `docs/dev/completed/20260413-17-phase-5-mainline-runtime-proof-shell-cutover-batch-3/plan-v1-01.md`

## 当前判断

- current_focus: 已完成第二批仍被旧 phase tests 直接锚定的 runtime proof roots 向 `MainlineFeaturePack` 的并回与 shell 化。
- why_done: `AsyncAwaitProof`、`ThreadingProof`、`NestedExceptionProof`、`GenericCollectionProof`、`DelegateChainProof` 已全部变成 `MainlineFeaturePack` 内的真实 `ChaosUnitTest` slice，legacy root 仅保留 compatibility shell 责任。
- done_definition: canonical subject、legacy shell、planner/schema/discovery、旧 phase2 tests 四层契约已经对齐，并通过全量测试验证。

## 最近摘要

- 2026-04-13 01:43:29 +08:00: 激活 `20260413-17-phase-5-mainline-runtime-proof-shell-cutover-batch-3`，冻结第二批 runtime proof shell cutover 范围。
- 2026-04-13 01:59:08 +08:00: 完成 5 个 runtime proof slice 的 `MainlineFeaturePack` 合并、5 个 legacy manifest shell 化，以及定向与全量 `pytest` 验证，结果为 `726 passed, 40 skipped`。

## 下一步

- next_action: 返回父 roadmap，激活 `20260413-18-phase-5-mainline-advanced-proof-shell-cutover-batch-4`，继续处理 `InterfaceDispatchProof`、`VTableDispatchProof`、`LinkerStrippingProof`、`MarshalingProof`、`CrossBoundaryExceptionProof`。
- owner: codex
- trigger: 需要继续压缩 remaining mainline proof-like roots 时立即执行。

## 风险 / 阻塞

### risks

- `InterfaceDispatchProof` 同时承载 native perf profile；下一批 cutover 需要保留 shell 上的 perf 入口，避免影响 dashboard / perf governance。
- `MarshalingProof` 与 `CrossBoundaryExceptionProof` 涉及 interop 与异常流语义；迁移时需要谨慎保持 `requires` 与 category 的 typed 表达，不放松现有断言。
- `VTableDispatchProof`、`CrossBoundaryExceptionProof` 当前缺少与 Batch 3 同等级的迁移断言；下一批需要先补 RED tests 再做 source cutover。

### blockers

- 当前无外部 blocker。
