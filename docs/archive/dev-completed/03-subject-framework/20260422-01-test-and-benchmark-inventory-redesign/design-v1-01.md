# Test And Benchmark Inventory Redesign Design v1.01

Date: 2026-04-22 14:54:00 +08:00
Status: drafted

## 1. 目标

把当前零散的测试与性能信息收口成三张可恢复、可导出、可直接扫描的 inventory 主表，满足以下目标：

- 对能力面有一份完整清单，而不是只看 subject summary
- 对 subject 的 UnitTest 方法有一份明确的阶段表
- 对 benchmark 有一份详细的性能清单，而不是只看 summary case
- 视图言简意赅，默认一次显示最完整内容
- 表头必须提供中文详细解释，降低阅读门槛

本设计不直接改 benchmark dashboard 的交互视觉，不在本轮做 chart/card/tab 强化。

## 2. 总体结构

主清单固定为三张表：

1. `Capability Inventory`
2. `UnitTest Inventory`
3. `Benchmark Inventory`

产物固定为三种：

- `JSON`
  - authority 产物
- `CSV`
  - 导出产物
- `HTML`
  - 宽表阅读产物

其中：

- `JSON` 是唯一 machine-readable authority
- `CSV` 与 `HTML` 都由 `JSON` 派生，不反向承载 authority

## 3. 表粒度

### 3.1 Capability Inventory

行粒度固定为：

- 一行一个 `capability item`

用途：

- 回答“仓库当前应该覆盖什么能力”

### 3.2 UnitTest Inventory

行粒度固定为：

- 一行一个 `declared-unit-test x stage`

用途：

- 回答“某个 UnitTest 方法在每个阶段应当如何被验证”

### 3.3 Benchmark Inventory

行粒度固定为：

- 一行一个 `declared-benchmark x device`

用途：

- 回答“某个 benchmark case 在某台设备上的完整状态和最新性能证据”

不采用 `x platform`，原因如下：

- 现有性能证据天然携带 `deviceId` / `deviceName`
- 同一平台可存在多个设备，按平台聚合会丢失最新证据边界
- inventory 的目标是最大化可追溯性，而不是做汇总摘要

## 4. Authority 与来源

### 4.1 Declared Entry Authority

来源：

- `build/toolchains/run/testing/declarations.py`
- `build/toolchains/run/testing/compiled_catalog.py`

用于提供：

- `stableId`
- `alias`
- `assemblyName`
- `declaringType`
- `methodName`
- `methodSignature`
- `entryIndex`
- `category`
- `metrics`
- `modes`
- `requires`
- `evidence`
- `priority`
- `warmupCount`
- `iterationCount`
- `invocationCount`

### 4.2 Capability / Owner / Support Authority

来源：

- `subjects/*/subject.features.json`
- `build/toolchains/run/testing/capability_coverage.py`

用于提供：

- `ownerSubjectId`
- `capabilityFamily`
- `capabilityFamilyLabel`
- `capabilityItem`
- `capabilityItemLabel`
- `supportStates`
- `supportStateLabels`
- `proofRequired`
- `benchmarkRequired`

### 4.3 Registry Projection Authority

来源：

- `build/toolchains/run/testing/registry.py`

用于提供：

- `defaultGoalId`
- `defaultMatrixId`
- `goalIds`
- `matrixIds`
- `sourceEntry`
- `workloadEntry`
- `archetype`
- `archetypeLabel`
- `hotUpdateCapability`
- `hotUpdateCapabilityLabels`

### 4.4 Benchmark Latest Evidence Authority

来源：

- `docs/benchmark/overview.json`
- `docs/benchmark/subjects/*.json`

用于提供：

- `deviceId`
- `deviceName`
- `platformId`
- `modeStatus`
- `keyMetrics`
- `recordedAt`
- `gitCommit`
- `isStale`
- `staleReasonCode`
- `staleReasonLabel`

