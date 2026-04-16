# Phase 0 Gap Ledger And Owner Evidence Audit Design v1.01

Date: 2026-04-16 09:05:23 +08:00
Status: completed

## 1. 目标

把剩余 AOT core 缺口冻结成 capability-first ledger，并给每个 family 绑定：

- 代码锚点
- 影响 stage
- owner subject
- proof / benchmark / gate 证据要求
- 对应 child phase

这样后续执行不再按零散报错推进，而按 `loader -> IR -> planner -> emitter + Scriban -> owner evidence` 纵向闭环推进。

## 2. 输入证据

### 2.1 Loader 侧

代码锚点：`src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`

已确认的关键边界：

- `unsupported managed entry point handle kind`
- `unsupported IL opcode in loader`
- `unsupported type reference opcode`
- `unsupported ldtoken handle kind in loader`
- `unsupported calli signature handle kind in loader`
- `unsupported exception region kind in loader`
- `unsupported method reference handle kind`
- `unsupported field reference handle kind`

### 2.2 IR 侧

代码锚点：`src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`

已确认的关键边界：

- typed-il block / instruction / opcode 对齐强依赖
- method pointer registration 缺失时直接失败
- typed-il method 缺失时直接失败
- local instruction 仍依赖特定 operand 形态
- field / member subjectId 的 declaring type identity 必须完整

### 2.3 Planner 侧

代码锚点：`src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`

已确认的关键边界：

- entry 仍要求 `parameterless`
- entry 仍要求返回 `System.Int32`
- `callvirt` dispatch kind 仍有限
- 仍有一组 opcode legalize 未完成
- EH linear lowering 仍有限
- ABI return / parameter / argument carrier 仍有 unsupported 分支

### 2.4 Emitter 侧

代码锚点：

- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`

已确认的关键边界：

- `PlanKind` 固定为 `generic-managed-entry`
- `WorkloadAbi` 固定为 `int(void)`

### 2.5 Owner evidence 侧

主体目录：`subjects/SolutionCorePack/source/`

当前 owner evidence 已具备的形态：

- `FeatureSlices/CoreRuntimeFeatures/**` 承载 proof
- `Benchmarks/CoreRuntimeBenchmarks/**` 承载 benchmark

设计要求：

- 纯 AOT 能力默认继续挂在 `SolutionCorePack`
- `HotUpdateHostPack` 与 `MixedExecutionFeaturePack` 只在 family 本身明确跨运行时语义时补充，不做 owner

## 3. 设计决策

### 3.1 ledger 按 capability family 记账

不按源码文件记账，也不按单条报错记账。每个 family 都必须跨越多个 stage。

### 3.2 owner evidence matrix 显式区分三种要求

- proof mandatory
- benchmark optional / mandatory
- gate mandatory

### 3.3 reference bundle 作为单独审计维度进入 full-solution family

用户此前强调仓库内 reference bundle 与完整 solution 验证，这里不把它混成单个 opcode 问题，而是单独纳入 full-solution / reference-heavy family。

## 4. 预期产物

- `gap-ledger-v1-01.md`
- `owner-evidence-matrix-v1-01.md`
- child-task completion record

## 5. 验收标准

- 所有主要 gap family 都能映射到唯一 child phase
- 每个 family 都有明确 owner subject 与 evidence 要求
- 纯 AOT owner subject 边界固定到 `SolutionCorePack`
- 至少有一组代表性 compatibility 用例验证 ledger 所依赖的现有事实仍成立
