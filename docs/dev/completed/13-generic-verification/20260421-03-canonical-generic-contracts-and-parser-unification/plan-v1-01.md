# Canonical Generic Contracts And Parser Unification 执行计划 v1.01

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:project-test-governance`、`dev:test-driven-development` 执行本计划。
> 目标：把 generic authority 从旧的 `GenericContextArtifact` 硬切到 `GenericInstantiationKey` / `RuntimeGenericContextArtifact` / `GenericDiagnosticArtifact`，并统一 `ManagedNaming` 里的 generic parser authority。

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `native-generic-runtime-kernel`
- capabilityItem: `canonical-generic-contracts-and-parser-unification`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `false`
- hotupdateImpact: `Proof`
- formalVerificationObjects:
  - `contracts parser source tests`
  - `aot-core-ir generic contract tests`
  - `metadata closure generic contract tests`
  - `generated aot-core-ir review`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

## 功能需求 Checklist

- [ ] 固化单一 generic parser authority
- [ ] 引入 `GenericInstantiationKey`
- [ ] 引入 `SharedGenericBodyId`
- [ ] 引入 `InstantiationStubId`
- [ ] 引入 `RuntimeGenericContextArtifact`
- [ ] 引入 `GenericDiagnosticArtifact`
- [ ] 从执行面 contract 移除 `GenericContextArtifact`
- [ ] 保留 `subjectId` 仅作诊断/UX surface
- [ ] 在 contract 中带出 matrix / specialization / reason-code 面向字段

## 文件审计 Checklist

- [ ] `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureModels.cs`
- [ ] `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs`
- [ ] `src/managed/Chaos.IL2CPP.Contracts/ManagedNaming.cs`
- [ ] `src/managed/Chaos.IL2CPP.Contracts/TypedIlAndAotCoreIrContracts.cs`
- [ ] `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- [ ] `src/managed/Chaos.IL2CPP.MetadataWriter/MetadataWriterStage.cs`
- [ ] `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- [ ] `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs`

## 架构审查 Checklist

- [ ] parser authority 只有一处
- [ ] execution contract 与 diagnostic contract 分离
- [ ] 没有新的 contract 把 `subjectId` 重新变成执行 authority
- [ ] body/stub/context id 的命名稳定且可 diff

## 生成代码审查 Checklist

- [ ] `aot-core-ir.json` 不再输出 `genericContext`
- [ ] `supplemental-metadata-template.json` 不再输出 `genericContext`
- [ ] native-aot 注释/消费面切到 `runtimeGenericContext`
- [ ] `SharedGenericBodyId` / `InstantiationStubId` 字段存在且命名稳定

## 验证 Checklist

- [ ] RED: source-based contracts/parser tests 先失败
- [ ] GREEN: source-based contracts/parser tests 通过
- [ ] GREEN: `test_phase4c_aot_core_ir_generics.py`
- [ ] GREEN: `test_phase4c_generic_layout_aot_core_ir.py`
- [ ] GREEN: `test_phase4c_metadata_closure_contract.py`
- [ ] GREEN: `test_phase4c_generic_layout_native_aot.py`
- [ ] GREEN: `test_phase4d_dispatch_aot_core_ir.py`
- [ ] GREEN: `test_phase7_loader_generic_method_reference_decode.py`
