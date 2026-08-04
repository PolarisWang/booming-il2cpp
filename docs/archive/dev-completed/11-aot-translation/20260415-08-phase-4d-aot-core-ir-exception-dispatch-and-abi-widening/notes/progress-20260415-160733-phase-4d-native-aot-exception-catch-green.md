# Phase 4D Native AOT Exception Catch-Only 闭环

- 时间：2026-04-15 16:07:33 +08:00
- 结论：已完成 Phase 4D 的 native-aot exception 最小 catch-only 闭环，下一步切入 ABI widening RED。

## 本轮变更

- 新增 retained proof：`subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/ExceptionsAndControlFlow/ExceptionCatchProof.cs`
- 更新 `tests/unit/compatibility/test_phase4d_exception_native_aot.py`，把 Native AOT exception RED 收窄到 int-only、无 string、`Assert` 驱动的 catch-only proof。
- `AotCoreIrLowering` 修正 `newobj` 的外部构造降级路径：异常对象构造现在输出 type carrier，而不再退回 method carrier。
- `NativeAotLoweringPlanner` 新增最小 EH 支持：
  - `throw` / `pop` / `leave`
  - `struct chaos_managed_exception`
  - catch-only exception method emission
  - catch type 的 type-id 收集

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4d_exception_native_aot.py -q`
  - 结果：`3 passed`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_exception_aot_core_ir.py tests/unit/compatibility/test_phase4d_exception_native_aot.py -q`
  - 结果：`31 passed`
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
  - 结果：`Build succeeded`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果：`Build succeeded`

## 备注

- 本轮只闭环 catch-only retained slice，未把 `finally` / `filter` / `fault` 混入当前 patch。
- 继续保持 `Chaos.TestFramework` 只作为 managed dependency，没有混入 `AotCoreIr` / planner / emitter。
