# Phase 6 Capability Batch B 实现计划 v1.02

> 目标：在不提前卷入 `engine binding` 的前提下，把 `delegates/callbacks`、`exceptions` 与必要 `reflection/interop` closure 接回完整 `MainlineFeaturePack`，完成 `Phase A` 的 Batch B 收口。
>
> 架构原则：继续沿用“一个完整 `MainlineFeaturePack` + 多个 focused proof slice + 一个统一 perf 入口”的结构；所有 lowering/runtime 决策都以 generated native 工程的 `C++` 热路径性能为第一优先级。

## 变更范围

- `subjects/MainlineFeaturePack/subject.manifest.json`
- `subjects/MainlineFeaturePack/source/*`
- `tests/unit/run/test_subject_manifest_schema.py`
- `tests/unit/run/test_subject_planner.py`
- `tests/unit/run/test_phase6_capability_batch_b.py`
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- `src/managed/Chaos.IL2CPP.SemanticWorld/SemanticWorldStage.cs`
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/*`
- `src/native/runtime-core/*`
- `src/native/bootstrap/*`
- `build/toolchains/run/testing/subject_workers.py`
- `tests/unit/run/test_subject_workers_perf.py`

## 任务清单

- [x] 任务 1：把 `Phase 6` 切到执行态并冻结计划、状态与进展入口
  - 结果：`Phase 6` child 已进入执行态，并以 TDD 推进实现。

- [x] 任务 2：先做 manifest / planner 的 RED -> GREEN
  - 结果：新增 `windows-delegate-check`、`windows-exception-check`、`windows-reflection-interop-closure-check` 三条 focused matrix，以及对应 proof profile / `source.entry` override。

- [x] 任务 3：按 TDD 落地 delegate / exception / reflection-interop closure 能力
  - 结果：
    - `delegates` 采用 exact-target 优先直调，并保留 runtime helper fallback。
    - `exceptions` 采用 cold-path EH，不向热路径引入全链路 `status + payload` 传递。
    - `reflection/interop` 只补 `Phase A closure` 所需最小闭环，不重开并行 subject 主线。

- [x] 任务 4：跑 focused correctness、统一 perf，并回写 `Phase A closure`
  - focused correctness：
    - `windows-delegate-check`：run id `20260410-000311-windows-4450`
    - `windows-exception-check`：run id `20260410-001006-windows-0624`
    - `windows-reflection-interop-closure-check`：run id `20260409-235604-windows-2909`
  - perf：
    - 先前 run `20260410-001540-windows-b39b` 暴露首样本冷启动污染，唯一回归来自 `maxDurationMs`
    - 已将 native perf worker 改为显式记录 1 个 warmup sample，并仅让 `countedInSummary=true` 的热态样本进入 baseline compare
    - 复跑 `perf.profile`：run id `20260410-002943-windows-9cae`
    - 结果：`sampleCount=5`，`meanDurationMs=12.299`，`minDurationMs=10.393`，`maxDurationMs=15.146`，`regressionStatus=ok`
    - 冷启动样本 `127.884ms` 仍保留在 `perf.samples.json`，供后续复盘

## 验证记录

- [x] `python -m unittest tests.unit.run.test_subject_manifest_schema tests.unit.run.test_subject_planner tests.unit.run.test_phase5_capability_batch_a tests.unit.run.test_phase6_capability_batch_b tests.unit.run.test_native_reference_bootstrap_support tests.unit.run.test_native_runtime_core tests.unit.run.test_subject_workers_perf tests.unit.run.test_subject_perf_policy`
  - 结果：`38` tests，`OK`
- [x] `python -m unittest tests.unit.run.test_subject_workers_perf tests.unit.run.test_subject_perf_policy tests.unit.run.test_subject_executor tests.unit.run.test_subject_reporting tests.tooling.run.test_subject_command`
  - 结果：`14` tests，`OK`
- [x] `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-delegate-check --validation-profile proof-delegate --variant CHECK --json`
- [x] `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-exception-check --validation-profile proof-exception --variant CHECK --json`
- [x] `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal correctness.dev --matrix windows-reflection-interop-closure-check --validation-profile proof-reflection-interop-closure --variant CHECK --json`
- [x] `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile --validation-profile perf-profile --variant PROFILE --json`

## 收口结论

- `Phase 6` 技术目标已满足。
- `Phase A` 剩余缺口只剩 `Phase 7 engine binding tail roll-in`。
- 这仍然不等价于“完整 `C# -> C++ total solution` 已完成”。
- `contract` 下的 subject 污染清理仍是单独 follow-up，不在本任务内。
