# Phase A Engine Subject Manifest And Execution Pipeline Draft v1.01

Date: 2026-04-07
Status: design-discuss

## 1. 问题点

前面三轮已经分别固定了：

- `engine binding` 是正式边界
- `LoweringPlan` 里需要 `EngineBindings + HostBindings`
- 首轮 engine proof 固定为三个最小 subject：
  - `EngineLogWriteLite`
  - `EngineObjectHandleLite`
  - `EngineLifecycleCallbackLite`

那么下一步必须回答的问题就是：

- **这三个 subject 的 `subject.manifest.json` 到底怎么写**
- **它们的 `executionPipelines` 到底是完全复用既有主线，还是需要做最小 engine-aware 增量**

这一步不先讲清楚，会立刻出现三个风险：

1. subject 只有名字，没有稳定 machine-readable contract
2. engine 语义会被错误地重复写进 manifest 和 lowering plan 两处
3. pipeline 会在实现时临时分叉，最后变成“引擎专属执行链”

所以这一轮真正要回答的，不是“这三个工程叫什么”，而是：

- 它们如何作为正式 `subject` 被 registry / planner / executor 看懂

## 2. 问题的具体描述

当前通用 `subject.manifest.json` 已经有比较清晰的基础骨架：

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

这套骨架对普通 canonical subject 已经够用。

但 engine subject 有两个额外复杂度：

### 第一层复杂度：manifest 里要不要显式写 engine proof 的“关注点”

例如这三个 subject，虽然都属于 engine proof，但各自的关注点不同：

- `EngineLogWriteLite`
  - `service-call`
- `EngineObjectHandleLite`
  - `object-handle`
- `EngineLifecycleCallbackLite`
  - `lifecycle-callback`

如果 manifest 完全不写这些信息，那么：

- registry / reporting 很难做清晰分类
- 用户也很难从 manifest 直接知道这个 subject 验证什么

但如果 manifest 写得太重，又会和 `LoweringPlan` 里的：

- `EngineBindings`
- `HostBindings`
- `EngineCapabilityId`

形成重复描述。

所以需要回答：

- manifest 里到底要写“多少” engine-specific 信息才合适

### 第二层复杂度：pipeline 到底复用到什么程度

engine subject 明显还是沿用同一条 artifact-chain：

- `source`
- `host-input`
- `analysis`
- `generated`
- `build`
- `runtime`
- `report`

但在 stage kind 上存在一个容易分叉的点：

- 是继续复用：
  - `generated-native-proof`
  - `runtime-observe`
- 还是改成：
  - `generated-engine-proof`
  - `runtime-engine-observe`

如果完全不做 engine-aware 区分，可能会让 worker 选择和 reporting 语义不够清楚。

如果过度区分，又会把 engine proof 变成一条平行 pipeline。

所以这一步真正要解的是：

- **怎样让 engine subject 的 manifest 可读、可执行、可扩展**
- **同时又不把同一层语义在 manifest / analysis / lowering plan 三处写三遍**

## 3. 有哪些可解决的路径

### 路径 A：完全复用通用 manifest，不新增任何 engine-specific 字段；pipeline 也完全沿用既有 stage kind

做法：

- 三个 subject 只写通用字段：
  - `subjectId`
  - `source`
  - `executionPipelines`
  - `environmentMatrices`
- 不新增任何：
  - `engineProofProfile`
  - `engineFocus`
  - `expectedEngineCapabilities`
- pipeline 也直接沿用既有 stage kind：
  - `source-resolve`
  - `host-input-build`
  - `analysis-frontend`
  - `generated-native-proof`
  - `build-target`
  - `runtime-observe`
  - `runtime-trace-compare`
  - `report-assemble`

优点：

- 最轻
- 不会引入新 schema 字段
- 完全复用现有执行框架

缺点：

- manifest 读不出这是哪类 engine proof
- reporting / registry 难以直接按 engine 语义分类
- engine subject 的“关注点”只能藏在文档和源码名里
- executor/reporting 如果后续要显示 engine-specific 结果，会缺一个稳定入口

判断：

- 不推荐作为主路径

### 路径 B：保持通用 manifest 骨架，但新增一个轻量 `engineProofProfile`，pipeline 只做最小 engine-aware 增量

做法：

- manifest 继续以通用字段为主
- 仅新增一个轻量 subject 级字段：
  - `engineProofProfile`
- 它只表达：
  - 当前 subject 的 engine proof 关注点
  - 预期验证的 engine capability 类别
  - 预期的 evidence 形状
