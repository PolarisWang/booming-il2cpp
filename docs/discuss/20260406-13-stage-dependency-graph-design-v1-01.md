# stage dependency graph 设计 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

在已经确定：

- `subject + goal + matrix + artifact-chain`
- 阶段产物落点
- 阶段复用与失效规则

之后，执行器还缺最后一块核心信息：

- 某个 matrix 到底依赖哪些 stage
- 哪些 stage 是共享前置
- 哪些 stage 是 matrix 独有终段

这就是 `stage dependency graph` 要解决的问题。

## 2. 结论

推荐：

- `subject.manifest.json` 显式声明 stage dependency graph
- 但不是每个 matrix 内联一整份 graph
- 而是在 manifest 顶层增加可复用的 `executionPipelines`
- 每个 matrix 只引用一个 `pipelineId`

也就是：

- graph 是显式的
- graph 是可复用的
- graph 是 subject 级资产
- matrix 负责选择 graph，不负责重复定义 graph

## 3. 为什么必须显式声明

如果不显式声明 graph，执行器就只能靠：

- `validationMode`
- `artifactPlan`
- 硬编码 stage 顺序

来推断执行链。

这会有三个问题：

- 执行器会越来越像“隐式规则仓库”，而不是稳定模型
- diagnostic / benchmark subject 很快会偏离默认顺序
- 复用与失效传播虽然已经定义了，但没有正式 graph 承载

因此，前面已经确认的：

- `shared/`
- `matrices/<matrix-id>/`
- fingerprint / invalidation

都要求有一份正式 graph 来驱动。

## 4. 三种方案

## 4.1 方案 A：完全隐式 graph

做法：

- 不在 manifest 里声明 graph
- 执行器根据 `validationMode + artifactPlan + runtimeProfile` 推导

优点：

- manifest 更短
- 第一眼看起来更省字段

缺点：

- graph 逻辑埋在代码里
- subject 无法真正描述自己的执行链
- 一旦出现非标准 diagnostic subject，推导规则会失控

结论：

- 不推荐

## 4.2 方案 B：每个 matrix 内联完整 graph

做法：

- 每个 matrix 直接写自己的 `stages[]`
- 包含 `dependsOn`

优点：

- 最直观
- matrix 自描述性最强

缺点：

- 重复极多
- `windows-dev-output` 和 `windows-reference-trace` 前半段几乎一样
- 后续 subject 多了以后，维护成本很高

结论：

- 不推荐作为长期模型

## 4.3 方案 C：subject 级 `executionPipelines`，matrix 引用 `pipelineId`

做法：

- manifest 顶层定义可复用 pipeline
- matrix 只声明自己用哪条 pipeline

优点：

- graph 显式化
- 不重复
- 可以和 `shared/` / `matrix` scope 对齐
- 最适合未来大量 subject 扩展

缺点：

- manifest 比最小版稍复杂

推荐结论：

- 采用方案 C

## 5. graph 应该放在哪一层

推荐放在 `subject.manifest.json` 顶层，而不是：

- 执行器代码里
- 单独外部文件
- 全局 registry

原因：

- graph 是 subject 语义的一部分
- 不同 subject 未来会有不同 graph
- 同一个 subject 的多个 matrix 共享 graph 最合理

所以推荐新增字段：

- `executionPipelines`

而 `environmentMatrices[]` 中新增：

- `pipelineId`

## 6. pipeline 的最小结构

第一版推荐最小字段：

- `pipelineId`
- `displayName`
- `stages`

其中每个 stage 至少包含：

- `stageId`
- `kind`
- `scope`
- `bucket`
- `dependsOn`

### 字段语义

- `stageId`
  - 当前 pipeline 内的逻辑 stage 标识
- `kind`
  - 执行器识别的稳定 stage 类型
- `scope`
  - `shared` 或 `matrix`
- `bucket`
  - 本 stage 产物写入哪个 bucket
- `dependsOn`
  - 明确依赖关系

## 7. 为什么 `scope` 必须进 graph

现在我们已经确定：

- `source/host-input/analysis/generated` 落在 `shared/`
- `build/runtime/report` 落在 `matrices/<matrix-id>/`

如果 graph 不显式带 `scope`，执行器就只能再写一套额外规则去推。

这会让：

- graph
- 目录结构
- 复用策略

再次脱节。

所以：

- `scope` 必须是 graph 的一部分

## 8. 为什么 `bucket` 也应进 graph

虽然 `artifactPlan` 已经有：

- `requiredBuckets`
- `terminalBucket`

但它只表达：

- 这条 matrix 理论覆盖到哪里

它不表达：

- 某个具体 stage 的输出属于哪个 bucket

