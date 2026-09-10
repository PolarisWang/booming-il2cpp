---
task_id: 20260413-19-phase-6-benchmark-hotupdate-cutover-batch-1
title: Phase 6 Benchmark And Hot-Update Cutover Batch 1
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 02:52:33 +08:00
updated_at: 2026-04-13 03:23:50 +08:00
current_dir: docs/dev/completed/20260413-19-phase-6-benchmark-hotupdate-cutover-batch-1
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- suspended_native_aot_benchmark_roadmap: `docs/dev/hanging/20260412-05-phase-b-generic-native-aot-benchmark-backend/roadmap-v1-01.md`
- plan: `docs/dev/completed/20260413-19-phase-6-benchmark-hotupdate-cutover-batch-1/plan-v1-01.md`

## 当前判断

- current_focus: 已完成 Phase 6 第一批 pure runtime benchmark cutover，建立 `PerformanceFeaturePack` 作为 canonical benchmark subject，并将 4 个 legacy `Bench*` roots 退化为 compatibility shell。
- why_done: `BenchArithmetic`、`BenchAllocation`、`BenchDispatch`、`BenchGeneric` 已全部改为指向 canonical source/project/entry，declared benchmark metadata、planner/schema/discovery 与 shell redirect 语义已经对齐。
- done_definition: canonical benchmark pack、legacy shell、registry/discovery/planner/schema 回归以及全量 `pytest` 均已通过，父 roadmap 可以继续切入 hot-update host pack 主线。

## 最近摘要

- 2026-04-13 02:52:33 +08:00: 激活 `20260413-19-phase-6-benchmark-hotupdate-cutover-batch-1`，承接 Phase 5 收口后的 benchmark/hot-update root 重组工作。
- 2026-04-13 03:23:50 +08:00: 完成 `PerformanceFeaturePack` 更名收口、4 个 pure runtime benchmark shell redirect 对齐，以及相关 planner/schema/discovery/performance 测试更新；定向验证结果为 `64 passed`，全量验证结果为 `745 passed, 40 skipped`。

## 下一步

- next_action: 返回父 roadmap，激活 `20260413-20-phase-6-hot-update-host-pack-cutover-batch-2`，先把 `BenchHotUpdate*` 与 `HotUpdateSkeletonProof` 归并到 `HotUpdateHostPack`，再处理更复杂的 mixed execution / interpreter roots。
- owner: codex
- trigger: 需要继续推进 Phase 6 hot-update / mixed execution declared-test migration 时立即执行。

## 风险 / 阻塞

### risks

- `BenchHotUpdate*`、`BenchMixed`、`MixedExecutionProof`、`Interpreter*` 仍然跨越 benchmark、host、patch、interpreter/AOT bridge 多层语义，后续批次必须继续按边界拆分。
- `PerformanceFeaturePack` 已避开 `Bench*` 命名扫描冲突，但 Phase 6 后续新增 canonical subject 时仍需避免与 legacy selector 规则产生前缀耦合。
- Phase 7 之前 registry / selector / artifact / pipeline 仍处于 hybrid 过渡态，后续 cutover 需要继续保持 shell 兼容面稳定。

### blockers

- 当前无外部 blocker。
