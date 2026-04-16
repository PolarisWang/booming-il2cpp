# Phase 2 AotCoreIr Semantic Surface Expansion 实现计划 v1.02

Date: 2026-04-16 10:36:50 +08:00
Status: completed

## 计划清单

- [x] 用真实 C# fixture 收口 `ldtoken targetReference`
- [x] 覆盖 `type / field / method` 三类 token 形态
- [x] 扩展 `AotCoreIrAbiCarrierKind`，补齐 `Int64 / UInt64`
- [x] 扩展 `AotCoreIrLowering.ResolveAbiSlot(...)`
- [x] 扩展 `NativeAotLoweringPlanner` 的 return / parameter / invocation ABI 映射
- [x] 新增 `Int64 / UInt64` bit-preserving helper
- [x] 在 `SolutionCorePack` owner benchmark / proof 中补齐对应真实 surface
- [x] 运行局部与相邻 compatibility 回归
- [x] 回写父 roadmap / 子任务状态与进展记录

## 涉及文件

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/PrimitivesAndOps/IntegerCarrierProof.cs`
- `subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/PrimitivesAndOps/IntegerCarrierBenchmark.cs`
- `subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/InteropAndMarshaling/NativeCallInteropBenchmark.cs`
- `tests/unit/compatibility/test_phase2_aot_core_ir_ldtoken_target_reference.py`
- `tests/unit/compatibility/test_phase2_int64_carrier_native_aot.py`
- `tests/unit/compatibility/test_phase2_uint64_interop_carrier_native_aot.py`

## 验证命令

- `python -m pytest tests/unit/compatibility/test_phase2_int64_carrier_native_aot.py -q`
- `python -m pytest tests/unit/compatibility/test_phase2_uint64_interop_carrier_native_aot.py -q`
- `python -m pytest tests/unit/compatibility/test_phase2_int64_carrier_native_aot.py tests/unit/compatibility/test_phase2_uint64_interop_carrier_native_aot.py tests/unit/compatibility/test_phase2_small_integer_carrier_native_aot.py tests/unit/compatibility/test_phase2_floating_point_carrier_native_aot.py tests/unit/compatibility/test_phase2_aot_core_ir_ldtoken_target_reference.py tests/unit/compatibility/test_phase1_loader_typed_reference_decode.py tests/unit/compatibility/test_phase1_loader_vararg_decode.py tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py tests/unit/compatibility/test_loader_switch_opcode.py -q`

## 后续衔接

- `Phase 2` 完成后，下一任务切到 `20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization`
- 下一批优先锁定纯 `64-bit literal + bitwise legalization` 这类 planner 层真实 failing surface
