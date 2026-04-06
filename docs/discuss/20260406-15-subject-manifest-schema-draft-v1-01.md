# subject.manifest.json 增量 Schema 草案 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

本文件把前面已经逐步确认的新字段统一收敛成一版完整的 `subject.manifest.json` 增量 schema 草案。

这份草案的目的不是马上写 JSON Schema 文件，而是先固定对象模型：

- 哪些字段是 subject 顶层字段
- 哪些字段属于 matrix
- 哪些字段属于 pipeline
- 字段之间是什么关系

## 2. 结论

在原先最小模型基础上，首批需要正式新增四组字段：

- matrix 级：`supportedGoals`
- matrix 级：`artifactPlan`
- matrix 级：`pipelineId`
- subject 顶层：`executionPipelines`

所以第一版正式模型应分成四层：

- subject 顶层
- source 定义
- executionPipelines
- environmentMatrices

## 3. 第一版完整字段布局

## 3.1 subject 顶层

推荐固定字段：

- `subjectId`
- `displayName`
- `category`
- `defaultGoal`
- `defaultMatrix`
- `source`
- `tags`
- `expectedArtifacts`
- `executionPipelines`
- `environmentMatrices`

### 字段说明

- `subjectId`
  - 稳定 subject 标识
- `displayName`
  - 人读名称
- `category`
  - `canonical | diagnostic | benchmark`
- `defaultGoal`
  - 未显式给 `--goal` 时的默认目标
- `defaultMatrix`
  - 未显式给 `--matrix` 时的默认 matrix
- `source`
  - subject 输入定义
- `tags`
  - 辅助标签
- `expectedArtifacts`
  - 该 subject 理论可覆盖到的最大 bucket 包络
- `executionPipelines`
  - 可复用执行图定义
- `environmentMatrices`
  - 实际可执行 matrix 列表

## 3.2 `source`

推荐固定字段：

- `type`
- `path`
- `entry`

第一版支持：

- `dotnet-project`
- `managed-assembly`
- `native-project`
- `source-file`

## 3.3 `executionPipelines[]`

推荐固定字段：

- `pipelineId`
- `displayName`
- `stages`

其中 `stages[]` 每项固定字段：

- `stageId`
- `kind`
- `scope`
- `bucket`
- `dependsOn`

### `scope`

第一版固定为：

- `shared`
- `matrix`

### `bucket`

第一版固定为：

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

## 3.4 `environmentMatrices[]`

推荐固定字段：

- `matrixId`
- `pipelineId`
- `supportedGoals`
- `executionContext`
- `validationIntent`
- `artifactPlan`

### `executionContext`

固定字段：

- `hostPlatform`
- `targetPlatform`
- `toolchainProfile`
- `runtimeProfile`

### `validationIntent`

固定字段：

- `validationMode`
- `adaptationLevel`
- `expectedOutcome`

### `artifactPlan`

固定字段：

- `requiredBuckets`
- `evidenceTerminalBucket`

## 4. 字段关系

这份 schema 里最重要的不是字段数量，而是字段关系。

## 4.1 `defaultMatrix` 必须出现在 `environmentMatrices`

规则：

- `defaultMatrix` 的值必须能在 `environmentMatrices[].matrixId` 中找到

## 4.2 `defaultGoal` 必须被默认 matrix 支持

规则：

- `defaultGoal` 必须出现在 `defaultMatrix.supportedGoals[]` 中

否则：

- subject 配置无效

## 4.3 `pipelineId` 必须出现在 `executionPipelines`

规则：

- 每个 matrix 的 `pipelineId` 必须能在 `executionPipelines[].pipelineId` 中找到

## 4.4 `artifactPlan` 必须与 pipeline 终点一致

规则：

- `artifactPlan.terminalBucket` 必须等于该 `pipelineId` 最后一个 stage 的 `bucket`

例如：

- `proof-runtime-output` 的最后 stage 在 `report`
- 但它运行时 evidence 终点在 `runtime`

这里第一版建议：

- `report-assemble` 仍属于 `report` bucket
- `artifactPlan.terminalBucket` 填 `runtime` 或 `report` 二选一会造成歧义

所以需要一条额外规则：

### 推荐修正

第一版把 `artifactPlan` 拆成：

- `requiredBuckets`
- `evidenceTerminalBucket`

而不是继续用 `terminalBucket`

原因：

- `report-assemble` 总会在最后执行
- 但用户真正关心的验证终点，往往是 `build` 或 `runtime`

例如：

- `windows-android-buildable`
  - evidence 终点是 `build`
- `windows-reference-trace`
  - evidence 终点是 `runtime`

而不应该因为总有 `report-assemble`，就一律把终点说成 `report`

