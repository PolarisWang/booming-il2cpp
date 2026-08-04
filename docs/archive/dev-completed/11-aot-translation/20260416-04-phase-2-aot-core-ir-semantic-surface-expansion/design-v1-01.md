# Phase 2 AotCoreIr Semantic Surface Expansion Design v1.01

Date: 2026-04-16 09:58:34 +08:00
Status: in-progress

## 1. 目标

以 `AotCoreIr` 为唯一 owner，冻结并扩大当前已经由 loader 解锁、但在 IR 层仍未完整表达的真实语义面。本子任务不直接扩 planner / emitter，而是先让 `aot-core-ir.json` 自身成为稳定 contract。

本批优先收口：

- `ldtoken` 在 `AotCoreIr` 中的 `targetReference`

## 2. 输入证据

### 2.1 已知前置条件

- `LoaderStage` 已支持 `ldtoken`
- loader 能解析三类 token：
  - type
  - field
  - method
- `AotCoreIrLowering.ResolveTargetReference(...)` 目前未覆盖 `ldtoken`

### 2.2 当前 contract 缺口

- `aot-core-ir.json` 中的 `ldtoken` 指令目前只有 `reference`
- 下游如果需要使用 token 的强类型身份，只能重新解读 `reference.subjectKind` 或硬编码 opcode 特判

## 3. 设计决策

### 3.1 `ldtoken` 在 IR 层必须落成强类型 `targetReference`

- type token -> `AotCoreIrReferenceKind.Type`
- field token -> `AotCoreIrReferenceKind.Field`
- method token -> `AotCoreIrReferenceKind.Method`

### 3.2 `targetReference` 的构造规则复用现有 helper

- type token 复用 `CreateTypeReference(...)`
- field token 复用 `CreateFieldReference(...)`
- method token 复用 `CreateMethodReference(...)`

### 3.3 本批只修复 IR contract，不提前扩大运行时语义

- 不在本批中实现 `ldtoken` 对应的 planner / emitter / runtime token materialization
- 不混入 `Chaos.TestFramework`
- 不新增字符串协议

## 4. 非目标

- 不在本批中解决 `ldtoken` native lowering
- 不在本批中扩大 `RuntimeServiceKind`
- 不在本批中引入新的 benchmark / owner subject 入口

## 5. 验收标准

- 新增真实 `ldtoken` C# fixture，覆盖 type / field / method token
- `typed-il-ir.json` 保留三类 token 的 `reference`
- `aot-core-ir.json` 为三类 `ldtoken` 指令补齐 `targetReference`
- `targetReference.kind` 与 `subjectId` 和 token 实体一致
- 相邻 `AotCoreIr` compatibility 基线无回归
