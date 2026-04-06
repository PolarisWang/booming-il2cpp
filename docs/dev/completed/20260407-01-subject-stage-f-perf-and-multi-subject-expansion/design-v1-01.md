# Subject Stage F Perf And Multi-Subject Expansion 设计 v1.01

Date: 2026-04-07
Status: plan-ready

## Scope Correction

This design originally framed Stage F around `GenericEcho` as the second formal subject.
The user later corrected the immediate requirement: all projects under `tests/smoke/input/` should migrate to `subjects/`.

Implemented scope correction:
- Added canonical subject/source trees for `HelloWorld`, `GenericEcho`, `ReflectionLite`, `PInvokeLite`, and `HostEmbeddingLite`.
- Switched smoke suite manifests and legacy run-manifest smoke commands to `subjects/*/source`.
- Kept `tests/smoke/input/*` in place as compatibility and long-term baseline assets rather than deleting them.

Design implication:
- The migration concern is now complete for the smoke-input set.
- Remaining Stage F perf/reporting work should be treated as follow-up on top of the new multi-subject baseline.

## 1. 目标

在现有 `subject` spine 之上完成 `Stage F` 的首版收口：

- 接入第二个正式 subject：`GenericEcho`
- 引入 subject-aware 的 `perf.dev` / `perf.release`
- 固定第二个 subject onboarding pattern
- 固定 perf baseline / report 在新 subject 模型下的落点

并保持以下原则不回退：

- 不删除现有测试工程
- 不把 `Stage F` 再拆成独立 perf follow-up
- 不把第二个 subject 重新绑回旧 `suite/module/system/pipeline` 作为正式真源

## 2. 非目标

本设计不包含：

- 多 subject 批量调度
- macOS / Linux 的首批 perf host
- 全量 smoke/gate/workflow 对象都迁成 subject
- 对 `HelloWorldObject` native proof worker 做彻底泛化

这些仍属于 `Stage F` 之后的扩展范围。

## 3. 已知现状

### 3.1 当前 subject spine 的强项

- manifest / planner / executor / reporting / registry / public command 已可稳定服务 `HelloWorldObject`
- `--goal` / `--matrix` 已能通过 `run test subject` 进入 planner
- session summary / TUI / registry 已能消费 subject 结果

### 3.2 当前 subject spine 的明显边界

- `subject_workers.py` 的 `build-target` / `runtime-observe` 明显绑定 `HelloWorldObject` 的 native proof CMake 路径
- 第二个 subject 目前还没有正式 manifest
- perf 仍停留在旧 `tests/perf/<suite>/baselines/<host>.json` 规则，没有 subject-aware 调度

## 4. 设计决定

## 4.1 第二个 subject 固定为 `GenericEcho`

`GenericEcho` 作为第二个正式 subject，原因是：

- 用户已经明确要求保留 `echo`
- 仓库内已有稳定的 managed smoke 输入
- 它适合承担 benchmark / perf subject 的首个样板

正式 canonical source 固定为：

- `subjects/GenericEcho/source/`

旧路径：

- `tests/smoke/input/GenericEcho/`

继续保留为 compatibility / long-term baseline 输入，不删除。

## 4.2 第二个 subject 采用 benchmark 类别

`GenericEcho` 的 subject category 固定为：

- `benchmark`

原因：

- 它不需要复用 `HelloWorldObject` 的 native proof correctness 路径
- 它可以直接承担 `perf.dev` / `perf.release`
- 它天然适合作为第二个 subject onboarding pattern 的最小样板

## 4.3 Stage F 先走 managed perf pipeline，而不是泛化 native proof pipeline

为了避免把 `HelloWorldObject` 的 native proof 特例路径硬泛化到第二个 subject，本轮 `GenericEcho` 引入新的 managed perf pipeline：

- `source-resolve`
- `host-input-build`
- `runtime-perf-collect`
- `perf-evaluate`
- `report-assemble`

设计含义：

- `GenericEcho` 的 perf 不依赖 `analysis/generated/build-target`
- 第二个 subject onboarding pattern 可以先在 managed runtime 上稳定
- `HelloWorldObject` 的 native correctness 路径保持原边界，不在本轮被迫泛化

## 4.4 perf baseline 改为 subject-aware 路径，但继续放在 `tests/perf/`

