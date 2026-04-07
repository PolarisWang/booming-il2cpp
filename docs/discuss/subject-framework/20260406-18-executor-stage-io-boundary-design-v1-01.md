# executor / stage worker 输入输出边界设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

在已经固定：

- `subject.manifest.json` 增量 schema
- manifest 消费边界
- planner 输出对象
- artifact bucket 命名与落点
- stage reuse / invalidation 规则

之后，下一步必须补上：

- executor 的最小输入边界
- 单个 stage worker 的最小输入边界
- 单个 stage worker 的最小输出边界
- success / fail / reuse / invalidation 在 executor 层如何落成正式结果

否则后面很容易再次退化成：

- planner 输出了 plan
- 但 executor 还是重新解释 graph / reuse / report
- worker 还是偷偷改 matrix report

这会把边界重新打散。

## 2. 结论

推荐正式固定四层边界：

- `planner output`
  - executor 的主输入
- `stage worker request`
  - 单个 worker 的唯一执行输入
- `stage worker result`
  - 单个 worker 的唯一返回对象
- `matrix execution result`
  - executor 交给 reporting / report-assemble 的稳定结果对象

并固定三个原则：

- worker 只处理“一个 stage + 一个 bucket”
- worker 只写“当前 bucket 内的正式产物与局部报告”
- executor 负责 orchestration、cleanup、reuse 兑现、事件记录、失败阻断

最重要的一点是：

- `invalidated` 不是 worker mode
- 它是 executor 在执行前应用的一次“重建原因”

也就是：

- `reused`
  - 不调用 worker
- `executed`
  - 直接调用 worker
- `invalidated`
  - 先清理当前 bucket，再调用 worker

## 3. executor 的直接输入

executor 的最小直接输入推荐只有两部分：

- `plan`
  - 即 planner 已经展开好的执行计划
- `runContext`
  - 本次 run/session 的最小上下文

推荐结构：

```json
{
  "runContext": {
    "runId": "20260406-hello-001",
    "startedAt": "2026-04-06T15:00:00+08:00",
    "host": {
      "os": "windows",
      "arch": "x64"
    }
  },
  "plan": {
    "planVersion": "v1",
    "request": {},
    "selection": {},
    "artifactsRoot": {},
    "stagePlan": []
  }
}
```

说明：

- executor 不应再去读取 `subject.manifest.json` 重新做选择
- executor 不应再去重新展开 `executionPipelines`
- executor 不应再重新判断 default goal / default matrix

这些都已经是 planner 的责任。

## 4. stage worker request

## 4.1 目标

单个 worker 收到的请求，应该已经是：

- 单 stage
- 单 bucket
- 单一写入范围
- 已决策完 reuse / invalidation 语义

worker 不应该再自己去理解：

- 整体 graph
- 这个 matrix 合不合法
- 要不要回退到别的 pipeline

## 4.2 推荐最小结构

```json
{
  "runContext": {
    "runId": "20260406-hello-001"
  },
  "selection": {
    "subjectId": "HelloWorldObject",
    "goalId": "correctness.platform",
    "matrixId": "windows-reference-trace",
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
  "stage": {
    "stageId": "analysis-frontend",
    "kind": "analysis-frontend",
    "scope": "shared",
    "bucket": "analysis",
    "dependsOn": ["host-input-build"],
    "planMode": "invalidated",
    "requestedFingerprint": "f-analysis-2",
    "upstreamFingerprints": {
      "host-input": "f-host-1"
    },
    "reuse": {
      "decision": "mismatch",
      "reason": "fingerprint-mismatch:driver-changed"
    }
  },
  "paths": {
    "bucketRoot": "artifacts/subjects/HelloWorldObject/shared/analysis",
    "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
    "reportPaths": [
      "artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"
    ]
  },
  "upstream": {
    "host-input": {
      "manifestPath": "artifacts/subjects/HelloWorldObject/shared/host-input/host-input.manifest.json"
    }
  }
}
```

