# 20260421-04 Loader Instantiation Graph Hard Cut

## 当前状态
- 状态: `completed`
- 当前阶段: `Step 4 GREEN`
- 当前目标: `20260421-04` 已收口，交接给 `20260421-05`

## 当前结论

- loader 已完成 demand-graph-first 主链切换：
  - 引入并稳定透传 `GenericInstantiationDemandGraph`
  - cross-assembly generic demand 变成显式 authority
  - family taxonomy 固化为 `ClosedGenericType` / `ClosedMethodOnGenericType` / `ClosedGenericMethod` / `ClosedGenericMethodOnGenericType`
- managed closure 已正式落盘 `generic-instantiation-demand-graph.json`，`closure.manifest.json` 和 analysis frontend manifest 都显式注册该产物。
- `AotCoreIrLowering` 与 `MetadataWriterStage` 现在都优先消费 loader demand graph 作为 generic authority，不再把 `ManagedNaming.TryCreateRuntimeGenericContext(...)` 当成首选来源。
- `aot-core-ir.json` 现在把以下字段显式提升为顶层 authority 字段，而不是只藏在 `RuntimeGenericContextArtifact` 内部：
  - `openDefinitionSubjectId`
  - `sharedGenericBodyId`
  - `instantiationStubId`
- `GenericCompatibilityProjection` 仍然保留为 loader 内部兼容投影，用来维持当前 `LinkedWorldModel` 形状；它已经不再是 downstream generic authority，最终清理留给 `20260421-08`.

## 本轮修改文件

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs`
- `src/managed/Chaos.IL2CPP.Contracts/TypedIlAndAotCoreIrContracts.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `src/managed/Chaos.IL2CPP.MetadataWriter/MetadataWriterStage.cs`
- `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
- `build/toolchains/run/testing/subject_workers.py`
- `tests/unit/compatibility/test_loader_instantiation_demand_graph_managed_closure_artifact.py`
- `tests/unit/compatibility/test_loader_instantiation_demand_graph_aot_core_ir_authority_contract.py`
- `tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py`
- `tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py`
- `tests/unit/compatibility/test_phase4c_metadata_closure_contract.py`
- `tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py`
- `tests/unit/execution/test_subject_workers_frontend_codegen_analysis.py`

## 架构审视

- loader 输出 authority 已经从 clone/materialization-first 切到 demand-graph-first。
- downstream 现在优先消费 demand graph，而不是靠闭包字符串推断 generic authority。
- 兼容投影仍存在，但它只承担旧模型形状兼容，不再承担 runtime authority。

## 生成产物审视

- `generic-instantiation-demand-graph.json` 可稳定落盘、可 diff、可被 AOT IR 与 metadata 消费。
- `aot-core-ir.json` 已能独立观察 open definition / shared body / instantiation stub。
- generic layout proof 证明 method/type/field reference 的 authority 字段与 `RuntimeGenericContext` 保持一致。

## 验证

- `python -m pytest tests/unit/compatibility/test_loader_instantiation_demand_graph_contract.py tests/unit/compatibility/test_loader_instantiation_demand_graph_pipeline_contract.py tests/unit/compatibility/test_loader_instantiation_demand_graph_managed_closure_artifact.py tests/unit/compatibility/test_loader_instantiation_demand_graph_aot_core_ir_authority_contract.py tests/unit/compatibility/test_full_assembly_closure_without_entry_point.py tests/unit/compatibility/test_managed_closure_contract_bundle.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py tests/unit/compatibility/test_core_contract_cutover.py tests/unit/compatibility/test_linker_stripping_proof_subject.py tests/unit/compatibility/test_il2cpp_codegen_structure_governance_pipeline_contracts.py tests/unit/execution/test_subject_workers_frontend_codegen_analysis.py -q`
  - 结果: 通过
- `python -m pytest tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py -q`
  - 结果: `3 passed`
- `python -m pytest tests/unit/compatibility/test_loader_instantiation_demand_graph_aot_core_ir_authority_contract.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py -q`
  - 结果: `7 passed`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果: `0 Warning(s), 0 Error(s)`

## 下一步

- 启动 `20260421-05-shared-generic-aot-ir-hard-cut`
- 把 planner/emitter 从 `TargetSymbol` 闭包直连继续切到显式 `openDefinition/sharedBody/stub/context` authority
- 为后续 `20260421-06` 的 generic matrix / gate 提供稳定 IR 观察面

## 残余风险

- `NativeAotLoweringPlanner` 仍以闭包 `TargetSymbol` 作为最终直接调用入口，shared-body/thin-stub 语义还没有真正落到 planner/emitter。
- `GenericCompatibilityProjection` 仍在 loader 内部存在，虽然已经不再是 authority，但最终删除要在 `20260421-08` 完成。
