# Phase 3 Generic Byref Mixed Dispatch Closure Design v1.01

Date: 2026-04-16 00:31:55 +08:00
Status: completed

## 1. 目标

收口 `generic + byref + value-type` 在 dispatch / invocation / wrapper 路径上的混合 lowering。

## 2. 设计决策

- mixed dispatch 必须依赖 Phase 1/2 已冻结的 shape 与 ABI contract，不能在单个 subject 里做特化补丁。
- generic virtual/interface dispatch 与 devirtualized receiver tracking 必须同时收口，否则 evidence 会失真。

## 3. 实际落点

- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`

## 4. 完成定义

- generic mixed dispatch 在 planner / linker / native AOT 路径上已形成闭环。
- interface / virtual / direct invoke 的关键 mixed shape 有正式回归测试。
