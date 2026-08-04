---
task_id: 20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure
title: Phase 4C AotCoreIr Generic Sharing And Metadata Closure
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 14:13:26 +08:00
updated_at: 2026-04-15 14:50:48 +08:00
current_dir: docs/dev/completed/20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure
parent_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_status: docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md
- parent_roadmap: docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md
- phase_4b_status: docs/dev/completed/20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface/STATUS.md
- phase_4_gap_ledger: docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/aot-core-ir-codegen-gap-ledger-v1-01.md
- brainstorm: docs/dev/completed/20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure/brainstorm-v1-01.md
- design: docs/dev/completed/20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure/design-v1-01.md
- plan: docs/dev/completed/20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure/plan-v1-01.md

## 当前结论

- Phase 4C 已完成并归档，当前切片已经闭环：
  - `ManagedClosureContracts` / `AotCoreIrLowering` / `MetadataWriterStage` 正式携带 `genericContext`
  - `AotCoreIrMethodArtifact` / `AotCoreIrReferenceArtifact` / supplemental metadata type+method template entry 已接入 typed generic carrier
  - `NativeAotLoweringPlanner` 已开始消费 `method.GenericContext` 与 `targetReference.GenericContext`，并通过 planner 输出审计注释
  - retained generic proof `GenericLayoutProofEntry` 已重塑为 Phase-4C-native-friendly 形状，并在 `emit-native-aot` 路径上闭环
- 本轮继续保持用户确认过的生成链结构：
  - `NativeAotLoweringPlanner`
  - `NativeAotEmitter`
  - `Templates/NativeAot*.scriban`
- `Chaos.TestFramework` 继续只作为 `src/reference/Chaos.TestFramework` 中的真实 managed dependency，没有混入 `AotCoreIr` / planner / emitter。
- Phase 4D 保留为下一阶段：
  - exception lowering
  - instance / virtual / interface / bridge dispatch widening
  - ABI widening

## 最近摘要

- 2026-04-15 14:13:26 +08:00: 创建 `20260415-07`，承接父 roadmap 的 `Phase 4C`。
- 2026-04-15 14:38:15 +08:00: 完成 4C 第一刀 RED -> GREEN，`genericContext` 正式进入 `AotCoreIr` 与 supplemental metadata template，并在 retained generic proofs 上回归通过。
- 2026-04-15 14:50:48 +08:00: 完成 4C 第二刀 RED -> GREEN，`NativeAotLoweringPlanner` 开始消费 `genericContext`，`GenericLayoutProofEntry` 在 native-aot 生成链上闭环通过；阶段收口完成。

## 下一步

- next_action: 返回父 roadmap，执行 `20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening`
- next_action_detail: 进入 exception / dispatch / ABI widening 的下一阶段切片，继续沿用 `NativeAotLoweringPlanner + NativeAotEmitter + Templates/NativeAot*.scriban`
- owner: codex
- trigger: Phase 4C exit criteria 已满足，主线转入 Phase 4D

## 风险 / 阻塞

### risks

- exception / dispatch / ABI widening 仍未闭环；如果 Phase 4D 不继续推进，`AotCoreIr` 仍会停留在“generic contract 已接入、但真实运行面仍偏窄”的状态。
- 当前 planner 对 `genericContext` 的消费仍是 Phase 4C 所需的最小审计边界；后续如果 dispatch widening 引入更复杂的 template model，仍要保持“planner 产出 model、emitter 只渲染”的分层。
- retained generic proof 已收口，但更宽的 generic dispatch / generic virtual / external runtime call 仍需在 Phase 4D 或 5 上继续扩大 proof 面。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py -q`
  - 结果：4 passed
- `python -m pytest tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_mainline_capability_batch_a.py -q`
  - 结果：7 passed
- `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py -q`
  - 结果：13 passed
- `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/compatibility/test_mainline_capability_batch_a.py -q`
  - 结果：19 passed
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
  - 结果：Build succeeded
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果：Build succeeded

## wiki

- 当前继续只在 `docs/dev` 维护执行状态。
- 待 Phase 4D 把 dispatch / exception / ABI widening contract 稳定后，再评估是否沉淀到长期 architecture 文档。
