# Subject Perf And Multi-Subject Expansion v1

Date: 2026-04-07
Status: active

## 1. 目标

把 `Stage F` 已经稳定下来的长期边界写成正式文档，固定：

- smoke 项目的 canonical source 一律收敛到 `subjects/*/source`
- `tests/smoke/input/*` 继续保留，但只作为长期基线与兼容资产
- `subject` 模型下的 `perf.dev` / `perf.release` baseline、report 与 release attachments 路径

## 2. Canonical Smoke Subject Source

当前已迁移的 smoke 项目为：

- `HelloWorld`
- `GenericEcho`
- `ReflectionLite`
- `PInvokeLite`
- `HostEmbeddingLite`

它们的正式 source root 固定为：

`subjects/<subject-id>/source/`

对应的 legacy `tests/smoke/input/<subject-id>/` 仍然保留在仓库中，但不再作为 smoke suite 与 run-manifest 的活动 source path。

## 3. Subject-Aware Perf Baseline

`build/toolchains/run/testing/perf.py` 现已固定支持 subject-aware baseline 路径：

`tests/perf/subjects/<subject-id>/<matrix-id>/baselines/<host>.json`

规则为：

- baseline 按 subject 维度组织，而不是继续复用旧 suite perf 路径
- baseline 比较结果直接进入 matrix report
- 当前回归判定沿用“数值更高视为 regression”的规则

## 4. Perf Matrix Report And Release Artifacts

`build/toolchains/run/testing/subject_reporting.py` 现已固定：

- matrix report 增量包含 `metrics`
- matrix report 增量包含 `baseline`
- matrix report 增量包含 `regressionStatus`
- matrix report 增量包含 `performance`
- matrix report 增量包含 `releaseReportPaths`

`perf.release` 的正式附件目录固定为：

`artifacts/subjects/<subject-id>/matrices/<matrix-id>/report/`

当前最少会写出：

- `summary.json`
- `baseline-compare.json`
- `samples.json`

## 5. Public Command Boundary

当前 subject perf 仍沿用 canonical subject command：

`run test subject --id subject/<subject-id> --goal perf.dev`

`run test subject --id subject/<subject-id> --goal perf.release`

这意味着 perf 目标继续通过 subject planner / executor / reporting 闭环，而不是回退到 legacy suite perf 入口。

## 6. Multi-Subject Onboarding Boundary

`GenericEcho` 现在是第二个正式 subject。Stage F 固定出的 onboarding pattern 为：

1. 先落 `subjects/<subject-id>/subject.manifest.json`
2. 再落 `subjects/<subject-id>/source/`
3. 再接入 subject-aware planner / worker / report / baseline
4. legacy test project 保留，不作为真源

后续若扩展更多 subject，应复用这个 pattern，而不是继续把新需求强绑到 `HelloWorldObject` 特例路径。