## 4.3 这份 request 的关键含义

- `selection`
  - 表示这次执行的大语义上下文
- `stage`
  - 表示这次只执行哪一个 stage
- `paths`
  - 表示这次只允许写到哪里
- `upstream`
  - 表示上游正式输入从哪里读

因此 worker 的输入模型应该建立在：

- 读上游 `*.manifest.json`
- 写当前 bucket `*.manifest.json`

而不是：

- 直接扫整个 `artifacts/subjects/<subject-id>/`

## 4.4 worker 的写权限边界

推荐第一版明确限制：

- worker 只能写自己的 `bucketRoot`
- worker 只能写 planner 预先声明的 `manifestPath`
- worker 只能写 planner 预先声明的 `reportPaths`

worker 不应写：

- 别的 bucket
- `matrices/<matrix-id>/report.json`
- `subject-report/summary.json`
- session `summary.json`

这点必须定死，否则最后还是会变成“谁都能顺手改报告”。

## 5. stage worker result

## 5.1 推荐最小结构

```json
{
  "status": "ok",
  "bucketManifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
  "reportPaths": [
    "artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"
  ],
  "primaryEvidencePaths": [
    "artifacts/subjects/HelloWorldObject/shared/analysis/typed-il-ir.json",
    "artifacts/subjects/HelloWorldObject/shared/analysis/aot-manifest.json"
  ],
  "metrics": {
    "durationMs": 3280
  },
  "diagnostics": {
    "stdoutPath": null,
    "stderrPath": null
  },
  "failure": null
}
```

## 5.2 最小字段语义

- `status`
  - `ok | fail`
- `bucketManifestPath`
  - 本 stage 写出的正式 bucket manifest
- `reportPaths`
  - 本 stage 写出的局部校验报告
- `primaryEvidencePaths`
  - reporting / 人工排障最常看的关键证据
- `metrics`
  - 至少保留 `durationMs`
- `diagnostics`
  - 指向 stdout/stderr 等原始诊断落点
- `failure`
  - 失败时的结构化原因

推荐 `failure` 最小结构：

```json
{
  "code": "frontend-driver-failed",
  "message": "frontend pipeline exited with non-zero status"
}
```

## 5.3 worker result 不应包含什么

第一版不建议 worker result 直接包含：

- 整个 matrix 的最终结论
- 下游 stage 是否还要跑
- graph 级 invalidation 传播
- session / subject 聚合结果

原因：

- 这些都是 executor 或 reporting 的职责

## 6. 谁负责写 fingerprint

这里容易混乱，需要明确：

- planner 负责算出“本次请求想要的 fingerprint”
- worker 负责把这个 fingerprint 写入当前 bucket manifest
- executor 负责在 stage 完成后校验 manifest 已正确写出

也就是：

- fingerprint 的语义归 planner
- fingerprint 的落盘动作由 worker 完成
- fingerprint 的验收由 executor 完成

这样不会出现：

- worker 自己发明 fingerprint 口径

## 7. executor 对三种计划模式的兑现方式

## 7.1 `reused`

executor 行为：

- 不调用 worker
- 直接读取现有 bucket manifest
- 写一条 `stageResult`
- 事件流记录 `stage-reused`

这时最小结果应包含：

- `planMode = reused`
- `actionTaken = reused`
- `status = ok`

## 7.2 `executed`

executor 行为：

- 不做额外 cleanup
- 直接调用 worker
- 验收当前 bucket manifest
- 写 `stageResult`

适用场景：

- 首次执行
- bucket 不存在
- 但不存在旧产物需要先删

## 7.3 `invalidated`

executor 行为：

- 记录 invalidation 事件
- 清理当前 bucket
- 再调用 worker
- 验收当前 bucket manifest
- 写 `stageResult`

这里要特别强调：

- `invalidated` 不是最终动作
- 它一定会落成一次新的 `executed`

所以在 executor 结果里，推荐拆成两个概念：

- `planMode`
  - `reused | executed | invalidated`
