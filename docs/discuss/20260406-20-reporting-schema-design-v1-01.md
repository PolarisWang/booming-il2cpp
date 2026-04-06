# reporting schema 设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

基于上一轮已经对比过的 reporting 方案，这一版直接收敛为推荐方向：

- 采用方案 `B2`

也就是：

- `matrix report`
  - 适度自包含，足够支撑一线排障
- `subject summary`
  - 保持偏薄，只做 subject 聚合与导航
- `session summary`
  - 保持对现有 `summary.json` 的兼容，只做增量扩展

本文件要正式固定三层 schema：

- `matrices/<matrix-id>/report.json`
- `subject-report/summary.json`
- `artifacts/logs/tests/<run-id>/summary.json`

## 2. 为什么选 B2

这次不再回到 A / B / C 泛讨论，直接给出定稿理由。

### 不选 A

方案 A 的问题是：

- `report.json` 太薄
- 需要频繁跳转 bucket manifest
- 无法很好承接“快速定位 IL2CPP 卡在哪一层”的目标

### 不选 C

方案 C 的问题是：

- report / summary 层会过重
- schema 膨胀过快
- executor 结果、matrix report、subject summary、session summary 会多层重复

### 选 B2

方案 B2 的平衡点最好：

- 把诊断信息集中到 `matrix report`
- 把导航信息留在 `subject/session summary`
- 让后续 TUI / CI 既能快速读结果，也不用为每一层背负太多重复 schema

## 3. 三层职责正式固定

## 3.1 matrix report

职责：

- 表达单个 matrix 的最终执行结果
- 提供 stage 级诊断摘要
- 提供关键证据路径

不负责：

- 复制 bucket manifest 的完整内容
- 承担 subject 级或 session 级聚合

## 3.2 subject summary

职责：

- 聚合一个 subject 在本次请求中的 matrix 结果
- 提供从 subject 视角浏览 matrix 的入口

不负责：

- 内嵌完整 stage 级结果
- 复制每个 matrix report 的详细诊断

## 3.3 session summary

职责：

- 聚合一次 run/session 的最终结果
- 为旧入口保留现有 `summary.json` 语义
- 增量挂入 subject 级结果入口

不负责：

- 变成 subject / matrix 结果的完整转储

## 4. `matrices/<matrix-id>/report.json` schema v1

## 4.1 顶层结构

推荐正式结构：

```json
{
  "reportVersion": "v1",
  "runId": "20260406-hello-001",
  "generatedAt": "2026-04-06T16:20:00+08:00",
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-reference-trace",
  "goalId": "correctness.platform",
  "selection": {},
  "status": "ok",
  "terminalStageId": "runtime-trace-compare",
  "terminalBucket": "runtime",
  "stageResults": [],
  "artifactResults": [],
  "errors": []
}
```

## 4.2 顶层字段

- `reportVersion`
  - `report.json` 自身 schema 版本
- `runId`
  - 这份报告由哪个 session 产生
- `generatedAt`
  - 报告生成时间
- `subjectId`
  - 当前 subject
- `matrixId`
  - 当前 matrix
- `goalId`
  - 当前 goal
- `selection`
  - 当前 matrix 的执行语义摘要
- `status`
  - 当前 matrix 的最终结果
- `terminalStageId`
  - 最终停在哪个 stage
- `terminalBucket`
  - 最终证据停在哪个 bucket
- `stageResults`
  - 各 stage 的诊断摘要
- `artifactResults`
  - 各 bucket 的正式产物入口
- `errors`
  - 当前 matrix 的顶层错误摘要

## 4.3 `selection`

`selection` 保留“足够解释这条 matrix 是什么”的最小上下文：

```json
{
  "pipelineId": "proof-runtime-trace",
  "executionContext": {
    "hostPlatform": "windows-x64",
    "targetPlatform": "windows-x64",
    "toolchainProfile": "msvc-reference",
    "runtimeProfile": "reference-trace"
  },
  "validationIntent": {
    "validationMode": "trace",
    "adaptationLevel": "traceable",
    "expectedOutcome": "pass"
  }
}
```

这里不复制：

- 完整 planner output
- 完整 subject manifest

只保留能解释“这条 matrix 在验证什么”的语义信息。

## 4.4 `status`

第一版推荐允许：

- `ok`
- `fail`
- `aborted`

说明：

- `blocked` 只出现在 `stageResults[]`
- 不直接作为 matrix 顶层状态

因为：

- 一旦某个 stage fail，下游 blocked，本质上整个 matrix 顶层仍然是 `fail`

## 4.5 `stageResults[]`

推荐每项结构：

