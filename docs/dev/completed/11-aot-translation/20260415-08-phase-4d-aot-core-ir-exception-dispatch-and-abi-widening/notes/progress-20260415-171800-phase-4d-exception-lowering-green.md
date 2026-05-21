# 2026-04-15 17:18:00 +08:00 Phase 4D Exception Lowering Green

## 完成项

- 新增 `tests/unit/compatibility/test_phase4d_exception_aot_core_ir.py`，把 Phase 4D exception lowering 的 RED 边界冻结下来。
- `ManagedClosureContracts` 新增 `AotCoreIrExceptionRegionKind : byte` 与 `AotCoreIrExceptionRegionArtifact`，`AotCoreIrMethodArtifact` 现在显式携带 `ExceptionRegions`。
- `AotCoreIrLowering` 现在会把 loader 已解出的 managed EH regions lowering 到 `aot-core-ir.json`，不再只保留 `ExceptionRegionCount`。
- `ManagedExceptionRegionModel` 补充 `FilterOffset` 透传，为后续 filter / fault / native EH planner 留出 typed carrier。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase4d_exception_aot_core_ir.py -q`
  - 结果：`2 passed`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_generic_layout_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_exception_aot_core_ir.py -q`
  - 结果：`28 passed`
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
  - 结果：`Build succeeded`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果：`Build succeeded`

## 下一步

- 基于新的 typed EH carrier 建立 Native AOT exception RED，先只收敛最小 `catch/finally` 路径。
- ABI widening 继续放在 exception planner slice 之后，避免把 EH 与 ABI 同时混做。
