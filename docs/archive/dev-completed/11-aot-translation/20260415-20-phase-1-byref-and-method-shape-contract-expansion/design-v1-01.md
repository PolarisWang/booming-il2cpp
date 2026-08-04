# Phase 1 Byref And Method Shape Contract Expansion Design v1.01

Date: 2026-04-16 00:31:55 +08:00
Status: completed

## 1. 目标

把 deferred `byref` / widened method shape 从 loader / metadata decode 一直打通到 `AotCoreIr` 输入合同。

## 2. 设计决策

- 先在 loader / metadata 层显式建模，再让 `AotCoreIr` 消费，不允许 emitter 反推 shape。
- `ref readonly` 与 mutable byref 共享同一合同面，差异放到 carrier/metadata 标记，而不是分叉新的协议。

## 3. 实际落点

- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- `src/managed/Chaos.IL2CPP.Loader/Metadata/TypeProviders.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`

## 4. 完成定义

- loader 能解出 widened method shape 所需的 metadata 形状。
- `AotCoreIr` 能持有后续 planner 需要的 carrier / shape 信息。
- 相关 compatibility 测试不再在 decode / IR 入口硬失败。