```json
{
  "stageId": "analysis-frontend",
  "kind": "analysis-frontend",
  "bucket": "analysis",
  "status": "ok",
  "planMode": "invalidated",
  "actionTaken": "executed",
  "invalidation": {
    "applied": true,
    "reason": "fingerprint-mismatch:driver-changed"
  },
  "fingerprint": "f-analysis-2",
  "durationMs": 3280,
  "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
  "reportPaths": [
    "artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"
  ],
  "primaryEvidencePaths": [
    "artifacts/subjects/HelloWorldObject/shared/analysis/typed-il-ir.json"
  ],
  "diagnostics": {
    "stdoutPath": null,
    "stderrPath": null
  },
  "failure": null
}
```

### 字段约束

- `status`
  - `ok | fail | blocked`
- `planMode`
  - `reused | executed | invalidated`
- `actionTaken`
  - `reused | executed`
- `invalidation`
  - 仅在 `planMode = invalidated` 时出现
- `fingerprint`
  - 当前 bucket 的目标 fingerprint
- `durationMs`
  - 即使 `reused` 也允许为 `0`
- `manifestPath`
  - 当前 bucket 的正式 manifest
- `reportPaths`
  - 当前 stage 产出的局部报告
- `primaryEvidencePaths`
  - 人工排障最常需要看的证据入口
- `diagnostics`
  - stdout/stderr 等原始诊断路径
- `failure`
  - 仅在 `status = fail` 时出现

### 为什么 `primaryEvidencePaths` 必须保留

这就是 B2 和 B1 的关键差异。

我建议正式保留它，因为用户读 `report.json` 时最常问的不是：

- manifest 在哪

而是：

- 这一步最关键的证据文件在哪

例如：

- `typed-il-ir.json`
- `aot-manifest.json`
- `HelloWorldObject.generated.cpp`
- `trace.runtime.json`

## 4.6 `failure`

失败项推荐最小结构：

```json
{
  "code": "frontend-driver-failed",
  "message": "frontend pipeline exited with non-zero status"
}
```

如果需要再补：

- `detailsPath`

也应指向 bucket 内已有诊断文件，而不是把大段文本塞进 `report.json`。

## 4.7 `artifactResults[]`

推荐每项结构：

```json
{
  "bucket": "analysis",
  "scope": "shared",
  "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
  "producedByStageId": "analysis-frontend"
}
```

作用是：

- 提供 bucket 级正式入口
- 不重复 bucket manifest 的内部字段

## 4.8 `errors[]`

顶层 `errors[]` 只保留 matrix 级错误摘要，不复制所有 stage failure：

```json
[
  {
    "stageId": "analysis-frontend",
    "code": "frontend-driver-failed",
    "message": "frontend pipeline exited with non-zero status"
  }
]
```

如果整条 matrix `ok`，这里通常是空数组。

## 5. `subject-report/summary.json` schema v1

## 5.1 顶层结构

推荐正式结构：

```json
{
  "summaryVersion": "v1",
  "runId": "20260406-hello-001",
  "generatedAt": "2026-04-06T16:20:05+08:00",
  "subjectId": "HelloWorldObject",
  "requestedGoalId": "correctness.platform",
  "status": "fail",
  "matrixStatusCounts": {
    "total": 4,
    "ok": 3,
    "fail": 1,
    "skip": 0,
    "aborted": 0
  },
  "matrixResults": []
}
```

## 5.2 顶层字段

- `summaryVersion`
  - `subject summary` 的 schema 版本
- `runId`
  - 当前 subject summary 对应的 session
- `generatedAt`
  - 汇总生成时间
- `subjectId`
  - 当前 subject
- `requestedGoalId`
  - 这次请求的目标维度
- `status`
  - 当前 subject 的最终状态
- `matrixStatusCounts`
  - 本次 subject 内 matrix 聚合统计
- `matrixResults`
  - 各 matrix 的结果入口

## 5.3 `status`

第一版推荐允许：

- `ok`
- `fail`
- `aborted`

规则：

- 只要任一 matrix `fail`，subject 顶层就是 `fail`
- 若无 fail，但有中断，则为 `aborted`
- 全部 ok 才是 `ok`

## 5.4 `matrixResults[]`

推荐每项结构：

```json
{
  "matrixId": "windows-reference-trace",
  "goalId": "correctness.platform",
  "status": "ok",
  "terminalBucket": "runtime",
  "executionContext": {
    "targetPlatform": "windows-x64",
    "toolchainProfile": "msvc-reference",
    "runtimeProfile": "reference-trace"
  },
  "reportPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/report.json"
}
```

这里保留少量 `executionContext` 是有价值的，因为：

- 同一个 subject 未来会扩展很多 matrix
- 只看 `matrixId` 不一定足够直观

但这里仍然不放：

- 完整 `selection`
- 完整 `stageResults`