## 5. 对现有 `artifactPlan` 的修正建议

前面的讨论里，`artifactPlan` 用的是：

- `requiredBuckets`
- `terminalBucket`

现在看，这个命名已经不够精确。

推荐改成：

- `requiredBuckets`
- `evidenceTerminalBucket`

### 为什么

因为 pipeline 最后总会有：

- `report-assemble`

如果继续写 `terminalBucket`，会出现两种冲突解释：

- 是“执行图最后一个 bucket”
- 还是“验证证据最后一个 bucket”

你真正需要的是后者。

### 推荐结论

正式改为：

- `requiredBuckets`
- `evidenceTerminalBucket`

## 6. 第一版枚举值整理

## 6.1 `category`

- `canonical`
- `diagnostic`
- `benchmark`

## 6.2 `validationMode`

- `schema`
- `compile-only`
- `build`
- `startup`
- `output`
- `trace`
- `perf`
- `report`

## 6.3 `adaptationLevel`

- `unsupported`
- `compile-only`
- `buildable`
- `startup`
- `observable-output`
- `traceable`
- `perf-reportable`

## 6.4 `expectedOutcome`

- `pass`
- `fail`
- `known-fail`
- `not-applicable`

## 6.5 `bucket`

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

## 6.6 `scope`

- `shared`
- `matrix`

## 7. `HelloWorldObject` 的完整推荐草案

下面是一版把所有新增字段统一后的推荐草案。

