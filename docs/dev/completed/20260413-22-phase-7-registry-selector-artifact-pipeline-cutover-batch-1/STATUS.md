---
task_id: 20260413-22-phase-7-registry-selector-artifact-pipeline-cutover-batch-1
title: Phase 7 Registry Selector Artifact Pipeline Cutover Batch 1
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 04:18:00 +08:00
updated_at: 2026-04-13 05:58:37 +08:00
current_dir: docs/dev/completed/20260413-22-phase-7-registry-selector-artifact-pipeline-cutover-batch-1
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档
- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- completed_previous_batch: `docs/dev/completed/20260413-21-phase-6-mixed-execution-feature-pack-cutover-batch-3/plan-v1-01.md`
- next_batch: `docs/dev/in-progress/20260413-23-phase-8-compatibility-closure-and-legacy-cleanup-batch-1/plan-v1-01.md`
- plan: `docs/dev/completed/20260413-22-phase-7-registry-selector-artifact-pipeline-cutover-batch-1/plan-v1-01.md`

## 当前判断

- current_focus: 已完成 compiled catalog 到 registry / selector / CLI / report / artifact 主入口的统一切换，让 `subject`、`engineering-validation`、`engineering-workload`、`declared-unit-test`、`declared-benchmark` 成为 registry 一等对象。
- why_done: registry scan、selector 语义、动态 test command、planner entry selection 与 family-specific reporting / artifact path 已全部打通；Phase 7 的统一对象模型已经可执行、可报告、可回归。
- done_definition: 本批目标与验证全部满足，父 roadmap 可以正式结束 Phase 7 并切入 Phase 8 legacy cleanup。

## 最近摘要
- 2026-04-13 04:18:00 +08:00: 激活 `20260413-22-phase-7-registry-selector-artifact-pipeline-cutover-batch-1`，承接 Phase 6 全量 cutover 完成后的统一主入口切换工作。
- 2026-04-13 05:58:37 +08:00: 完成 unified registry object family、selector/CLI、planner entry selection、family-specific summary/report/artifact path 的切换；定向验证结果为 `65 passed`、`78 passed`，全量验证结果为 `782 passed, 40 skipped`。

## 下一步
- next_action: 返回父 roadmap，激活 `20260413-23-phase-8-compatibility-closure-and-legacy-cleanup-batch-1`，先冻结 cleanup inventory，再处理 tests taxonomy 收口与 legacy wrapper 退役顺序。
- owner: codex
- trigger: 需要继续推进 Phase 8 compatibility closure and legacy cleanup 时立即执行。

## 风险 / 阻塞

### risks

- Phase 8 仍残留大量 legacy proof / benchmark / golden shell roots，它们内部仍保留 expected / baseline / redirect 资产，不能在没有盘点的情况下直接删除。
- `tests/perf/` 仍有运行时基线入口被 `build/toolchains/run/testing/perf.py` 直接读取，legacy 顶层目录并非全部只是空壳。
- `tests/unit/run` 仍保留 30+ 个非 shim 的 `test_phase*` / `test_stage*` 文件，收口时需要同步更新 `test_repo_layout.py` 与分桶布局断言。

### blockers

- 当前无外部 blocker。
