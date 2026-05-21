# 20260421-03 Canonical Generic Contracts And Parser Unification

## 当前状态

- 状态: completed
- 当前阶段: Step 4 GREEN
- 当前目标: `20260421-03` 已收口，等待父 roadmap 继续推进 `20260421-04`

## 当前结论

- contracts/parser authority 已切到：
  - `GenericInstantiationKey`
  - `SharedGenericBodyId`
  - `InstantiationStubId`
  - `RuntimeGenericContextArtifact`
  - `GenericDiagnosticArtifact`
- `ManagedNaming` 现在由单一 generic parser helper 驱动三类产物：
  - semantic identity
  - runtime generic context
  - diagnostic artifact
- `AotCoreIr`、supplemental metadata template、native-aot 注释消费面都已移除旧的 execution-facing `GenericContextArtifact`

## 本轮执行记录

- 2026-04-22: 建立子任务目录、执行计划与状态文件。
- 2026-04-22: 完成 contracts / tests / consumer 搜索，确认旧 `GenericContextArtifact` 仍贯穿 source tests、AOT IR、metadata template、native-aot comment 消费面。
- 2026-04-22: 先补 RED，更新 phase4c / phase4d / phase7 contracts/tests 到新 runtime/diagnostic generic shape。
- 2026-04-22: 完成 contracts 改造：
  - `ManagedClosureModels.cs`
  - `ManagedClosureArtifactModels.cs`
  - `ManagedNaming.cs`
  - `TypedIlAndAotCoreIrContracts.cs`
- 2026-04-22: 完成直接消费者改造：
  - `AotCoreIrLowering.cs`
  - `MetadataWriterStage.cs`
  - `NativeAotLoweringPlanner.cs`
  - `NativeAotLoweringPlanner.MethodEmission.cs`
  - `NativeAotLoweringPlanner.ObjectModelUtilities.cs`
  - `NativeAotLoweringPlanner.MetadataSupport.cs`
- 2026-04-22: 定向验证通过：
  - `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - `python -m pytest tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py -q`

## 下一步

- 创建并推进 `20260421-04-loader-instantiation-graph-hard-cut`
- 审计 loader 里 clone/materialization 仍作为主路径的残余点
- 先补 RED，把 instantiation demand graph 的 contract 和输出 shape 锁住

## 风险

- `SharedGenericBodyId` / `InstantiationStubId` 当前仍是稳定命名落地，真正 shared-body 语义要在 `20260421-05` 再硬切。
- loader 仍保留 `GenericMaterialization` 主路径，下一阶段如果 contract/loader 之间 authority 没切干净，会重新把 clone-first 语义带回 IR。
