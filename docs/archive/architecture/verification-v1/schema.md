# Verification V1 Schema

## 目标

本文档把 [`spec.md`](./spec.md) 中已经冻结的架构规则进一步落成“对象 schema + 文件 contract”。它只回答三类问题：

- formal verification 层到底写哪些对象
- 每个对象的稳定主键、归并键和路径键是什么
- projection 层如何从 formal source 派生出 `testing-inventory` 和 `benchmark` 视图

本文档不重新定义 authority、closure policy 或状态语义；这些规则以 [`spec.md`](./spec.md) 为准。

## 1. Formal 文件布局

正式验证层固定使用以下目录：

```text
verification/archive/
  latest/
    closure-snapshot.json
    capability-snapshot.json
    stage-snapshot.json
    result-snapshot.json
  master/
    closure-master.json
    capability-master.json
    stage-master.json
    result-master.json
  reports/
    <closure-kind>/
      <scope-code>/
        summary.md
        authority.md
        semantic.md
        contract.md
        lifecycle.md
        packaging.md
        perf.md

subjects/<OwnerPack>/verification/
  codegen-stubs/
    <capability>/
      <route>[-<profile>]/
        stub-index.json
        ...
```

临时目录固定为：

```text
artifacts/
```

`artifacts/` 中的数据不是 formal source，不参与长期归档。

## 2. 公共类型

### 2.1 公共字段

所有 formal JSON 文件都应包含以下公共字段：

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `schemaVersion` | `int` | 当前 schema 主版本 |
| `generatedAtUtc` | `string` | ISO 8601 UTC 时间 |
| `producerId` | `string` | 产出该文件的聚合器/生成器 |
| `sourceSpecVersion` | `string` | 对应的 `verification-v1` 规格版本 |

### 2.2 公共枚举

#### `closureKind`

- `completed`
- `release`
- `platform_gate`
- `perf_gate`

#### `evidenceKind`

- `semantic`
- `contract`
- `lifecycle`
- `perf`
- `packaging`

#### `verificationState`

- `passed`
- `failed`
- `blocked`
- `missing`
- `planned`
- `not_required`

#### `supportState`

叶子层：

- `supported`
- `unsupported`
- `not_applicable`

聚合层允许追加：

- `partial`

### 2.3 公共引用类型

#### `SourceRef`

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `kind` | `string` | 引用类型，如 `collection` / `manifest` / `report` / `stub` / `log` |
| `path` | `string` | 仓库内相对路径 |
| `label` | `string` | 给人看的短说明 |
| `hash` | `string?` | 可选内容指纹 |

#### `BlockerRef`

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `code` | `string` | 稳定 blocker/reason code |
| `lane` | `string` | 对应 lane，如 `loader` / `contracts` |
| `message` | `string` | 给人读的说明 |

## 3. 核心 formal 对象

### 3.1 `ClosureRecord`

`closure-master` 和 `closure-snapshot` 的行对象。

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `closureId` | `string` | closure authority id，也是 `closure-master` merge key |
| `closureKind` | `string` | 见 `closureKind` |
| `scopeCode` | `string` | 当前收口范围代码 |
| `templateId` | `string` | closure template |
| `overlayId` | `string` | scope overlay |
| `targetCapabilities` | `string[]` | 目标 capability 集合 |
| `requiredRoutes` | `string[]` | 强制路由集合 |
| `requiredPlatforms` | `string[]` | 强制平台集合 |
| `requiredDeviceProfiles` | `string[]` | 强制设备 profile 集合 |
| `requiredEvidenceKinds` | `string[]` | 强制 evidence kind 集合 |
| `mandatoryLanes` | `string[]` | 强制权责审核 lane |
| `verificationState` | `string` | 聚合结果 |
| `supportState` | `string` | 聚合 support state |
| `blockers` | `BlockerRef[]` | 当前 closure blocker |
| `obligationCount` | `int` | obligation 数 |
| `passedCount` | `int` | 已通过数 |
| `failedCount` | `int` | 已失败数 |
| `blockedCount` | `int` | 已阻断数 |
| `missingCount` | `int` | 缺证据数 |
| `notRequiredCount` | `int` | 不要求数 |

