# Phase 4C 收口记录

时间：2026-04-15 14:50:48 +08:00

## 本轮完成

- 将 `GenericLayoutProofEntry` 改为 `int + readonly field + ChaosAssertState` 形状，移除 `ldstr` 与旧实例方法调用路径。
- 扩展 `NativeAotLoweringPlanner`，开始消费 `method.GenericContext` 与 `targetReference.GenericContext`。
- 在 native-aot 生成结果中输出 generic context 审计注释，同时保持 `NativeAotLoweringPlanner + NativeAotEmitter + Templates/NativeAot*.scriban` 的结构不变。
- 完成 retained generic proof 的 native-aot 生成闭环回归。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_mainline_capability_batch_a.py -q`
  - 结果：7 passed
- `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/compatibility/test_mainline_capability_batch_a.py -q`
  - 结果：19 passed
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
  - 结果：Build succeeded
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果：Build succeeded

## 下一步

- 返回父 roadmap，切入 `20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening`。
