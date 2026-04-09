# Phase 5 Capability Batch A Dispatch Generic Layout And Array Boxing 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划，并严格按 TDD 先补 RED 再推进实现。
> **目标：** 在不拆分 `MainlineFeaturePack` 的前提下，落地 Batch A 的 `dispatch`、`closed generics/layout`、`arrays/boxing`，并继续以 generated native 工程的 `C++` 性能为第一目标。
> **架构：** 先补 `subject planner` 的 matrix-scoped `source.entry` override 与 `MainlineFeaturePack` capability slices，再补 loader/linker/codegen/runtime 的最小语义闭环，最后跑 focused subject 验证与状态回写。
> **技术栈：** C#、Python unittest、dotnet、CMake、native runtime/bootstrap、Scriban template
> **设计文档：** `docs/dev/in-progress/20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing/design-v1-01.md`
> **预期知识沉淀：** 暂不写 wiki；等 `Phase 5` 稳定后再统一沉淀完整 subject capability-slice / entry-override 规则。

## 目标文件与职责

- `tests/unit/run/test_subject_planner.py`
  - 为 matrix-scoped `source.entry` override 补 RED
- `tests/unit/run/test_subject_manifest_schema.py`
  - 锁定 `MainlineFeaturePack` 的 capability slice matrices / manifest 结构
- `tests/unit/run/test_phase5_capability_batch_a.py`
  - 锁定 Batch A 的 source slice、loader opcode、lowering family 与 runtime helper 边界
- `build/toolchains/run/testing/subject_planner.py`
  - 对 matrix 级 `source.entry` 做 selection overlay
- `subjects/MainlineFeaturePack/subject.manifest.json`
  - 新增 capability slice matrices 与 proof profile
- `subjects/MainlineFeaturePack/source/*`
  - 新增 dispatch / generic-layout / array-boxing proof slices 与 perf harness 入口
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
  - 增加 `newarr` / `box` / 必要数组 store opcode 解码
- `src/managed/Chaos.IL2CPP.SemanticWorld/SemanticWorldStage.cs`
  - 暴露 Batch A 所需 method role / capability 信号
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
  - 补齐 `DispatchFact` / `LayoutFact` / boxing 与 array 事实
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
  - 如需要，扩展 lowering plan artifact 的 Batch A 字段
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs`
  - 新增 Batch A lowering family 与 plan 生成
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`
  - 消费新的 managed lowering plan kind
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/*`
  - 为 dispatch / arrays-boxing 新增或扩展 native proof 模板
- `src/native/runtime-core/runtime_core.cpp`
  - 把 `ArrayNew` 从 stub 变成 Batch A 所需的最小实现
- `src/native/bootstrap/bootstrap.cpp`
  - 把 `BoxValue` / `UnboxValue` / `ResolveVirtualMethod` / `InvokeVirtual` 补到 Batch A 需要的最小实现

## Chunk 1：Planner / Manifest RED

- [ ] 在 `tests/unit/run/test_subject_planner.py` 新增 matrix-level `source.entry` override RED
- [ ] 在 `tests/unit/run/test_subject_manifest_schema.py` 新增 `MainlineFeaturePack` capability slice matrix RED
- [ ] 运行：
  - `python -m unittest tests.unit.run.test_subject_planner tests.unit.run.test_subject_manifest_schema`
- [ ] 确认失败点集中在 planner 仍直接复制 manifest 根部 `source`，以及 `MainlineFeaturePack` manifest 尚未声明 capability slice matrices

## Chunk 2：MainlineFeaturePack Slice 接入

- [ ] 为 `MainlineFeaturePack` 新增 dispatch / generic-layout / array-boxing proof slice source
- [ ] 在 `subject.manifest.json` 新增 capability slice matrices 与对应 validation profile
- [ ] 为 `Phase 5` 新增 subject-level RED：proof slice / matrix 命名与 perf harness 入口
- [ ] 运行：
  - `python -m unittest tests.unit.run.test_phase5_capability_batch_a`
- [ ] 确认失败点集中在 loader / lowering / runtime 还不支持 Batch A 语义

## Chunk 3：Loader / Semantic / Linker RED-GREEN

- [ ] 扩展 loader 支持 `newarr`、`box` 与最小 array store 路径
- [ ] 补充 SemanticWorld 的 capability / method-role 识别
- [ ] 扩展 Linker 的 `DispatchFact` / `LayoutFact`，让 generic-layout 与 array/boxing 成为显式 optimization facts
- [ ] 运行：
  - `python -m unittest tests.unit.run.test_phase5_capability_batch_a`
- [ ] 确认 Typed IL / Semantic / OptimizationFacts 已满足 Batch A lowering 选择前提

## Chunk 4：CodeGen / Runtime / Native Proof GREEN

- [ ] 为 dispatch 增加“优先去虚，保留 runtime virtual fallback”的 lowering path
- [ ] 为 arrays/boxing 增加独立 lowering family 与 template
- [ ] 把 runtime/bootstrap helper 从 stub 补到 Batch A proof 所需的最小实现
- [ ] 运行：
  - `python -m unittest tests.unit.run.test_phase5_capability_batch_a`
  - `dotnet test subjects/MainlineFeaturePack/validation/unit/MainlineFeaturePack.Subject.UnitTests/MainlineFeaturePack.Subject.UnitTests.csproj -c Release`

## Chunk 5：Focused Subject Verification 与状态回写

- [ ] 运行 Batch A focused subject correctness：
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-dispatch-check --validation-profile proof-dispatch --variant CHECK --json`
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-generic-layout-check --validation-profile proof-generic-layout --variant CHECK --json`
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-array-boxing-check --validation-profile proof-array-boxing --variant CHECK --json`
- [ ] 运行统一 perf 入口：
  - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json`
- [ ] 如 focused suites 通过，再补充项目级 required suite
- [ ] 回写 `STATUS.md`、`ACTIVE.md`、`notes/progress-*.md` 与索引；满足条件后归档为 completed

## 验证命令

- [ ] `python -m unittest tests.unit.run.test_subject_planner tests.unit.run.test_subject_manifest_schema`
- [ ] `python -m unittest tests.unit.run.test_phase5_capability_batch_a`
- [ ] `dotnet test subjects/MainlineFeaturePack/validation/unit/MainlineFeaturePack.Subject.UnitTests/MainlineFeaturePack.Subject.UnitTests.csproj -c Release`
- [ ] `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-dispatch-check --validation-profile proof-dispatch --variant CHECK --json`
- [ ] `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-generic-layout-check --validation-profile proof-generic-layout --variant CHECK --json`
- [ ] `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-array-boxing-check --validation-profile proof-array-boxing --variant CHECK --json`
- [ ] `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json`

## 完成标准

- [ ] planner 支持 matrix-scoped `source.entry` override
- [ ] `MainlineFeaturePack` 内存在独立 capability proof slices，而不是回退成 mega proof entry 或多 subject
- [ ] Batch A 的 loader / linker / codegen / runtime 最小闭环打通
- [ ] `dispatch`、`closed generics/layout`、`arrays/boxing` 都有 focused correctness 入口
- [ ] 统一 perf 入口仍留在 `MainlineFeaturePack`
- [ ] focused verification 通过
