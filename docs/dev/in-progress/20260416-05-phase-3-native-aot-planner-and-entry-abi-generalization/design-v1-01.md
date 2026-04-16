# Phase 3 NativeAot Planner And Entry ABI Generalization Design v1.01

Date: 2026-04-16 10:36:50 +08:00
Status: in-progress

## 1. 目标

在 `Phase 2` 已冻结的 `AotCoreIr` contract 之上，继续扩展 `NativeAotLoweringPlanner` 的 opcode legalization 与 widened carrier 消费能力。

本批优先目标：

- `ldc.i8` 64-bit literal
- `and / or / xor` bitwise integer opcode

## 2. 真实 failing surface

- 当前 planner 仍不支持 `ldc.i8`
- 当前 planner 仍不支持 `and / or / xor`
- 这直接阻断了纯 C# `UInt64` literal / bitwise owner workload，迫使 subject 侧依赖 interop 或其他绕路

## 3. 设计决策

### 3.1 先在 planner 层补 literal / bitwise legalization

不在本批先扩 subject 协议，不在 benchmark/reporting 层做额外兜底。

### 3.2 owner evidence 继续固定在 `SolutionCorePack`

优先扩现有 `PrimitivesAndOps` owner slice，而不是新开 subject。

### 3.3 仍保持分层

- `Phase 3` 只解决 planner legalization
- 如果需要 emitter / Scriban 形状扩展，留在后续 `Phase 4`

## 4. 非目标

- 本批不处理更泛化的 framework intrinsic host gating
- 本批不混入 `Chaos.TestFramework`
- 本批不扩大 subject 内部 string protocol
