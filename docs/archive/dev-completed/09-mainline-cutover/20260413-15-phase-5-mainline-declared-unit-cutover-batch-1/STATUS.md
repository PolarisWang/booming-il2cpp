---
task_id: 20260413-15-phase-5-mainline-declared-unit-cutover-batch-1
title: Phase 5 Mainline Declared Unit Cutover Batch 1
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-13 00:59:47 +08:00
updated_at: 2026-04-13 01:27:27 +08:00
current_dir: docs/dev/completed/20260413-15-phase-5-mainline-declared-unit-cutover-batch-1
parent_task_id: 20260412-06-test-system-suite-redesign
source_task_id: 20260412-06-test-system-suite-redesign
source_relation: roadmap-child-batch
active: false
---

## 关键文档

- parent_design: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`
- parent_roadmap: `docs/dev/in-progress/20260412-06-test-system-suite-redesign/roadmap-v1-03.md`
- plan: `docs/dev/completed/20260413-15-phase-5-mainline-declared-unit-cutover-batch-1/plan-v1-01.md`

## 当前判断

- current_focus: Phase 5 Batch 1 已完成 `MainlineFeaturePack` 的第一轮 declared-unit cutover，使其成为当前 roadmap 下第一个真实落地 `ChaosUnitTest` 的 canonical solution subject。
- why_done: `MainlineFeaturePack` 已切换到 `testDeclarationMode=require`，source project 已直接引用 `src/reference/Chaos.TestFramework`，真实编译产物可被 declaration discovery 稳定识别出 7 个 `declaredUnitTests[]`。
- done_definition: manifest/source/discovery/schema/planner 相关契约已经对齐，且没有破坏现有工程级 planner / perf / trace 主链。

## 最近摘要

- 2026-04-13 00:59:47 +08:00: Phase 4 Batch 1 归档完成，切换到 `20260413-15-phase-5-mainline-declared-unit-cutover-batch-1`，准备为 `MainlineFeaturePack` 补第一次真实声明式 proof 落地。
- 2026-04-13 01:27:27 +08:00: 完成 `MainlineFeaturePack` manifest/source cutover、`Directory.Build.props` 中间目录冲突修复，以及 declaration discovery/schema/planner 定向回归验证；Batch 1 达到归档条件。

## 下一步

- next_action: 返回父 roadmap，创建 Phase 5 Batch 2，开始把第一批 legacy opcode/object proof roots 并回 `MainlineFeaturePack`，并把旧 subject 目录退化为 compatibility shell。
- owner: codex
- trigger: 需要推进 proof-like top-level roots 的第一批真实物理收敛时立即执行。

## 风险 / 阻塞

### risks

- 仍有大量 `*Proof` roots 尚未并回 canonical subject；Batch 1 只证明了 declared-unit 主线可行，未完成 legacy root 收口。
- registry / selector / artifact / pipeline 仍未切到 unified hybrid catalog；后续批次要避免让 compatibility shell 与 canonical declared catalog 语义混淆。

### blockers

- 当前无外部 blocker。