所以 graph 里仍然需要：

- `bucket`

两者关系应是：

- `artifactPlan` 负责整体边界
- `executionPipelines.stages[].bucket` 负责逐阶段落点

## 9. stage `kind` 和 worker 的关系

第一版不建议在 manifest 里直接写 worker 名。

manifest 里只写：

- `kind`

例如：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target`
- `runtime-observe`
- `runtime-trace-compare`
- `report-assemble`

真正的 worker 选择由执行器根据：

- `kind`
- `executionContext`
- `runtimeProfile`
- `toolchainProfile`

去解析。

原因：

- worker 是实现细节
- `kind` 才是稳定语义

## 10. `HelloWorldObject` 的推荐 graph 设计

对首批 Windows 四个 matrix，推荐定义三条 pipeline。

## 10.1 `proof-runtime-output`

用于：

- `windows-dev-output`

graph：

```json
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
}
```

## 10.2 `proof-runtime-trace`

用于：

- `windows-reference-trace`

与 `proof-runtime-output` 的差异只有：

- `runtime-trace-compare` 插在 `runtime-observe` 后面
- `report-assemble` 依赖 `runtime-trace-compare`

## 10.3 `platform-buildable`

用于：

- `windows-android-buildable`
- `windows-linux-buildable`

graph：

- 前四段与上面一致
- `build-target` 后直接接 `report-assemble`
- 不包含 `runtime-observe`
- 不包含 `runtime-trace-compare`

这正好对应之前已经确认的：

- buildable matrix 终止于 `build`

## 11. matrix 侧最小改动

matrix 不再自己定义整条 graph，只增加：

- `pipelineId`

例如：

```json
{
  "matrixId": "windows-dev-output",
  "pipelineId": "proof-runtime-output"
}
```

```json
{
  "matrixId": "windows-reference-trace",
  "pipelineId": "proof-runtime-trace"
}
```

```json
{
  "matrixId": "windows-android-buildable",
  "pipelineId": "platform-buildable"
}
```

```json
{
  "matrixId": "windows-linux-buildable",
  "pipelineId": "platform-buildable"
}
```

## 12. `artifactPlan` 和 `pipelineId` 会不会重复

不会。

它们各自负责不同层：

### `pipelineId`

负责：

- 这条 matrix 的 stage graph 是什么

### `artifactPlan`

负责：

- 这条 matrix 应覆盖哪些 bucket
- 最终停在哪个 bucket

所以：

- `pipelineId` 决定“怎么跑”
- `artifactPlan` 决定“理论边界到哪”

两者应同时存在。

## 13. graph 是否允许 future override

第一版建议：

- graph 结构固定
- matrix 不允许局部覆写 stage graph

也就是：

- matrix 只能选 `pipelineId`
- 不能在 matrix 内再 patch `dependsOn`

原因：

- 第一版越简单越好
- 否则 graph 规则会再次分散到两层

如果未来确实有需要，再加：

- `stageOverrides`

但首批不建议引入。

## 14. 对复用/失效规则的直接收益

这份 graph 一旦存在，前面已经确定的复用/失效规则就有了正式挂点：

- `scope=shared` 的 stage 参与跨 matrix 复用
- `scope=matrix` 的 stage 不参与跨 matrix 复用
- `dependsOn` 直接定义单向失效传播链

这样执行器不必再靠隐藏规则猜。

## 15. 对未来大量 subject 的价值

你已经明确：

- `subjects/` 下未来会扩展大量调试项目

那就意味着执行模型不能只靠默认硬编码。

`executionPipelines` 的价值就在这里：

- canonical subject 可以复用主线 pipeline
- diagnostic subject 可以声明更短的 pipeline
- benchmark subject 以后可以声明带 `perf` / `report` 终段的 pipeline

而不需要每次都改执行器核心分支。

## 16. 当前可以正式固定的规则

这次可以正式固定为：

- `subject.manifest.json` 需要显式声明 stage dependency graph
- graph 以 `executionPipelines` 形式放在 subject 顶层
- matrix 通过 `pipelineId` 选择 graph
- graph stage 至少包含：
  - `stageId`
  - `kind`
  - `scope`
  - `bucket`
  - `dependsOn`
- manifest 不直接写 worker 名
- 第一版 matrix 不允许局部覆写 graph

## 17. 这一步之后最自然的下一个问题

如果继续往下收敛，最自然的下一个问题就是：

- `subject.manifest.json` 的完整增量 schema 草案

因为现在已经累计需要补的字段有：

- `supportedGoals`
- `artifactPlan`
- `pipelineId`
- `executionPipelines`

下一步就该把这些字段和现有字段汇总成一版完整 schema/draft。
