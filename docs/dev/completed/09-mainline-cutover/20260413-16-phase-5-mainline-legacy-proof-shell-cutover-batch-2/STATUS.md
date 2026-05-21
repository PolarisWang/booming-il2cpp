---
task_id: 20260413-16-phase-5-mainline-legacy-proof-shell-cutover-batch-2
title: Phase 5 Mainline Legacy Proof Shell Cutover Batch 2
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 01:30:52 +08:00
updated_at: 2026-04-13 01:42:10 +08:00
current_dir: docs/dev/completed/20260413-16-phase-5-mainline-legacy-proof-shell-cutover-batch-2
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- plan: `docs/dev/completed/20260413-16-phase-5-mainline-legacy-proof-shell-cutover-batch-2/plan-v1-01.md`

## 当前判断

- current_focus: Phase 5 Batch 2 已完成第一批 legacy opcode/object proof shell cutover，证明 top-level `*Proof` roots 可以在不破坏旧 subject id 的前提下并回 `MainlineFeaturePack`。
- why_done: `MainlineFeaturePack` 已新增 6 个真实 `ChaosUnitTest` slices，6 个 legacy proof subjects 已退化为 compatibility shell，并且 `testDeclarationMode=none` 在 catalog 层具备稳定短路语义。
- done_definition: canonical subject、compatibility shell、planner/schema/catalog 三层契约已对齐，`python -m pytest -q` 结果为 `720 passed, 40 skipped`。

## 最近摘要

- 2026-04-13 01:30:52 +08:00: 激活 `20260413-16-phase-5-mainline-legacy-proof-shell-cutover-batch-2`，冻结第一批 legacy proof shell cutover 范围。
- 2026-04-13 01:42:10 +08:00: 完成 `MainlineFeaturePack` batch2 proof slices、6 个 legacy shell manifests、`compiled_catalog.py` 的 `none` 模式短路，以及定向回归与全量 `pytest` 验证。

## 下一步

- next_action: 返回父 roadmap，创建 Phase 5 Batch 3，继续处理第二批仍由旧 phase tests 直接锚定的 mainline/runtime proof roots。
- owner: codex
- trigger: 需要继续压缩 proof-like top-level roots 时立即执行。

## 风险 / 阻塞

### risks

- 仍有一批 `AsyncAwaitProof`、`ThreadingProof`、`NestedExceptionProof`、`LinkerStrippingProof` 等 roots 仍被旧阶段测试直接锚定；下一批需要一并改测试和 shell 语义。
- compatibility shell 目前仍保留旧 `expected/` 资产目录；后续 Phase 8 仍需决定这些历史资产的最终归宿。

### blockers

- 当前无外部 blocker。
