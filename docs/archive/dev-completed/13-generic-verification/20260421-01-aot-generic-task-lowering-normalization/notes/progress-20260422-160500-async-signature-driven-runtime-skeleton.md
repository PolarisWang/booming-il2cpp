# 20260422-160500 async signature-driven runtime-skeleton

## 摘要

- 把 `NativeReferenceProofEmitter.cs` 中的 async runtime-skeleton 路径从 `SupportedAsyncTaskResultShapes` 白名单切成 `AsyncTaskFamilyShape + AsyncManagedTypeShape`
- factory stub 参数 carrier 改为基于真实方法参数推导，不再沿用“结果类型决定参数 C++ 类型”的错误耦合
- get-result wrapper 验证改为基于 producer 方法参数与 literal carrier 校验，不再按 `int/bool/string` 结果类型猜测
- 引用结果 `get-result` stub 不再把 `nullptr` 当作模板层内部错误

## 新增回归

- `tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py`
  - 锁 `arg0_cpp_type` / `arg1_cpp_type`
  - 锁 async helper 已切到 signature-driven shape resolver
- `tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_validators.py`
  - 锁 `TryParseClosedTaskResultType` / `TryResolveAsyncManagedTypeShape` / `ResolveAsyncLiteralParameterShapes`
- `tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py`
  - 新增 `Task<string>(int)` fixture，验证 generated runtime-skeleton page 必须出现 `int32_t arg0`

## 验证

- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_validators.py -q`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py -q`
- `python -m pytest tests/unit/compatibility -k "native_aot" -q`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`

## 风险

- runtime-skeleton 仍未把任意自定义值类型 / 引用类型参数完全泛化
- `ValueTask<T>` runtime-skeleton wrapper 仍未单独扩张
