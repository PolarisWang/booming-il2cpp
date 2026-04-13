---
task_id: 20260413-18-phase-5-mainline-advanced-proof-shell-cutover-batch-4
title: Phase 5 Mainline Advanced Proof Shell Cutover Batch 4
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 01:59:08 +08:00
updated_at: 2026-04-13 02:52:33 +08:00
current_dir: docs/dev/completed/20260413-18-phase-5-mainline-advanced-proof-shell-cutover-batch-4
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- plan: `docs/dev/completed/20260413-18-phase-5-mainline-advanced-proof-shell-cutover-batch-4/plan-v1-01.md`

## 当前判断

- current_focus: 已完成最后一批 advanced `mainline` proof-like roots 向 `MainlineFeaturePack` 的并回、shell 化，以及 convert/declaration 回归闭环。
- why_done: `InterfaceDispatchProof`、`VTableDispatchProof`、`LinkerStrippingProof`、`MarshalingProof`、`CrossBoundaryExceptionProof` 已全部变成 `MainlineFeaturePack` 内的真实 `ChaosUnitTest` slice，legacy root 仅保留 compatibility shell 与既有 perf 入口。
- done_definition: canonical subject、legacy shell、planner/schema/discovery、旧 phase2 tests、loader/linker capability 回归已经对齐，并通过定向与全量测试验证。

## 最近摘要

- 2026-04-13 01:59:08 +08:00: 激活 `20260413-18-phase-5-mainline-advanced-proof-shell-cutover-batch-4`，承接 Batch 3 之后的 remaining mainline proof root cutover。
- 2026-04-13 02:52:33 +08:00: 完成 5 个 advanced proof slice 的 `MainlineFeaturePack` 合并、5 个 legacy manifest shell 化，以及 loader/semantic world 回归修复；定向与全量 `pytest` 结果为 `740 passed, 40 skipped`。

## 下一步

- next_action: 返回父 roadmap，激活 `20260413-19-phase-6-benchmark-hotupdate-cutover-batch-1`，先盘点 `Bench*` 与 hot-update / mixed execution roots 的归宿，再切第一批 pure runtime benchmark cutover。
- owner: codex
- trigger: 需要继续推进 Phase 6 benchmark/hot-update declared-test migration 时立即执行。

## 风险 / 阻塞

### risks

- Phase 6 需要先区分 runtime benchmark、engineering workload 与 hot-update host workload，否则容易把 `generated-native-aot`、`ChaosBenchmark` 与工程 workload 再次混在一起。
- `InterfaceDispatchProof` 的 perf surface 现已由 shell 保留，但接下来 benchmark root cutover 时仍需避免冲击现有 dashboard / perf governance 口径。
- hot-update / mixed execution roots 同时跨 host、patch、benchmark 三层语义；若 batch 边界不清晰，容易让 canonical subject 重新失焦。

### blockers

- 当前无外部 blocker。
