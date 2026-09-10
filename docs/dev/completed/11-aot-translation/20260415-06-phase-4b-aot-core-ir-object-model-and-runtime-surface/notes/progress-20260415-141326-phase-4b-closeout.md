# 2026-04-15 14:13:26 +08:00 Phase 4B closeout

## 本轮完成

- 依据父 roadmap `Phase 4B` 退出条件完成 closeout audit。
- 确认 `AotCoreIr` 已覆盖本阶段约定的 object/runtime surface：
  - `newobj`
  - `ldfld/stfld`
  - `ldsfld/stsfld`
  - `newarr`
  - `castclass/isinst`
  - `box/unbox/unbox.any`
  - minimal managed-pointer / `ldelema` / `ldobj` / `stobj`
- 确认 Native AOT 生成链已升级为 `NativeAotLoweringPlanner + NativeAotEmitter + Templates/NativeAot*.scriban`。
- 确认 retained owner subject proof 已在新生成链上闭环，且 proof 使用 `Assert` / `ChaosAssertState`，未把 `Console.WriteLine` 当成验证机制。

## 验证依据

1. `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4b_object_field_native_aot.py tests/unit/compatibility/test_phase4b_array_boxing_native_aot.py tests/unit/compatibility/test_phase4b_array_ops_native_aot.py tests/unit/compatibility/test_phase4b_cast_type_check_native_aot.py tests/unit/compatibility/test_phase4b_boxed_struct_field_read_native_aot.py tests/unit/compatibility/test_phase4b_object_return_native_aot.py tests/unit/compatibility/test_phase4b_array_covariance_native_aot.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_mainline_capability_batch_a.py -q`
   - 结果：26 passed
2. `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
   - 结果：Build succeeded
3. `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
   - 结果：Build succeeded

## 下一步

- 归档 `20260415-06` 到 `docs/dev/completed/`
- 创建并激活 `20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure`
- 在父 roadmap 中把主线切换到 Phase 4C
