# 2026-04-15 11:58 inheritance cast compatibility slice

## 本轮完成

- 为 `ManagedTypeModel` 与 `AotCoreIrReferenceArtifact` 新增 `BaseTypeSubjectId`。
- `LoaderStage` 已为 type definition / type spec 传播 base-type identity。
- `AotCoreIrLowering` 已把 base-type identity lowering 到 `AotCoreIr`。
- `NativeAotEmitter` 已新增：
  - `chaos_get_base_type_id`
  - `chaos_is_type_compatible`
  - reference-type C++ 继承布局 emission
- `CastTypeCheckProofEntry` 已升级为 `Base -> Middle -> Derived` 三层继承 proof，覆盖 inherited `stfld` / `ldfld` 与 `isinst` / `castclass`。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4b_cast_type_check_native_aot.py -q`
  - 结果：6 passed
- `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4b_object_field_native_aot.py tests/unit/compatibility/test_phase4b_array_boxing_native_aot.py tests/unit/compatibility/test_phase4b_array_ops_native_aot.py tests/unit/compatibility/test_phase4b_cast_type_check_native_aot.py tests/unit/compatibility/test_phase4b_boxed_struct_field_read_native_aot.py tests/unit/compatibility/test_phase4b_object_return_native_aot.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_mainline_capability_batch_a.py -q`
  - 结果：22 passed
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
  - 结果：Build succeeded
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果：Build succeeded

## 下一步

- 继续把 non-exact type compatibility 从当前 class inheritance 扩展到 interface / array covariance。
- 继续补 wider managed-pointer surface：`ldobj` / `ldind*` / structured `unbox.any` / byref call。
