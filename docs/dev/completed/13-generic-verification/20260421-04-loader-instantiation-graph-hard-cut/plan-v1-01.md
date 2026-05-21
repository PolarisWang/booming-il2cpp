# Loader Instantiation Graph Hard Cut 执行计划 v1.01

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:project-test-governance`、`dev:test-driven-development` 执行本计划。
> 目标：把 loader 的 generic 主路径从 clone/materialization 硬切到 instantiation demand graph，为后续 shared-generic IR 提供 canonical 输入。

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `native-generic-runtime-kernel`
- capabilityItem: `loader-instantiation-graph-hard-cut`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `false`
- hotupdateImpact: `Proof`
- formalVerificationObjects:
  - `loader instantiation graph source tests`
  - `cross-assembly demand tests`
  - `aot-core-ir loader input review`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

## 功能需求 Checklist

- [ ] 用 instantiation demand graph 替换 clone/materialization 作为 loader 主输出
- [ ] 显式收集 cross-assembly generic instantiation demand
- [ ] 输出 open definition、canonical instantiation、specialization policy、family taxonomy
- [ ] 停止把 `SubstituteText` / `SubstituteMethodBody` 当成主 generic 执行路径
- [ ] 停止把 `MemberReference` / `MethodSpec` materialization 当成最终 runtime payload

## 文件审计 Checklist

- [ ] `src/managed/Chaos.IL2CPP.Loader/LoaderStage.GenericMaterialization.cs`
- [ ] `src/managed/Chaos.IL2CPP.Loader/LoaderStage.CrossAssemblyInstantiation.cs`
- [ ] `src/managed/Chaos.IL2CPP.Loader/LoaderStage.MetadataResolution.cs`
- [ ] `src/managed/Chaos.IL2CPP.Loader/LoaderStage.AssemblyLoading.cs`
- [ ] `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`

## 架构审查 Checklist

- [ ] loader 输出 graph inputs，而不是 clone-first payload
- [ ] 没有 metadata-only compatibility layer 回流
- [ ] cross-assembly demand 显式且可重现
- [ ] family taxonomy 输入可被后续 IR/runtime 消费

## 生成代码审查 Checklist

- [ ] loader 产物中能看到 canonical instantiation / specialization 字段
- [ ] clone 数量不再作为主要成功信号
- [ ] graph 输出对 `20260421-05` 足够稳定可 diff

## 验证 Checklist

- [ ] RED: loader/source tests 先失败
- [ ] GREEN: loader/source tests 通过
- [ ] GREEN: cross-assembly demand tests
- [ ] GREEN: 受影响 loader compatibility tests
