---
task_id: 20260414-16-phase-3-low-level-and-interop-batch-b
title: Phase 3 Low-Level And Interop Batch B
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 14:07:36 +08:00
updated_at: 2026-04-14 14:07:36 +08:00
current_dir: docs/dev/completed/20260414-16-phase-3-low-level-and-interop-batch-b
parent_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_design: `docs/dev/completed/20260414-12-il2cpp-core-capability-development-roadmap/design-v1-01.md`
- parent_roadmap: `docs/dev/completed/20260414-12-il2cpp-core-capability-development-roadmap/roadmap-v1-01.md`
- phase_0_coverage_ledger: `docs/dev/completed/20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze/coverage-ledger-freeze-v1-01.md`
- phase_2_status: `docs/dev/completed/20260414-15-phase-2-il2cpp-core-semantics-batch-a/STATUS.md`

## 当前结论

- current_focus: 为 `SolutionCorePack` 补齐 Phase 3 的低层 runtime / interop capability proof 与 selective benchmark。
- why_now: Phase 2 已经把 Batch A 的 declared entry 粒度收口，Phase 3 可以沿同一条 metadata / discovery / launcher / benchmark 链路继续扩 capability。
- success_definition:
  - `SpanAndMemory`、`RefStructAndStackalloc`、`UnsafePointer`、`FunctionPointer`、`NativeCallInterop`、`StructMarshaling`、`DelegateCallbackInterop` 全部进入 declared catalog。
  - 既有 `StringAndUtf8Marshaling` proof 继续保留，并补齐 benchmark 入口。
  - 新条目不引入新的 subject root，也不扩大 string 调度面。

## 最近摘要

- 2026-04-14 14:07:36 +08:00: 在 `InteropAndMarshaling` / `ObjectModelAndDispatch` 下新增 `SpanAndMemoryProofEntry`、`RefStructAndStackallocProofEntry`、`UnsafePointerProofEntry`、`FunctionPointerProofEntry`、`NativeCallInteropProofEntry`、`StructMarshalingProofEntry`、`DelegateCallbackInteropProofEntry`。
- 2026-04-14 14:07:36 +08:00: 在 `CoreRuntimeBenchmarks` 下新增 `SpanAndMemoryBenchmarkEntry`、`UnsafePointerBenchmarkEntry`、`FunctionPointerBenchmarkEntry`、`NativeCallInteropBenchmarkEntry`、`StringAndUtf8MarshalingBenchmarkEntry`、`StructMarshalingBenchmarkEntry`、`DelegateCallbackInteropBenchmarkEntry`。
- 2026-04-14 14:07:36 +08:00: 通过 declared discovery、benchmark source 与 interop compatibility 回归，确认 source layout、catalog 和 retained subject 入口未回退。

## 下一步

- next_action: 返回父 roadmap，切入 `20260414-17-phase-4-metadata-and-aot-closure-batch-c`。
- owner: codex
- trigger: Phase 3 已完成。

## 风险 / 阻塞

### risks

- 后续若再把 interop / low-level capability 混回 `MarshalingProof` 这类 coarse proof，会破坏 level-2 capability 的长期可追踪性。

### blockers

- 当前无外部 blocker。

## wiki

- 本阶段仅在 `docs/dev` 记录执行结论。
