---
task_id: 20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface
title: Phase 4B AotCoreIr Object Model And Runtime Surface
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 07:42:00 +08:00
updated_at: 2026-04-15 14:13:26 +08:00
current_dir: docs/dev/completed/20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface
parent_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_status: docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md
- parent_roadmap: docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md
- phase_4a_status: docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/STATUS.md
- phase_4_gap_ledger: docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/aot-core-ir-codegen-gap-ledger-v1-01.md
- brainstorm: docs/dev/completed/20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface/brainstorm-v1-01.md
- design: docs/dev/completed/20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface/design-v1-01.md
- plan: docs/dev/completed/20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface/plan-v1-01.md

## 当前结论

- Phase 4B 已完成并归档，当前切片已经闭环：
  - object allocation + instance/static field load/store
  - array allocation + `ldlen` + `ldelem*` + `stelem*`
  - array-element byref `ldelema + ldind.i4 + stind.i4`
  - boxing + `unbox` + `unbox.any`
  - class/interface `castclass` / `isinst`
  - `ldloca` + `initobj` + minimal managed-pointer value-type slice
  - minimal reference-return direct-call ABI
- 本轮已按用户确认的方案 3 完成 Native AOT 生成链重构：
  - `NativeAotLoweringPlanner`
  - `NativeAotEmitter`
  - `Templates/NativeAot.TranslationUnit.cpp.scriban`
  - `Templates/NativeAot.ObjectModel.cpp.scriban`
  - `Templates/NativeAot.Method.cpp.scriban`
- `AotCoreIrLowering` 已继续把 array element carrier 送入 `AotCoreIr`，array covariance proof 在新生成链上闭环。
- `Chaos.TestFramework` 继续保持为真实 managed dependency，没有混入 `AotCoreIr` / planner / emitter。
- 后续所有 Native AOT 子任务都必须沿用 `NativeAotLoweringPlanner + NativeAotEmitter + Templates/NativeAot*.scriban`，不允许回退为单体 emitter patch。

## 最近摘要

- 2026-04-15 07:42:00 +08:00: 创建 `20260415-06`，承接 roadmap 的 `Phase 4B`，聚焦 object model / runtime surface。
- 2026-04-15 10:03:35 +08:00: 完成首批 object/runtime carrier contract + lowering，覆盖 `newobj / field / newarr / castclass / isinst / box / unbox`。
- 2026-04-15 11:58:14 +08:00: 完成 class inheritance + interface cast/type-check slice，阶段回归提升到 `22 passed`。
- 2026-04-15 12:48:00 +08:00: 完成 array-element byref slice，`ArrayOpsProofEntry` 覆盖 `ldelema + ldind.i4 + stind.i4`。
- 2026-04-15 13:45:00 +08:00: 用户确认当前轮必须把 Native AOT 生成链迁到 `NativeAotLoweringPlanner + NativeAotEmitter + Scriban templates`，并选择方案 3。
- 2026-04-15 13:53:24 +08:00: 完成 planner/emitter/templates 迁移、array covariance 对齐与阶段回归验证。
- 2026-04-15 14:13:26 +08:00: 完成 Phase 4B closeout audit，确认退出条件满足，切换到 `20260415-07 Phase 4C`。

## 下一步

- next_action: 返回父 roadmap，执行 `20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure`
- next_action_detail: 进入 `generic context / instantiation / sharing contract / metadata closure` 的正式 contract 与 lowering 闭环，并继续沿用 planner + emitter + Scriban 结构
- owner: codex
- trigger: Phase 4B exit criteria 已满足；主线转入 Phase 4C

## 风险 / 阻塞

### risks

- generic sharing 与 metadata closure 仍未成为 `AotCoreIr` 正式 contract；如果不继续推进，owner subject 的 generic 能力仍会停留在 gap 记录。
- `NativeAotLoweringPlanner` 当前已摆脱单体 emitter，但更宽的 generic/ABI/disptach widening 仍可能要求继续细化 template model。
- array covariance 已闭环，但 array + interface + generic 组合一旦扩展，仍需重新审视 carrier 是否足够。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py -q`
  - 结果：6 passed
- `python -m pytest tests/unit/compatibility/test_phase4b_array_covariance_native_aot.py -q`
  - 结果：1 passed
- `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4b_object_field_native_aot.py tests/unit/compatibility/test_phase4b_array_boxing_native_aot.py tests/unit/compatibility/test_phase4b_array_ops_native_aot.py tests/unit/compatibility/test_phase4b_cast_type_check_native_aot.py tests/unit/compatibility/test_phase4b_boxed_struct_field_read_native_aot.py tests/unit/compatibility/test_phase4b_object_return_native_aot.py tests/unit/compatibility/test_phase4b_array_covariance_native_aot.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_mainline_capability_batch_a.py -q`
  - 结果：26 passed
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
  - 结果：Build succeeded
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果：Build succeeded

## wiki

- 当前继续只在 `docs/dev` 维护执行痕迹。
- 等 Phase 4C / 4D 把 `AotCoreIr` 与 Native AOT planner/emitter 边界进一步稳定后，再评估是否沉淀到长期 architecture 文档。
