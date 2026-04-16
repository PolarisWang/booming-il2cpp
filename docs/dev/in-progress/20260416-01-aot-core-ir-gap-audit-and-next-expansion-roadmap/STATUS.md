---
task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
title: AOT Core IR Gap Audit And Next Expansion Roadmap
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-16 08:54:25 +08:00
updated_at: 2026-04-16 13:07:19 +08:00
current_dir: docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
parent_task_id:
source_task_id: 20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap
source_relation: follow-up-on-remaining-aot-core-ir-capability-gaps
active: true
---

## 关键文档

- source_roadmap: `docs/dev/completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/roadmap-v1-01.md`
- source_status: `docs/dev/completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/STATUS.md`
- previous_core_roadmap: `docs/dev/completed/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/roadmap-v1-01.md`
- brainstorm: `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`

## 当前结论

- `20260416-02` 已完成 capability-first gap ledger 与 owner evidence matrix 冻结。
- `20260416-03` 与 `20260416-09` 已完成 `Phase 1` loader/typed-il 输入收口。
- `20260416-04` 已完成 `Phase 2` 的 `ldtoken + Int64/UInt64 carrier` 收口。
- `20260416-05` 仍在执行，但已额外收口一条新的 generic dispatch capability chain：
  - 修复 `callvirt` direct-vs-virtual 误判
  - 修复 generic constraint interface slot 闭包
  - 增补 planner `ldarga` 最小 lowering
  - 回补 linker interface dispatch implementation retention
  - 将 `GenericConstraintProofEntry::Run()` 推进到可完整 `emit-native-aot`
- 聚合 owner audit 现已推进到新的 capability family：`AsyncTaskMethodBuilder<int>::Create()` external-runtime direct-call target

## 最近摘要

- 2026-04-16 08:54:25 +08:00: 完成新一轮源码审计，确认 `LoaderStage / AotCoreIr / NativeAotLoweringPlanner / NativeAotEmitter` 仍存在显著 unsupported 边界，需要新 roadmap 收口。
- 2026-04-16 09:09:27 +08:00: `20260416-02-phase-0-gap-ledger-and-owner-evidence-audit` 已完成；代表性 compatibility 验证结果为 `28 passed`。
- 2026-04-16 09:31:14 +08:00: `20260416-03-phase-1-loader-and-typed-il-input-closure` 已完成第一批 loader widening；局部回归结果为 `8 passed`。
- 2026-04-16 09:43:20 +08:00: `20260416-09-phase-1b-member-reference-parent-and-vararg-loader-closure` 已完成第二批 loader widening；局部回归结果为 `10 passed`。
- 2026-04-16 10:13:21 +08:00: `20260416-04` 完成 batch A `ldtoken targetReference` 收口；局部回归结果为 `19 passed`。
- 2026-04-16 10:36:50 +08:00: `20260416-04` 完成 batch B/C `Int64 + UInt64` ABI carrier widening，并将相邻 compatibility 回归压到 `30 passed`；主线切入 `20260416-05`。
- 2026-04-16 11:19:03 +08:00: `20260416-05` 完成 `not + beq/bgt/ble/bge/bge.un` widening，同时为 branch proof 回补 `LoaderStage` 的 `Bge_un / Bge_un_s` decode。
- 2026-04-16 13:07:19 +08:00: `20260416-05` 完成 generic constraint dispatch 收口，并将 aggregate `ProofEntry::Run()` 推进到 async/state-machine external-runtime helper blocker。

## 下一步

- next_action: 继续沿 `20260416-05` 审计 async/state-machine external-runtime helper 这条新暴露的 capability family，先冻结 `AsyncTaskMethodBuilder<int>::Create()` failing surface，再决定是否需要扩展当前 child task 或拆出新的 child task。
- owner: codex
- trigger: 用户要求持续推进 AOT core gap 收口主线

## 风险 / 阻塞

### risks

- 如果不继续按 owner-driven aggregate audit 推进，主线会再次退化成按异常文本逐条补洞。
- generic constraint 这轮已经证明：真实 blocker 常常跨 loader / linker / planner 多层联动；后续 async family 也很可能如此。
- 当前 parent roadmap 的 phase 拆分尚未显式覆盖 async external-runtime helper 这条新链；如果证据继续累积，需要及时把它映射到独立 child task。

### blockers

- 当前主 blocker：`System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Int32>::Create()`
- 说明：`CoreRuntimeFeatures/ProofEntry::Run()` 的 aggregate native-aot 仍失败，但失败面已经从 generic-dispatch family 推进到 async/state-machine family

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4_generic_constraint_dispatch_native_aot.py -q`
  - 结果：`2 passed`
- `python -m pytest tests/unit/compatibility/test_phase4_generic_constraint_dispatch_native_aot.py tests/unit/compatibility/test_phase4_generic_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_phase3_direct_callvirt_native_aot.py -q`
  - 结果：`11 passed`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_phase2_aot_core_ir_ldtoken_target_reference.py -q`
  - 结果：`12 passed`

## wiki

- 暂不写入 wiki；待 async/state-machine family 的 stage impact 与长期模式稳定后再决定是否沉淀
