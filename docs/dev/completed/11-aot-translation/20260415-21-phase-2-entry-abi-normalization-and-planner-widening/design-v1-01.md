# Phase 2 Entry ABI Normalization And Planner Widening Design v1.01

Date: 2026-04-16 00:31:55 +08:00
Status: completed

## 1. 目标

把 widened method shape 落到 `NativeAotLoweringPlanner` 的 entry ABI normalization、wrapper / thunk / return carrier 规划上。

## 2. 设计决策

- planner 是 ABI 规划的唯一事实来源；不能再让 emitter 临时拼接 fallback ABI。
- return carrier、hidden receiver、wrapper / thunk shape 统一进入 lowering plan。

## 3. 实际落点

- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`

## 4. 完成定义

- widened entry ABI 已有统一 planner model。
- native AOT 不再依赖 legacy fallback ABI 来消费新 shape。
