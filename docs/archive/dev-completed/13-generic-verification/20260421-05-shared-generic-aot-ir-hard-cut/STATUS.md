# 20260421-05 Shared Generic AOT IR Hard Cut

## 当前状态
- 状态: `completed`
- 完成阶段: `Step 2 Execution Authority Hard Cut`
- 完成定义: generic direct invocation / `ldftn` / function-pointer target 已从闭包 body symbol authority 切到 `InstantiationStubId -> stub symbol`

## 当前结论

- `AotCoreIrMethodArtifact` / `AotCoreIrReferenceArtifact` 已稳定携带：
  - `openDefinitionSubjectId`
  - `sharedGenericBodyId`
  - `instantiationStubId`
  - `runtimeGenericContext`
- native planner / emitter / invocation ABI 已不再把 closed body `NativeSymbol` 当成 generic execution 的唯一 authority。
- `NativeAotLoweringPlanner` 已为 lowerable generic method 生成 thin stub，并把 direct call-site / `ldftn` / function-pointer 解析统一切到 stub symbol。
- 这一刀的完成边界是 execution authority hard cut，不包含 shared body 物理去重；body 仍保留原 `NativeSymbol`，stub 负责转发。

## 本轮修改文件

- `src/managed/Chaos.IL2CPP.Contracts/ManagedNaming.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Planning/NativeAotLoweringPlanner.InvocationPlanning.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.InvocationAbi.cs`
- `tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py`

## 架构审视

- 现在 generic execution authority 的最终入口已经是 `InstantiationStubId`，而不是闭包实例 body symbol。
- planner/emitter/runtime helper 已对齐到同一条 authority 语义，`aot-core-ir.json -> lowering planner -> generated cpp` 的 generic authority 不再断链。
- `shared body` 物理复用仍是后续任务，不应混入本任务的完成定义，否则会把 authority hard cut 和 body dedupe 两件事耦在一起。

## 生成产物审视

- `aot-core-ir.json` 可直接审查 `openDefinitionSubjectId / sharedGenericBodyId / instantiationStubId`。
- `generated/native-aot.generated.cpp` 现在同时出现：
  - body declaration / definition
  - thin stub declaration / definition
  - call-site / `ldftn` 对 stub symbol 的消费
- 这意味着 downstream review 已能直接看到 generic execution authority，而不是靠注释猜测。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py -k "planner_consumes_runtime_generic_context_when_rendering_generic_layout or driver_emits_native_aot_cpp_for_generic_layout_proof" -q`
  - 结果: `2 passed`
- `python -m pytest tests/unit/compatibility/test_loader_instantiation_demand_graph_aot_core_ir_authority_contract.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py -q`
  - 结果: `10 passed`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果: `0 Warning(s), 0 Error(s)`

## 下一步

- 切到 `20260421-06-generic-matrix-and-ir-gates`
- 用正式 matrix / gate artifact 把当前 authority cut 收口成可审计产物，再作为 `20260421-01` async family cutover 的前置 gate

## 残余风险

- `sharedGenericBodyId` 目前还是逻辑 authority，不代表 native body 已物理复用。
- 若后续 `20260421-01` 在 async family 内部重新引入 closed-symbol shortcut，会破坏这次 hard cut 的边界，需要用 `20260421-06` gate 把住。
