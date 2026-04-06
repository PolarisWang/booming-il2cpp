# event schema 对齐设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

在已经固定：

- `matrix report / subject summary / session summary` 三层 reporting schema
- executor / worker 的责任边界
- matrix / subject / session 三层产物落点

之后，下一步必须把 `events.jsonl` 也对齐。

否则后面会出现三类典型问题：

- `report.json` 说的是一套语义，`events.jsonl` 说的是另一套语义
- matrix / subject / session 三层事件流重复但不一致
- 当前 `test watch` / TUI 继续依赖旧事件，而新 subject 管线又另起炉灶

所以这一步要回答：

- 三层事件流分别服务谁
- 哪些事件是 canonical domain events
- 哪些事件只是给当前 watch/TUI 用的兼容投影
- 事件 envelope 最小字段应该是什么

## 2. 现有约束

从当前代码和测试看，已经有几个现实约束不能忽略：

- session 级 `events.jsonl`
  - 已经被 `test watch` / TUI 直接消费
- 当前 TUI 直接识别的事件类型主要是：
  - `session-start`
  - `progress`
  - `stage-start`
  - `warning`
  - `artifact`
  - `final-summary`
- 当前 TUI 通过 `payload.completedUnits / totalUnits`
  - 推导进度百分比
- 当前 test reporting 测试已经要求：
  - `final-summary` payload 不直接塞 `suiteResults`

这意味着：

- 我们不能简单把 session event stream 全部推倒重来
- 但也不应该让新 subject 事件永久困在旧 `suite` 命名里

## 3. 三个可选方向

## 3.1 方案 A：只保留一条 session 事件流

做法：

- 不引入 matrix / subject 专属事件流
- 所有信息都进 `artifacts/logs/tests/<run-id>/events.jsonl`

优点：

- 最简单
- 当前 watch/TUI 兼容最容易

问题：

- matrix 层排障太弱
- subject 内多个 matrix 会把一条 stream 搅在一起
- 不符合我们已经收敛出的 `matrix / subject / session` 三层模型

结论：

- 不推荐

## 3.2 方案 B：三层事件流都使用同一套 canonical 事件

做法：

- matrix / subject / session 全部统一成同一组 eventType
- 当前 TUI / watch 之后整体改造去适配新事件

优点：

- 架构最整齐
- 没有“兼容事件”这层技术债

问题：

- 对现有 watch/TUI 冲击太大
- 第一版实现时会把事件 schema 改造和 UI 改造强绑定

结论：

- 长远干净，但不适合第一版落地

## 3.3 方案 C：matrix / subject 采用 canonical 事件，session 保留兼容投影

做法：

- `matrix events.jsonl`
  - 作为 detailed canonical stream
- `subject events.jsonl`
  - 作为 aggregate canonical stream
- `session events.jsonl`
  - 继续服务统一 watch/TUI
  - 以兼容事件为主
  - 可增量附带部分 canonical 事件

优点：

- 不打断当前 watch/TUI
- matrix 层和 subject 层仍然能建立新的正规语义
- 实施切分合理，迁移风险最低

问题：

- 短期会存在“canonical + compatibility projection”两层语义

结论：

- 推荐采用方案 C

## 4. 推荐结论

正式推荐：

- 采用方案 C

并固定三条原则：

- matrix event stream
  - 是 stage 级执行细节的权威来源
- subject event stream
  - 是 subject 聚合过程的权威来源
- session event stream
  - 是 run/watch/TUI 的兼容与导航来源

这句话很重要，因为它决定了后面谁才是“真日志”：

- 真正用于定位 stage 行为的，看 matrix events
- 真正用于看 subject 汇总过程的，看 subject events
- 真正用于看统一入口实时进度的，看 session events

## 5. 三层事件文件的正式职责

## 5.1 matrix events

路径：

- `artifacts/subjects/<subject-id>/matrices/<matrix-id>/events.jsonl`

