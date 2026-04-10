---
task_id: 20260409-08-phase-6-capability-batch-b-delegate-exception-and-phase-a-closure
title: Phase 6 Capability Batch B Delegate Exception And Phase A Closure
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-09 21:43:44 +08:00
updated_at: 2026-04-10 00:45:00 +08:00
current_dir: docs/dev/in-progress/20260409-08-phase-6-capability-batch-b-delegate-exception-and-phase-a-closure
parent_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_task_id: 20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- brainstorm: `docs/dev/in-progress/20260409-08-phase-6-capability-batch-b-delegate-exception-and-phase-a-closure/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260409-08-phase-6-capability-batch-b-delegate-exception-and-phase-a-closure/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260409-08-phase-6-capability-batch-b-delegate-exception-and-phase-a-closure/plan-v1-02.md`
- latest_progress: `docs/dev/in-progress/20260409-08-phase-6-capability-batch-b-delegate-exception-and-phase-a-closure/notes/progress-v1-02.md`

## Execution Update

- execution_status: active
- execution_reason: 用户已明确要求继续执行到主任务结束，因此 `Phase 6` 不再停留在 design review gate，直接切到 TDD 实现。
- current_focus_override: 先冻结 `subject.manifest.json` / planner 的 `Phase 6` focused matrices，再进入 delegate / exception / reflection-interop closure 的 RED。
- next_action_override: 编写 `tests.unit.run.test_subject_manifest_schema`、`tests.unit.run.test_subject_planner` 与 `tests.unit.run.test_phase6_capability_batch_b` 的 RED。

## Review Gate

- review_status: approved-by-user
- issue: `Phase 6` 需要在不提前卷入 `engine binding` 的前提下，把 `delegates/callbacks`、`exceptions` 与必要 `reflection/interop` 补位接回完整 `MainlineFeaturePack`，并完成 `Phase A` closure。
- recommendation: 继续沿用一个完整 `MainlineFeaturePack` + 多个 proof slice 的结构；`delegates` 优先 exact-target 直调、保留 runtime fallback；`exceptions` 采用对热路径更友好的 cold-path EH 方案；`reflection/interop` 只补最小 closure 缺口，不恢复平行 subject 主线。
- next_action: 等用户审阅 `design-v1-01.md`；若认可，再写 `plan-v1-01.md` 并进入实现。

## 当前判断

- current_focus: 先冻结 `Phase 6` 设计边界，明确 `delegates/callbacks`、`exceptions` 与必要 `reflection/interop` 补位该如何在完整 subject 主线上落地。
- why_now: `Phase 5` 已完成，当前 roadmap 的下一批强制交付能力就是 `delegates/callbacks` 与 `exceptions`；同时需要在 `Phase A closure` 前把最小 `reflection/interop` 缺口从“参考能力”提升到“完整 subject 主线能力”。
- done_definition: 用户认可 `Phase 6` 设计；随后可进入 `plan-v1-01.md`，推进实现与验证。

## 最近摘要

- 2026-04-09 21:43:44 +08:00: `Phase 5` 已按“功能与 correctness 完成，perf 入口打通，带负载 perf baseline 留待独立复核”的口径归档。
- 2026-04-09 21:43:44 +08:00: 父 roadmap 已切到 `Phase 6`；本轮先写 `delegates/exceptions` 设计，不直接进入实现。

## 下一步

- next_action: 请用户审阅 `docs/dev/in-progress/20260409-08-phase-6-capability-batch-b-delegate-exception-and-phase-a-closure/design-v1-01.md`；如无修改，再进入 `plan-v1-01.md`。
- owner: codex
- trigger: `Phase 5` 已完成，主线继续推进。

## 风险 / 阻塞

### risks

- 如果把 `delegates/callbacks` 直接做成通用 runtime callback 系统，会提前把 `engine binding` 拖进主线。
- 如果把 `exceptions` 设计成每层都携带显式状态传播，会直接损伤 hot-path `C++` 性能。
- 如果把 `reflection/interop` 再次留在平行 reference subject，而不是接入 `MainlineFeaturePack`，`Phase A closure` 会继续失真。

### blockers

- 当前阻塞不是技术实现，而是 `Phase 6` 设计仍需用户 review 批准后才能进入 plan / implementation。

## 关联任务

### parent

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`

### source

- `20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap`（roadmap-child）

### children

- 无。

## 最新更新

- 2026-04-10 00:35:24 +08:00: `windows-delegate-check`、`windows-exception-check`、`windows-reflection-interop-closure-check` 已全部通过，run id 分别为 `20260410-000311-windows-4450`、`20260410-001006-windows-0624`、`20260409-235604-windows-2909`。
- 2026-04-10 00:35:24 +08:00: native perf worker 已改为显式 warmup sample + 热态样本摘要；`perf.profile` 复跑 `20260410-002943-windows-9cae` 为 `ok`，热态 `meanDurationMs=12.299`、`maxDurationMs=15.146`。
- 2026-04-10 00:45:00 +08:00: `Phase 6` 已正式收口；`Phase A` 剩余缺口只剩 `Phase 7 engine binding tail roll-in`。
