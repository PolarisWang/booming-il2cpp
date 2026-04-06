# planner 输出对象设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

在已经确定：

- `subject.manifest.json` 的增量 schema
- manifest 字段的消费边界
- stage dependency graph
- 阶段产物落点
- 复用与失效规则

之后，下一步必须定义：

- planner 到底输出什么对象给 executor

否则：

- planner 和 executor 之间仍然会靠隐式约定耦合

## 2. 结论

推荐 planner 的最小输出对象分成三层：

- request summary
- resolved subject context
- stage execution plan

并且：

- planner 输出必须是“已展开、可执行、可审计”的对象
- executor 不应再自己回头解释 manifest

也就是：

- planner 负责把 manifest 解析成执行计划
- executor 只负责执行这份计划

## 3. planner 输出对象的整体结构

第一版推荐结构：

```json
{
  "planVersion": "v1",
  "request": {},
  "selection": {},
  "artifactsRoot": {},
  "stagePlan": []
}
```

### 顶层字段含义

- `planVersion`
  - planner 输出对象自己的版本
- `request`
  - 用户原始请求的归一化结果
- `selection`
  - planner 解析后的最终选择
- `artifactsRoot`
  - 本次执行涉及的根路径
- `stagePlan`
  - executor 真正要执行/复用/失效的 stage 列表

## 4. `request`

职责：

- 记录 planner 接收到的规范化输入

推荐字段：

- `subjectId`
- `requestedGoalId`
- `requestedMatrixId`
- `hostPlatform`

例如：

```json
{
  "subjectId": "HelloWorldObject",
  "requestedGoalId": "correctness.platform",
  "requestedMatrixId": "windows-reference-trace",
  "hostPlatform": "windows"
}
```

说明：

- 这里记录的是“用户想要什么”
- 不是最终选中了什么

## 5. `selection`

职责：

- 记录 planner 解析后的最终执行上下文

推荐字段：

- `subjectId`
- `goalId`
- `matrixId`
- `pipelineId`
- `category`
- `executionContext`
- `validationIntent`
- `artifactPlan`

例如：

```json
{
  "subjectId": "HelloWorldObject",
  "goalId": "correctness.platform",
  "matrixId": "windows-reference-trace",
  "pipelineId": "proof-runtime-trace",
  "category": "canonical",
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
  },
  "artifactPlan": {
    "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
    "evidenceTerminalBucket": "runtime"
  }
}
```

说明：

- `selection` 是 executor 需要理解的总体执行语义

## 6. `artifactsRoot`

职责：

- 给 executor 一组稳定路径根

推荐字段：

- `subjectRoot`
- `sharedRoot`
- `matrixRoot`
- `subjectReportRoot`
- `sessionSummaryPath`

例如：

```json
{
  "subjectRoot": "artifacts/subjects/HelloWorldObject",
  "sharedRoot": "artifacts/subjects/HelloWorldObject/shared",
  "matrixRoot": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace",
  "subjectReportRoot": "artifacts/subjects/HelloWorldObject/subject-report",
  "sessionSummaryPath": "artifacts/logs/tests/<run-id>/summary.json"
}
```

说明：

- executor 不需要再自己拼所有路径
- planner 先把根路径规范好

## 7. `stagePlan[]`

这是最核心的一层。

每个 stagePlan item 推荐至少包含：

- `order`
- `stageId`
- `kind`
- `scope`
- `bucket`
- `dependsOn`
- `executionMode`
- `fingerprint`
- `upstreamFingerprints`
- `paths`
- `reuse`

## 7.1 字段说明

- `order`
  - 执行顺序
- `stageId`
  - 逻辑 stage 标识
- `kind`
  - executor 选 worker 的稳定语义
- `scope`
  - `shared | matrix`
- `bucket`
  - 本 stage 写入的 bucket
- `dependsOn`
  - 依赖的上游 stageId
- `executionMode`
  - `executed | reused | invalidated`
- `fingerprint`
  - 本 stage 当前请求的目标 fingerprint
- `upstreamFingerprints`
  - planner 看到的上游指纹
- `paths`
  - 本 stage 关心的 bucket 路径与关键 manifest 路径
- `reuse`
  - planner 对这一步作出的复用判断说明

## 7.2 为什么 `executionMode` 必须在 planner 输出里

因为前面已经确认：

- 是否 `executed/reused/invalidated`
- 是 planner 的职责，不是 executor 的职责

如果把它推迟到 executor 再判断，就会重新把：

- graph
- fingerprint
- invalidation

的解释权拉回执行层。

## 7.3 为什么 `paths` 也该由 planner 给出

因为路径不是纯执行细节，它是：

- graph scope
- subject root
- matrix root
- bucket type

共同决定的。

planner 已经拥有这些信息，应该一次展开。

这样 executor 不会再去重复拼：

- `shared/source`
- `shared/analysis`
- `matrices/<matrix-id>/runtime`

这些路径。

## 8. `reuse`

`reuse` 推荐再拆成几项字段：

- `decision`
- `reason`
- `existingManifestPath`

### `decision`

固定值：

- `missing`
- `match`
- `mismatch`

### `reason`

示例：

- `bucket-manifest-missing`
- `fingerprint-match`
- `fingerprint-mismatch:source-changed`
- `upstream-invalidated`

### 作用

