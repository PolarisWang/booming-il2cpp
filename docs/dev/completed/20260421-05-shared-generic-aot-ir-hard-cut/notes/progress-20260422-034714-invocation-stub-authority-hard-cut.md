# 20260422-034714 Invocation Stub Authority Hard Cut

- `20260421-05` 已完成 execution authority hard cut。
- planner / emitter / invocation ABI 现在统一消费 `InstantiationStubId -> stub symbol`，direct call-site 与 `ldftn` 不再把闭包 body symbol 当成最终 authority。
- 当前仍保留 thin-stub + body forwarding 结构；shared body 物理去重明确留给后续任务，不在本轮强推。
- 验证证据：
  - `tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py`
  - `tests/unit/compatibility/test_loader_instantiation_demand_graph_aot_core_ir_authority_contract.py`
  - `tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py`
  - `tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py`
  - `tests/unit/compatibility/test_phase4c_metadata_closure_contract.py`
  - `tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py`
  - `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
