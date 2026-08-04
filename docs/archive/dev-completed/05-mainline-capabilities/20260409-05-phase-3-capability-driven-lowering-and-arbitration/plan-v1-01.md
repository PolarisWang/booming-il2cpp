# Phase 3 Capability-Driven Lowering And Arbitration 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划，并按 TDD 先补失败测试，再推进实现。
> **目标：** 把 lowering family 选择与 managed lowering plan 真源前移到 `CodeGen`，同时把 `NativeReferenceProofEmitter` 收缩为 plan consumer。
> **架构：** 先补 RED 锁定 managed lowering plan artifact 与 emitter boundary，再补 `SemanticWorld` structure signals、`CodeGen` planner、`Driver` 写盘与 emitter consumer cutover。
> **技术栈：** C#、JSON contract、Python unittest
> **设计文档：** `docs/dev/completed/20260409-05-phase-3-capability-driven-lowering-and-arbitration/design-v1-01.md`
> **预期知识沉淀：** 暂不写 wiki；待 `Phase 3` 稳定后再统一沉淀 lowering boundary / plan contract。

## 目标文件与职责

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
  - 新增 managed lowering plan artifact 名称与 `ManagedClosureResult` surface
- `src/managed/Chaos.IL2CPP.SemanticWorld/SemanticWorldStage.cs`
  - 补最小 method structure signals
- `src/managed/Chaos.IL2CPP.CodeGen/CodeGenStage.cs`
  - 生成 managed lowering plan
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs`
  - family selection + plan assembly
- `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
  - 写出 `native-reference.lowering-plan.json`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`
  - 只读取 plan、校验与渲染
- `tests/unit/run/test_phase3_capability_driven_lowering.py`
  - 锁定 `CodeGen`/emitter 边界迁移
- `tests/unit/run/test_managed_closure_contract_bundle.py`
  - 锁定 managed lowering plan artifact 与 native proof 镜像行为
- `contracts/artifacts/v0/samples/native-reference.lowering-plan.min.json`
  - 冻结当前 canonical sample

## Chunk 1：RED

- [x] 更新 `docs/dev/ACTIVE.md`、本任务 `STATUS.md` 与 `notes/progress-v1-01.md`
- [x] 运行 `tests.unit.run.test_phase3_capability_driven_lowering`
- [x] 运行 `tests.unit.run.test_managed_closure_contract_bundle`

## Chunk 2：contract 与 planner 真源

- [x] 新增 managed lowering plan artifact 名称
- [x] 让 `ManagedClosureResult` / `closure.manifest.json` 显式携带 lowering plan artifact
- [x] 新增 `NativeReferenceLoweringPlanner`

## Chunk 3：semantic structure + CodeGen family selection

- [x] 在 `SemanticWorld` 补 `static-forwarder` / `instance-field-getter` 等最小 structure signals
- [x] 在 `CodeGen` 基于 semantic structure + capability bundle 选择 lowering family
- [x] 把 dependencies / reasons 降级为 consistency check

## Chunk 4：Driver / emitter cutover

- [x] `DriverEntry.Run(ManagedClosureRequest)` 写出 `native-reference.lowering-plan.json`
- [x] emitter 改成只读取 managed lowering plan
- [x] native proof 输出目录继续镜像 `native-proof.plan.json`

## Chunk 5：samples / regression / docs 回写

- [x] 更新 lowering plan canonical sample
- [x] 跑 focused regressions
- [x] 回写 child / parent task 状态

## 验证命令

- [x] `python -m unittest tests.unit.run.test_phase3_capability_driven_lowering`
- [x] `python -m unittest tests.unit.run.test_managed_closure_contract_bundle`
- [x] `python -m unittest tests.unit.run.test_phase3_capability_driven_lowering tests.unit.run.test_managed_closure_contract_bundle`

## 完成标准

- [x] managed closure root 产出 `native-reference.lowering-plan.json`
- [x] `CodeGen` 成为 lowering family 与 plan 真源
- [x] emitter 不再做 lowering discovery / proof-shape detection
- [x] native proof 输出目录保留 `native-proof.plan.json` 镜像
- [x] focused regressions 通过