### 3.2 `ObligationClaimRecord`

formal `result-master` 的核心主键对象。

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `obligationClaimId` | `string` | formal result merge key |
| `closureId` | `string` | 所属 closure |
| `capabilityId` | `string` | 对应 capability |
| `ownerSubjectId` | `string` | 正式 owner subject |
| `capabilityFamily` | `string?` | 可选能力族代码 |
| `requiredEvidenceKinds` | `string[]` | 强制证据类型 |
| `requiredRoutes` | `string[]` | 强制路由集合 |
| `requiredPlatforms` | `string[]` | 强制平台集合 |
| `requiredDeviceProfiles` | `string[]` | 强制设备 profile 集合 |
| `mandatoryLanes` | `string[]` | mandatory lane 集合 |
| `supportState` | `string` | support state |
| `verificationState` | `string` | 当前 obligation 聚合状态 |
| `blockers` | `BlockerRef[]` | blocker 列表 |
| `evidenceClaimIds` | `string[]` | 下游 evidence claim 集合 |
| `latestStageId` | `string?` | 最近更新阶段 |

### 3.3 `EvidenceClaimRecord`

formal 最小证明单元。

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `evidenceClaimId` | `string` | evidence claim authority id |
| `obligationClaimId` | `string` | 所属 obligation |
| `scenarioId` | `string` | `ScenarioBase` 稳定 id |
| `scenarioKind` | `string` | `unit-test` / `benchmark` / `proof` / `lifecycle` |
| `routeCode` | `string` | `managed` / `aot` / `hotupdate` 或子路由 |
| `platformCode` | `string` | 平台代码 |
| `deviceProfileCode` | `string` | 设备 profile 代码 |
| `evidenceKind` | `string` | 见 `evidenceKind` |
| `executionContractId` | `string` | 执行 contract |
| `verificationState` | `string` | 当前 claim 状态 |
| `supportState` | `string` | 当前 claim support state |
| `blockers` | `BlockerRef[]` | blocker 列表 |
| `latestEvidenceId` | `string?` | 最近一次 evidence kernel id |

### 3.4 `EvidenceRecord`

`EvidenceRecord` 由 `Evidence Kernel + Typed Payload` 组成。

#### `Evidence Kernel`

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `evidenceId` | `string` | 证据唯一 id |
| `evidenceClaimId` | `string` | 对应 evidence claim |
| `obligationClaimId` | `string` | 对应 obligation |
| `closureId` | `string` | 所属 closure |
| `stageId` | `string` | 所属阶段 |
| `routeCode` | `string` | 执行路由 |
| `platformCode` | `string` | 平台代码 |
| `deviceProfileCode` | `string` | 设备 profile |
| `actualDeviceId` | `string?` | 物理设备 id，仅事实层使用 |
| `verificationState` | `string` | 当前证据状态 |
| `supportState` | `string` | 当前证据 support state |
| `blockers` | `BlockerRef[]` | blocker 列表 |
| `sourceRefs` | `SourceRef[]` | 关联 collection/manifest/report/stub/log |
| `payloadHash` | `string` | payload 指纹 |
| `payloadKind` | `string` | 与 `evidenceKind` 对齐 |

#### `Typed Payload`

`Typed Payload` 至少支持以下形状：

- `SemanticPayload`
  - `assertions`
  - `exception`
  - `summary`
  - `stdoutRefs`
- `ContractPayload`
  - `schemaChecks`
  - `manifestChecks`
  - `abiChecks`
  - `codegenChecks`
- `LifecyclePayload`
  - `steps`
  - `loadResult`
  - `bindResult`
  - `activateResult`
  - `rollbackResult`
- `PerfPayload`
  - `metricSet`
  - `thresholdChecks`
  - `sampleSummary`
  - `baselineRef`
- `PackagingPayload`
  - `packageItems`
  - `versionChecks`
  - `integrityChecks`

### 3.5 `StageReportRecord`

