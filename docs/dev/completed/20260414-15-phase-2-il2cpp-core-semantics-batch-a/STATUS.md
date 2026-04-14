---
task_id: 20260414-15-phase-2-il2cpp-core-semantics-batch-a
title: Phase 2 IL2CPP Core Semantics Batch A
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 13:13:20 +08:00
updated_at: 2026-04-14 14:07:36 +08:00
current_dir: docs/dev/completed/20260414-15-phase-2-il2cpp-core-semantics-batch-a
parent_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/completed/20260414-12-il2cpp-core-capability-development-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/completed/20260414-12-il2cpp-core-capability-development-roadmap/roadmap-v1-01.md`
- phase_0_coverage_ledger: `docs/dev/completed/20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze/coverage-ledger-freeze-v1-01.md`
- phase_1_status: `docs/dev/completed/20260414-14-phase-1-subject-verification-closure/STATUS.md`
- plan: `docs/dev/completed/20260414-15-phase-2-il2cpp-core-semantics-batch-a/plan-v1-01.md`

## 当前结论

- current_focus: 以 `SolutionCorePack` 为 owner subject，完成 Batch A 核心运行时语义的细粒度 proof / selective benchmark 收口。
- why_now: Phase 1 已经收口 retained subject 的 declared metadata、dashboard identity 与 verification 入口，Phase 2 得以直接把新 capability 接到稳定的 subject 主线上。
- success_definition:
  - Batch A 的 10 个 capability item 全部映射到独立 proof entry，并保留必要的 selective benchmark。
  - 旧 coarse proof 的测试契约全部迁移到新 Batch A 切片，不再把聚合器当作 declared proof。
  - `SolutionCorePack` 的 manifest、launcher、declared discovery 与 assert-based proof 入口保持一致。

## 最近摘要

- 2026-04-14 13:13:20 +08:00: 创建 Phase 2 child task，并产出 `batch-a-audit-v1-01.md`，确认 Batch A 10 个 capability item 的 owner slice 与缺口。
- 2026-04-14 14:07:36 +08:00: 新增并接入 `TaskAndValueTaskFlow`、`IteratorStateMachine`、`FinallyAndFilter`、`TaskScheduling`、`MonitorAndLocking`、`ThreadLocalState`、`RuntimeHelpers`、`GcSensitiveFlow`、`ResourceLifecycle`、`LoaderContract` 对应的 declared proof / benchmark。
- 2026-04-14 14:07:36 +08:00: 更新旧 compatibility tests，移除对 `AsyncAwaitProof.cs`、`ThreadingProof.cs` 与 `Program.cs` declared attribute 的历史假设。
- 2026-04-14 14:07:36 +08:00: 通过 `tests/integration/registry/test_declared_metadata_discovery.py`、`tests/unit/compatibility/test_async_await_proof_subject.py`、`tests/unit/compatibility/test_threading_gc_proof_subject.py`、`tests/unit/compatibility/test_mainline_feature_pack_onboarding.py`、`tests/unit/compatibility/test_subject_manifest_schema.py`。

## 下一步

- next_action: 返回父 roadmap，切入 `20260414-16-phase-3-low-level-and-interop-batch-b`。
- owner: codex
- trigger: Phase 2 已完成。

## 风险 / 阻塞

### risks

- 若后续继续把多个 capability 混回大而全的 coarse proof，declared metadata 会再次失去 level-2 capability 的可追踪性。

### blockers

- 当前无外部 blocker。

## wiki

- 本阶段仅在 `docs/dev` 沉淀执行痕迹，未单独更新 `wiki/`。
