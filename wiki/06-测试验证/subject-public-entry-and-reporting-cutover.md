objectId: subject-public-cutover
objectType: guide

# Subject Public Entry And Reporting Cutover

## 适用范围

- 当前 public subject object 只覆盖 `subject/HelloWorldObject`
- 当前 public host 只覆盖 `windows`
- 当前仍然围绕既有四个 Windows matrices 组织，不提前并入多 subject / perf

## 何时使用

当你要验证 subject 级结果，而不是 suite/module/system 级对象时，优先从这里进入。

推荐顺序：

1. 先看 registry 里是否已有 `subject/<subject-id>`
2. 有的话直接执行它的 `canonicalCommand`
3. 再通过 subject summary / matrix report / session summary 定位结果

## Public Object 与 Canonical Command

registry 中 `subject` 的固定规则是：

- object type: `subject`
- object id: `subject/<subject-id>`
- canonical command: `run test subject --id subject/<subject-id>`

当前首个对象为：

`subject/<subject-id>`，具体对象以 registry 当前结果为准。

虽然 parser 也接受 `test subject --subject HelloWorldObject`，但正式文档、脚本、排障记录都应优先写 canonical command。

## 结果定位顺序

执行 `run test subject --id subject/HelloWorldObject` 后，优先看三层结果：

### 1. Subject summary

聚合入口：

见 `subject-public-entry-and-reporting-runs.md` 中的 run-scoped `subject-report/summary.json` 规则。

它回答：

- 这次 subject 级请求最终状态是什么
- 哪个 goal 被请求
- 本次纳入的 matrix 有哪些
- 每个 matrix 的 `reportPath` 在哪里

### 2. Matrix report

单个 matrix 的诊断入口：

见 `subject-public-entry-and-reporting-runs.md` 中的 run-scoped `pipeline-report/report.json` 规则。

它回答：

- 哪个 stage 成为 terminal stage
- 哪个 bucket 是 terminal bucket
- 每个 stage 的 `manifestPath`、`reportPaths`、`primaryEvidencePaths`
- 本次失败是哪个 stage/bucket 造成的

### 3. Session summary / final-summary

统一入口仍会给出：

- `artifacts/logs/tests/<run-id>/summary.json`
- `artifacts/logs/tests/<run-id>/events.jsonl`

但 subject 结果的正式字段是：

- `subjectStatusCounts`
- `subjectResults`

不要把 `suiteResults` 当成 subject 结果的真源。

## TUI / Summary / Watch

当前 public surface 已经打通以下消费面：

- `run test summary`
  - 会显示 `Subjects:`
- `run test watch`
  - 会显示 `Subjects:`
  - highlight 区域会直接给出 `Subject summary`
- TUI test menu
  - 提供 `subject` selector

如果需要从终端里快速跳转，优先使用这些入口，而不是手工搜索 `artifacts/subjects/**`。

## 边界与禁忌

- 不要把 `subjectResults` 回灌为旧 `suiteResults` 真源
- 不要绕过 registry/canonical command 回到手工拼 legacy 命令
- 不要在没有额外设计的情况下把多 subject 批处理或 perf 目标混进当前 public cutover

## 最近更新

- `2026-04-06`：`subject/HelloWorldObject` 已进入 registry/public command/TUI，subject summary 与 session `subjectResults` 成为正式 public 结果入口。
> 2026-04-07 更新：本页旧内容中的单 subject 与非 run-scoped 路径已经过时。活动规则以 [`subject-public-entry-and-reporting-runs.md`](./subject-public-entry-and-reporting-runs.md) 为准。