- `actionTaken`
  - `reused | executed`

这样才不会出现语义歧义。

## 8. 失败与阻断规则

## 8.1 stage 执行失败

如果 worker 返回 `fail`，推荐 executor：

- 保留当前 bucket 中已经写出的诊断文件
- 不做失败后的二次清理
- 记录 `stage-failed`
- 将所有下游 stage 标记为 `blocked`

原因：

- 失败现场通常就是最重要的排障证据

## 8.2 下游 `blocked`

`blocked` 不是 worker 返回的状态，而是 executor 计算出的结果。

推荐 `stageResult.status` 固定支持：

- `ok`
- `fail`
- `blocked`

不要把：

- `reused`
- `invalidated`

混进 `status`，因为它们不是结果状态，而是执行方式。

## 9. matrix execution result

executor 在跑完整条 matrix 后，应该产出一份稳定的结果对象，供 reporting 或 `report-assemble` 消费。

推荐结构：

```json
{
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-reference-trace",
  "goalId": "correctness.platform",
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
      "invalidation": {
        "applied": true,
        "reason": "fingerprint-mismatch:driver-changed"
      },
      "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
      "reportPaths": [
        "artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"
      ],
      "fingerprint": "f-analysis-2",
      "durationMs": 3280
    }
  ],
  "errors": []
}
```

说明：

- 这份对象是 reporting 的稳定输入
- reporting 可以把它落成 `report.json`
- 也可以继续聚合成 `subject-report/summary.json`

## 10. `report-assemble` 在这里的边界

当前设计里已经有：

- `report-assemble`

这个 stage。

在新的边界下，更合理的理解应该是：

- 普通 worker 不接触 matrix 最终报告
- `report-assemble` 只消费 `matrix execution result`
- 它本质上是一个终端汇总步骤，不应反向改写前面 stage 的语义

也就是说，即使后面还保留 `report-assemble` 这个 stage 名称，也应满足：

- 普通 stage worker 只产 bucket-local 结果
- matrix 级 `report.json` 只由终端汇总层产出

## 11. executor 真正拥有的落盘职责

推荐第一版把落盘职责拆成三类：

- worker 写：
  - bucket manifest
  - bucket 内局部报告
  - bucket 内日志/诊断文件
- executor 写：
  - matrix 级 `events.jsonl`
  - 运行期 `stageResults` 原始结构
- reporting / report-assemble 写：
  - matrix `report.json`
  - subject `summary.json`
  - session `summary.json`

其中：

- 如果第一版不想单独引入新的 `stage-results.json`
- 也可以先让 executor 以内存对象把 `matrix execution result` 直接传给 reporting

这比强行新增一层文件更稳。

## 12. 第一版不建议新增的东西

第一版不建议现在就加：

- 每个 stage 一个独立 `stage-result.json`
- worker 自己直接 append `events.jsonl`
- worker 直接更新 subject summary
- worker 自己决定下游是否跳过

原因：

- 这些都会把 orchestration 权限重新散回执行端

## 13. 当前可以正式固定的规则

这次可以正式固定为：

- executor 的主输入是 `planner output + runContext`
- worker 的主输入是“单 stage request”
- worker 只读上游 manifest，只写当前 bucket
- planner 决定 fingerprint 语义
- worker 负责把 fingerprint 落盘到 bucket manifest
- executor 负责 reuse / invalidation 的兑现与验收
- `invalidated` 不是 worker mode，而是 executor 的重建原因
- `status` 与 `execution mode` 必须拆开表达
- matrix 最终报告不由普通 worker 直接写

## 14. 这一步之后最自然的下一个问题

如果继续往下收敛，最自然的下一个问题是：

- reporting 对 `matrix execution result` 的正式 schema 怎么定

也就是继续解决：

- `report.json` 最终该保留哪些字段
- `stageResults / artifactResults / errors / statusCounts` 怎样统一
- subject / session 聚合如何建立在同一套结果对象之上
