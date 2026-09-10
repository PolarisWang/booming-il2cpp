# Phase 2 Scalar Conversion And ABI Carrier Widening Brainstorm v1.01

Date: 2026-04-15 21:23:30 +08:00
Status: active

## 1. 目标

本阶段不提前处理 EH / dispatch / emitter 大扩张，而是先把真实 arithmetic / conversion 能力稳定送进：

- `AotCoreIr` widened ABI carrier
- `NativeAotLoweringPlanner` widened arithmetic / compare / conversion 消费面

## 2. 首批切入点

- proof: `CoreRuntimeFeatures/ConversionOpsProofEntry::Run()`
- benchmark: `CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()`

原因：

- 都是 retained owner subject，已存在，不需要新增临时 subject
- 当前正好覆盖数值 conversion、算术运算与 benchmark/native workload
- 能直接暴露 `Int32` / `NativeInt` 最小集之外的 carrier 缺口

## 3. 执行原则

- 先 RED，再改实现
- 不把 `Chaos.TestFramework` 逻辑混进 `AotCoreIr` / planner / emitter
- 本阶段继续保持 `AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban` 单向消费，不重新引入字符串协议

## 4. 阶段内批次

1. Batch A: signed/unsigned integer conversion / compare / ABI carrier widening
2. Batch B: float / double conversion 与算术 widening
3. Batch C: 结合 benchmark/native entry 回归，收口 planner / ABI 边界
