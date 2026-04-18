objectId: subject-perf-and-smoke-baselines-runs
objectType: guide

# Subject Perf / Smoke / Validation 路径

## Canonical Source

- `subjects/<subject-id>/source/`：正式 smoke/source 工程
- `subjects/<subject-id>/validation/unit/`：正式 unit test 工程
- `src/tools/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness/`：共享 perf harness；subject manifest 的 `validation.perf.project` 统一指向这里

当前统一约束：

- unit test 使用 C# `xUnit`
- perf validation 先由 C# `perf harness` 驱动

## Perf Baseline

正式路径：

`subjects/<subject-id>/baselines/perf/<matrix-id>/<host>.json`

不要再回到 `tests/perf/subjects/...`。

## 结果落点

对于 `perf.dev` / `perf.release`：

1. `artifacts/subjects/<subject-id>/runs/<run-id>/subject-report/summary.json`
2. `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/pipeline-report/report.json`
3. `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/validations/perf/summary.json`
4. `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/validations/perf/baseline-compare.json`
5. `artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/validations/perf/samples.json`

## 禁忌

- 不要把新的 smoke 真源写回 `tests/smoke/input/*`
- 不要把 subject perf baseline 写回旧 suite perf 路径
- 不要绕过 `run test subject --id subject/<subject-id>` 手工拼 perf 流程

## 最近更新

- 2026-04-07：新增 subject-owned smoke/unit/perf 与 run-scoped perf 结果规则页。
- 2026-04-18：共享 perf harness 从 src/validation/ 合并到 src/tools/Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness/。
