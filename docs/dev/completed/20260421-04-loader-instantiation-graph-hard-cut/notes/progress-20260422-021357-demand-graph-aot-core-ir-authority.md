# 2026-04-22 02:13:57 +08:00 demand-graph to AOT IR authority

## 结论

- `AotCoreIrLowering` 不再只靠 `ManagedNaming.TryCreateRuntimeGenericContext(...)` 从 subjectId 现算 generic context
- 新逻辑会优先读取 `LinkedWorldModel.GenericInstantiationDemandGraph`：
  - 用 demand graph entry 生成 `RuntimeGenericContext`
  - 用 demand graph entry 生成 `GenericDiagnostic`
  - 同一 `subjectId` 出现冲突 demand 时直接抛错
- 这意味着 `generic-instantiation-demand-graph.json` 已经不只是“供 20260421-05 参考的中间 JSON”，而是开始成为真实 downstream authority 输入

## 验证

- `python -m pytest tests/unit/compatibility/test_loader_instantiation_demand_graph_aot_core_ir_authority_contract.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py -q`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`

## 下一步

- 继续排查 `MetadataWriter` / native emitters 中仍然绕过 graph 的 generic authority 解析
- 再回头切 loader 残余 `GenericCompatibilityProjection` 的 authority 地位
