---
task_id: 20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening
title: Phase 4D AotCoreIr Exception Dispatch And Abi Widening
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 14:50:48 +08:00
updated_at: 2026-04-15 17:40:00 +08:00
current_dir: docs/dev/completed/20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening
parent_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_status: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md`
- phase_4c_status: `docs/dev/completed/20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure/STATUS.md`
- brainstorm: `docs/dev/completed/20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening/design-v1-01.md`
- plan: `docs/dev/completed/20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening/plan-v1-01.md`

## 当前结论

- Phase 4D 已完成并归档，当前切片已经闭环：
  - `DispatchProofEntry` 的 dispatch widening 已闭环，`callvirt` / hidden `this` ABI / virtual type-id switch 已进入 `NativeAotLoweringPlanner`。
  - `AotCoreIr` 已正式携带 `AotCoreIrExceptionRegionKind : byte` 与 `AotCoreIrExceptionRegionArtifact`，`aot-core-ir.json` 不再只保留 `ExceptionRegionCount`。
  - `ExceptionCatchProofEntry` 已在 Native AOT 路径闭环，catch-only exception emission 保持在 planner 层完成。
  - `AotCoreIrAbiCarrierKind : byte`、`AotCoreIrAbiSlotArtifact`、`ReturnAbi`、`ParameterAbis` 已接入 `AotCoreIr`，`AbiWideningProofEntry` 已把 value-type by-value 参数与返回值链路打通。
  - `NativeAotLoweringPlanner` 现在消费 typed ABI carrier，而不是回退到 count/string 直推协议；`NativeAotEmitter` 仍然只走 `Templates/NativeAot*.scriban`。
- 本轮继续遵守用户确认过的边界：
  - `Chaos.TestFramework` 只保留在 `src/reference/Chaos.TestFramework`
  - 没有把测试框架逻辑混入 `AotCoreIr` / planner / emitter

## 最近摘要

- 2026-04-15 14:50:48 +08:00: 创建 `20260415-08`，承接父 roadmap 的 `Phase 4D`。
- 2026-04-15 16:34:00 +08:00: 完成 dispatch widening 闭环，`callvirt` / hidden `this` ABI / virtual type-id switch 进入 Native AOT planner。
- 2026-04-15 17:18:00 +08:00: 完成 exception lowering 合同闭环，`AotCoreIr` 正式输出 typed EH regions。
- 2026-04-15 17:40:00 +08:00: 完成 ABI widening 闭环，`AbiWideningProofEntry`、`AotCoreIrAbiSlotArtifact` 与 planner ABI-slot emission 全部转绿，Phase 4D 达到退出标准。

## 下一步

- next_action: 返回父 roadmap，创建并激活 `Phase 5: Feature Coverage Expansion On Owner Subjects`
- next_action_detail: 在 Phase 4B-4D contract 全部闭环后，把真实 capability gap 映射回 owner subjects，开始按 owner proof / support-state 收口
- owner: codex
- trigger: Phase 4D exit criteria 已满足，主线转入 Phase 5

## 风险 / 阻塞

### risks

- Phase 5 / 6 仍未开始；如果不继续把 capability ledger 映射回 owner subjects，当前 4D 的 contract 收口仍然只能停留在中层能力完成。
- `finally` / `filter` / `fault` 等更宽 EH 形态仍是后续切片，不应在没有 RED 的情况下混回当前已完成的 4D 收口。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4d_abi_widening_aot_core_ir.py tests/unit/compatibility/test_phase4d_abi_widening_native_aot.py -q`
  - 结果：`5 passed`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_exception_aot_core_ir.py tests/unit/compatibility/test_phase4d_exception_native_aot.py tests/unit/compatibility/test_phase4d_abi_widening_aot_core_ir.py tests/unit/compatibility/test_phase4d_abi_widening_native_aot.py -q`
  - 结果：`36 passed`
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
  - 结果：`Build succeeded`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果：`Build succeeded`

## wiki

- 当前继续只在 `docs/dev` 维护执行状态。
- 待 Phase 5 把 owner subject / support-state / capability gap 重新收口后，再评估是否沉淀到长期 architecture 文档。