```json
{
  "subjectId": "HelloWorldObject",
  "displayName": "HelloWorldObject",
  "category": "canonical",
  "defaultGoal": "correctness.dev",
  "defaultMatrix": "windows-dev-output",
  "source": {
    "type": "dotnet-project",
    "path": "subjects/HelloWorldObject/source/HelloWorldObject.csproj",
    "entry": "HelloWorldObject/Program::Main(System.String[])"
  },
  "tags": ["proof", "object", "debuggable", "reference"],
  "expectedArtifacts": [
    "source",
    "host-input",
    "analysis",
    "generated",
    "build",
    "runtime",
    "report"
  ],
  "executionPipelines": [
    {
      "pipelineId": "proof-runtime-output",
      "displayName": "Proof runtime output",
      "stages": [
        {
          "stageId": "source-resolve",
          "kind": "source-resolve",
          "scope": "shared",
          "bucket": "source",
          "dependsOn": []
        },
        {
          "stageId": "host-input-build",
          "kind": "host-input-build",
          "scope": "shared",
          "bucket": "host-input",
          "dependsOn": ["source-resolve"]
        },
        {
          "stageId": "analysis-frontend",
          "kind": "analysis-frontend",
          "scope": "shared",
          "bucket": "analysis",
          "dependsOn": ["host-input-build"]
        },
        {
          "stageId": "generated-native-proof",
          "kind": "generated-native-proof",
          "scope": "shared",
          "bucket": "generated",
          "dependsOn": ["analysis-frontend"]
        },
        {
          "stageId": "build-target",
          "kind": "build-target",
          "scope": "matrix",
          "bucket": "build",
          "dependsOn": ["generated-native-proof"]
        },
        {
          "stageId": "runtime-observe",
          "kind": "runtime-observe",
          "scope": "matrix",
          "bucket": "runtime",
          "dependsOn": ["build-target"]
        },
        {
          "stageId": "report-assemble",
          "kind": "report-assemble",
          "scope": "matrix",
          "bucket": "report",
          "dependsOn": ["runtime-observe"]
        }
      ]
    },
    {
      "pipelineId": "proof-runtime-trace",
      "displayName": "Proof runtime trace",
      "stages": [
        {
          "stageId": "source-resolve",
          "kind": "source-resolve",
          "scope": "shared",
          "bucket": "source",
          "dependsOn": []
        },
        {
          "stageId": "host-input-build",
          "kind": "host-input-build",
          "scope": "shared",
          "bucket": "host-input",
          "dependsOn": ["source-resolve"]
        },
        {
          "stageId": "analysis-frontend",
          "kind": "analysis-frontend",
          "scope": "shared",
          "bucket": "analysis",
          "dependsOn": ["host-input-build"]
        },
        {
          "stageId": "generated-native-proof",
          "kind": "generated-native-proof",
          "scope": "shared",
          "bucket": "generated",
          "dependsOn": ["analysis-frontend"]
        },
        {
          "stageId": "build-target",
          "kind": "build-target",
          "scope": "matrix",
          "bucket": "build",
          "dependsOn": ["generated-native-proof"]
        },
        {
          "stageId": "runtime-observe",
          "kind": "runtime-observe",
          "scope": "matrix",
          "bucket": "runtime",
          "dependsOn": ["build-target"]
        },
        {
          "stageId": "runtime-trace-compare",
          "kind": "runtime-trace-compare",
          "scope": "matrix",
          "bucket": "runtime",
          "dependsOn": ["runtime-observe"]
        },
        {
          "stageId": "report-assemble",
          "kind": "report-assemble",
          "scope": "matrix",
          "bucket": "report",
          "dependsOn": ["runtime-trace-compare"]
        }
      ]
    },
    {
      "pipelineId": "platform-buildable",
      "displayName": "Platform buildable",
      "stages": [
        {
          "stageId": "source-resolve",
          "kind": "source-resolve",
          "scope": "shared",
          "bucket": "source",
          "dependsOn": []
        },
        {
          "stageId": "host-input-build",
          "kind": "host-input-build",
          "scope": "shared",
          "bucket": "host-input",
          "dependsOn": ["source-resolve"]
        },
        {
          "stageId": "analysis-frontend",
          "kind": "analysis-frontend",
          "scope": "shared",
          "bucket": "analysis",
          "dependsOn": ["host-input-build"]
        },
        {
          "stageId": "generated-native-proof",
          "kind": "generated-native-proof",
          "scope": "shared",
          "bucket": "generated",
          "dependsOn": ["analysis-frontend"]
        },
        {
          "stageId": "build-target",
          "kind": "build-target",
          "scope": "matrix",
          "bucket": "build",
          "dependsOn": ["generated-native-proof"]
        },
        {
          "stageId": "report-assemble",
          "kind": "report-assemble",
          "scope": "matrix",
          "bucket": "report",
          "dependsOn": ["build-target"]
        }
      ]
    }
  ],
  "environmentMatrices": [
    {
      "matrixId": "windows-dev-output",
      "pipelineId": "proof-runtime-output",
      "supportedGoals": ["correctness.dev"],
      "executionContext": {
        "hostPlatform": "windows-x64",
        "targetPlatform": "windows-x64",
        "toolchainProfile": "msvc-reference",
        "runtimeProfile": "native-proof-output"
      },
      "validationIntent": {
        "validationMode": "output",
        "adaptationLevel": "observable-output",
        "expectedOutcome": "pass"
      },
      "artifactPlan": {
        "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
        "evidenceTerminalBucket": "runtime"
      }
    },
    {
      "matrixId": "windows-reference-trace",
      "pipelineId": "proof-runtime-trace",
      "supportedGoals": ["correctness.platform"],
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
    {
      "matrixId": "windows-android-buildable",
      "pipelineId": "platform-buildable",
      "supportedGoals": ["correctness.platform"],
      "executionContext": {
        "hostPlatform": "windows-x64",
        "targetPlatform": "android-arm64",
        "toolchainProfile": "android-ndk",
        "runtimeProfile": "platform-build"
      },
      "validationIntent": {
        "validationMode": "build",
        "adaptationLevel": "buildable",
        "expectedOutcome": "pass"
      },
      "artifactPlan": {
        "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "report"],
        "evidenceTerminalBucket": "build"
      }
    },
    {
      "matrixId": "windows-linux-buildable",
      "pipelineId": "platform-buildable",
      "supportedGoals": ["correctness.platform"],
      "executionContext": {
        "hostPlatform": "windows-x64",
        "targetPlatform": "linux-x64",
        "toolchainProfile": "linux-cross",
        "runtimeProfile": "platform-build"
      },
      "validationIntent": {
        "validationMode": "build",
        "adaptationLevel": "buildable",
        "expectedOutcome": "pass"
      },
      "artifactPlan": {
        "requiredBuckets": ["source", "host-input", "analysis", "generated", "build", "report"],
        "evidenceTerminalBucket": "build"
      }
    }
  ]
}
```

## 8. 这份草案对现有设计的正式修正

这次可以正式修正三点：

- `artifactPlan.terminalBucket` 更名为 `artifactPlan.evidenceTerminalBucket`
- `executionPipelines` 成为 subject 顶层正式字段
- `environmentMatrices[]` 增加 `pipelineId`

## 9. 当前可以正式固定的规则

这次可以正式固定为：

- `subject.manifest.json` 第一版完整结构由 subject/source/pipelines/matrices 四层组成
- `supportedGoals`
- `artifactPlan`
- `pipelineId`
- `executionPipelines`

都是正式候选字段，不再只是零散讨论字段

## 10. 这一步之后最自然的下一个问题

如果继续往下收敛，最自然的下一个问题就是：

- 这份 manifest 草案对应的执行器最小职责边界

也就是：

- 哪些字段由 registry 解析
- 哪些字段由 planner 消费
- 哪些字段由 executor 消费
- 哪些字段只进入 reporting