- 不直接在 manifest 重复写完整 `EngineBindings`

同时 pipeline 采用：

- 前半段完全复用通用主线
- 只允许在需要时把个别 stage kind 升格成 engine-aware 语义名

优点：

- manifest 仍然简洁
- engine subject 的机器可读身份变清楚
- 不会和 lowering plan 重复表达过多细节
- pipeline 仍然是主线，只做最小增量

缺点：

- 需要新增少量 schema 字段
- 需要明确哪些信息属于 `engineProofProfile`，哪些必须留给 analysis/lowering plan

判断：

- 推荐

### 路径 C：为 engine subject 单独定义一套专用 manifest schema 和专用 pipeline 体系

做法：

- `engine subject` 不再完全服从通用 `subject.manifest.json`
- 额外定义：
  - engine-specific subject schema
  - engine-specific pipeline schema
  - engine-specific report object

优点：

- 语义最显式
- 一眼就能看出这是 engine 体系

缺点：

- 太早把 engine proof 从主线里摘出来
- 长期会形成两套 subject 模型
- 和 `subject + matrix + artifact-chain` 主线冲突
- 实施成本最高

判断：

- 不推荐

## 4. 你的方案和理由

我推荐：

- **路径 B：保持通用 manifest 骨架，但新增一个轻量 `engineProofProfile`，pipeline 只做最小 engine-aware 增量**

理由有六个：

1. 它最符合现有主线
   - 仍然是统一 `subject.manifest.json`
   - 不是 engine 特化 schema

2. 它解决了“manifest 完全读不出语义”的问题
   - subject 除了名字之外，还能显式说明自己验证什么

3. 它避免和 `LoweringPlan` 重复
   - manifest 只写高层 proof profile
   - 不写具体 `EngineBindings` 明细

4. 它让 registry / reporting / summary 更容易做分类
   - 后续可以直接按：
     - `service-call`
     - `object-handle`
     - `lifecycle-callback`
     聚合

5. 它对 pipeline 的影响最小
   - 共用主线 graph
   - 只在必要时引入极少数 engine-aware stage kind

6. 它仍然属于 `Phase A` 能承受的收口范围
   - 这是轻量 schema 增量
   - 不是开始做 engine test platform

如果按这个方案推进，我建议先冻结四条规则。

### 规则 1：manifest 只写高层 engine proof profile，不写 lowering-plan 细节

建议 subject 顶层新增：

- `engineProofProfile`

最小字段建议为：

- `proofKind`
  - 固定为 `engine-binding`
- `focusArea`
  - `service-call`
  - `object-handle`
  - `lifecycle-callback`
- `expectedCapabilityIds`
  - 例如：
    - `engine.log.write`
    - `engine.object.handle.resolve`
    - `engine.lifecycle.update`
- `expectedEvidenceKinds`
  - 例如：
    - `log-message`
    - `handle-roundtrip`
    - `callback-order`
    - `thread-lane`

这里故意不写：

- `EngineBindings[]`
- `HostBindings[]`
- `OwnershipPolicy`
- `ExecutionLane`

因为这些应该留给 analysis/lowering plan 作为真源。

### 规则 2：三个 subject 的 manifest 只在 `engineProofProfile` 和断言面上分化，其他结构尽量统一

推荐三者共享：

- `category = canonical`
- `defaultGoal = correctness.dev`
- `defaultMatrix = windows-dev-output`
- `expectedArtifacts`
- `executionPipelines`
- 首轮两条 `environmentMatrices`

只在下面这些地方分化：

- `subjectId`
- `displayName`
- `source.path`
- `tags`
- `engineProofProfile`

这样三者就会成为非常稳定的 onboarding pattern。

### 规则 3：execution pipeline 继续共用主线 graph，只做最小 engine-aware stage 命名增量

推荐前半段完全共用：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `build-target`
- `report-assemble`

真正需要讨论的只有 generated/runtime 两段。

我建议：

- `generated` 段使用：
  - `generated-engine-proof`
- `runtime` 段使用：
  - `runtime-engine-observe`
  - `runtime-engine-trace-compare`

原因是：

- 这两段已经开始关心 engine-specific evidence
- 如果仍然叫通用名字，后续 reporting/worker 选择会越来越含混

但同时不建议把整条 graph 都改成 engine 前缀化，因为那会过度分叉。

