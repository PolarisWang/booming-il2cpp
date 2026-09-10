# 2026-04-15 11:30 +08:00

## 本轮推进

- 为 `ManagedTypeModel` 补入 `IsValueType`，并让 loader / materialized type-spec 继续把 value-type shape 传递进 closure。
- 为 `AotCoreIrReferenceArtifact` 补入 `AotCoreIrTypeShapeKind`、`DeclaringTypeSubjectId`、`DeclaringTypeShape`，避免 emitter 回绑 loader 世界。
- 为 `AotCoreIrRuntimeServiceKind` 补入 `InitObject`，并让 `AotCoreIrLowering` 正式承接 `initobj`。
- 为 `NativeAotEmitter` 补入 `ldloca`、`initobj`、`unbox`，并引入 local-slot tag + `chaos_resolve_managed_value_pointer<T>` 作为最小 managed-pointer 表面。
- 新增 owner subject proof：`BoxedStructFieldReadProofEntry`，真实闭环 `ldloca.s -> initobj -> stfld -> box -> unbox -> ldfld`。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4b_object_field_native_aot.py tests/unit/compatibility/test_phase4b_array_boxing_native_aot.py tests/unit/compatibility/test_phase4b_array_ops_native_aot.py tests/unit/compatibility/test_phase4b_cast_type_check_native_aot.py tests/unit/compatibility/test_phase4b_boxed_struct_field_read_native_aot.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_mainline_capability_batch_a.py -q`
  - 结果：20 passed
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
  - 结果：Build succeeded
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果：Build succeeded

## 后续缺口

- 仍未覆盖 object return / wider ABI surface。
- `castclass` / `isinst` 仍是 exact-type only。
- managed-pointer 仍缺 `ldobj` / `ldind*` / structured `unbox.any` / byref call。
- `NativeAotEmitter` 仍然是 `StringBuilder` 主导，Scriban 迁移尚未开始。
