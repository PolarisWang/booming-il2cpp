objectId: subject-public-entry-runs
objectType: guide

# Subject Public Entry 与 Run-Scoped Reporting

## Public Object

- object type：`subject`
- object id：`subject/<subject-id>`
- canonical command：`run test subject --id subject/<subject-id>`

不要再把 `subject/HelloWorldObject` 当成唯一 public subject。活动对象以 registry 当前结果为准。

## 结果定位顺序

执行 `run test subject --id subject/<subject-id>` 后，优先按下列顺序定位。

### 1. Subject summary

`artifacts/subjects/<subject-id>/runs/<run-id>/subject-report/summary.json`

### 2. Matrix report

`artifacts/subjects/<subject-id>/runs/<run-id>/matrices/<matrix-id>/pipeline-report/report.json`

### 3. Run report

- `artifacts/subjects/<subject-id>/runs/<run-id>/run-report/summary.json`
- `artifacts/subjects/<subject-id>/runs/<run-id>/run-report/events.jsonl`

公共日志入口仍会写：

- `artifacts/logs/tests/last.json`
- `artifacts/logs/tests/current.json`

但 subject 结果的正式字段是 `subjectResults` 与 `subjectStatusCounts`，不是 `suiteResults`。

## TUI / Summary / Watch

- `run test summary` 会显示 `Subjects:`
- `run test watch` 会显示 `Subjects:`
- TUI test menu 提供 `subject` selector

## 禁忌

- 不要再使用非 run-scoped 的 `artifacts/subjects/<subject-id>/subject-report/...`
- 不要再把 `.../matrices/<matrix-id>/report.json` 当成正式 matrix report 路径
- 不要绕过 registry/canonical command 手工拼 legacy 命令

## 最近更新

- `2026-04-07`：新增 run-scoped subject public entry 规则页。
