# Phase 2 Scalar Conversion And ABI Carrier Widening Design v1.01

Date: 2026-04-15 21:23:30 +08:00
Status: completed

## 1. 设计目标

让 `AotCoreIr` 与 `NativeAotLoweringPlanner` 对 widened scalar carrier / conversion 具备稳定、可消费的 typed contract：

- `AotCoreIrAbiCarrierKind` 不再把非 `System.Int32` 的所有标量都近似成 `NativeInt`
- planner 能消费 widened scalar ABI 参数与返回
- real-driver benchmark / proof 能给出真实 native AOT 证据

## 2. 本阶段边界

本阶段负责：

- `ManagedClosureContracts` 中的 ABI carrier 枚举扩展
- `AotCoreIrLowering.ResolveAbiSlot(...)`
- `NativeAotLoweringPlanner` 中的 scalar literal / conversion / ABI consume
- 与 retained owner benchmark / proof 对应的 RED-GREEN tests

本阶段不负责：

- structured EH / control-flow widening
- widened virtual dispatch family
- entry ABI / workload ABI / emitter-template widening
- float/double 真正算术族的 widening

## 3. Batch A：Integer-First Carrier Widening

### 3.1 真实驱动

- proof: `CoreRuntimeFeatures/IntegerCarrierProofEntry::Run()`
- benchmark: `CoreRuntimeBenchmarks/IntegerCarrierBenchmarkEntry::RunWorkload()`

### 3.2 收口结果

- 新增 `Int8 / UInt8 / Int16 / UInt16` carrier
- `AotCoreIrLowering` 为 `System.SByte / System.Byte / System.Int16 / System.UInt16` 产出精确 ABI slot
- `NativeAotLoweringPlanner` 为 widened 整数参数、返回、call ABI 与 `conv.i1/i2/u1/u2` 路径产出真实 native C++

## 4. Batch B：Float/Double Carrier And Conversion Widening

### 4.1 真实驱动

- benchmark: `CoreRuntimeBenchmarks/FloatingPointCarrierBenchmarkEntry::RunWorkload()`

### 4.2 收口结果

- 新增 `Float32 / Float64` carrier
- `AotCoreIrLowering` 为 `System.Single / System.Double` 产出精确 ABI slot
- `NativeAotLoweringPlanner` 补齐 `ldc.r4 / ldc.r8 / conv.r4 / conv.r8`
- native AOT 现在可以消费 float/double ABI 参数与返回，并通过 `chaos_store_float32 / chaos_store_float64` 与 `chaos_load_float32 / chaos_load_float64` 在 eval stack 上保持位级稳定

## 5. 验证策略

- 先用 real-driver test 锁定 `aot-core-ir.json` 与 generated C++ 的 RED
- 再跑 targeted compatibility regression
- 最后跑完整 `tests/unit/compatibility -q`，确认 widened ABI 没带坏既有路径