职责：

- 记录当前 matrix 的详细执行时间线
- 记录 stage start / reuse / invalidation / finish / warning / artifact
- 为后续排障和细粒度 UI 提供最直接数据源

不负责：

- 聚合多个 matrix
- 承担 session 统一 watch 兼容

## 5.2 subject events

路径：

- `artifacts/subjects/<subject-id>/subject-report/events.jsonl`

职责：

- 记录当前 subject 的 matrix 聚合过程
- 记录 `subject-start`
- 记录各 matrix 的完成摘要
- 记录 `subject-summary`

不负责：

- 转储每个 stage 的详细执行过程

也就是说：

- subject events 不镜像 matrix 的所有 stage 事件

## 5.3 session events

路径：

- `artifacts/logs/tests/<run-id>/events.jsonl`

职责：

- 继续服务 `test watch` / TUI / 当前 run 级用户体验
- 提供统一入口的实时进度与最终摘要
- 作为 subject 新模型接入旧入口的兼容桥接层

不负责：

- 成为 matrix 级详细排障日志的唯一真源

## 6. canonical event envelope

推荐新 subject 框架里的 canonical event 最小 envelope 为：

```json
{
  "schemaVersion": "2026-04-06",
  "streamScope": "matrix",
  "eventType": "stage-finished",
  "timestampUtc": "2026-04-06T08:20:00Z",
  "runId": "20260406-hello-001",
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-reference-trace",
  "goalId": "correctness.platform",
  "stageId": "analysis-frontend",
  "bucket": "analysis",
  "stageScope": "shared",
  "status": "ok",
  "payload": {}
}
```

## 6.1 顶层字段

- `schemaVersion`
  - 事件 schema 版本
- `streamScope`
  - `matrix | subject | session`
- `eventType`
  - 事件类型
- `timestampUtc`
  - UTC 时间戳
- `runId`
  - 当前 run
- `subjectId`
  - 当前 subject
- `matrixId`
  - 当前 matrix
- `goalId`
  - 当前 goal
- `stageId`
  - 当前 stage
- `bucket`
  - 当前 bucket
- `stageScope`
  - `shared | matrix`
- `status`
  - 当前事件对应的结果状态
- `payload`
  - 事件具体内容

## 6.2 与现有 `build_event()` 的兼容策略

当前 `build_event()` 只有：

- `runId`
- `suiteId`
- `stage`
- `status`

所以推荐未来改为 additive 扩展，而不是破坏性替换：

- 保留 `suiteId`
  - 仅用于 legacy suite/test pipeline
- 保留 `stage`
  - 仅作为兼容字段
- 新增：
  - `streamScope`
  - `subjectId`
  - `matrixId`
  - `goalId`
  - `stageId`
  - `bucket`
  - `stageScope`

也就是：

- 旧消费方继续可读
- 新 subject 事件不再被迫塞进 `suiteId / stage`

## 7. canonical eventType 集合

第一版推荐固定以下 canonical eventType：

- `subject-start`
- `matrix-start`
- `stage-start`
- `stage-reused`
- `stage-invalidated`
- `stage-finished`
- `artifact`
- `warning`
- `matrix-summary`
- `subject-summary`
- `final-summary`

说明：

- `progress`
  - 不作为新 subject 框架的 canonical event
  - 它保留为 session compatibility projection

## 8. 每类 canonical 事件的最小语义

## 8.1 `subject-start`

出现位置：

- `subject events`
- `session events`

最小 payload：

```json
{
  "requestedGoalId": "correctness.platform",
  "matrixIds": [
    "windows-dev-output",
    "windows-reference-trace"
  ]
}
```

## 8.2 `matrix-start`

出现位置：

- `matrix events`
- `session events`

最小 payload：

```json
{
  "pipelineId": "proof-runtime-trace",
  "expectedTerminalBucket": "runtime",
  "reportPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/report.json"
}
```

## 8.3 `stage-start`

