# 2026-04-15 04:55:11 +08:00 Phase 4 Task 1-2 进展

## 完成项

- 完成 `AotCoreIr` 首刀 contract 落地：
  - `ManagedClosureArtifactNames.AotCoreIr`
  - `ManagedClosureResult.AotCoreIr`
  - `DriverEntry` 落盘 `aot-core-ir.json`
  - `NativeAotEmitter` 改为消费 `AotCoreIr`
- 完成 direct-call slice：
  - `AotCoreIrInstructionArtifact` 新增 `TargetSymbol / TargetParameterCount / TargetReturnType`
  - `AotCoreIrMethodArtifact` 新增 `NativeSymbol`
  - `AotCoreIrLowering` 基于 `CodeRegistrationArtifact` 回填 method symbol 与 direct-call target metadata
  - `NativeAotEmitter` 支持最小 static direct call、`ldarg`，并发出 reachable static helper method 定义
- 修正 multi-assembly native-aot 用例断言，让它对齐 `code-registration.json` 的真实 symbol，而不是硬编码偶发命名细节

## 验证

1. `python -m pytest tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_multi_assembly_entry_override.py::MultiAssemblyEntryOverrideTests::test_driver_supports_multi_assembly_entry_override_for_closure_and_native_aot -q`
   - 结果：4 passed
2. `python -m pytest tests/unit/compatibility/test_managed_closure_contract_bundle.py tests/unit/compatibility/test_multi_assembly_entry_override.py tests/unit/performance/test_native_aot_workload_entry_bundle.py -q`
   - 结果：9 passed
3. `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
   - 结果：Build succeeded（保留既有 nullable warnings）
4. `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
   - 结果：Build succeeded（保留既有 nullable warning）
5. `python -m pytest tests/unit/compatibility -q`
   - 结果：178 passed

## 下一步

- 进入 Task 3，先补 RED 测试，锁定 `AotCoreIr` 对对象模型 / metadata closure 的最小 carrier。
