# Phase 1 Loader And Typed-IL Input Closure Design v1.01

Date: 2026-04-16 09:20:00 +08:00
Status: completed

## 1. 目标

以真实 C# failing surface 为入口，完成 `LoaderStage` 的第一批 widening，使下游能够稳定消费：

- `TypedReference` 相关 low-level opcode
- `sizeof(non-primitive struct)` 的 type-token 输入

本批只负责 `loader -> typed-il -> aot-core-ir` 输入闭环，不扩展 planner / emitter 对这些 opcode 的执行语义。

## 2. 输入证据

### 2.1 真实 failing surface

- `TypedReference`
  - C# 语法：`__makeref` / `__reftype` / `__refvalue`
  - 原始失败：`unsupported IL opcode in loader: Mkrefany`
- `sizeof(non-primitive struct)`
  - C# 语法：`sizeof(Guid)`
  - 原始失败：`unsupported IL opcode in loader: Sizeof`

### 2.2 当前分层约束

- 不把 `Chaos.TestFramework` 混入 AOT core 逻辑
- 不提前扩展 `NativeAotLoweringPlanner` / `NativeAotEmitter`
- 工具链分层继续保持：
  - `LoaderStage`
  - `CodeGenStage` / `TypedIlIr`
  - `AotCoreIrLowering`
  - `NativeAotLoweringPlanner`
  - `NativeAotEmitter + Scriban`

## 3. 设计决策

### 3.1 用现有 type-reference decode 路径承载新 opcode

- `mkrefany`
  - operand: type subject id
  - resultType: `System.TypedReference`
- `refanytype`
  - 无 operand
  - resultType: `System.RuntimeTypeHandle`
- `refanyval`
  - operand: type subject id
  - resultType: `System.IntPtr`
- `sizeof`
  - operand: type subject id
  - resultType: `System.Int32`

### 3.2 compatibility 测试使用真实 C# fixture

- 入口方法直接调用 helper，使 helper 进入闭包，不依赖手工构造 JSON
- fixture 同时包含 `TypedReference` 与 `sizeof(Guid)`，确保这批 widening 有真实回归覆盖

## 4. 非目标

- 不处理 vararg / `__arglist`
- 不处理 `MemberReference.Parent = MethodDefinition`
- 不扩展 native planner / emitter 对新 opcode 的 lowering

## 5. 验收标准

- driver 能成功消费包含 `TypedReference + sizeof(Guid)` 的真实 C# fixture
- `typed-il-ir.json` 保留正确的 opcode / operand / resultType / reference
- `aot-core-ir.json` 保留相同的指令身份
- 相邻 loader compatibility 基线不回归
