# Phase 2 Core Contract Cutover 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划，并按 TDD 先补失败测试，再推进实现。
> **目标：** 把 `Phase 0/1` 已冻结的 performance-first contract 正式切进 `SemanticWorld -> Linker -> CodeGen` 主干代码，完成最小的 core contract cutover。
> **架构：** 先用 RED 锁定 artifact contract 与代码责任转移，再补 `SemanticWorld`/`LinkedWorld`/`CodeGen` contract 类型与 threading，最后做 focused regression 与父子任务状态回写。
> **技术栈：** C#、JSON contract、Python unittest
> **设计文档：** `docs/dev/in-progress/20260409-04-phase-2-core-contract-cutover/design-v1-01.md`
> **预期知识沉淀：** 暂不写 wiki；待 `Phase 3/4` 继续落地后再统一沉淀 core contract 与 lowering boundary。

## 目标文件与职责

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
  - 新增 canonical subjects / semantic shapes / capability bundles contract
  - 扩展 `SemanticWorldModel`、`LinkedWorldModel`、`TypedIlMethodArtifact`
- `src/managed/Chaos.IL2CPP.SemanticWorld/SemanticWorldStage.cs`
  - 构建 canonical subjects / shapes / capability bundles
  - 前移 frozen slice 的 concat canonicalization
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
  - 线程化新的 semantic contract
  - 移除 concat subjectId 归一化
- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
  - 消费 semantic shapes / capability bundles / optimization facts
  - 输出带显式 method contract 的 `typed-il-ir`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`
  - 消费 method role / body availability 做最小 contract 校验
- `contracts/artifacts/v0/schemas/typed-il-ir.schema.json`
  - 冻结新的 typed-il method 字段
- `contracts/artifacts/v0/samples/typed-il-ir.min.json`
- `contracts/artifacts/v0/snapshots/typed-il-ir.snapshot.json`
  - 更新 canonical sample/snapshot
- `tests/unit/run/test_managed_closure_contract_bundle.py`
  - 锁定新的 typed-il artifact surface
- `tests/unit/run/test_phase2_core_contract_cutover.py`
  - 锁定 `SemanticWorld` / `CodeGen` 的责任 cutover

## Chunk 1：任务初始化与 RED

- [ ] 更新 `docs/dev/ACTIVE.md`、本任务 `STATUS.md` 与 `notes/progress-v1-01.md`
- [ ] 先补失败测试，锁定新的 typed-il method contract 字段
- [ ] 先补失败测试，锁定 `CodeGen` 不再保留 concat normalization / `SemanticWorld` 接手 canonicalization

## Chunk 2：contracts 与 `SemanticWorld`

- [ ] 在 `ManagedClosureContracts.cs` 增加 canonical subjects / semantic shapes / capability bundles 模型
- [ ] 扩展 `SemanticWorldModel` 与 `LinkedWorldModel`
- [ ] 在 `SemanticWorldStage` 生成新的 semantic contract，并前移 concat canonicalization

## Chunk 3：`Linker` / `CodeGen` / `Emitter`

- [ ] 在线路上 thread 新的 semantic contract 到 `LinkedWorld`
- [ ] 让 `CodeGenStage` 输出 `methodRole / bodyAvailability / capabilities`
- [ ] 删除 `CodeGenStage` 内部的 concat normalization
- [ ] 让 `NativeReferenceProofEmitter` 最小消费新的 typed-il method contract

## Chunk 4：contract assets 与 focused 回归

- [ ] 更新 typed-il schema / sample / snapshot
- [ ] 运行 focused unittest 回归
- [ ] 回写 child `STATUS.md` / `notes/progress-*.md`

## Chunk 5：父 roadmap 回写

- [ ] 更新父 roadmap `STATUS.md`
- [ ] 更新父 roadmap `notes/progress-*.md`
- [ ] 更新 `docs/dev/INDEX.md`、`docs/dev/in-progress/INDEX.md`

## 验证命令

- [ ] `python -m unittest tests.unit.run.test_phase2_core_contract_cutover`
- [ ] `python -m unittest tests.unit.run.test_managed_closure_contract_bundle`
- [ ] `python -m unittest tests.unit.run.test_phase2_core_contract_cutover tests.unit.run.test_managed_closure_contract_bundle`

## 完成标准

- [ ] `SemanticWorld` 不再只是 passthrough
- [ ] `CodeGenStage` 不再保留 concat normalization
- [ ] `typed-il-ir` 已带显式 `methodRole / bodyAvailability / capabilities`
- [ ] typed-il contract sample/schema/snapshot 已同步
- [ ] focused 回归通过
