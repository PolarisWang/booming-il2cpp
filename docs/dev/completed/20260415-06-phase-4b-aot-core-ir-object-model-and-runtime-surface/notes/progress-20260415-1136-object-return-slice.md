# 2026-04-15 11:36 +08:00

## 本轮推进

- 为 `NativeAotEmitter` 的 direct-call return mapping 放宽到 `std::intptr_t` pointer carrier。
- 为 managed method `ret` 补齐 reference/object return，而不再只硬编码 `System.Int32`。
- 新增 owner subject proof：`ObjectReturnProofEntry`，验证 “static helper 返回引用类型对象 -> caller 消费实例字段”。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4b_object_return_native_aot.py -q`
  - 结果：2 passed
- `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4b_object_field_native_aot.py tests/unit/compatibility/test_phase4b_array_boxing_native_aot.py tests/unit/compatibility/test_phase4b_array_ops_native_aot.py tests/unit/compatibility/test_phase4b_cast_type_check_native_aot.py tests/unit/compatibility/test_phase4b_boxed_struct_field_read_native_aot.py tests/unit/compatibility/test_phase4b_object_return_native_aot.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_mainline_capability_batch_a.py -q`
  - 结果：22 passed
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
  - 结果：Build succeeded
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果：Build succeeded

## 后续缺口

- `castclass` / `isinst` 仍是 exact-type only。
- managed-pointer 仍缺 `ldobj` / `ldind*` / structured `unbox.any` / byref call。
- wider object/value ABI 仍未完成，尤其是更宽 value return / byref return。
