# Phase 2 Scalar Conversion And ABI Carrier Widening 实现计划

> 面向执行 Agent：必须使用 `RED -> GREEN -> REFACTOR` 推进，并同步维护 `STATUS.md`、`docs/dev/ACTIVE.md` 与 `notes/progress-*.md`。

## 目标

- 扩大 `AotCoreIr` / planner 对 widened scalar / conversion / ABI carrier 的支持面
- 用 retained owner proof / benchmark 给出真实 native AOT 证据

## 设计文档

- `docs/dev/completed/20260415-14-phase-2-scalar-conversion-and-abi-carrier-widening/design-v1-01.md`

## 涉及文件与职责

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/PrimitivesAndOps/IntegerCarrierBenchmark.cs`
- `subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/PrimitivesAndOps/FloatingPointCarrierBenchmark.cs`
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/PrimitivesAndOps/IntegerCarrierProof.cs`
- `tests/unit/compatibility/` 下新增的 Phase 2 real-driver tests

## 执行步骤

- [x] Task 1: 锁定 integer-first Batch A 的真实入口与失败形态
  - `CoreRuntimeFeatures/IntegerCarrierProofEntry::Run()`
  - `CoreRuntimeBenchmarks/IntegerCarrierBenchmarkEntry::RunWorkload()`
  - 新增 RED 测试并确认当前 ABI / planner 边界
- [x] Task 2: 扩大 `AotCoreIr` carrier contract
  - 补齐 `Int8 / UInt8 / Int16 / UInt16 / Float32 / Float64`
  - 保持枚举紧凑，不扩张新的 string 协议
- [x] Task 3: 扩大 planner conversion / ABI 消费面
  - 清理 `std::int32_t` / `NativeInt` 的单点假设
  - 让 small integer 与 float/double literal / conversion / ABI 路径都能转绿
- [x] Task 4: 跑完 Phase 2 验证并准备切入 Phase 3
  - targeted suites
  - 完整 `tests/unit/compatibility -q`
  - 更新父 roadmap / index / notes

## 本轮验证

- `python -m pytest tests/unit/compatibility/test_phase2_small_integer_carrier_native_aot.py -q`
- `python -m pytest tests/unit/compatibility/test_phase2_floating_point_carrier_native_aot.py -q`
- `python -m pytest tests/unit/compatibility/test_phase4d_abi_widening_aot_core_ir.py tests/unit/compatibility/test_phase4d_abi_widening_native_aot.py -q`
- `python -m pytest tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py -q`
- `python -m pytest tests/unit/compatibility -q`
