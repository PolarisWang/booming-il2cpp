---
task_id: 20260413-20-phase-6-hot-update-host-pack-cutover-batch-2
title: Phase 6 Hot-Update Host Pack Cutover Batch 2
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 03:25:29 +08:00
updated_at: 2026-04-13 03:47:47 +08:00
current_dir: docs/dev/completed/20260413-20-phase-6-hot-update-host-pack-cutover-batch-2
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档。

- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- completed_previous_batch: `docs/dev/completed/20260413-19-phase-6-benchmark-hotupdate-cutover-batch-1/plan-v1-01.md`
- next_batch: `docs/dev/in-progress/20260413-21-phase-6-mixed-execution-feature-pack-cutover-batch-3/plan-v1-01.md`
- plan: `docs/dev/completed/20260413-20-phase-6-hot-update-host-pack-cutover-batch-2/plan-v1-01.md`

## 当前判断

- current_focus: 已完成 `HotUpdateHostPack` canonical pack 收口，把 hot-update host proof/benchmark roots 合并为统一 source bundle，并将首批 legacy hot-update roots 退化为 compatibility shell。
- why_done: `HotUpdateHostPack` 已稳定承载 declared unit + declared benchmark metadata；`BenchHotUpdateDispatch`、`BenchHotUpdateLoad`、`BenchHotUpdateRoundtrip` 与 `HotUpdateSkeletonProof` 已切到 canonical source/project/entry/redirect 模型。
- done_definition: 本批目标与验证全部满足，父 roadmap 可以继续推进 mixed execution / interpreter family 的最后一批 Phase 6 cutover。

## 最近摘要

- 2026-04-13 03:25:29 +08:00: 激活 `20260413-20-phase-6-hot-update-host-pack-cutover-batch-2`，承接 Batch 1 完成后的 hot-update host pack 主线。
- 2026-04-13 03:47:47 +08:00: 完成 `HotUpdateHostPack` canonical source bundle、legacy hot-update benchmark/proof shell cutover，以及相关 planner/schema/discovery/runtime 测试更新；定向验证结果为 `72 passed`，hot-update e2e 为 `6 passed`，全量验证结果为 `756 passed, 40 skipped`。

## 下一步

- next_action: 返回父 roadmap，激活 `20260413-21-phase-6-mixed-execution-feature-pack-cutover-batch-3`，把 `BenchMixed`、`MixedExecutionProof`、`InterpreterArithmeticProof`、`InterpreterLoweringProof` 归并到 mixed/interpreter canonical feature pack。
- owner: codex
- trigger: 需要继续推进 Phase 6 mixed execution / interpreter declared-test migration 时立即执行。

## 风险 / 阻塞

### risks

- mixed execution / interpreter family 同时跨越 benchmark、lowering、AOT bridge、hot-update bridge 多层语义，下一批必须继续保持边界清晰，避免把工程 workload 错并成 `ChaosBenchmark`。
- `InterpreterArithmeticProof` 更像 support assembly 而不是单纯 proof entry，下一批需要明确 canonical pack 中 support project 与 declared entry 的关系。

### blockers

- 当前无外部 blocker。