出现位置：

- `matrix events`
- `session events`

最小 payload：

```json
{
  "order": 3,
  "completedStages": 2,
  "totalStages": 8,
  "activeUnit": "HelloWorldObject/windows-reference-trace/analysis-frontend"
}
```

## 8.4 `stage-reused`

出现位置：

- `matrix events`
- `session events` 可选

最小 payload：

```json
{
  "order": 3,
  "completedStages": 3,
  "totalStages": 8,
  "reason": "fingerprint-match",
  "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json"
}
```

说明：

- 它表达“这一步没有执行 worker，而是命中了复用”

## 8.5 `stage-invalidated`

出现位置：

- `matrix events`
- `session events` 可选

最小 payload：

```json
{
  "order": 3,
  "completedStages": 2,
  "totalStages": 8,
  "reason": "fingerprint-mismatch:driver-changed",
  "bucketRoot": "artifacts/subjects/HelloWorldObject/shared/analysis"
}
```

说明：

- 它表达“executor 对当前 bucket 应用了失效重建”
- 它不是 stage 执行完成事件

## 8.6 `stage-finished`

出现位置：

- `matrix events`
- `session events` 可选

最小 payload：

```json
{
  "order": 3,
  "completedStages": 3,
  "totalStages": 8,
  "planMode": "invalidated",
  "actionTaken": "executed",
  "durationMs": 3280,
  "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
  "reportPaths": [
    "artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"
  ],
  "primaryEvidencePaths": [
    "artifacts/subjects/HelloWorldObject/shared/analysis/typed-il-ir.json"
  ],
  "failure": null
}
```

## 8.7 `artifact`

出现位置：

- `matrix events`
- `session events`

最小 payload：

```json
{
  "path": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
  "artifactKind": "manifest"
}
```

这里要特别固定一条规则：

- `artifact` 不是为 bucket 内所有文件逐个打点
- 它只用于暴露用户或调试者真正会关心的 surfaced artifacts

例如：

- 当前 bucket manifest
- 局部 report
- 关键 trace / generated source
- 最终 matrix / subject / session summary

## 8.8 `warning`

出现位置：

- 三层都允许

最小 payload：

```json
{
  "message": "subject summary written with 1 failed matrix"
}
```

## 8.9 `matrix-summary`

出现位置：

- `matrix events`
- `subject events`
- `session events` 可选

最小 payload：

```json
{
  "status": "ok",
  "terminalBucket": "runtime",
  "reportPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/report.json",
  "eventsPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/events.jsonl"
}
```

## 8.10 `subject-summary`

出现位置：

- `subject events`
- `session events`

最小 payload：

```json
{
  "status": "fail",
  "matrixStatusCounts": {
    "total": 4,
    "ok": 3,
    "fail": 1,
    "skip": 0,
    "aborted": 0
  },
  "subjectSummaryPath": "artifacts/subjects/HelloWorldObject/subject-report/summary.json"
}
```

## 8.11 `final-summary`

出现位置：

- `session events`

它继续作为 session 的最终结束事件，但 payload 要与新的 session summary 对齐。

推荐最小 payload：

```json
{
  "finalStatus": "fail",
  "exitCode": 1,
  "errors": [],
  "summaryPath": "artifacts/logs/tests/20260406-hello-001/summary.json",
  "eventsPath": "artifacts/logs/tests/20260406-hello-001/events.jsonl",
  "sessionPath": "artifacts/logs/tests/20260406-hello-001/session.json",
  "consolePath": "artifacts/logs/tests/20260406-hello-001/console.log",
  "telemetryPath": "artifacts/logs/tests/20260406-hello-001/telemetry.json",
  "artifacts": [
    "artifacts/subjects/HelloWorldObject/subject-report/summary.json"
  ],
  "subjectStatusCounts": {
    "total": 1,
    "ok": 0,
    "fail": 1,
    "skip": 0,
    "aborted": 0
  },
  "subjectResults": [
    {
      "subjectId": "HelloWorldObject",
      "requestedGoalId": "correctness.platform",
      "status": "fail",
      "subjectSummaryPath": "artifacts/subjects/HelloWorldObject/subject-report/summary.json"
    }
  ]
}
```

