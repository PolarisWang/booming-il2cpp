# Phase 2 IL2CPP Core Semantics Batch A 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框语法跟踪。

**目标：** 补齐 Batch A 的核心运行时语义能力，并把 `SolutionCorePack` 的已有 proof/benchmark 收敛到新的 capability owner 模型。
**架构：** 先复用 `SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures` 与 `Benchmarks/CoreRuntimeBenchmarks` 的现有工程，不新增顶层 subject；通过细粒度 `ChaosUnitTest` / `ChaosBenchmark` 条目把 Batch A 能力收口进统一 declared metadata。
**技术栈：** C#、`Chaos.TestFramework`、`SolutionCorePack` retained subject、Python/pytest 回归、benchmark dashboard 现有管线。
**设计文档：** `docs/dev/completed/20260414-12-il2cpp-core-capability-development-roadmap/design-v1-01.md`

**预期知识沉淀：** 先留在本子任务目录；若 Batch A 收口稳定，再同步进 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考` capability matrix。

---

## 文件与职责

- `docs/dev/completed/20260414-15-phase-2-il2cpp-core-semantics-batch-a/STATUS.md`
  - 跟踪 Phase 2 状态、风险、摘要和下一步。
- `docs/dev/completed/20260414-15-phase-2-il2cpp-core-semantics-batch-a/plan-v1-01.md`
  - 记录 Batch A 的执行步骤与验收方式。
- `docs/dev/completed/20260414-15-phase-2-il2cpp-core-semantics-batch-a/notes/progress-*.md`
  - 记录本轮 audit 和实现节点。
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/**`
  - 承载 Batch A proof entry 的新增、拆分或 capability 重标注。
- `subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/**`
  - 承载 Batch A selective benchmark。
- `tests/unit/compatibility/test_solution_core_pack_subject.py`
  - 约束 `SolutionCorePack` retained source 结构与入口仍保持一致。
- `tests/tooling/run/test_subject_command.py`
  - 验证 declared proof / benchmark 入口对新增 Batch A 元数据仍能透传。
- `tests/unit/execution/test_subject_workers.py`
  - 验证 runtime perf / proof 执行路径对新增 workload 仍正确选取程序集和 entry。

## 执行步骤

- [x] Task 1: 基于 Phase 0 coverage ledger，产出 Batch A 10 个 capability item 的现状盘点与目标切片清单。
- [x] Task 2: 为 Batch A 的 declared metadata / discovery 行为补充 RED 测试，先锁定 capability 粒度与入口契约。
- [x] Task 3: 在 `SolutionCorePack` 中重标注已有 proof，并新增缺失的 Batch A proof entry。
- [x] Task 4: 按“selective”原则为 `TaskAndValueTaskFlow`、`TaskScheduling`、`MonitorAndLocking`、`GcSensitiveFlow` 评估并补齐 benchmark。
- [x] Task 5: 回归 declared discovery、subject command、subject workers 与 `SolutionCorePack` 兼容测试；必要时刷新 benchmark/docs。
- [x] Task 6: 回写 Phase 2 状态，并更新父 roadmap 的 child mapping。

## 当前 audit 基线

- 已存在可复用切片：
  - `AsyncAwaitProof.cs`
  - `ThreadingProof.cs`
  - `CrossBoundaryExceptionProof.cs`
  - `ObjectOpsProof.cs`
  - `Program.cs` 中的 `LoaderContract` 聚合入口
  - `AllocationBenchmark.cs`
- 已知待收敛问题：
  - `AsyncAwaitProof` 当前仍标到 `AsyncAwait`，未对齐 `TaskAndValueTaskFlow`。
  - `ThreadingProof` 当前仍标到 `Threading`，未拆分成 `TaskScheduling` / `MonitorAndLocking` / `ThreadLocalState`。
  - `CrossBoundaryExceptionProof` 标到 `CrossBoundaryException`，但其中实际包含 `finally/filter` 语义，可抽出 `FinallyAndFilter` 专用 proof。
  - `GcSensitiveFlow` 当前只有 benchmark，没有明确 proof。
  - `IteratorStateMachine`、`ResourceLifecycle` 仍缺显式 proof。

## 验证

- `python -m pytest -q tests/unit/compatibility/test_solution_core_pack_subject.py`
- `python -m pytest -q tests/tooling/run/test_subject_command.py tests/unit/execution/test_subject_workers.py`
- 根据改动范围追加 declared metadata / benchmark generator 相关回归。

## 状态维护

- 每完成一个 Batch A capability 切片，更新 `STATUS.md` 和 `notes/progress-*.md`。
- 如果进入真实代码修改，优先按 TDD 增量推进，不做无边界大批量重写。
