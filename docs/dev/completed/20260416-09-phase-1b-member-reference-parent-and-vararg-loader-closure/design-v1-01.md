# Phase 1B Member Reference Parent And Vararg Loader Closure Design v1.01

Date: 2026-04-16 09:43:20 +08:00
Status: completed

## 1. 目标

以真实 `__arglist` / vararg C# fixture 为入口，完成 `LoaderStage` 在 `Phase 1` 的第二批 widening，使下游能够稳定消费：
- `MemberReference.Parent = MethodDefinition`
- `ILOpCode.Arglist`

本批仍然只负责 `loader -> typed-il -> aot-core-ir` 输入闭环，不扩展 planner / emitter 对 vararg 的执行语义。

## 2. 输入证据

### 2.1 真实 failing surface

- C# 复现：
  - `return Helper(1, __arglist(2, 3));`
  - `ArgIterator iterator = new ArgIterator(__arglist);`
- 初始失败：
  - `System.NotSupportedException: unsupported type handle kind: MethodDefinition`
  - 栈锚点：
    - `MetadataTypeResolver.ResolveTypeIdentity`
    - `LoaderStage.DescribeMemberReferenceMethod`

### 2.2 本批 contract 约束

- 不把 `Chaos.TestFramework` 混入 AOT core / planner / emitter
- 不提前扩展 vararg 的 native lowering / runtime semantics
- 继续保持 `SolutionCorePack` 为纯 AOT owner subject；本批只用最小 repro fixture 锁定 loader contract

## 3. 设计决策

### 3.1 MemberReference owner 解析改为支持 MethodDefinition parent

- 当 `MemberReference.Parent.Kind == MethodDefinition` 时：
  - 通过 `MethodDefinition.GetDeclaringType()` 解析 declaring type
  - `DefinitionSubjectId` 基于真实 method definition 的 fixed-parameter signature 生成
- call-site `SubjectId` 继续保留 member reference 已闭合的 vararg 参数形状

### 3.2 新增 arglist decode

- `ILOpCode.Arglist`
  - `op = "arglist"`
  - 无 operand
  - `resultType = "System.RuntimeArgumentHandle"`

### 3.3 测试锁定真实 vararg materialized 形状

- vararg helper 在 typed-il / aot-core-ir 中的 `subjectId` 为 call-site 级：
  - `VarArgDecodeRepro/Program::Helper(System.Int32,System.Int32,System.Int32)`
- 但 `identity.definitionSubjectId` 仍然回指 fixed-parameter definition：
  - `VarArgDecodeRepro/Program::Helper(System.Int32)`

## 4. 非目标

- 不让 planner / emitter 直接支持 vararg native codegen
- 不引入新的 string 协议
- 不把更多未冻结的 metadata shape 混入这一批

## 5. 验收标准

- driver 能成功消费包含 `__arglist` 的真实 C# fixture
- `typed-il-ir.json` 保留：
  - `arglist`
  - vararg helper 的 call-site `subjectId`
  - fixed-parameter `definitionSubjectId`
- `aot-core-ir.json` 保留相同的指令身份
- 相邻 loader compatibility 基线不回归
