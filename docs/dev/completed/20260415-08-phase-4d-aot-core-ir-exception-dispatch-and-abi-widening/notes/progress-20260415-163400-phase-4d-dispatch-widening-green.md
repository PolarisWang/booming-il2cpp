# 2026-04-15 16:34:00 +08:00 Phase 4D Dispatch Widening Green

## 本轮完成

- retained proof `DispatchProofEntry` 已切成 Phase 4D 可落地的 virtual dispatch proof。
- `NativeAotLoweringPlanner` 已支持：
  - `callvirt` opcode lowering
  - instance method hidden `this` ABI
  - retained virtual target discovery
  - type-id based virtual dispatch emission
- `DispatchLeaf::ReadValue()` 现在会作为 Native AOT method section 发射，并由 generated C++ 里的 virtual switch 调用。

## 保持的边界

- 没有把 `Chaos.TestFramework` 逻辑混入 `AotCoreIr` / planner / emitter。
- Native AOT 生成继续走 `NativeAotLoweringPlanner + NativeAotEmitter + Templates/NativeAot*.scriban`。
- 本轮没有把 ctor 语义、exception lowering、ABI widening 混做成一个大补丁。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py -q`
  - `4 passed`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py -q`
  - `23 passed`
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
  - `Build succeeded`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - `Build succeeded`

## 下一步

- 建立 Phase 4D 第二刀 RED：exception lowering / ABI widening。
