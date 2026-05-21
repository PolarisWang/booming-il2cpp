# Phase 0 Translation Gap Freeze And Batch Contract 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 执行本计划，并持续维护 `STATUS.md`、`docs/dev/ACTIVE.md` 与 `notes/progress-*.md`。

## 目标

- 把当前真实 translation gap 固化成 capability-first 执行台账。
- 为 `20260415-13` 到 `20260415-17` 提供稳定边界。

## 设计文档

- `docs/dev/in-progress/20260415-12-phase-0-translation-gap-freeze-and-batch-contract/design-v1-01.md`

## 涉及文件与职责

- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
  - opcode / reference / exception region decode gap
- `src/managed/Chaos.IL2CPP.Loader/Metadata/TypeProviders.cs`
  - function pointer / primitive type decode gap
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
  - exception region / carrier contract gap
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
  - opcode / dispatch / ABI / EH widening gap
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`
  - plan kind / workload ABI / template boundary gap
- `tests/unit/compatibility/test_phase4*.py`
  - 现有 AOT Core IR / NativeAOT 测试锚点
- `tests/unit/compatibility/test_loader_switch_opcode.py`
  - loader decode 现有锚点
- `docs/dev/in-progress/20260415-12-phase-0-translation-gap-freeze-and-batch-contract/gap-freeze-v1-01.md`
  - 本任务的核心产物

## 执行步骤

- [x] Task 1: 审计真实未支持点
  - 汇总 loader / type provider / IR lowering / planner / emitter 的当前 `NotSupportedException` 与 contract 边界
  - 锁定现有测试锚点
- [x] Task 2: 产出 gap freeze 台账
  - 形成 gap group / stage tag / child-task mapping / priority / deferred 判断
- [x] Task 3: 回写父 roadmap 状态
  - 更新父 roadmap 的 child mapping 与最近摘要
  - 切换 active task 与索引
- [x] Task 4: 收口并切下一阶段
  - 本 child task 标记 completed
  - Phase 1 child task 进入 ready / in-progress

## 本轮验证

- 本阶段以文档冻结为主，不要求运行项目测试。

## 完成结论

- `gap-freeze-v1-01.md` 已完成。
- 本 child task 可以归档，并切入 `20260415-13`。

## 状态维护要求

- 每完成一个 Task，都要更新：
  - `docs/dev/in-progress/20260415-12-phase-0-translation-gap-freeze-and-batch-contract/STATUS.md`
  - `docs/dev/ACTIVE.md`
  - `docs/dev/in-progress/20260415-12-phase-0-translation-gap-freeze-and-batch-contract/notes/progress-*.md`
  - `docs/dev/INDEX.md`
  - `docs/dev/in-progress/INDEX.md`