### 4.5 Stage / Gate Authority

来源：

- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/managed-native-hotupdate-test-pipeline.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`

用于定义：

- 哪些 gate/阶段需要出现在 `UnitTest Inventory`
- 各阶段如何派生 `required / optional / unsupported / covered`

## 5. 三张表字段定义

### 5.1 Capability Inventory

建议字段：

`OwnerSubject | CapabilityFamily | CapabilityItem | CapabilityLabel | SupportStates | ProofReq | BenchmarkReq | DeclaredUnitCount | DeclaredBenchmarkCount | ContractStatus`

字段说明：

- `OwnerSubject`
  - capability owner
- `CapabilityFamily`
  - family label，可附 code
- `CapabilityItem`
  - item code
- `CapabilityLabel`
  - item label
- `SupportStates`
  - contract declared support states
- `ProofReq`
  - 是否要求 formal proof
- `BenchmarkReq`
  - 是否要求 benchmark
- `DeclaredUnitCount`
  - 当前 capability 关联的 declared unit 数量
- `DeclaredBenchmarkCount`
  - 当前 capability 关联的 declared benchmark 数量
- `ContractStatus`
  - `ok / missingProof / missingBenchmark / unsupported / proofOptional`

### 5.2 UnitTest Inventory

建议字段：

`Subject | StableId | Alias | Method | Stage | StageOrder | Capability | Owner | SupportStates | Required | Goal | Matrix | Evidence | Priority | StageStatus | StatusReason`

字段说明：

- `Subject`
  - declared test 所在 subject
- `StableId`
  - formal stable id
- `Alias`
  - 人类可读短名
- `Method`
  - `Assembly / DeclaringType / MethodSignature`
- `Stage`
  - 当前行对应阶段
- `StageOrder`
  - 仅用于排序，可在 HTML 默认隐藏
- `Capability`
  - `family/item/label`
- `Owner`
  - capability owner subject
- `SupportStates`
  - capability contract support states
- `Required`
  - 当前阶段是否为该方法必经阶段
- `Goal`
  - 默认 goal id
- `Matrix`
  - 默认 matrix id
- `Evidence`
  - declared evidence labels
- `Priority`
  - declared priority
- `StageStatus`
  - 当前阶段状态
- `StatusReason`
  - 当前状态的一句解释

### 5.3 Benchmark Inventory

建议字段：

`Platform | Device | Subject | StableId | Alias | Method | Capability | Owner | SupportStates | SupportedModes | Metrics | Warmup | Iter | Invoke | M.Status | M.ms | M.ops | N.Status | N.ms | N.ops | N.Rel | I.Status | I.ms | I.ops | I.Rel | Stale | LastRecordedAt`

字段说明：

- `Platform`
  - 设备平台，如 `windows-x64`
- `Device`
  - `deviceName`，必要时附 `deviceId`
- `Subject`
  - owning subject
- `StableId`
  - benchmark stable id
- `Alias`
  - benchmark alias
- `Method`
  - benchmark workload entry
- `Capability`
  - capability label
- `Owner`
  - capability owner subject
- `SupportStates`
  - capability contract support states
- `SupportedModes`
  - declared benchmark modes
- `Metrics`
  - metric labels
- `Warmup`
  - warmup count
- `Iter`
  - iteration count
- `Invoke`
  - invocation count
- `M.*`
  - managed mode 最新状态与核心指标
- `N.*`
  - native mode 最新状态与核心指标
- `I.*`
  - interpreter mode 最新状态与核心指标
- `N.Rel`
  - native 相对 managed 的比值
- `I.Rel`
  - interpreter 相对 managed 的比值
- `Stale`
  - 该设备最新记录是否 stale
- `LastRecordedAt`
  - 该行最新记录时间

## 6. UnitTest 阶段字典

`UnitTest Inventory` 固定使用以下阶段：

1. `collector`
2. `registry`
3. `workspace`
4. `managed-proof`
5. `native-proof`
6. `hotupdate-proof`

### 6.1 为什么不放 `hotupdate-smoke`

本表粒度是 `declared-unit-test x stage`，因此阶段必须围绕 formal proof 展开。  
`hotupdate-smoke` 更适合 consumer smoke / integration 视图，不适合作为 declared unit method 的标准阶段。

### 6.2 StageOrder

排序固定为：

- `collector = 10`
- `registry = 20`
- `workspace = 30`
- `managed-proof = 40`
- `native-proof = 50`
- `hotupdate-proof = 60`

## 7. UnitTest 阶段派生规则

### 7.1 `collector`

含义：

- 该方法是否成功进入 declared collection

判定来源：

- declared catalog / collection

典型状态：

- `covered`
  - declared entry 已存在
- `missing-evidence`
  - 应存在但未进入 declared collection

### 7.2 `registry`

含义：

- 该方法是否成功进入 unified registry

判定来源：

- registry item existence

典型状态：

- `covered`
- `missing-evidence`

### 7.3 `workspace`

含义：

- 该方法是否可从 workspace collection + entryIndex 追到最终执行入口

判定来源：

- workspace manifest
- declared collection entry

典型状态：

- `covered`
- `missing-evidence`

### 7.4 `managed-proof`

含义：

- 该方法是否属于 managed proof 正式对象

判定来源：

- declared unit test 本身
- owner subject proof requirement

典型状态：

- 默认 `required`
- 若 capability contract 显式 proof optional，则 `optional`

### 7.5 `native-proof`

含义：

- 该方法是否需要 native proof

判定规则：

- 如果 capability `supportStates` 包含 `NativeGenerated`
  - 默认 `required`
- 如果 capability 不含 `NativeGenerated`
  - `unsupported` 或 `not-applicable`

### 7.6 `hotupdate-proof`

含义：

- 该方法是否需要 hotupdate proof

判定规则：

- 若该 declared unit test 带 `hotUpdateCapability`
  - `required`
- 若 capability / owner 规则不要求穿过 hotupdate 边界
  - `not-applicable`
- 若 contract 明确不支持 hotupdate 路径
  - `unsupported`

### 7.7 StageStatus 状态集合

统一状态集合：

- `required`
- `optional`
- `not-applicable`
- `unsupported`
- `missing-evidence`
- `covered`

说明：

- `required / optional / not-applicable / unsupported`
  - 表示阶段 obligation 判定
- `covered / missing-evidence`
  - 表示阶段 evidence 判定

实现时允许拆成：

- `StageRequirement`
- `StageCoverage`

但 HTML/CSV 最终可压缩成：

- `StageStatus`
- `StatusReason`

若压缩，优先级如下：

1. `unsupported`
2. `not-applicable`
3. `missing-evidence`
4. `covered`
5. `required`
6. `optional`

更推荐保留双字段：

- `StageRequirement`
- `StageCoverage`

如果下游坚持单状态列，再做压缩映射。

## 8. Benchmark 行规则

`Benchmark Inventory` 每一行固定绑定：

- 一个 declared benchmark
- 一个 device

每行内横向展示三种模式：

- `managed`
- `native`
- `interpreter`

模式状态使用现有 benchmark evidence 口径：

- `recorded`
- `missing`
- `unsupported`

额外补充：

- `stale` 不单独作为模式状态，而是行级/模式级附加信号

### 8.1 为什么不继续用现有 benchmark dashboard 作为主清单

现有 benchmark dashboard 的结构重心是：

- `summaryBenchmarkCase`
- `subject overview`
- `device comparison`
- `case detail`

这适合 summary/report，不适合 inventory。

inventory 的目标是：

- 默认一次展示全量 case
- 对每个 case 的设备状态做扁平展开
- 允许直接导出 CSV 扫描缺口

因此 benchmark dashboard 应降级为辅助报告。

## 9. 中文表头 Hint 规范

所有主表表头必须支持中文悬浮 `hint`。  
`hint` 不是一句话，而是完整说明卡。

### 9.1 Hint Schema

每个字段统一提供以下结构：

- `label`
- `summary`
- `definition`
- `why`
- `source`
- `values`
- `rule`
- `emptyMeans`

推荐 JSON 结构：

```json
{
  "key": "SupportStates",
  "label": "SupportStates",
  "hint": {
    "summary": "能力 contract 声明的执行可用形态。",
    "definition": "用于表示该 capability 当前是否有 native body、interpreter 路径、bridge 路径，或者被明确标记为 unsupported。",
    "why": "避免把“没有 native body”与“明确不支持”混为一谈。",
    "source": [
      "subjects/<subject>/subject.features.json",
      "build/toolchains/run/testing/capability_coverage.py"
    ],
    "values": [
      "1 NativeGenerated：存在 native 生成体",
      "2 InterpreterReady：可由解释器执行",
      "3 BridgeDispatch：通过桥接执行",
      "4 MetadataOnly：只有元数据可见性",
      "5 ExternalRuntime：依赖外部运行时",
      "6 Unsupported：当前 contract 明确不支持"
    ],
    "rule": "只要包含 6，该 capability 视为 unsupported by contract。",
    "emptyMeans": "通常表示 capability contract 未正确解析，属于异常数据。"
  }
}
```

### 9.2 HTML 交互要求

- 表头右侧放 `?`
- hover 时展示 hint 卡片
- hint 卡片固定分段显示：
  - `字段含义`
  - `为什么有这列`
  - `数据来源`
  - `取值说明`
  - `判定规则`
  - `空值代表什么`

### 9.3 语言要求

- 全部使用中文描述
- 不允许只写英文短语
- 英文术语只作为字段名或枚举值保留

## 10. 生成链路

建议最小生成链路如下：

1. 生成 `inventory-source.json`
   - 汇总 compiled catalog、capability contract、registry projection、benchmark evidence
2. 派生三张 authority JSON：
   - `capability-inventory.json`
   - `unit-test-inventory.json`
   - `benchmark-inventory.json`
3. 从 authority JSON 派生：
   - `capability-inventory.csv`
   - `unit-test-inventory.csv`
   - `benchmark-inventory.csv`
   - `inventory.html`

建议产物路径：

- `docs/testing-inventory/capability-inventory.json`
- `docs/testing-inventory/unit-test-inventory.json`
- `docs/testing-inventory/benchmark-inventory.json`
- `docs/testing-inventory/capability-inventory.csv`
- `docs/testing-inventory/unit-test-inventory.csv`
- `docs/testing-inventory/benchmark-inventory.csv`
- `docs/testing-inventory/inventory.html`

## 11. 与现有系统的关系

保留：

- compiled catalog
- capability contract
- registry item projection
- benchmark evidence JSON

降级：

- 现有 benchmark dashboard
  - 保留为辅助报告
  - 不再承担 inventory authority

不在本轮处理：

- 重新设计 benchmark dashboard 视觉
- 引入新的 benchmark chart 体系
- 新增 smoke/integration inventory

## 12. 风险与约束

### 12.1 风险

- `UnitTest Inventory` 的阶段 evidence 一部分来自派生，不全是现成字段
- `StageStatus` 若压成单字段，语义可能损失
- benchmark evidence 当前偏 summary 结构，抽平时要避免混掉 device 边界

### 12.2 约束

- 不允许再引入花哨布局作为主视图
- 不允许让 HTML 成为 authority
- 不允许在 hint 中只写术语，不写中文解释

## 13. 当前建议

建议按以下顺序落地：

1. 先做 authority JSON schema
2. 再做 CSV 导出
3. 最后做最薄 HTML 宽表壳

优先保证：

- 字段齐
- 语义稳
- 可恢复
- 可导出

而不是优先追求视觉表现。
