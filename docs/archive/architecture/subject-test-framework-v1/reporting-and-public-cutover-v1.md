# Subject Reporting And Public Cutover v1

Date: 2026-04-06
Status: active

## 1. 目标

把 `Stage D + Stage E` 已经稳定下来的长期边界写成正式文档，固定：

- `matrix -> subject -> session` 三层 reporting 真源
- canonical matrix event envelope
- `subject` 作为 registry / public command / TUI 的一等对象

## 2. Reporting 真源

### 2.1 Matrix report

每个 subject matrix 的诊断主入口固定为：

`artifacts/subjects/<subject-id>/matrices/<matrix-id>/report.json`

它由 `build/toolchains/run/testing/subject_reporting.py::build_matrix_report()` 组装，最少包含：

- `reportVersion`
- `runId`
- `subjectId`
- `matrixId`
- `goalId`
- `selection`
- `status`
- `terminalStageId`
- `terminalBucket`
- `stageResults`
- `artifactResults`
- `errors`

### 2.2 Subject summary

subject 聚合入口固定为：

`artifacts/subjects/<subject-id>/subject-report/summary.json`

它当前由 public subject dispatch 负责生成，并通过 `build_subject_summary()` / `build_subject_result()` 固定最少字段：

- `summaryVersion`
- `runId`
- `subjectId`
- `requestedGoalId`
- `status`
- `matrixStatusCounts`
- `matrixResults`

### 2.3 Session compatibility bridge

session 级 summary/event 继续保留 legacy `suite` 消费面，但 subject 结果的正式真源不回灌为 `suiteResults`。固定规则为：

- session `summary.json` 增量暴露 `subjectStatusCounts`
- session `summary.json` 增量暴露 `subjectResults`
- `final-summary` event payload 同步投影上述两组字段
- `suiteResults` 继续只描述 suite 对象，不承担 subject 真源职责

## 3. Canonical Matrix Events

`build/toolchains/run/testing/events.py::build_event()` 现在固定支持 subject/matrix envelope 字段：

- `streamScope`
- `subjectId`
- `matrixId`
- `goalId`
- `stageId`
- `bucket`
- `stageScope`

matrix canonical event type 固定为：

- `stage-reused`
- `stage-invalidated`
- `stage-start`
- `stage-finished`
- `matrix-summary`

这些事件由 `subject_executor.execute_plan()` 发出；worker 仍然不直接写 session `events.jsonl`。

## 4. Public Object Surface

### 4.1 Registry object

`build/toolchains/run/testing/registry.py` 现已固定：

- 新对象类型：`subject`
- object id 形态：`subject/<subject-id>`
- 当前首个 public subject：`subject/HelloWorldObject`
- `supportedHosts` 由 subject manifest 的 matrix `executionContext.hostPlatform` 归一化得出

### 4.2 Canonical command

public canonical command 固定为：

`run test subject --id subject/<subject-id>`

其中 `--id` 是正式 canonical selector；`--subject <subject-id>` 只作为输入便利，不是 canonical command。

### 4.3 Dispatch boundary

`build/toolchains/run/commands/test.py` 对 `test subject` 的固定职责为：

- 用 `selectors/manifest` 解析 subject selector
- 从 registry 解析 `selectedObject`
- 直接调用 `subject_planner + subject_executor`
- 写出 matrix report 与 subject summary
- 把 `subjectResults` 投影到 session `summary.json` / `final-summary`

它不再回退到 legacy suite session，也不把 subject 拆回手工拼接的旧 public 命令链。

## 5. TUI / Summary / Watch

`build/toolchains/run/tui.py` 的稳定行为现在包括：

- test menu 提供 `subject` selector 入口
- `test summary` 与 `test watch` 会渲染 `Subjects:`
- progress/highlight 区域会直接导航到 `Subject summary`

因此 public surface 已经可以从：

1. registry object
2. canonical command
3. session summary/watch/TUI

三条路径一致地定位 subject 结果。

## 6. 当前范围边界

本轮 public cutover 仍然固定在：

- `HelloWorldObject`
- 既有四个 Windows matrices
- `subject` 单对象 public entry

本轮明确不并入：

- 多 subject 批处理
- perf 目标
- 把 subject 真源回灌为 `suiteResults`

这些内容仍留给后续 `Stage F` 或新的 follow-up 任务处理。