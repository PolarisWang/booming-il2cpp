# 2026-04-15 05:02:07 +08:00 Phase 4 Task 3-4 与收口验证

## 完成项

- `AotCoreIrInstructionArtifact` 新增 `Reference`
- `AotCoreIrLowering` 现在把 `managedInstruction.Reference` 带入 `aot-core-ir.json`
- 建立 `aot-core-ir-codegen-gap-ledger-v1-01.md`
  - 冻结当前 supported / partial / unsupported surfaces
  - 明确 generic / exception / object model / ABI 的当前 gap

## 验证

1. `python -m pytest tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_multi_assembly_entry_override.py::MultiAssemblyEntryOverrideTests::test_driver_supports_multi_assembly_entry_override_for_closure_and_native_aot -q`
   - 结果：5 passed
2. `python -m pytest tests/unit/compatibility/test_managed_closure_contract_bundle.py tests/unit/compatibility/test_multi_assembly_entry_override.py tests/unit/performance/test_native_aot_workload_entry_bundle.py -q`
   - 结果：9 passed
3. `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
   - 结果：Build succeeded（保留既有 nullable warnings）
4. `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
   - 结果：Build succeeded（保留既有 nullable warning）
5. `python -m pytest tests/unit/compatibility -q`
   - 结果：179 passed

## 下一步

- 更新父 roadmap / ACTIVE / 索引，判断是否将当前 child task 归档并切入 Phase 5。
