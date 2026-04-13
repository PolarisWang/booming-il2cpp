---
task_id: 20260413-21-phase-6-mixed-execution-feature-pack-cutover-batch-3
title: Phase 6 Mixed-Execution Feature Pack Cutover Batch 3
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 03:47:47 +08:00
updated_at: 2026-04-13 04:18:00 +08:00
current_dir: docs/dev/completed/20260413-21-phase-6-mixed-execution-feature-pack-cutover-batch-3
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档
- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- completed_previous_batch: `docs/dev/completed/20260413-20-phase-6-hot-update-host-pack-cutover-batch-2/plan-v1-01.md`
- next_batch: `docs/dev/in-progress/20260413-22-phase-7-registry-selector-artifact-pipeline-cutover-batch-1/plan-v1-01.md`
- plan: `docs/dev/completed/20260413-21-phase-6-mixed-execution-feature-pack-cutover-batch-3/plan-v1-01.md`

## 当前判断

- current_focus: 已完成 `MixedExecutionFeaturePack` canonical mixed/interpreter source bundle 收口，并把 `BenchMixed`、`MixedExecutionProof`、`InterpreterArithmeticProof`、`InterpreterLoweringProof` 退化为 compatibility shell 或 canonical bundle 内 support project 投影。
- why_done: `MixedExecutionFeaturePack` 已稳定承载 mixed execution declared unit/benchmark metadata；`InterpreterArithmeticProof` 已保留为 canonical bundle 内 support project；legacy mixed/interpreter roots 已切到 canonical source/path/entry/redirect 模型。
- done_definition: 本批目标与验证全部满足，父 roadmap 可以正式结束 Phase 6，并切入 Phase 7 registry / selector / artifact / pipeline 主入口切换。

## 最近摘要
- 2026-04-13 03:47:47 +08:00: 激活 `20260413-21-phase-6-mixed-execution-feature-pack-cutover-batch-3`，承接 Batch 2 完成后的 mixed execution / interpreter 收口工作。
- 2026-04-13 04:18:00 +08:00: 完成 `MixedExecutionFeaturePack` canonical source bundle、legacy mixed/interpreter shell cutover、support-project 收口与相关 planner/schema/discovery/runtime 测试更新；`dotnet build subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj -c Release -m:1` 通过，定向验证结果为 `74 passed`，`tests/unit/run/test_phase7_interpreter_mixed_execution.py -q` 结果为 `11 passed`，全量验证结果为 `771 passed, 40 skipped`。

## 下一步
- next_action: 返回父 roadmap，激活 `20260413-22-phase-7-registry-selector-artifact-pipeline-cutover-batch-1`，把 compiled catalog 真正接入 registry / selector / CLI / reporting / artifact 主入口。
- owner: codex
- trigger: 需要继续推进 Phase 7 registry / selector / artifact / pipeline cutover 时立即执行。

## 风险 / 阻塞

### risks

- compiled catalog 已具备 engineering / declared 数据结构，但 registry / selector / CLI 仍主要停留在旧的 `suite / subject / module / system / pipeline` 对象层，Phase 7 需要避免只做“显示层接线”而不完成真实执行入口切换。
- `HotUpdateHostPack` 这类同时存在 declared unit 与 declared benchmark 的 subject，后续要补齐 benchmark 级 matrix/goal 归宿，避免 declared benchmark 只能被 discovery 到、却无法成为一等可执行对象。

### blockers

- 当前无外部 blocker。
