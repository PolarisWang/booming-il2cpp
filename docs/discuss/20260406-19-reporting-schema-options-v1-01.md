# reporting schema 方案对比 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

在已经固定：

- artifact bucket 与 `*.manifest.json` 边界
- `planner output`
- `executor / stage worker` 输入输出边界
- `matrix execution result` 作为 reporting 的稳定输入

之后，下一步需要正式回答：

- `report.json` 应该保留哪些字段
- `subject-report/summary.json` 应该保留哪些聚合字段
- `artifacts/logs/tests/<run-id>/summary.json` 如何与新 subject 框架衔接
- reporting 到底应该是“轻索引层”还是“厚自包含层”

这一步很关键，因为它直接决定：

- 调试时是看一个文件，还是沿着多个 manifest 跳转
- executor 结果对象和最终报告对象是否高度重复
- 后续 TUI / watch / CI summary 的适配成本

## 2. 先固定不变的前提

无论选哪个方案，我建议下面几点都不再回退：

- `report.json`
  - 是单个 matrix 的最终结果
- `subject-report/summary.json`
  - 是单个 subject 的聚合结果
- `artifacts/logs/tests/<run-id>/summary.json`
  - 是单次 run/session 的聚合结果
- bucket 级正式产物仍然只存在于：
  - `source.manifest.json`
  - `host-input.manifest.json`
  - `analysis.manifest.json`
  - `generated.manifest.json`
  - `build.manifest.json`
  - `runtime.manifest.json`
- 普通 worker 不直接写 matrix 最终报告
- `events.jsonl` 继续保留

所以这次讨论的不是：

- 要不要有 report / summary

而是：

- report / summary 里面到底“装多少东西”

## 3. 三个候选方案

## 3.1 方案 A：薄报告，厚 manifest

核心思想：

- `report.json` 尽量薄
- 只保留最终状态、关键路径、少量 stage 摘要
- 详细证据全部回到 bucket manifest 和局部 `*.report.json`

### `report.json` 典型结构

```json
{
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-reference-trace",
  "goalId": "correctness.platform",
  "status": "ok",
  "terminalBucket": "runtime",
  "stageResults": [
    {
      "stageId": "analysis-frontend",
      "status": "ok",
      "planMode": "invalidated",
      "actionTaken": "executed"
    },
    {
      "stageId": "runtime-trace-compare",
      "status": "ok",
      "planMode": "executed",
      "actionTaken": "executed"
    }
  ],
  "artifactResults": [
    {
      "bucket": "analysis",
      "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json"
    },
    {
      "bucket": "runtime",
      "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/runtime.manifest.json"
    }
  ],
  "errors": []
}
```

### 优点

- 冗余最少
- schema 稳定
- executor 到 reporting 的映射最直接
- 后续 bucket manifest 演化时，不需要频繁同步 matrix report schema

### 问题

- 人工排障时，需要在多个 manifest / report 之间跳转
- TUI / CI 如果想直接展示关键证据，需要二次读取 bucket manifest
- `report.json` 本身的信息密度偏低

### 适用场景

- 目标是先把架构边界做干净
- 第一版优先“实现风险低”，不是“单文件可读性最强”

## 3.2 方案 B：平衡报告，薄聚合

核心思想：

- `report.json` 保留足够的人读调试信息
- 但不复制 bucket 内完整产物清单
- `subject summary` 和 `session summary` 继续偏薄，只做聚合与索引

这是“matrix 层信息稍厚，聚合层信息稍薄”的方案。

### `report.json` 典型结构

```json
{
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-reference-trace",
  "goalId": "correctness.platform",
  "selection": {
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
  },
  "status": "ok",
  "terminalStageId": "runtime-trace-compare",
  "terminalBucket": "runtime",
  "stageResults": [
    {
      "stageId": "analysis-frontend",
      "kind": "analysis-frontend",
      "bucket": "analysis",
      "status": "ok",
      "planMode": "invalidated",
      "actionTaken": "executed",
      "fingerprint": "f-analysis-2",
      "durationMs": 3280,
      "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
      "reportPaths": [
        "artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"
      ],
      "primaryEvidencePaths": [
        "artifacts/subjects/HelloWorldObject/shared/analysis/typed-il-ir.json"
      ]
    },
    {
      "stageId": "runtime-trace-compare",
      "kind": "runtime-trace-compare",
      "bucket": "runtime",
      "status": "ok",
      "planMode": "executed",
      "actionTaken": "executed",
      "durationMs": 540,
      "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/runtime.manifest.json",
      "reportPaths": [
        "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/trace-compare.report.json"
      ],
      "primaryEvidencePaths": [
        "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/trace.runtime.json"
      ]
    }
  ],
  "artifactResults": [
    {
      "bucket": "analysis",
      "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json"
    },
    {
      "bucket": "runtime",
      "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/runtime.manifest.json"
    }
  ],
  "errors": []
}
```

### `subject-report/summary.json` 典型结构

```json
{
  "subjectId": "HelloWorldObject",
  "requestedGoalId": "correctness.platform",
  "statusCounts": {
    "total": 4,
    "ok": 3,
    "fail": 1,
    "skip": 0,
    "aborted": 0
  },
  "matrixResults": [
    {
      "matrixId": "windows-reference-trace",
      "status": "ok",
      "terminalBucket": "runtime",
      "reportPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/report.json"
    }
  ]
}
```