否则它就会变厚。

## 5.5 为什么不用 `defaultMatrixId`

前面的早期草稿曾出现过：

- `defaultMatrixId`

但在新的 subject summary 语义下，它不是必须项。

因为这份文件回答的是：

- 这次 run 中这个 subject 跑了什么、结果如何

而不是：

- subject 的静态默认配置是什么

静态默认配置属于 manifest，不属于 run summary。

## 6. `artifacts/logs/tests/<run-id>/summary.json` 增量 schema v1

## 6.1 兼容原则

这里必须非常明确：

- 不推翻现有 session summary
- 不把 subject/matrix 硬塞成旧 `suiteResults`

原因：

- `suite`
  - 是旧测试对象模型
- `subject + matrix`
  - 是新测试对象模型

这两个概念不应该为了兼容而混成一个字段。

## 6.2 推荐策略

保留现有字段不动：

- `runId`
- `command`
- `hostPlatform`
- `finalStatus`
- `exitCode`
- `errors`
- `artifacts`
- `caseCounts`
- `trafficLightCounts`
- `phaseResults`
- `suiteResults`

在此基础上新增：

- `subjectStatusCounts`
- `subjectResults`

## 6.3 推荐结构

```json
{
  "runId": "20260406-hello-001",
  "command": "python -m build.toolchains.run testing run-subject HelloWorldObject",
  "hostPlatform": "windows",
  "finalStatus": "fail",
  "exitCode": 1,
  "errors": [],
  "artifacts": [],
  "caseCounts": {
    "total": 0,
    "ok": 0,
    "fail": 0,
    "skip": 0,
    "aborted": 0
  },
  "trafficLightCounts": {
    "green": {"total": 0, "ok": 0, "fail": 0, "skip": 0, "aborted": 0},
    "yellow": {"total": 0, "ok": 0, "fail": 0, "skip": 0, "aborted": 0},
    "red": {"total": 0, "ok": 0, "fail": 0, "skip": 0, "aborted": 0}
  },
  "phaseResults": [],
  "suiteResults": [],
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
      "matrixStatusCounts": {
        "total": 4,
        "ok": 3,
        "fail": 1,
        "skip": 0,
        "aborted": 0
      },
      "subjectSummaryPath": "artifacts/subjects/HelloWorldObject/subject-report/summary.json"
    }
  ]
}
```

## 6.4 `finalStatus` 如何计算

推荐保持现有 session summary 语义，只是在计算时把新 subject 结果也并入：

- 只要任一 legacy suite 或任一 subject 为 `fail`
  - `finalStatus = fail`
- 否则如果有中断
  - `finalStatus = fail`
- 全部成功
  - `finalStatus = ok`

也就是：

- 对 session 顶层来说，第一版仍然保持 `ok | fail`

不新增新的 session 顶层状态枚举。

## 7. 三层之间的引用关系

正式固定：

- `matrix report`
  - 引用 bucket manifest / 局部报告
- `subject summary`
  - 引用 matrix report
- `session summary`
  - 引用 subject summary

即：

```text
session summary
  -> subject summary
      -> matrix report
          -> bucket manifest / local report
```

这样可以保证：

- 导航链条稳定
- 各层职责清晰
- 不需要在 session 层复制全部 stage 细节

## 8. 第一版明确不做的事情

第一版不建议现在就做：

- 在 `subject summary` 内嵌 stage 级结果
- 在 `session summary` 内嵌 matrix 级结果细节
- 新增 `trafficLight` 到 subject / matrix schema
- 把 `subjectResults` 映射回 `suiteResults`
- 用 session summary 直接替代 matrix report 的诊断职责

这些都会把聚合层做得过厚。

## 9. 当前可以正式固定的规则

这次可以正式固定为：

- reporting 采用 `B2`
- `matrix report` 是诊断主入口
- `subject summary` 是 subject 聚合入口
- `session summary` 是 run 聚合入口
- `matrix report` 保留：
  - `selection`
  - `stageResults`
  - `artifactResults`
  - `errors`
- `stageResults[]` 正式保留：
  - `primaryEvidencePaths`
  - `failure`
  - `diagnostics`
- `subject summary` 不再保留 `defaultMatrixId`
- `session summary` 通过新增：
  - `subjectStatusCounts`
  - `subjectResults`
  来兼容 subject 新模型
- 不把 `subject/matrix` 强行塞进旧 `suiteResults`

## 10. 这一步之后最自然的下一个问题

如果继续往下收敛，最自然的下一个问题是：

- event schema 如何与这三层 reporting 对齐

也就是继续解决：

- matrix `events.jsonl` 至少有哪些事件
- subject `events.jsonl` 是否只做聚合事件
- session `final-summary` 事件怎样补 `subjectResults`
