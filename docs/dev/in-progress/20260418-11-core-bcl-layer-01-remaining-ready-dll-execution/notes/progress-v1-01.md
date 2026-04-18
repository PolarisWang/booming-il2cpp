# 进展记录 v1.01

## 时间

- 2026-04-18 14:56:35 +08:00
- 2026-04-18 19:41:19 +08:00

## 本轮完成

- 新建 `20260418-11-core-bcl-layer-01-remaining-ready-dll-execution` child task。
- 冻结 `core-bcl-layer-01` 剩余 `21` 个 ready DLL，覆盖 runtime-services、io-and-networking、reflection-and-metadata、text-and-encoding、unsafe-and-low-level 五类能力。
- 生成 `remaining-ready-scope-v1-01.json`，明确 selected assemblies、required gates、monitor signals 与聚合后的 formal verification aliases。
- 补齐 design / plan / STATUS 基础骨架，并重新建立 `docs/dev/ACTIVE.md` 指针。
- 将 `gameplay-core-ready` 并入当前 child，固定为 `15` 个 DLL 的阶段性验收目标。
- 把“DLL 完整 native 化”明确为 `surface complete -> semantic complete -> native executable evidence complete -> per-DLL certified complete` 四层判定。
- 将 `gameplay-core-ready` 的范围、优先顺序与 machine-readable milestone 落入 design / plan / scope JSON / roadmap。

## 当前判断

- `20260418-10` 已把 representative execution template 跑通，当前最有价值的推进不是再做新的代表切片，而是直接消化剩余 ready DLL。
- `gameplay-core-ready` 现在成为当前 child 的第一阶段验收口径，先证明基础游戏业务逻辑所需的核心 DLL 已经可以逐 DLL 收口。
- 下一步先补 `20260418-10` 的 `8` 个基线 DLL report 输入，再刷新 registry snapshot，并优先投影当前 `7` 个 gameplay core DLL。

## 验证

- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/design-v1-01.md -Raw | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/plan-v1-01.md -Raw | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/dll-full-certified-acceptance-path-v1-01.md -Raw | Out-Null`
  - 结果: 通过
- `Get-Content docs/dev/in-progress/20260418-11-core-bcl-layer-01-remaining-ready-dll-execution/remaining-ready-scope-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
  - 结果: 通过
## 2026-04-18 22:47 追加记录

- 已清理 `Chaos.IL2CPP.CodeGen.dll` 构建锁，并恢复 `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release` 的稳定通过。
- 已复验 async native-aot 相关最小单测集，`test_phase3_async_task_builder_native_aot.py` 与 `test_phase5_virtual_dispatch_native_aot.py -k task_and_valuetask_benchmark` 全部通过。
- 已确认 `TaskAndValueTaskFlowBenchmarkEntry::RunWorkload()` 在当前仓库 authority 下是 managed-only benchmark：源码标记 `Modes = ChaosExecutionMode.Managed`，declared benchmark 矩阵选择逻辑也会按 mode 选中 `windows-managed-perf`。
- 因此，这一条 benchmark 不再作为 native benchmark blocker 继续追；当前 child 的下一执行点重新回到 `20260418-10` 的 `8` 个 baseline DLL report 输入映射。

## 2026-04-18 22:47 验证

- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release`
  - 结果: 通过
- `python -m pytest tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py -q`
  - 结果: `2 passed`
- `python -m pytest tests/unit/compatibility/test_phase5_virtual_dispatch_native_aot.py -k task_and_valuetask_benchmark -q`
  - 结果: `1 passed, 5 deselected`
- `python build/toolchains/run/run.py test declared-benchmark --id "declared-benchmark/SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry::RunWorkload()" --json`
  - 结果: run id `20260418-224041-windows-ad67`，`finalStatus = ok`，仅 `windows-managed-perf`
- `python -m pytest tests/unit/registry/test_declared_registry_matrix_selection.py -q`
  - 结果: `3 passed`
- `python -m pytest tests/unit/performance/test_benchmark_subject_sources.py -k solution_core_pack_declares_internal_performance_slice_benchmarks -q`
  - 结果: `1 passed, 3 deselected`