为了兼容项目既有 perf baseline 规则，同时让 baseline 跟 subject/matrix 对齐，本轮固定：

- baseline 根仍放在 `tests/perf/`
- 但 subject perf 的正式路径改成：
  - `tests/perf/subjects/<subject-id>/<matrix-id>/baselines/<host>.json`

原因：

- 保留“baseline 属于 repo 内正式测试资产”的既有规则
- 避免把 baseline 混进 `artifacts/`
- 让 subject perf 不再伪装成旧 suite perf

## 4.5 `perf.dev` 与 `perf.release` 共用一条 subject 入口

公开入口不新增新命令族，统一保持：

- `run test subject --id subject/GenericEcho --goal perf.dev`
- `run test subject --id subject/GenericEcho --goal perf.release`

这意味着：

- perf 在新模型下仍是 `subject + goal + matrix`
- 不再额外引入独立 `test perf subject ...` 语法

## 4.6 `perf.release` 允许扩出 `report/` 附件目录

当前 `HelloWorldObject` 的 matrix 报告只写：

- `matrices/<matrix-id>/report.json`

本轮对 `perf.release` 增量允许：

- `matrices/<matrix-id>/report/`

作为 perf 附件目录，例如：

- `samples.json`
- `summary.json`
- `baseline-compare.json`

同时继续保留：

- `matrices/<matrix-id>/report.json`

作为 machine-first 总入口。

## 4.7 第二个 subject onboarding pattern 只固定到 `GenericEcho`

本轮不批量把：

- `ReflectionLite`
- `PInvokeLite`
- `HostEmbeddingLite`

也一起迁成 subject。

而是只把 `GenericEcho` 固定成第二个正式 subject，用来沉淀：

- canonical source 落点
- benchmark subject manifest 写法
- managed perf pipeline
- perf baseline 规则

## 4.8 测试工程长期基线整理只做映射与归档语义，不做删除

Stage F 的“测试工程整理”在本轮只固定两件事：

- 哪些旧工程已经有新的 canonical subject
- 哪些旧工程继续作为 compatibility / baseline 资产保留

不做：

- 删除旧工程
- 大规模 rename

## 5. 模块边界

## 5.1 `subjects/`

新增：

- `subjects/GenericEcho/subject.manifest.json`
- `subjects/GenericEcho/source/`

负责第二个 subject 的正式 canonical source 与 pipeline 定义。

## 5.2 `subject_workers.py`

新增：

- `runtime-perf-collect`
- `perf-evaluate`

必要时抽出 managed runtime helper，避免把 `HelloWorldObject` native proof worker 的特例逻辑继续塞进同一个 worker 分支。

## 5.3 `subject_reporting.py`

增量支持：

- perf matrix report 的 metrics / baseline / compare 摘要字段
- `perf.release` 的附件索引

但仍保持：

- `report.json` 是 matrix machine-first 主入口

## 5.4 `perf.py`

从“旧 suite perf baseline helper”扩成“双入口 helper”：

- 旧 `suite` perf 继续可用
- 新 subject perf 走 subject-aware baseline path

## 5.5 registry / public command / TUI

现有 `subject` object 与 `test subject` 入口继续沿用，只做增量：

- registry 扫描到 `subject/GenericEcho`
- public summary/watch/TUI 能正确展示新的 perf goal / benchmark subject 结果

## 6. 范围收敛

本轮 Stage F 固定为：

- host: `windows`
- second subject: `GenericEcho`
- goals: `perf.dev` / `perf.release`

这样可以保证：

- `Stage F` 真正开始解决 perf 与 multi-subject
- 但不会同时把 host 扩展、多 subject 批处理和 native perf 一起带进来

## 7. 预期长期沉淀

当本计划未来执行闭环后，应把长期边界回写到：

- `docs/architecture/subject-test-framework-v1/perf-and-multi-subject-expansion-v1.md`

并按实际 perf / second-subject public surface 更新：

- `wiki/06-测试验证/`

## 8. 结论

`Stage F` 的首条实施主线可以固定为：

1. 先接入第二个正式 subject `GenericEcho`
2. 再为它建立 managed perf pipeline 与 subject-aware baseline
3. 最后收口 `perf.release` report 与第二个 subject onboarding pattern

只要保持这个顺序，就不会把 `HelloWorldObject` 的 native proof 特例路径误当成所有未来 subject 的唯一模板。