- `executionMode` 告诉 executor 要怎么做
- `reuse` 告诉 reporting 为什么会这样

## 9. `paths` 的最小结构

推荐字段：

- `bucketRoot`
- `manifestPath`
- `reportPaths`

例如：

```json
{
  "bucketRoot": "artifacts/subjects/HelloWorldObject/shared/analysis",
  "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
  "reportPaths": [
    "artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"
  ]
}
```

对于 `build-target`：

```json
{
  "bucketRoot": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/build",
  "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/build/build.manifest.json",
  "reportPaths": []
}
```

## 10. `windows-reference-trace` 的示例

下面是一版压缩后的 planner 输出示例：

```json
{
  "planVersion": "v1",
  "request": {
    "subjectId": "HelloWorldObject",
    "requestedGoalId": "correctness.platform",
    "requestedMatrixId": "windows-reference-trace",
    "hostPlatform": "windows"
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
    },
    "artifactPlan": {
      "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
      "evidenceTerminalBucket": "runtime"
    }
  },
  "artifactsRoot": {
    "subjectRoot": "artifacts/subjects/HelloWorldObject",
    "sharedRoot": "artifacts/subjects/HelloWorldObject/shared",
    "matrixRoot": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace",
    "subjectReportRoot": "artifacts/subjects/HelloWorldObject/subject-report"
  },
  "stagePlan": [
    {
      "order": 1,
      "stageId": "source-resolve",
      "kind": "source-resolve",
      "scope": "shared",
      "bucket": "source",
      "dependsOn": [],
      "executionMode": "reused",
      "fingerprint": "f-source-1",
      "upstreamFingerprints": {},
      "reuse": {
        "decision": "match",
        "reason": "fingerprint-match",
        "existingManifestPath": "artifacts/subjects/HelloWorldObject/shared/source/source.manifest.json"
      },
      "paths": {
        "bucketRoot": "artifacts/subjects/HelloWorldObject/shared/source",
        "manifestPath": "artifacts/subjects/HelloWorldObject/shared/source/source.manifest.json",
        "reportPaths": []
      }
    },
    {
      "order": 2,
      "stageId": "host-input-build",
      "kind": "host-input-build",
      "scope": "shared",
      "bucket": "host-input",
      "dependsOn": ["source-resolve"],
      "executionMode": "reused",
      "fingerprint": "f-host-1",
      "upstreamFingerprints": {
        "source": "f-source-1"
      },
      "reuse": {
        "decision": "match",
        "reason": "fingerprint-match",
        "existingManifestPath": "artifacts/subjects/HelloWorldObject/shared/host-input/host-input.manifest.json"
      },
      "paths": {
        "bucketRoot": "artifacts/subjects/HelloWorldObject/shared/host-input",
        "manifestPath": "artifacts/subjects/HelloWorldObject/shared/host-input/host-input.manifest.json",
        "reportPaths": []
      }
    },
    {
      "order": 3,
      "stageId": "analysis-frontend",
      "kind": "analysis-frontend",
      "scope": "shared",
      "bucket": "analysis",
      "dependsOn": ["host-input-build"],
      "executionMode": "executed",
      "fingerprint": "f-analysis-2",
      "upstreamFingerprints": {
        "host-input": "f-host-1"
      },
      "reuse": {
        "decision": "mismatch",
        "reason": "fingerprint-mismatch:driver-changed",
        "existingManifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json"
      },
      "paths": {
        "bucketRoot": "artifacts/subjects/HelloWorldObject/shared/analysis",
        "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
        "reportPaths": [
          "artifacts/subjects/HelloWorldObject/shared/analysis/contract-validate.report.json"
        ]
      }
    }
  ]
}
```

## 11. planner 输出不应该包含什么

第一版不建议 planner 输出里直接包含：

- worker 命令行
- 具体 shell command
- 全量环境变量
- 实际 stdout/stderr

原因：

- 这些是 executor 运行期细节
- planner 只负责执行计划，不负责具体命令物化

## 12. planner 输出和 reporting 的关系

planner 输出对象本身不等于 report，但 report 会大量引用它。

例如：

- `matrix report`
  - 可直接保留 `selection`
  - 可保留 `stagePlan` 的精简版

但 reporting 不应直接把整个 planner 输出原样透传成最终报告。

更合理的是：

- planner output = 内部执行计划
- report = 对执行结果的稳定汇总

## 13. 对 executor 的直接收益

这份对象一旦存在，executor 的输入就会非常明确：

- 选定的 subject / matrix / goal
- 展开的 stage 列表
- 每个 stage 的 bucket 路径
- 每个 stage 的执行模式
- 每个 stage 的 fingerprint

这样 executor 可以做到：

- 不再解释 manifest
- 不再自己推 graph
- 不再自己判断复用策略

## 14. 当前可以正式固定的规则

这次可以正式固定为：

- planner 输出对象分为 `request / selection / artifactsRoot / stagePlan`
- `stagePlan[]` 是 executor 的直接输入
- `executionMode` 由 planner 决定
- `paths` 由 planner 展开
- planner 不直接输出 worker 命令行

## 15. 这一步之后最自然的下一个问题

如果继续往下收敛，最自然的下一个问题就是：

- executor 的最小输入/输出边界

因为现在 planner 输出已经成型，下一步就该定义：

- executor 接到单个 stage item 后，最少要返回什么结果结构
