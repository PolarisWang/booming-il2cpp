# Generic Matrix And IR Gates 执行计划 v1.01

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:project-test-governance`、`dev:test-driven-development` 执行本计划。
> 目标：把 `generic-capability-matrix.json` 落成正式 sidecar，先把 generic family budget、authority summary、canonical HotUpdate naming、unsupported leak gate 变成 managed closure 的固定产物，再用它作为后续 async family / dispatch / hotupdate cutover 的 gate 基线。

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `native-generic-runtime-kernel`
- capabilityItem: `generic-matrix-and-ir-gates`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `Proof`
- formalVerificationObjects:
  - `generic matrix`
  - `aot-core-ir generic contracts`
  - `generated code review`
  - `native generic proofs`
  - `hotupdate generic boundary proofs`
  - `generated cpp size/memory benchmark`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`
  - `native-host`
  - `hotupdate-host`

## 功能需求 Checklist

- [x] managed closure 产出正式 `generic-capability-matrix.json`
- [x] matrix sidecar 聚合 loader demand graph 与 AOT IR authority
- [x] sidecar 编码 canonical HotUpdate naming
  - `HotUpdateNone`
  - `HotUpdateMetadataAugmented`
  - `HotUpdateGenericSharing`
- [x] sidecar 编码 per-family budget
- [x] sidecar 编码 per-demand authority summary
- [x] sidecar 编码 gate status
  - `unsupportedLeakCount`
  - `missingAuthorityCount`
  - `nonCanonicalHotUpdateNameCount`
- [x] 把 generated cpp size/symbol/peak-memory baseline 接入 budget/gate
- [x] 把 dispatch/reflection/hotupdate proof 逐项绑定到 matrix case

## 文件审计 Checklist

- [x] `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- [x] `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureModels.cs`
- [x] `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs`
- [x] `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
- [x] `src/managed/Chaos.IL2CPP.CodeGen/GenericCapabilityMatrixBuilder.cs`
- [x] `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
- [x] `build/toolchains/run/testing/subject_workers.py`
- [x] `build/toolchains/run/testing/subject_reporting.py`
- [x] `tests/unit/compatibility/test_phase4c_generic_matrix_gates.py`
- [x] `tests/unit/compatibility/test_managed_closure_contract_bundle.py`
- [x] owner proof assets / benchmark baseline ingestion files

## 架构审视 Checklist

- [x] matrix authority 以 loader demand graph 为 canonical source
- [x] AOT IR authority 以 observation 形式并入 matrix，而不是重新成为独立 authority
- [x] HotUpdate mode naming 不含外部产品名
- [x] full-assembly-closure / zero-demand bundle 也能稳定产出 zero-state matrix
- [x] dispatch/reflection/hotupdate case 已经逐项落成 formal matrix row
- [x] benchmark baseline 已经成为 gate，而不是备注信息

## 生成产物审视 Checklist

- [x] `generic-capability-matrix.json` 能直接看到：
  - owner subject
  - entry subject
  - hotupdate modes
  - family budgets
  - per-demand authority
  - gate status
- [x] no-generic bundle 产出空 `entries`
- [x] subject worker manifest 暴露 `genericCapabilityMatrixPath`
- [x] matrix case 与 native/hotupdate proof artifact 做正式关联
- [x] size/memory baseline sidecar 纳入同一套 gate summary

## 验证 Checklist

- [x] RED: `tests/unit/compatibility/test_phase4c_generic_matrix_gates.py`
- [x] GREEN: `tests/unit/compatibility/test_phase4c_generic_matrix_gates.py`
- [x] GREEN: `tests/unit/compatibility/test_managed_closure_contract_bundle.py`
- [x] GREEN: `tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py`
- [x] GREEN: `tests/unit/compatibility/test_loader_instantiation_demand_graph_managed_closure_artifact.py`
- [x] GREEN: `tests/unit/compatibility/test_loader_instantiation_demand_graph_aot_core_ir_authority_contract.py`
- [x] GREEN: `tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py`
- [x] GREEN: `tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py`
- [x] GREEN: `tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py`
- [x] GREEN: `tests/unit/compatibility/test_phase4c_metadata_closure_contract.py`
- [x] GREEN: `tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py`
- [x] GREEN: `tests/unit/reporting/test_subject_reporting_matrix_proof_linkage.py`
- [x] GREEN: `tests/unit/reporting/test_subject_reporting_codegen_metrics.py`
- [x] GREEN: `tests/unit/reporting/test_subject_reporting_artifacts.py`
- [x] GREEN: `tests/unit/reporting/test_subject_reporting_summary.py`
- [x] GREEN: `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
- [x] GREEN: benchmark baseline gate

## 当前边界

- 本计划已完成：
  - `boundaryCases truth chain`
  - native `codegen metric gate`
  - `generic-matrix-proof-linkage.json`
  - `subject summary` 的 codegen/linkage 上卷
- 后续如需更高层可视化，再继续把这些 summary 字段接入 dashboard。