阶段报告的 JSON 索引对象。Markdown 报告正文可再引用它。

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `stageId` | `string` | 阶段 id |
| `closureId` | `string` | 所属 closure |
| `scopeCode` | `string` | 所属 scope |
| `stageKind` | `string` | `authority` / `semantic` / `contract` / `lifecycle` / `packaging` / `perf` |
| `verificationState` | `string` | 阶段聚合结果 |
| `supportState` | `string` | 阶段 support state |
| `coveredObligationClaimIds` | `string[]` | 本阶段覆盖的 obligation |
| `successEvidenceIds` | `string[]` | 成功 evidence |
| `failedEvidenceIds` | `string[]` | 失败 evidence |
| `blockedEvidenceIds` | `string[]` | 阻断 evidence |
| `missingEvidenceClaimIds` | `string[]` | 缺失 claim |
| `blockers` | `BlockerRef[]` | blocker 列表 |
| `reportRefs` | `SourceRef[]` | markdown / html 报告引用 |

## 4. Formal 文件封装

### 4.1 `latest/*-snapshot.json`

所有 latest 文件统一采用如下封装：

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `schemaVersion` | `int` | schema 版本 |
| `generatedAtUtc` | `string` | 生成时间 |
| `closureId` | `string` | 本次 closure |
| `items` | `object[]` | 对应 record 列表 |

建议映射如下：

| 文件 | `items` 类型 |
| --- | --- |
| `closure-snapshot.json` | `ClosureRecord[]` |
| `capability-snapshot.json` | `CapabilitySummaryRecord[]` |
| `stage-snapshot.json` | `StageReportRecord[]` |
| `result-snapshot.json` | `ObligationClaimRecord[]` |

### 4.2 `master/*-master.json`

所有 master 文件统一采用如下封装：

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `schemaVersion` | `int` | schema 版本 |
| `generatedAtUtc` | `string` | 生成时间 |
| `mergeKey` | `string` | 当前 master 使用的 merge key 说明 |
| `items` | `object[]` | 已归并行对象 |

固定 merge key 如下：

| 文件 | merge key |
| --- | --- |
| `closure-master.json` | `closureId` |
| `capability-master.json` | `capabilityId` |
| `stage-master.json` | `stageId + scopeCode` |
| `result-master.json` | `obligationClaimId` |

## 5. Capability 汇总对象

### 5.1 `CapabilitySummaryRecord`

用于 `capability-snapshot` 和 `capability-master`。

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `capabilityId` | `string` | capability master merge key |
| `displayName` | `string` | 给人看的名称 |
| `ownerSubjectId` | `string` | owner subject |
| `defaultRoutes` | `string[]` | 默认路由 |
| `defaultPlatforms` | `string[]` | 默认平台 |
| `defaultDeviceProfiles` | `string[]` | 默认设备 profile |
| `requiredEvidenceKinds` | `string[]` | 默认 evidence kind |
| `verificationState` | `string` | 聚合验证状态 |
| `supportState` | `string` | 聚合 support state |
| `blockers` | `BlockerRef[]` | blocker 列表 |
| `latestClosureId` | `string?` | 最近一次 closure |
| `latestObligationClaimIds` | `string[]` | 最近 obligation 集合 |

## 6. Codegen 存根 contract

### 6.1 `stub-index.json`

每个 codegen 存根目录至少包含一个索引文件：

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `schemaVersion` | `int` | schema 版本 |
| `ownerSubjectId` | `string` | owner subject |
| `capabilityId` | `string` | capability |
| `routeCode` | `string` | 路由 |
| `profileCode` | `string?` | 可选 profile |
| `mergeKey` | `string` | `ownerSubjectId + capabilityId + routeCode [+ profileCode]` |
| `managedSourceRefs` | `SourceRef[]` | 对照的 managed 源码引用 |
| `stubRefs` | `SourceRef[]` | 存根文件引用 |
| `generatedFromEvidenceIds` | `string[]` | 来自哪些 evidence |

### 6.2 存根文件选择规则

正式存根不要求把所有中间生成物都搬过来，只保留最关键的对照面：

