---
task_id: 20260409-04-phase-2-core-contract-cutover
title: Phase 2 Core Contract Cutover
task_type: plan
lifecycle_status: completed
phase: archived
created_at: 2026-04-09 10:58:04 +08:00
updated_at: 2026-04-09 13:12:00 +08:00
current_dir: docs/dev/completed/20260409-04-phase-2-core-contract-cutover
parent_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/completed/20260409-04-phase-2-core-contract-cutover/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260409-04-phase-2-core-contract-cutover/design-v1-01.md`
- roadmap_or_plan: `docs/dev/completed/20260409-04-phase-2-core-contract-cutover/plan-v1-01.md`
- latest_progress: `docs/dev/completed/20260409-04-phase-2-core-contract-cutover/notes/progress-v1-02.md`

## 执行概况

- total_tasks: 5
- current_task: 已完成归档。

## 当前判断

- current_focus: 把 performance-first contract 从 discuss/roadmap 层正式切进 `SemanticWorld -> Linker -> CodeGen` 代码骨架。
- why_now: `Phase 0/1` 已冻结 complete/performance governance 与 `OptimizationFacts`/native perf contract，但 `SemanticWorld` 仍是 passthrough，`CodeGen` 仍保留 semantic-ish normalization，`Emitter` 仍明显依赖 sample shape。
- done_definition: `SemanticWorld` 不再只是透传；`Linker` 不再承担字符串 concat 归一化；`CodeGen` 输出显式消费 semantic shape/capability contract；相关 contract sample/schema/snapshot 与回归测试更新完成。

## 最近摘要

- 2026-04-09 10:58:04 +08:00: 父 roadmap 已正式派生 `20260409-04-phase-2-core-contract-cutover` 作为 `Phase 2` child task。
- 2026-04-09 10:58:04 +08:00: 本任务当前采用 “最小 contract cutover” 路线：先把 canonical subjects / semantic shapes / capability bundles 与 typed-il contract threaded 起来，不提前做完整 lowering arbitration 重写。
- 2026-04-09 10:58:04 +08:00: 对 `String.Concat(string,string,string)` 的现有 pairwise canonicalization，当前决策是在 `Phase 2` 前移到 `SemanticWorld`，仅作为已冻结 slice 的 canonical body cutover；更完整的 lowering family 明确化仍留给 `Phase 3`。

## 下一步

- next_action: 无。后续回到父 roadmap，派生 `phase-3-capability-driven-lowering-and-arbitration`。
- owner: codex
- trigger: `Phase 2` focused regressions 已通过。

## 风险 / 阻塞

### risks

- 如果只在文档层声明 `SemanticWorld`/`CodeGen` 边界而不把 contract 写进代码与 artifact，`Phase 2` 会退化成没有执行价值的讨论续篇。
- 如果在本阶段就彻底重写 `NativeReferenceProofEmitter` 的 family selection，会把 `Phase 3` 的 lowering arbitration 提前混进来，扩大改动面。
- 如果把 optimization-relevant 事实继续藏在 `CodeGen` 的局部 rewrite 或 `Emitter` 的 shape detection 里，`Phase 0/1` 冻结的 performance-first contract 仍然不算 structural。

### blockers

- 无。

## Completion Notes

- 2026-04-09 13:12:00 +08:00: 已把 `SemanticWorld` 切换为 canonical body / shape / capability builder，并前移 `String.Concat(string,string,string)` -> pairwise concat canonicalization。
- 2026-04-09 13:12:00 +08:00: 已把 `Linker -> CodeGen -> NativeReferenceProofEmitter` 切换为消费显式 `methodRole / bodyAvailability / capabilities` contract，并同步 `typed-il-ir` schema/sample/snapshot/example。
- 2026-04-09 13:12:00 +08:00: 验证通过 `python -m unittest tests.unit.run.test_phase2_core_contract_cutover`。
- 2026-04-09 13:12:00 +08:00: 验证通过 `python -m unittest tests.unit.run.test_managed_closure_contract_bundle`。
- 2026-04-09 13:12:00 +08:00: 组合回归通过 `python -m unittest tests.unit.run.test_phase2_core_contract_cutover tests.unit.run.test_managed_closure_contract_bundle`。
- 2026-04-09 13:12:00 +08:00: 用户已确认 subject-bearing contract 的全量清理不在本轮 `Phase 2` 拆开，后续另开 follow-up task 收口。

## 关联任务

### parent

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`

### source

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`（roadmap-child）

### children

- 无。