### 规则 4：首轮三份 manifest 先共用两条 pipeline，不为每个 subject 各写一套 graph

推荐固定两条 pipeline：

1. `engine-runtime-output`
2. `engine-runtime-trace`

三个 subject 都只引用这两条 pipeline，不各自复制一份 graph。

这能确保：

- pipeline 是 subject 级共享资产
- subject 差异主要体现在：
  - source
  - engineProofProfile
  - runtime evidence 断言

而不是体现在图结构本身。

## 4.1 推荐的通用 manifest 骨架

推荐在现有 schema 上，给 engine subject 增量加入：

- `engineProofProfile`

整体骨架建议为：

- `subjectId`
- `displayName`
- `category`
- `defaultGoal`
- `defaultMatrix`
- `source`
- `tags`
- `expectedArtifacts`
- `engineProofProfile`
- `executionPipelines`
- `environmentMatrices`

## 4.2 三个 subject 的推荐草案

### `EngineLogWriteLite`

推荐核心字段：

```json
{
  "subjectId": "EngineLogWriteLite",
  "displayName": "Engine Log Write Lite",
  "category": "canonical",
  "defaultGoal": "correctness.dev",
  "defaultMatrix": "windows-dev-output",
  "source": {
    "type": "dotnet-project",
    "path": "subjects/EngineLogWriteLite/source/EngineLogWriteLite.csproj",
    "entry": "EngineLogWriteLite/Program::Main(System.String[])"
  },
  "tags": ["engine", "binding", "service-call", "log"],
  "expectedArtifacts": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
  "engineProofProfile": {
    "proofKind": "engine-binding",
    "focusArea": "service-call",
    "expectedCapabilityIds": ["engine.log.write"],
    "expectedEvidenceKinds": ["log-message"]
  }
}
```

### `EngineObjectHandleLite`

推荐核心字段：

```json
{
  "subjectId": "EngineObjectHandleLite",
  "displayName": "Engine Object Handle Lite",
  "category": "canonical",
  "defaultGoal": "correctness.dev",
  "defaultMatrix": "windows-dev-output",
  "source": {
    "type": "dotnet-project",
    "path": "subjects/EngineObjectHandleLite/source/EngineObjectHandleLite.csproj",
    "entry": "EngineObjectHandleLite/Program::Main(System.String[])"
  },
  "tags": ["engine", "binding", "object-handle", "lifetime"],
  "expectedArtifacts": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
  "engineProofProfile": {
    "proofKind": "engine-binding",
    "focusArea": "object-handle",
    "expectedCapabilityIds": [
      "engine.object.handle.create",
      "engine.object.handle.resolve"
    ],
    "expectedEvidenceKinds": ["handle-roundtrip"]
  }
}
```

### `EngineLifecycleCallbackLite`

推荐核心字段：

```json
{
  "subjectId": "EngineLifecycleCallbackLite",
  "displayName": "Engine Lifecycle Callback Lite",
  "category": "canonical",
  "defaultGoal": "correctness.dev",
  "defaultMatrix": "windows-dev-output",
  "source": {
    "type": "dotnet-project",
    "path": "subjects/EngineLifecycleCallbackLite/source/EngineLifecycleCallbackLite.csproj",
    "entry": "EngineLifecycleCallbackLite/Program::Main(System.String[])"
  },
  "tags": ["engine", "binding", "lifecycle", "callback", "thread-lane"],
  "expectedArtifacts": ["source", "host-input", "analysis", "generated", "build", "runtime", "report"],
  "engineProofProfile": {
    "proofKind": "engine-binding",
    "focusArea": "lifecycle-callback",
    "expectedCapabilityIds": [
      "engine.callback.register",
      "engine.lifecycle.update"
    ],
    "expectedEvidenceKinds": ["callback-order", "thread-lane"]
  }
}
```

## 4.3 推荐的 executionPipelines 草案

推荐三者共用下面两条 pipeline。

### `engine-runtime-output`

```json
{
  "pipelineId": "engine-runtime-output",
  "displayName": "Engine runtime output",
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
      "stageId": "generated-engine-proof",
      "kind": "generated-engine-proof",
      "scope": "shared",
      "bucket": "generated",
      "dependsOn": ["analysis-frontend"]
    },
    {
      "stageId": "build-target",
      "kind": "build-target",
      "scope": "matrix",
      "bucket": "build",
      "dependsOn": ["generated-engine-proof"]
    },
    {
      "stageId": "runtime-engine-observe",
      "kind": "runtime-engine-observe",
      "scope": "matrix",
      "bucket": "runtime",
      "dependsOn": ["build-target"]
    },
    {
      "stageId": "report-assemble",
      "kind": "report-assemble",
      "scope": "matrix",
      "bucket": "report",
      "dependsOn": ["runtime-engine-observe"]
    }
  ]
}
```