### 优点

- `report.json` 足够支撑大部分人工排障
- subject/session 聚合仍然比较干净
- 与 executor 的 `matrix execution result` 对接自然
- 对现有 session summary 改动中等，不会过重

### 问题

- 相比方案 A，会多一层字段冗余
- `report.json` schema 会比最薄方案更容易增长

### 适用场景

- 第一版就希望 `report.json` 有较强可读性
- 同时不希望 subject/session summary 变成大而全转储文件

## 3.3 方案 C：厚报告，厚聚合

核心思想：

- `report.json` 尽量自包含
- `subject summary` 也尽量自包含
- session summary 尽量直接可供 CI / dashboard 消费

### 特征

- `report.json` 内直接保留大部分 `selection`
- 保留完整 `stageResults`
- 保留更详细的 `artifactResults`
- `subject summary` 内直接嵌各 matrix 的 condensed stage 信息
- session summary 内也直接保留更多 subject / matrix 细节

### 优点

- 单文件读体验最好
- CI / dashboard 读取一层文件就能拿到大量信息
- 对外部系统最友好

### 问题

- 重复数据最多
- schema 膨胀最快
- 容易出现 executor 结果对象、matrix report、subject summary、session summary 四层重复
- 一旦 bucket manifest 字段调整，多个报告层都可能同步改

### 适用场景

- 非常强调单文件消费
- 后续会接外部 dashboard / BI / 长期存档系统

但就当前阶段，我不建议第一版直接走这么重。

## 4. 我对三种方案的判断

## 4.1 方案 A 的本质

它最像：

- “report 是索引”

这个方案架构最干净，但可能太克制。

对你这个项目来说，测试框架重构后的一个重要目标其实是：

- 能快速定位 IL2CPP 问题在 source、host-input、analysis、generated、build、runtime 哪一层

如果 `report.json` 太薄，虽然理论边界漂亮，但一线排障体验会差一点。

## 4.2 方案 C 的本质

它最像：

- “report 是快照数据库”

这对未来也许有价值，但第一版会明显过重。

当前我们还在收敛：

- subject 模型
- matrix 模型
- stage graph
- worker 边界

这时如果先把 reporting 做得太厚，后面会一直跟着 schema 迁移。

## 4.3 方案 B 的本质

它最像：

- “matrix report 负责可读性，summary 负责聚合性”

我认为这和当前阶段最匹配。

原因有三点：

- matrix 才是定位问题的主战场，应该给足信息
- subject/session 更多是导航层，不需要复制一遍全部细节
- 和当前 `summary.json + events.jsonl` 的兼容成本可控

## 5. 推荐结论

我推荐：

- 选方案 B 作为默认方向

也就是：

- `report.json`
  - 适度自包含
  - 保留 `selection`、`stageResults`、`artifactResults`、`errors`
  - 每个 stage result 保留关键证据路径与 duration
- `subject-report/summary.json`
  - 只保留 `matrixResults + statusCounts`
- `session summary`
  - 继续保持偏薄
  - 记录 subject summary 路径与最终聚合统计

## 6. 如果你想进一步细分，还可以有两个变体

在方案 B 之下，还有两个常见变体：

### B1：更偏瘦

- `report.json` 保留 `selection`
- 但 `stageResults` 不放 `primaryEvidencePaths`

优点：

- 冗余更少

代价：

- 调试时还得多跳一次 bucket manifest

### B2：更偏厚

- `report.json` 保留 `selection`
- `stageResults` 保留 `primaryEvidencePaths`
- 对失败 stage 额外保留 `failure.code / failure.message / diagnostics`

优点：

- 失败定位更快

代价：

- 比 B1 略重

如果在 B 里再选一个，我更偏向：

- B2

因为这个项目的报告最主要服务的不是“漂亮归档”，而是：

- 快速定位 IL2CPP 管线哪一层出了问题

## 7. 与现有 session reporting 的衔接建议

无论 A/B/C，我都建议：

- 现有 `artifacts/logs/tests/<run-id>/summary.json` 先不推翻
- 只做增量扩展

推荐第一版 session summary 至少新增：

- `subjectResults`

示意：

```json
{
  "runId": "20260406-hello-001",
  "finalStatus": "fail",
  "subjectResults": [
    {
      "subjectId": "HelloWorldObject",
      "status": "fail",
      "subjectSummaryPath": "artifacts/subjects/HelloWorldObject/subject-report/summary.json"
    }
  ]
}
```

这样旧入口还能继续工作，新 subject 管线也能接进去。

## 8. 当前我建议你只做一个重要选择

这一轮最重要的选择其实只有一个：

- `report.json` 要走 A、B、还是 C

我的推荐顺序是：

1. B
2. A
3. C

如果你选 B，我下一步就继续把它正式收成：

- `report.json` schema v1
- `subject-report/summary.json` schema v1
- `session summary` 增量字段 v1

## 9. 当前可以正式固定的内容

这次至少可以先固定：

- reporting 采用三层：
  - matrix report
  - subject summary
  - session summary
- 不建议第一版走厚聚合方案 C
- 默认推荐走方案 B
- 如果选 B，建议进一步选 B2，而不是 B1

## 10. 下一步

如果你确认方案，我下一步就不再做泛讨论，而是直接把对应 schema 定成正式设计稿。