- 与 capability 直接相关的 managed 入口
- 关键 generated C# / C++ / manifest 片段
- 影响 contract 或 dispatch 的 glue 代码
- 必要时附一个结构化 diff 或 review 摘要

## 7. Projection Schema

Projection 层只读派生，不反向改写 formal source。

### 7.1 公共表头 `ColumnHint`

当前 `inventory.html` 已经使用富 tooltip。新模型继续保留这一能力，并冻结通用 hint 结构：

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `label` | `string` | 列名 |
| `summary` | `string` | 单行概述 |
| `definition` | `string` | 详细定义 |
| `why` | `string` | 为什么需要这一列 |
| `source` | `string[]` | 该列的 formal 来源 |
| `values` | `string[]` | 常见取值或读法 |
| `rule` | `string` | 使用规则 |
| `emptyMeans` | `string` | 空值含义 |

### 7.2 `UnitTestInventoryRow`

`docs/testing-inventory/unit-test-inventory.*` 的正式行粒度固定为：

```text
一行一个方法 x route/stage
```

推荐字段：

- `subjectId`
- `stableId`
- `alias`
- `method`
- `routeCode`
- `stageId`
- `stageKind`
- `capabilityFamily`
- `capabilityItem`
- `capabilityLabel`
- `ownerSubjectId`
- `supportState`
- `verificationState`
- `blockerCodes`
- `evidenceClaimId`
- `latestEvidenceId`

### 7.3 `BenchmarkInventoryRow`

`docs/testing-inventory/benchmark-inventory.*` 的正式行粒度固定为：

```text
一行一个 declared-benchmark x device-profile
```

推荐字段：

- `platformCode`
- `deviceProfileCode`
- `subjectId`
- `stableId`
- `alias`
- `method`
- `capabilityFamily`
- `capabilityItem`
- `capabilityLabel`
- `ownerSubjectId`
- `routeCode`
- `supportState`
- `verificationState`
- `metricSummary`
- `thresholdVerdict`
- `latestEvidenceId`

### 7.4 `CapabilityInventoryRow`

`docs/testing-inventory/capability-inventory.*` 的正式行粒度固定为：

```text
一行一个 capability x closure
```

推荐字段：

- `capabilityId`
- `capabilityLabel`
- `closureId`
- `closureKind`
- `ownerSubjectId`
- `requiredRoutes`
- `requiredPlatforms`
- `requiredDeviceProfiles`
- `supportState`
- `verificationState`
- `blockerCodes`
- `latestStageId`

### 7.5 Projection 与物理设备的边界

Projection 默认遵守以下规则：

- 首页矩阵使用 `deviceProfileCode`，不使用 `actualDeviceId`
- `actualDeviceId` 只允许出现在明细 tooltip 或 detail 弹窗
- `benchmark dashboard` 可展示物理设备名，但 formal merge 仍以 `deviceProfileCode` 为准

## 8. 兼容与迁移约束

### 8.1 与现有 inventory 的兼容点

当前已有视图里，以下设计可以直接延续：

- `unit-test-inventory` 的 `header + hint` 模型
- `benchmark-inventory` 的中文 tooltip 模型
- `inventory.html` 作为组合 projection 容器
- `verification/projections/benchmark/overview.json` 中“平台、subject、模式、设备”四层可读汇总

### 8.2 需要收敛的点

旧模型中以下字段需要在迁移时收敛到新 formal schema：

- `deviceId`
  - 降级为 execution fact 或 detail 字段，不再进入 formal path key
- `recorded/stale/unsupported`
  - 需要映射为 `verificationState + supportState + blocker`
- `mode`
  - 统一收敛为 `routeCode`
- 任何 run 级目录
  - 迁出 formal 层，仅保留在 `artifacts/`

### 8.3 实施优先级

建议实现顺序固定为：

1. 写 formal JSON schema/DTO
2. 写 master merge 逻辑
3. 写 stage report 生成器
4. 写 projection generator
5. 最后切 benchmark / inventory UI 消费口

这样可以保证 UI 切换前，formal source 已经稳定。
