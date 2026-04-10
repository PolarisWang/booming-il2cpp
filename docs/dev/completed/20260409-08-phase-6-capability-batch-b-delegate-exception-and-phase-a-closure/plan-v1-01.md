# Phase 6 Capability Batch B 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 在当前会话持续推进本计划。任务状态使用 `- [ ]` / `- [x]` 复选框维护，并同步更新 `STATUS.md`、`docs/dev/ACTIVE.md` 与 `notes/progress-*.md`。

**目标：** 在不提前卷入 `engine binding` 的前提下，把 `delegates/callbacks`、`exceptions` 与必要 `reflection/interop` closure 接回完整 `MainlineFeaturePack`，完成 `Phase A` 的 Batch B 收口。

**架构：** 继续沿用“一个完整 `MainlineFeaturePack` + 多个 focused proof slice + 一个统一 perf 入口”的结构，不回退到多 subject 分裂方案。所有新增 lowering/runtime 决策都优先保证 generated native 工程的 `C++` hot path 不被额外状态传递污染。

**技术栈：** `subject.manifest.json` / Python planner 测试、`Loader -> SemanticWorld -> Linker -> CodeGen`、native `runtime-core` / `bootstrap`、Windows `msbuild` + `python build/toolchains/run/run.py`。

**设计文档：** `docs/dev/completed/20260409-08-phase-6-capability-batch-b-delegate-exception-and-phase-a-closure/design-v1-01.md`

**预期知识沉淀：** `wiki/runtime/phase-a-batch-b-delegate-exception-and-closure.md`

---

## 变更范围

- `docs/dev/completed/20260409-08-phase-6-capability-batch-b-delegate-exception-and-phase-a-closure/*`
  - 冻结执行计划、状态与进展记录。
- `subjects/MainlineFeaturePack/source/*`
  - 增加 `DelegateProof.cs`、`ExceptionProof.cs`、`ReflectionInteropClosureProof.cs`，必要时调整 `Program.cs`。
- `subjects/MainlineFeaturePack/subject.manifest.json`
  - 增加 `Phase 6` focused validation profiles 与 matrices。
- `tests/unit/run/test_subject_manifest_schema.py`
  - 冻结 `Phase 6` manifest surface。
- `tests/unit/run/test_subject_planner.py`
  - 冻结 planner 对 `Phase 6` proof slice entry override 的选择。
- `tests/unit/run/test_phase6_capability_batch_b.py`
  - 冻结 `Phase 6` source/codegen/runtime surface 与关键决策。
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
  - 补齐 `exceptions` 与新 lowering plan 所需 contract 字段。
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
  - 补齐 delegate / exception proof 需要的 IL opcode、必要 EH/branch 信息装载。
- `src/managed/Chaos.IL2CPP.SemanticWorld/SemanticWorldStage.cs`
  - 增加 delegate / exception / closure 能力标签与 method role 裁决。
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
  - 输出 `delegates` / `exceptions` / closure 所需 optimization facts。
- `src/managed/Chaos.IL2CPP.CodeGen/*`
  - 增加 `Phase 6` lowering family、plan 生成、template 选择与 proof emitter 校验。
- `src/native/runtime-core/*`
  - 补齐 cold-path exception carrier 与必要 helper。
- `src/native/bootstrap/*`
  - 补齐 delegate fallback/helper、exception-aware invoke glue。

## 任务清单

- [ ] 任务 1：切换 `Phase 6` 到 executing 并冻结执行入口
  - 产物：`plan-v1-01.md`、`STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-v1-01.md`
  - 验证：人工校对任务状态一致；`ACTIVE.md` 指向本计划文件。

- [ ] 任务 2：先做 manifest/planner 的 RED -> GREEN
  - 产物：`subject.manifest.json`、`test_subject_manifest_schema.py`、`test_subject_planner.py`
  - 目标：增加 `windows-delegate-check`、`windows-exception-check`、`windows-reflection-interop-closure-check` 与对应 validation profile / `source.entry` override。
  - 验证：
    - `python -m unittest tests.unit.run.test_subject_manifest_schema tests.unit.run.test_subject_planner`

- [ ] 任务 3：按 TDD 落 delegate / exception / reflection-interop closure 能力
  - 产物：`test_phase6_capability_batch_b.py`、`subjects/MainlineFeaturePack/source/*`、`LoaderStage.cs`、`SemanticWorldStage.cs`、`LinkerStage.cs`、`ManagedClosureContracts.cs`、`NativeReferenceLoweringPlanner.cs`、`NativeReferenceProofEmitter.cs`、新增 templates、`runtime_core.*`、`bootstrap.cpp`
  - 目标：
    - `delegates`：exact-target 优先直调，保留 runtime helper fallback。
    - `exceptions`：cold-path EH，不引入全路径 `status + payload` 传递。
    - `reflection/interop`：只补 `Phase A closure` 所需最小闭环，不重开平行 subject。
  - 验证：
    - `python -m unittest tests.unit.run.test_phase6_capability_batch_b`
    - `python -m unittest tests.unit.run.test_subject_manifest_schema tests.unit.run.test_subject_planner tests.unit.run.test_phase5_capability_batch_a tests.unit.run.test_phase6_capability_batch_b tests.unit.run.test_native_reference_bootstrap_support tests.unit.run.test_native_runtime_core`

- [ ] 任务 4：运行 focused correctness、统一 perf，并回写 `Phase A closure`
  - 产物：新的 run artifacts、`STATUS.md`、`notes/progress-*.md`、父 roadmap 状态更新
  - 目标：跑通三条 focused correctness 与统一 `perf.profile`，明确 `Phase A` 剩余缺口只剩 `engine binding` tail roll-in。
  - 验证：
    - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-delegate-check --validation-profile proof-delegate --variant CHECK --json`
    - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-exception-check --validation-profile proof-exception --variant CHECK --json`
    - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-reflection-interop-closure-check --validation-profile proof-reflection-interop-closure --variant CHECK --json`
    - `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json`
    - 必要的 Python 单元组合套件补跑并记录结果。

## 执行约束

- 不把当前路线表述成完整 `C# -> C++ total solution`。
- 一切设计继续服从 generated native 工程 `C++` 性能优先。
- `exceptions` 不能再降级到未来再做。
- `engine binding` 保持在 roadmap 最后阶段。
- `contract` 与 `subject contract` 的后续清理不在本任务内。
