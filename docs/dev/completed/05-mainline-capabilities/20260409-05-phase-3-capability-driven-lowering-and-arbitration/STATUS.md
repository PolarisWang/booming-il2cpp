---
task_id: 20260409-05-phase-3-capability-driven-lowering-and-arbitration
title: Phase 3 Capability-Driven Lowering And Arbitration
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-09 11:54:10 +08:00
updated_at: 2026-04-09 13:13:08 +08:00
current_dir: docs/dev/completed/20260409-05-phase-3-capability-driven-lowering-and-arbitration
parent_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260409-05-phase-3-capability-driven-lowering-and-arbitration/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260409-05-phase-3-capability-driven-lowering-and-arbitration/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260409-05-phase-3-capability-driven-lowering-and-arbitration/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260409-05-phase-3-capability-driven-lowering-and-arbitration/notes/progress-v1-02.md`

## 执行概况

- total_tasks: 5
- current_task: 已完成归档。

## 当前判断

- current_focus: `CodeGen` 已成为 managed lowering plan 真源，`NativeReferenceProofEmitter` 已收缩为 plan consumer；本任务完成后由父 roadmap 切入 `Phase 4` 的 `MainlineFeaturePack` onboarding。
- why_now: `Phase 2` 之后剩余的主决策漂移点就是 emitter 内的 lowering discovery；只有把它迁回 `CodeGen`，后续完整 subject 和 native perf 主线才有稳定入口。
- done_definition: managed closure root 产出 `native-reference.lowering-plan.json`；`CodeGen` 选择 canonical lowering family；emitter 只消费 plan 并镜像 `native-proof.plan.json`。

## 最近摘要

- 2026-04-09 11:54:10 +08:00: 父 roadmap 已把 `Phase 2` 标记完成，并派生本 child 作为新的活动入口。
- 2026-04-09 11:54:10 +08:00: 已完成 RED 测试补充，当前失败点明确集中在 lowering plan artifact 缺失与 emitter 仍持有 lowering discovery。
- 2026-04-09 11:54:10 +08:00: 用户已确认 `contract` 与 `subject` 的边界清理延后为单独任务，本轮不插入。
- 2026-04-09 13:13:08 +08:00: 已把 `native-reference.lowering-plan.json` 接入 `ManagedClosureContracts`、`CodeGenStage`、`DriverEntry` 与 `closure.manifest.json`，managed closure root 现在会显式产出 lowering plan artifact。
- 2026-04-09 13:13:08 +08:00: 已将 `NativeReferenceProofEmitter` 切换为只消费 managed lowering plan，并把 lowering family 选择迁移到 `NativeReferenceLoweringPlanner`。
- 2026-04-09 13:13:08 +08:00: 验证通过 `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1 /p:ChaosTempIntermediateRoot=<temp>/`、`python -m unittest tests.unit.run.test_phase3_capability_driven_lowering`、`python -m unittest tests.unit.run.test_managed_closure_contract_bundle`。

## 下一步

- next_action: 无。后续回到父 roadmap，派生并执行 `20260409-06-phase-4-mainline-feature-pack-onboarding`。
- owner: codex
- trigger: RED 已确认。

## 风险 / 阻塞

### risks

- 如果只做 artifact 命名迁移、不真正移除 emitter 内的 lowering discovery，`Phase 3` 会退化成包装性重构。
- 如果本轮把 role contract 一次性重写过深，会把 `Phase 3` 和后续 plan contract 重构绑死，扩大回归面。
- 如果让 dependency / reason 重新回到 primary classifier，`CodeGen` 仍会被 BCL 细节牵着走。

### blockers

- 无。

## Completion Notes

- 2026-04-09 13:13:08 +08:00: `ManagedClosureResult`、`closure.manifest.json` 与 driver 输出已显式携带 `native-reference.lowering-plan.json`。
- 2026-04-09 13:13:08 +08:00: `SemanticWorld` 已补 `static-forwarder`、`instance-field-getter` 结构信号，`NativeReferenceLoweringPlanner` 已按 capability/structure 驱动选择 lowering family。
- 2026-04-09 13:13:08 +08:00: `NativeReferenceProofEmitter` 不再负责 lowering discovery，只校验并镜像 managed lowering plan 到 `native-proof.plan.json`。
- 2026-04-09 13:13:08 +08:00: focused regressions 通过：`python -m unittest tests.unit.run.test_phase3_capability_driven_lowering tests.unit.run.test_managed_closure_contract_bundle`。

## 关联任务

### parent

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`

### source

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`（roadmap-child）

### children

- 无。
