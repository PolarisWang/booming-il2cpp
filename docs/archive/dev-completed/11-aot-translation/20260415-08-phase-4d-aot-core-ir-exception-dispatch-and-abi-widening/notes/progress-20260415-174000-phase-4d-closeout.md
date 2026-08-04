# Phase 4D 收口记录

时间：2026-04-15 17:40:00 +08:00

## 本轮完成

- 把 `AotCoreIrAbiCarrierKind : byte` 与 `AotCoreIrAbiSlotArtifact` 接入 `AotCoreIrMethodArtifact.ReturnAbi` / `ParameterAbis`。
- 扩展 `AotCoreIrLowering`，让 widened ABI carrier 正式进入 `aot-core-ir.json`。
- 扩展 `NativeAotLoweringPlanner`，让 direct call / callvirt / method return 全部走 ABI-slot 参数签名、调用参数展开与 value-type by-value 返回值回写。
- 让对象模型声明追踪 ABI-only value type，避免 `Echo` 这类签名-only widened struct 丢失 native declaration。
- 新增 `AbiWideningProofEntry` retained proof，并把 `test_phase4d_abi_widening_aot_core_ir.py` / `test_phase4d_abi_widening_native_aot.py` 拉到绿色。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4d_abi_widening_aot_core_ir.py tests/unit/compatibility/test_phase4d_abi_widening_native_aot.py -q`
  - 结果：`5 passed`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_exception_aot_core_ir.py tests/unit/compatibility/test_phase4d_exception_native_aot.py tests/unit/compatibility/test_phase4d_abi_widening_aot_core_ir.py tests/unit/compatibility/test_phase4d_abi_widening_native_aot.py -q`
  - 结果：`36 passed`
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
  - 结果：`Build succeeded`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果：`Build succeeded`

## 下一步

- 返回父 roadmap，开始 `Phase 5: Feature Coverage Expansion On Owner Subjects`。
