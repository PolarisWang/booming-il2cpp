---
task_id: 20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization
title: Phase 3 NativeAot Planner And Entry ABI Generalization
task_type: plan
lifecycle_status: in_progress
phase: executing
created_at: 2026-04-16 10:36:50 +08:00
updated_at: 2026-04-16 13:07:19 +08:00
current_dir: docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization
parent_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_relation: roadmap-child-phase-3
design_doc: docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/design-v1-01.md
plan_doc: docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/plan-v1-01.md
total_tasks: 5
current_task: batch-f-async-external-runtime-owner-audit
active: true
---

## 关键文档

- parent_status: `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`
- design: `docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/design-v1-01.md`
- plan: `docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/plan-v1-01.md`

## 当前结论

- `Phase 3` 既有 batch A-D 仍已完成：
  - batch A: `ldc.i8 + and / or / xor`
  - batch B: `shl / shr / shr.un`
  - batch C: `not`
  - batch D: `beq / bgt / ble / bge / bge.un`
- 本轮继续沿 `SolutionCorePack` owner-driven audit 前进，完成了 generic-constraint dispatch 这条真实 failing surface 的收口：
  - loader generic instantiation 不再把 placeholder 泛型实例折叠回 open definition
  - `GenericConstraintProofEntry::FormatValue<...>` 的 `callvirt` 已闭包到 `IGenericConstraintFormatter<GenericConstraintValue>::Format(GenericConstraintValue)`
  - planner 已补齐 `ldarga` 的最小 lowering，使 reference-like 参数地址可在调用点标准化回真实 instance
  - linker 已补齐 interface dispatch implementation retention，使 reachable interface slot 会把已达 concrete type 的实现方法一起带入 closure
  - owner proof 已移除字符串插值噪音，避免 `DefaultInterpolatedStringHandler` 抢占 aggregate 审计面
- `GenericConstraintProofEntry::Run()` 现在可以完整 `emit-native-aot`
- 聚合 `CoreRuntimeFeatures/ProofEntry::Run()` 的下一层真实 blocker 已变成 async/state-machine family：
  - `System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Int32>::Create()`
  - 这已经不再是 generic-dispatch / `ldarga` / interface-retention 问题

## 最近摘要

- 2026-04-16 10:36:50 +08:00: `20260416-04` 归档完成，正式激活 `20260416-05`，第一批锁定 `ldc.i8 + bitwise` planner legalization。
- 2026-04-16 11:00:55 +08:00: 完成 `ldc.i8 + and/or/xor + shl/shr/shr.un` 两批 widening，并新增 `ShiftOpsBenchmark` focused evidence。
- 2026-04-16 11:19:03 +08:00: 完成 `not + beq/bgt/ble/bge/bge.un` widening，并为 branch proof 回补 `LoaderStage` 的 `Bge_un / Bge_un_s` decode。
- 2026-04-16 13:07:19 +08:00: 完成 generic constraint dispatch 收口：新增 focused RED test，修复 generic slot 闭包、planner `ldarga`、linker dispatch retention，并将 aggregate `ProofEntry::Run()` 推进到新的 async external-runtime blocker。

## 下一步

- next_action: 继续沿 `ProofEntry::Run()` 审计 async/state-machine family；优先冻结 `AsyncTaskMethodBuilder<int>::Create()` external-runtime direct-call failing surface，再判断应做 owner 纯化、planner bridge 还是新的 capability child task。
- owner: codex
- trigger: 用户要求继续推进当前 AOT core roadmap，直到主线收口或出现需要升级成新 child task 的明确 blocker

## 风险 / 阻塞

### risks

- 如果继续把 async external-runtime helper 问题硬塞进 generic / dispatch 这条链，会重新混淆 capability family 边界。
- `ldarga` 当前 widening 只覆盖本轮 owner surface 已验证的 `NativeInt / ValueTypeByValue` 参数地址路径；后续若出现更多 carrier，需要继续按证据扩展，而不是提前泛化。
- linker 新增的 interface dispatch retention 依赖“reachable interface slot + reachable concrete type”共同出现；后续若 owner surface 涉及更复杂的继承/override 组合，还需要继续审计。

### blockers

- 当前 aggregate blocker：`System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<System.Int32>::Create()`
- 现状：`emit-native-aot` 对 `CoreRuntimeFeatures/ProofEntry::Run()` 仍失败，失败点已稳定落在 async/state-machine external-runtime helper

## 验证

- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果：成功；存在既有 nullable warning
- `dotnet build subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/CoreRuntimeFeatures.csproj -c Release`
  - 结果：成功；存在 subject 既有 nullable warning
- `python -m pytest tests/unit/compatibility/test_phase4_generic_constraint_dispatch_native_aot.py -q`
  - 结果：`2 passed`
- `python -m pytest tests/unit/compatibility/test_phase4_generic_constraint_dispatch_native_aot.py tests/unit/compatibility/test_phase4_generic_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_phase3_direct_callvirt_native_aot.py -q`
  - 结果：`11 passed`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_phase2_aot_core_ir_ldtoken_target_reference.py -q`
  - 结果：`12 passed`
- `dotnet ... CoreRuntimeFeatures.dll ... --entry-point-subject-id CoreRuntimeFeatures/ProofEntry::Run()`
  - 结果：bundle 生成成功
- `dotnet ... emit-native-aot <bundle> <emit>`
  - 结果：失败；新 blocker 为 `AsyncTaskMethodBuilder<System.Int32>::Create()`

## wiki

- 暂不写入 wiki；待 async / state-machine / external-runtime helper 的能力边界固定后，再决定是否沉淀成长期指引
