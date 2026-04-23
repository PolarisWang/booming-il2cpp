# Shared Generic AOT IR Hard Cut 执行计划 v1.01

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:project-test-governance`、`dev:test-driven-development` 执行本计划。  
> 目标：把 `aot-core-ir.json` 从“只携带 generic context 附注”推进到“显式建模 open definition / shared body / thin stub / runtime context 的 authority 中心”，并继续向 planner/emitter 消费面推进。

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `native-generic-runtime-kernel`
- capabilityItem: `shared-generic-aot-ir-hard-cut`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `false`
- hotupdateImpact: `Proof`
- formalVerificationObjects:
  - `aot-core-ir generic contracts`
  - `generic layout proof`
  - `native planner authority review`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

## 功能需求 Checklist

- [ ] AOT IR 显式建模 open definition
- [ ] AOT IR 显式建模 `SharedGenericBodyId`
- [ ] AOT IR 显式建模 `InstantiationStubId`
- [ ] AOT IR 保留 `RuntimeGenericContextArtifact`
- [ ] call site / planner 能消费 body/stub/context 语义，而不是只盯着闭包 `TargetSymbol`
- [ ] IR 保留 support / specialization 观察面

## 文件审计 Checklist

- [ ] `src/managed/Chaos.IL2CPP.Contracts/TypedIlAndAotCoreIrContracts.cs`
- [ ] `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- [ ] `src/managed/Chaos.IL2CPP.CodeGen/Planning/NativeAotLoweringPlanner.InvocationPlanning.cs`
- [ ] `src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.InvocationAbi.cs`
- [ ] `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs`

## 架构审视 Checklist

- [ ] AOT IR 对 generic execution authority 只有一套显式表达
- [ ] method/reference/call-site 不再把闭包 subject/symbol 当成唯一 authority
- [ ] `RuntimeGenericContextArtifact` 只承担 runtime status/context，不重新吞回所有 authority

## 生成产物审视 Checklist

- [ ] `aot-core-ir.json` 可直接 diff 出 open definition / body / stub
- [ ] generic layout 等 proof 能观察到顶层 authority 字段
- [ ] native planner/comment/summary 不再只把 shared generic 语义藏在注释里

## 验证 Checklist

- [ ] RED: contracts/source tests 先失败
- [ ] GREEN: `test_phase4c_aot_core_ir_generics.py`
- [ ] GREEN: `test_phase4c_generic_layout_aot_core_ir.py`
- [ ] GREEN: `test_loader_instantiation_demand_graph_aot_core_ir_authority_contract.py`
- [ ] GREEN: 受影响 `dotnet build` 通过
