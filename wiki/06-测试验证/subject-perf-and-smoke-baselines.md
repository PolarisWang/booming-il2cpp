objectId: subject-perf-and-smoke-baselines
objectType: guide

# Subject Perf And Smoke Baselines

## 适用范围

- 已迁移到 `subjects/*/source` 的 smoke 项目
- `subject` 模型下的 `perf.dev` / `perf.release`
- 当前已经正式接入的第二个 subject：`GenericEcho`

## Canonical Smoke Source

所有已经迁移的 smoke 项目，正式 source path 固定为：

`subjects/<subject-id>/source/`

当前覆盖：

- `HelloWorld`
- `GenericEcho`
- `ReflectionLite`
- `PInvokeLite`
- `HostEmbeddingLite`

`tests/smoke/input/<subject-id>/` 仍然保留，但只作为长期基线和兼容资产，不再是活动构建真源。

## Perf Baseline 路径

subject-aware perf baseline 的正式路径固定为：

见 `subject-perf-and-smoke-baselines-runs.md` 中的正式 baseline 路径规则。

排查 perf 回归时，优先查看这个 baseline，而不是回退到旧 suite perf 路径。

## Perf 结果定位顺序

执行：

`run test subject --id subject/<subject-id> --goal perf.dev`

或：

`run test subject --id subject/<subject-id> --goal perf.release`

之后，优先按这个顺序定位结果：

1. 见 `subject-perf-and-smoke-baselines-runs.md` 中的 run-scoped `subject-report/summary.json`
2. 见 `subject-perf-and-smoke-baselines-runs.md` 中的 run-scoped `pipeline-report/report.json`
3. `artifacts/subjects/<subject-id>/matrices/<matrix-id>/report/summary.json`
4. `artifacts/subjects/<subject-id>/matrices/<matrix-id>/report/baseline-compare.json`
5. `artifacts/subjects/<subject-id>/matrices/<matrix-id>/report/samples.json`

## 边界与禁忌

- 不要把新的 smoke 真源写回 `tests/smoke/input/*`
- 不要把 subject perf baseline 继续塞回旧 suite perf 路径
- 不要因为 canonical source 已迁移，就删除 legacy smoke 项目目录
- 不要绕过 `run test subject --id subject/<subject-id>` 手工拼 perf 流程
> 2026-04-07 更新：本页旧内容中的 `tests/perf/subjects/...` 与非 run-scoped report 路径已经过时。活动规则以 [`subject-perf-and-smoke-baselines-runs.md`](./subject-perf-and-smoke-baselines-runs.md) 为准。