### `engine-runtime-trace`

```json
{
  "pipelineId": "engine-runtime-trace",
  "displayName": "Engine runtime trace",
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
      "stageId": "generated-engine-proof",
      "kind": "generated-engine-proof",
      "scope": "shared",
      "bucket": "generated",
      "dependsOn": ["analysis-frontend"]
    },
    {
      "stageId": "build-target",
      "kind": "build-target",
      "scope": "matrix",
      "bucket": "build",
      "dependsOn": ["generated-engine-proof"]
    },
    {
      "stageId": "runtime-engine-observe",
      "kind": "runtime-engine-observe",
      "scope": "matrix",
      "bucket": "runtime",
      "dependsOn": ["build-target"]
    },
    {
      "stageId": "runtime-engine-trace-compare",
      "kind": "runtime-engine-trace-compare",
      "scope": "matrix",
      "bucket": "runtime",
      "dependsOn": ["runtime-engine-observe"]
    },
    {
      "stageId": "report-assemble",
      "kind": "report-assemble",
      "scope": "matrix",
      "bucket": "report",
      "dependsOn": ["runtime-engine-trace-compare"]
    }
  ]
}
```

## 4.4 推荐的 environmentMatrices 草案

三者首轮都建议共用：

```json
[
  {
    "matrixId": "windows-dev-output",
    "pipelineId": "engine-runtime-output",
    "supportedGoals": ["correctness.dev"],
    "executionContext": {
      "hostPlatform": "windows-x64",
      "targetPlatform": "windows-x64",
      "toolchainProfile": "msvc-reference",
      "runtimeProfile": "engine-proof-output"
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
    "pipelineId": "engine-runtime-trace",
    "supportedGoals": ["correctness.platform"],
    "executionContext": {
      "hostPlatform": "windows-x64",
      "targetPlatform": "windows-x64",
      "toolchainProfile": "msvc-reference",
      "runtimeProfile": "engine-reference-trace"
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
]
```

## 5. 审查下这个问题本身，是否合理，以及是否有其他方式让这个方案在整体框架中更合理地得以解决

我认为这个问题本身非常合理，而且它就是前一轮之后最自然的下一问。

因为前一轮已经回答了：

- 第一批 subject 是谁
- 第一批 matrix 是谁
- artifact-chain 继续共用主线

那这一步就必须回答：

- 它们作为正式 subject，manifest 到底怎么写
- pipeline 到底如何表达

否则“这三个 subject 是 canonical object”仍然只是文档概念，不是可执行对象。

不过，这个问题也要避免问得过大。

更精确的问法不应该是：

- “完整 engine test schema 怎么设计？”

而应该是：

- “在 `Phase A`，为了让 `registry -> planner -> executor` 真正能承接这三个 engine subject，manifest 里最小该增加什么，pipeline 又最小该调整什么？”

为了让这个方案在整体框架里更合理，我建议再补三条约束：

### A. manifest 不要成为第二份 lowering plan

manifest 应该表达：

- subject 身份
- proof 关注点
- pipeline 选择
- matrix 选择

而不是直接复制：

- `EngineBindings[]`
- `HostBindings[]`
- lowering role 细节

### B. pipeline 的 engine-aware 增量只应发生在 generated/runtime 两段

因为真正出现 engine-specific 差异的地方主要是：

- proof codegen/render
- runtime evidence observe

source / host-input / analysis / build / report 仍然是共通主线。

### C. 三个 subject 应先形成同构 onboarding pattern

比起现在就追求“每个 subject 完全贴身定制”，更重要的是先形成：

- 同一套 manifest 布局
- 同一套 pipelineId
- 同一套 matrix 命名

这样后面 engine subject 扩张时才不会失控。

所以，这一轮的最终结论是：

- **推荐在通用 `subject.manifest.json` 基础上增量加入轻量 `engineProofProfile`**
- **推荐三份 engine subject manifest 共享两条 pipeline：`engine-runtime-output` 与 `engine-runtime-trace`**
- **推荐 pipeline 只在 generated/runtime 两段引入最小 engine-aware stage kind，不把整条主线重命名成平行体系**