## 9. session compatibility projection

这是当前最关键的兼容点，需要单独定死。

推荐规则：

- session stream 允许同时包含：
  - canonical 高层事件
  - 兼容 watch/TUI 的 projection 事件

其中兼容事件继续沿用：

- `session-start`
- `progress`
- `stage-start`
- `warning`
- `artifact`
- `final-summary`

## 9.1 `progress` 的定位

正式规定：

- `progress`
  - 不是 canonical event
  - 只是 session compatibility projection

它存在的唯一主要理由是：

- 当前 TUI / watch 依赖它的 `completedUnits / totalUnits / activeUnit`

## 9.2 `progress` 的最小 payload

第一版推荐继续兼容当前字段：

```json
{
  "completedUnits": 3,
  "totalUnits": 8,
  "activeUnit": "HelloWorldObject/windows-reference-trace/analysis-frontend",
  "suiteStatus": "ok",
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-reference-trace",
  "stageId": "analysis-frontend"
}
```

这里保留 `suiteStatus` 虽然命名不理想，但第一版不建议改名。

原因：

- 当前 TUI 直接读取这个字段

未来如果 TUI 完成新 subject 事件适配，再考虑把它升级成更合理的命名。

## 10. 单写者原则

为了避免事件顺序错乱，推荐正式固定：

- 每个 `events.jsonl` 文件只允许一个逻辑写者

具体是：

- matrix events
  - 由 executor 负责 append
- subject events
  - 由 executor 负责 append
- session events
  - 由 executor / test command orchestration 负责 append

reporting 不直接自己追加事件文件。

更合理的模式是：

- reporting 生成 summary/report 对象
- executor 拿到结果后统一写对应 summary event

这样不会出现：

- executor 和 reporting 同时往同一文件抢写

## 11. 与 reporting 三层 schema 的关系

正式固定这条引用关系：

- `matrix-summary` payload
  - 指向 matrix `report.json`
- `subject-summary` payload
  - 指向 subject `summary.json`
- `final-summary` payload
  - 指向 session `summary.json`

因此：

- summary/report 是稳定对象
- event 只是时间线入口

event 不负责重复 summary/report 的全部字段。

## 12. 第一版明确不做的事情

第一版不建议现在就做：

- 为每个 bucket 单独建立事件文件
- 在 subject events 里转储全部 stage 事件
- 让 worker 直接 append 事件文件
- 让 session events 成为 matrix 排障的唯一真源
- 彻底移除 `progress`

这些要么太细，要么会打断当前兼容面。

## 13. 当前可以正式固定的规则

这次可以正式固定为：

- 事件流采用三层：
  - matrix detailed stream
  - subject aggregate stream
  - session compatibility stream
- 采用方案 C：
  - matrix / subject canonical
  - session compatibility projection
- `progress` 不是 canonical event
- matrix events 是 stage 行为的权威来源
- subject events 是 subject 聚合过程的权威来源
- session events 主要服务 watch/TUI 与统一入口导航
- event envelope 需要增量支持：
  - `streamScope`
  - `subjectId`
  - `matrixId`
  - `goalId`
  - `stageId`
  - `bucket`
  - `stageScope`
- `artifact` 只暴露 surfaced artifacts，不穷举 bucket 内全部文件
- 每个事件文件坚持单写者原则

## 14. 这一步之后最自然的下一个问题

如果继续往下收敛，最自然的下一个问题是：

- registry / planner / executor / reporting 进入正式实施时，第一批模块切分与 API 形状如何定

也就是从“设计对象”继续走到：

- Python 模块边界
- 命令入口 cutover
- 首批实现顺序
