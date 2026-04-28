# Verification V1 规格

## 摘要

`Verification V1` 采用 `graph-first` 架构，把验证系统拆成四个稳定层：

- `Authority Graph`：回答“谁负责、谁审核、边界在哪里”。
- `Claim Graph`：回答“本次必须证明什么、需要哪些证据”。
- `Execution Facts`：回答“实际跑了什么、产生了什么事实”。
- `Projection`：回答“人应该先看到什么视图”。

本规格的核心目标是：

- 支撑 `managed -> aot -> hotupdate` 主线的全链路验证。
- 支撑多平台、多设备、多宿主的扩展。
- 在不使用 `run-id` 目录归档的前提下，保留最新正式结果、归并后的总表、阶段报告和关键 codegen 存根。
- 把 `AOT / IL2CPP / codegen / contracts / loader / test governance` 六条主线纳入固定的“权责图审核”。
- 降低正式落地文档的写入压力，只保留真正有 authority 的正式产物。

## 关联 Authority

- [`docs/architecture/subject-test-framework-v1/INDEX.md`](../subject-test-framework-v1/INDEX.md)
  - 拥有执行主线、collection/manifest/codegen 分层与 managed/native/hotupdate 宿主边界 authority。
- [`docs/architecture/unsupported-feature-policy.md`](../unsupported-feature-policy.md)
  - 拥有对外 `supportState` 词汇与 reason code 使用边界。
- `verification/projections/testing-inventory/*`
  - 是面向人的投影视图与展示层，不是 formal verification authority。
- `verification/projections/benchmark/*`
  - 是 perf 展示层，不是 formal verification authority。

---

## 1. 背景与目标

当前验证链路已经不再是单一的“跑一组测试并产出一份报告”。它同时需要回答以下问题：

- 某个 capability 的 owner 是谁。
- 它在 `managed / aot / hotupdate` 哪些路由上必须闭合。
- 哪些平台、哪些设备 profile 是 release 必需，哪些只是扩展覆盖。
- 哪些问题属于实现失败，哪些属于结构缺口，哪些只是当前 closure 不要求。
- 哪些 codegen、contract、loader 产物必须留存，便于和 owner subject 的 managed 源码对照。

旧模型的问题主要有五类：

- 把 authority、计划、执行和展示混在同一条 pipeline 中，导致边界不清。
- 用 `run-id` 组织正式归档，目录会持续膨胀，且不利于读者直接看到当前结论。
- 首页过早暴露 scenario 和物理 device 细节，掩盖了 closure、owner、route 和 blocker 这些更高优先级信息。
- `failed / blocked / missing / unsupported` 混用，导致结果难以判断。
- codegen 关键产物离 owner subject 太远，难以与 managed source、测试项 manager 代码对照。

`Verification V1` 的目标不是修补旧测试管线，而是重新定义正式验证层的 authority、对象模型、归并规则和展示入口。它必须满足：

- 以 closure 为入口，而不是以一次 run 为入口。
- 以 obligation claim 为正式归并单元，而不是以原始 job 为归并单元。
- 以 projection 为 UI 入口，而不是直接暴露底层 graph。
- 以 stable key 归并，而不是以 `runId` 归档。
- 以自动执行为默认路径，但在进入自动执行前必须完成一次结构化自审。

---

## 2. 设计原则

### 2.1 Graph First

正式验证系统首先是图模型，而不是脚本流水线。脚本、runner、设备调度、上传动作都只是图模型的执行器。

### 2.2 Authority 与事实分离

“谁负责”与“实际发生了什么”必须是两个层次：

- `Authority Graph` 不记录临时执行结果。
- `Execution Facts` 不定义 owner、gate 或 closure policy。

### 2.3 Closure 驱动

所有正式验证都从 closure 出发。closure 定义“这次要收口什么”，然后再展开 obligation、evidence 和 job。

### 2.4 Correctness First

planner 的展开顺序固定为：

1. 权责图审核与结构闭合
2. semantic / contract / lifecycle 正确性
3. packaging
4. perf

性能从来不是 correctness 缺口的替代品。

### 2.5 Projection 优先于原始图

人先看固定投影视图，而不是直接看 graph dump。主页默认只展示 closure、responsibility、route、platform-device 四类信息；scenario 和物理 device 只在 detail 中展开。

### 2.6 正式归档最小化

正式层只保留：

- 最新正式快照
- 按 stable key 归并后的 master 总表
- 阶段测试报告
- codegen 存根

过程产物全部进入临时目录，允许清理。

### 2.7 Owner 就近留存

关键 codegen 存根必须放在 owner subject 附近，便于与 managed source、proof/benchmark manager、host 代码直接对照。

### 2.8 状态词汇严格分层

- `supportState` 与 `verificationState` 分离。
- `partial` 只允许出现在聚合层。
- `unsupported` 通过 `supportState + blocker` 表达，不能伪装成普通 `failed`。
- `failed / blocked / missing` 必须全程保持区分。

### 2.9 自动执行默认开启

正常路径不引入人工复核 gate。系统应尽量自动推进；只有在自审阶段发现结构问题、authority 冲突或未冻结的开放问题时，才暂停并要求用户确认。

---

## 3. 总体架构

`Verification V1` 的正式主干如下：

```text
Closure
  |
  v
Authority Graph -----------+
  |                        |
  v                        |
Claim Graph                |
  |                        |
  v                        |
Planner Preflight Review   |
  |                        |
  +---- structural gap ----+--> blocked report
  |
  v
Evidence Claims
  |
  v
Jobs / Executors / Devices
  |
  v
Execution Facts
  |
  v
Merge + Archive
  |
  v
Projection Views / Inventory / Benchmark Dashboard
```

该架构中最重要的边界如下：

- `Authority Graph` 决定 owner、lane、边界和审核责任。
- `Claim Graph` 决定此次 closure 要证明什么。
- `Execution Facts` 记录执行事实，不拥有 policy。
- `Projection` 只是正式结果的只读展开，不反向成为 authority。

固定主页投影视图如下：

```text
[Closure Strip]
    |
    +-- [Responsibility View]
    +-- [Route Topology View]
    +-- [Platform-Device Matrix]
    +-- [Detail]
```

其中：

- `Closure Strip` 先回答“这次要收口什么、现在闭合到哪里”。
- `Responsibility View` 先回答“谁负责、谁卡住了”。
- `Route Topology View` 先回答“managed -> aot -> hotupdate 哪一段有缺口”。
- `Platform-Device Matrix` 先回答“哪个平台/设备 profile 已闭合、哪个还缺失”。
- `Detail` 才进入 scenario、method、物理 device、payload 和日志引用。

---

## 4. Authority Graph

### 4.1 作用

`Authority Graph` 是验证系统的权责骨架，用来表达：

- 哪条主线拥有最终 authority。
- 哪个 owner subject 负责某项 capability。
- 哪些主线必须参与审核。
- 哪些边界属于 contract，哪些属于 implementation。

### 4.2 固定审核主线

以下六条 lane 在 `Verification V1` 中是固定主线，必须进入权责图审核：

| Lane | 负责问题 |
| --- | --- |
| `aot` | AOT 语义闭合、路由要求、平台限制 |
| `il2cpp` | managed -> native 转译与 native 语义闭合 |
| `codegen` | 生成边界、模板与关键产物的 owner |
| `contracts` | collection/schema/manifest/ABI/host contract |
| `loader` | patch 装载、绑定、版本、rollback、生命周期 |
| `test_governance` | closure policy、验证口径、formal object 与 gate 规则 |

### 4.3 节点类型

`Authority Graph` 至少包含以下节点：

| 节点类型 | 含义 |
| --- | --- |
| `AuthorityLane` | 一条主线审核责任 |
| `AuthorityUnit` | 具体 authority 单元，例如文档、模块、规则集 |
| `OwnerSubject` | 负责 capability 的正式 subject |
| `CapabilityBase` | 稳定的能力基座 |
| `ClosureTemplate` | 一类 closure 的模板 authority |
| `ScenarioBase` | 稳定场景定义的 owner 边界 |

### 4.4 边类型

`Authority Graph` 至少支持以下边：

| 边类型 | 含义 |
| --- | --- |
| `owns` | 谁拥有该对象的最终变更权 |
| `governs` | 哪个 authority unit 负责解释规则 |
| `audits` | 哪个 lane 必须参与审核 |
| `depends_on` | 该对象依赖哪个上游 authority |
| `hosts` | 哪个 owner subject 承载该 capability 的正式验证资产 |

### 4.5 权责图审核要求

凡是命中主线的正式 closure，在 job 生成前必须完成一次 `authority` 阶段审核。审核至少回答：

- owner subject 是谁。
- 六条主线中哪些 lane 是 mandatory。
- 每条 mandatory lane 的输入 contract、输出 contract 和 blocker 条件是什么。
- capability 的 formal evidence kind 由谁要求、谁消费。
- codegen 存根、stage report 和 master 归并分别落在哪里。

权责图审核通过后，planner 才能继续展开 obligation claim。若审核失败，则直接形成 `blocked` 结果，不进入执行阶段。

### 4.6 Authority 不得悬空

以下情况都视为 authority 缺口：

- capability 没有 owner subject。
- capability 命中主线，但没有 mandatory lane。
- closure 需要某类 evidence，但 authority graph 中没有对应 owner。
- codegen、loader、contract 的责任同时落在多个节点且无主次关系。

这些缺口都必须在 preflight 阶段阻断。

---

## 5. Claim Graph

### 5.1 双层 Claim 模型

`Verification V1` 采用双层 claim：

- `ObligationClaim`
- `EvidenceClaim`

两者职责不同：

- `ObligationClaim` 回答“本次 closure 下，这个 capability 必须被证明到什么程度”。
- `EvidenceClaim` 回答“为了满足这个 obligation，需要在哪个 route/platform/device profile/scenario 上拿到哪类证据”。

### 5.2 展开关系

展开顺序固定为：

```text
Closure
  -> ObligationClaim
  -> EvidenceClaim
  -> Job
```

其中：

- 一个 `Closure` 会展开为多个 `ObligationClaim`。
- 一个 `ObligationClaim` 会展开为多个 `EvidenceClaim`。
- 一个 `EvidenceClaim` 可以由一个或多个 job 共同满足，但 formal 归并仍回到同一个 `EvidenceClaim`。

### 5.3 ObligationClaim

`ObligationClaim` 是正式 master 归并的核心单位。它至少包含：

| 字段 | 含义 |
| --- | --- |
| `obligationClaimId` | 稳定 obligation 标识 |
| `closureId` | 所属 closure |
| `capabilityId` | 对应 capability |
| `ownerSubjectId` | 正式 owner |
| `requiredEvidenceKinds` | 需要的 evidence kind 集合 |
| `requiredRoutes` | 需要闭合的 route |
| `requiredPlatforms` | 需要闭合的平台集合 |
| `supportState` | 支持状态，不等于验证结果 |

### 5.4 EvidenceClaim

`EvidenceClaim` 是执行面最小正式证明单元。它至少包含：

| 字段 | 含义 |
| --- | --- |
| `evidenceClaimId` | 稳定 evidence claim 标识 |
| `obligationClaimId` | 所属 obligation |
| `scenarioId` | 对应 scenario |
| `routeCode` | `managed / aot / hotupdate` 或其子路由 |
| `platformCode` | 目标平台代码 |
| `deviceProfileCode` | 设备 profile，而不是物理设备 |
| `evidenceKind` | `semantic / contract / lifecycle / perf / packaging` |
| `executionContractId` | 实际执行 contract |

### 5.5 Claim 是 formal 层，不是 runtime 临时对象

以下内容不能直接成为 formal 归并对象：

- 裸 job
- 临时 executor session
- 一次 runner 进程
- 单次设备连接

这些只属于 `Execution Facts`。formal 层必须先回写到 `EvidenceClaim` 和 `ObligationClaim`。

---

## 6. Projection 模型

### 6.1 原则

投影视图只读，不拥有 policy。所有页面、表格、dashboard 都必须从 formal graph 和 master 表派生，而不是反向定义规则。

对于 evidence-driven projection，例如 `Program / DLL / Verification Project / Artifact` 这类 DLL-first reporting 视图，必须显式区分：

- `primary evidence`：只允许引用 `artifacts/**` 中的真实执行产物，用于驱动 `passed / missing / blocked` 等状态判定，并进入 artifact index。
- `support refs`：允许引用 `docs/**`、`subjects/**`、`verification/**` 等辅助定位材料，但它们只用于说明上下文、authority 或实现入口，不能把项目状态置为 `passed`，也不能混入 primary artifact table。

### 6.2 固定主页视图

| 视图 | 主问题 | 主键 | 默认不展示 |
| --- | --- | --- | --- |
| `Closure Strip` | 这次 closure 闭合到哪里 | `closureId` | scenario、物理 device |
| `Responsibility View` | 谁负责、谁阻断 | `ownerSubjectId + lane` | 原始 payload |
| `Route Topology View` | 哪条 route 断了 | `closureId + route` | 单条 method |
| `Platform-Device Matrix` | 哪个平台/设备 profile 缺口最大 | `platformCode + deviceProfileCode` | 物理 device 明细 |
| `Detail` | 具体缺口和证据是什么 | `evidenceClaimId` | 无 |

### 6.3 首页展示顺序

默认顺序固定，不允许随意互换：

1. `Closure Strip`
2. `Responsibility View`
3. `Route Topology View`
4. `Platform-Device Matrix`
5. `Detail`

这样做的原因是：

- 先看 closure，确认本次到底在收什么口。
- 再看 responsibility，快速定位谁拥有 blocker。
- 再看 route 和 platform-device，判断缺口分布。
- 最后才下钻到 scenario 和 payload。

### 6.4 Scenario 与设备的展示边界

- `scenario` 不进入首页主列表。
- `物理 device` 不进入首页主矩阵。
- `deviceProfile` 可以进入首页矩阵，因为它属于 planning 维度。
- `actualDeviceId` 只在 `Detail` 中展示，并且仅作为事实引用，不参与 formal merge key。

### 6.5 下游派生视图

以下现有视图都是 projection，不是 authority：

| 文件/页面 | 角色 | 推荐粒度 |
| --- | --- | --- |
| `docs/testing-inventory/unit-test-inventory.*` | unit test 明细投影 | `一行一个方法 x route/stage` |
| `docs/testing-inventory/benchmark-inventory.*` | benchmark 明细投影 | `一行一个 declared benchmark x device profile` |
| `docs/testing-inventory/capability-inventory.*` | capability 总览投影 | `一行一个 capability x closure` |
| `docs/testing-inventory/inventory.html` | 人工浏览入口 | 组合以上三类投影 |
| `verification/projections/benchmark/*` | perf 展示入口 | 从 perf evidence 和 benchmark master 派生 |
| `verification/projections/foundation-dll-audit/program.json` | foundation DLL 报告投影入口 | 三轴进度（DLL Completion / Capability Closure / Workflow Progress）+ DLL Matrix |
| `verification/projections/foundation-dll-audit/dll-matrix.json` | DLL 矩阵投影 | 每 DLL 一行，含 gate 列 + Closure/Progress 双轴列 |
| `verification/projections/foundation-dll-audit/dlls/<assembly>.json` | DLL 详情投影 | 每 DLL 一个，含 capability families、waiver records、source links |
| `docs/verification/foundation-dll-audit/dashboard.html` | 人工浏览入口（HTML） | 从 projection JSON 派生 |
| `docs/verification/foundation-dll-audit/dlls/<assembly>.html` | DLL 详情浏览入口（HTML） | 从 projection JSON 派生 |
| `docs/verification/foundation-dll-audit/artifact-index.html` | artifact 索引（HTML，独立次级入口） | 每 artifact 一行 |

### 6.5.1 Capability-Family Ledger 投影

foundation-dll-audit 投影系列是 evidence-driven closure 投影的一个实例，遵循以下权威链：

1. **分母 authority**：`capability-family ledger`（由 audit input manifest + surface ledger + semantic ledger + nativeization plan 合成）
2. **进度 authority**：`workflow gate contract`（family 级 gate 的 policyState 和 executionState 决定 passedGates / requiredGates）
3. **闭包 authority**：`DLL completed contract`（6 条硬规则：family 全 closed、无 in-progress、所有 required gates passed、completion-certification passed、waiver authority 有效、evidence 全部 exists）
4. **豁免 authority**：`waiver contract`（waiver / exclusion / platform-blocked 三类，必须带正式 authorityRef，不允许引用 task STATUS.md）

该投影的 schemaVersion 当前为 2，采用 CapabilityClosure + WorkflowProgress 双轴字段结构。

这些页面上的表头提示、tooltip、中文解释都来自 formal schema 元数据，而不是页面内部手写口径。

---

## 7. Closure Policy

### 7.1 采用 Template + Scope Overlay

每个 closure 由两部分组成：

- `ClosureTemplate`
- `ScopeOverlay`

其中：

- `ClosureTemplate` 定义这类 closure 的默认 obligation、默认 evidence kinds、默认 blocking 规则。
- `ScopeOverlay` 定义这一次具体作用于哪些 capability、route、platform、device profile、release scope。

### 7.2 固定 Closure Kind

`Verification V1` 固定支持四类 closure：

| Closure Kind | 用途 |
| --- | --- |
| `completed` | 判断某项能力是否可正式收口为 completed |
| `release` | 判断某个 release scope 是否可发布 |
| `platform_gate` | 判断某个平台/宿主组合是否准入 |
| `perf_gate` | 判断性能门禁是否满足 |

### 7.3 Closure 冻结规则

进入 planner 前，closure 必须冻结以下信息：

- `closureKind`
- `scopeCode`
- `targetCapabilities`
- `requiredRoutes`
- `requiredPlatforms`
- `requiredDeviceProfiles`
- `requiredEvidenceKinds`
- `mandatoryLanes`

如果这些信息在执行过程中发生变化，必须生成新的 `closureId`，而不是在原 closure 下热修。

### 7.4 Closure 只描述“本次收口范围”

closure 不负责描述：

- 单条 job 怎么运行
- 某个设备这次连接是否成功
- 哪个 runner 用了哪条命令行

这些属于 execution contract 与 execution facts。

### 7.5 Closure 输出

每个 closure 至少产出以下正式对象：

- 一条 closure 聚合结果
- 一组 obligation claim
- 一组 stage report
- 一个 latest snapshot 更新
- 一个 master 归并更新

---

## 8. Capability Obligation

### 8.1 Capability Base + Obligation Fragments

能力模型拆为：

- `CapabilityBase`
- `ObligationFragments`

`CapabilityBase` 只定义稳定语义，不塞入 closure 特有要求。`ObligationFragments` 负责描述：

- 哪些 route 必须验证
- 哪些平台必须验证
- 哪些 evidence kind 是 mandatory
- 是否要求 perf
- 是否要求 packaging
- 是否要求 hotupdate lifecycle

### 8.2 CapabilityBase 负责稳定语义

`CapabilityBase` 至少定义：

- `capabilityId`
- `displayName`
- `ownerSubjectId`
- `semanticBoundary`
- `defaultSupportEnvelope`
- `canonicalScenarioFamilies`

它不应直接携带某次 release、某个平台 gate 的局部策略。

### 8.3 Obligation Fragment 负责可组合要求

`ObligationFragment` 是可组合的规则片段，可按 closure 和 scope 叠加。典型片段包括：

- `semantic-proof`
- `contract-compliance`
- `lifecycle-hosting`
- `perf-required`
- `packaging-required`
- `hotupdate-impact`
- `platform-specific`
- `route-specific`

### 8.4 主线 Lane 要求

命中以下主线时，相关 fragment 必须显式挂入 obligation，而不是隐式推断：

- `aot`
- `il2cpp`
- `codegen`
- `contracts`
- `loader`
- `test_governance`

这使得“权责图审核”可以直接从 obligation claim 回溯到 lane。

### 8.5 not_required 的来源

`not_required` 只能从 closure scope 推导，不能从执行失败倒推。换言之：

- closure 不要求某条 route，则该 route 的相关 claim 可以是 `not_required`。
- closure 明确要求某条 route，但当前未实现，只能是 `blocked` 或 `missing`，不能伪装成 `not_required`。

---

## 9. Scenario Contract

### 9.1 采用 Scenario Base + Binding Fragments + Execution Contract

场景模型拆成三层：

- `ScenarioBase`
- `BindingFragments`
- `ExecutionContract`

### 9.2 ScenarioBase

`ScenarioBase` 表示稳定的验证语义单元。对于当前仓库，推荐粒度如下：

- unit test：一个方法一个 `ScenarioBase`
- proof：一个正式 proof 入口一个 `ScenarioBase`
- benchmark：一个 declared benchmark 一个 `ScenarioBase`
- lifecycle：一个装载/绑定/回滚场景一个 `ScenarioBase`

### 9.3 BindingFragments

`BindingFragments` 不改变场景语义，只补足执行绑定信息，例如：

- `routeCode`
- `platformCode`
- `deviceProfileCode`
- `hostFlavor`
- `loaderMode`
- `packageProfile`
- `codegenProfile`

这保证同一个 `ScenarioBase` 可以被多个 route/platform 复用，而不产生重复定义。

### 9.4 ExecutionContract

`ExecutionContract` 负责把 claim 映射到可执行对象。它至少定义：

- 调用哪个 runner/host
- 读取哪些 collection/manifest/package
- 需要哪个 device allocator
- 成功/失败如何回写 evidence payload

### 9.5 Scenario 与表格粒度

本仓库的两个核心细表采用如下正式粒度：

- `unit-test-inventory`：`一行一个方法 x route/stage`
- `benchmark-inventory`：`一行一个 declared benchmark x device profile`

其中：

- 方法和 declared benchmark 来自 `ScenarioBase`
- route、stage、device profile 来自 `BindingFragments`
- tooltip、列说明、中文解释来自 schema metadata

### 9.6 物理设备不进入合同主键

物理设备只属于运行事实。formal 合同层只能依赖：

- `deviceProfileCode`
- `platformCode`
- `routeCode`

`actualDeviceId` 只进入 evidence payload，不进入 scenario 合同主键。

---

## 10. Evidence Contract

### 10.1 采用 Kernel + Typed Payload

所有正式证据采用统一外壳：

- `Evidence Kernel`
- `Typed Payload`

`Kernel` 提供统一检索、归并和状态判断字段；`Payload` 根据证据类型承载详细数据。

### 10.2 固定 Evidence Kind

`Verification V1` 固定支持五类 evidence：

| Evidence Kind | 说明 |
| --- | --- |
| `semantic` | 语义正确性、断言与输出行为 |
| `contract` | schema、manifest、ABI、binding、codegen contract |
| `lifecycle` | load、bind、activate、rollback、unload 生命周期 |
| `perf` | benchmark 指标、分布、阈值与回归判断 |
| `packaging` | package、产物装配、版本、发布物完整性 |

### 10.3 Evidence Kernel 必备字段

`Kernel` 至少包含：

| 字段 | 含义 |
| --- | --- |
| `evidenceClaimId` | 对应 claim |
| `obligationClaimId` | 对应 obligation |
| `closureId` | 所属 closure |
| `stageId` | 所属阶段 |
| `routeCode` | 执行路由 |
| `platformCode` | 平台 |
| `deviceProfileCode` | 设备 profile |
| `actualDeviceId` | 物理设备标识，可空 |
| `verificationState` | 正式验证状态 |
| `supportState` | 正式支持状态 |
| `blockerCodes` | 阻断原因列表 |
| `producerId` | 哪个执行器产出 |
| `producedAtUtc` | 产出时间 |
| `sourceRefs` | 关联 collection/manifest/report/stub 路径 |
| `payloadHash` | 载荷指纹 |

### 10.4 Typed Payload

不同证据类型的 payload 负责承载详细内容，例如：

- `semantic`
  - 断言结果、异常、返回值摘要、关键 stdout/stderr 引用
- `contract`
  - schema diff、manifest 对比、ABI 校验、codegen 审查结果
- `lifecycle`
  - load/bind/activate/unload/rollback 序列与阶段性状态
- `perf`
  - 原始样本摘要、指标统计、阈值比较、设备 profile
- `packaging`
  - package 组成、版本关系、签名/结构检查结果

### 10.5 Codegen 存根

重要 codegen 产物不直接埋在临时目录，也不要求完整保留所有中间文件。正式系统只保留“可对照的存根”：

- 只保留最能解释 route/contract 差异的关键文件
- 必须能回溯到 `ownerSubjectId + capabilityId + route [+ profile]`
- 必须和 owner subject 的 managed source 距离足够近，便于对照

这些 codegen 存根通过 `sourceRefs` 挂到 evidence kernel 上。

---

## 11. Planner Expansion

### 11.1 固定展开顺序

planner 展开顺序固定为：

```text
closure
  -> obligation claims
  -> evidence claims
  -> jobs
```

任何 job 都必须有上游 evidence claim；不允许绕过 claim 直接调 executor。

### 11.2 自动执行前自审

在进入 job 生成前，planner 必须执行一次 `preflight self-review`。它至少检查：

1. authority 是否完整
2. closure scope 是否冻结
3. 是否仍存在未关闭的 open question
4. obligation fragments 是否缺失
5. route/platform/device profile 绑定是否可解析
6. evidence kind 是否有对应 execution contract
7. formal 输出路径是否确定
8. codegen 存根是否有 owner 落点

### 11.3 阻断规则

若自审发现以下问题，planner 必须停止自动执行并向用户报告 blocker，而不是边跑边猜：

- authority 冲突
- owner subject 不唯一
- closure scope 未冻结
- open question 未关闭
- 关键 route 无 execution contract
- formal 路径或 merge key 无法稳定确定

### 11.4 正常自动推进条件

只要以下条件同时满足，系统应自动继续：

- 自审通过
- 没有结构 blocker
- 所需执行器、平台和 device allocator 可用
- 当前 closure 不需要额外用户意图确认

正常路径不增加人工复核环节。

### 11.5 正确性优先、性能后置

job 生成顺序固定为：

1. `authority`
2. `semantic`
3. `contract`
4. `lifecycle`
5. `packaging`
6. `perf`

`perf` 只能在 correctness 链路可判定后进入正式 gate。

### 11.6 结构缺口不应伪装成执行失败

例如：

- 某平台没有对应 loader contract
- 某 route 缺少 codegen profile
- 某 capability 没有 owner subject

这些都应在 preflight 阶段转为 `blocked`，而不是等 job 跑挂后写成 `failed`。

---

## 12. 归并与归档

### 12.1 目录职责

`Verification V1` 的落地目录职责固定如下：

```text
artifacts/
  # 临时目录，只放过程产物

verification/archive/
  latest/
  master/
  reports/

subjects/<OwnerPack>/verification/
  codegen-stubs/
```

### 12.2 临时目录

`artifacts/` 只放过程产物，例如：

- 原始 runner 输出
- 临时 schema dump
- 设备执行日志
- 中间转换文件
- 待归并的 execution facts

这些内容都不是 formal archive，可以清理。

### 12.3 正式目录

`verification/archive/` 是 formal verification 结果目录，推荐结构如下：

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
```

### 12.4 Latest 与 Master

- `latest` 表示本次 closure 归并后的最新正式快照，允许覆盖。
- `master` 表示按 stable merge key 累积归并的总表，不按 run 维度膨胀。

### 12.5 阶段报告

每个正式 closure 都必须产出阶段报告。阶段报告至少包含：

- 该阶段覆盖了哪些 obligation claim
- 成功项、失败项、阻断项、缺失项
- 对应 evidence kind
- 关键 blocker 和 reason code
- 关联 formal 产物引用

阶段报告必须同时记录成功项和失败项，不能只记录异常。

### 12.6 Codegen 存根落点

关键 codegen 存根的正式落点固定为：

```text
verification/evidence/owners/<OwnerPack>/codegen-stubs/<capability>/<route>[-<profile>]/
```

存根目录采用 stable path 覆盖更新，不按 run 追加新目录。

### 12.7 下游投影

`verification/projections/testing-inventory/*` 根目录和 `verification/projections/benchmark/*` 页面属于下游投影层，应从 `verification/archive/latest` 与 `verification/archive/master` 重新生成，而不是直接消费临时运行目录。

---

## 13. ID / Merge / Path

### 13.1 三层标识分离

`Verification V1` 明确区分三种标识：

| 类型 | 作用 |
| --- | --- |
| `Authority ID` | 标识 graph 中“这是谁、这是什么” |
| `Merge Key` | 标识 formal master 如何归并 |
| `Path Key` | 标识文件系统正式落点 |

这三者可以相关，但不能混为一个概念。

### 13.2 固定 Merge Key

正式 master 的 merge key 固定如下：

| 对象 | Merge Key |
| --- | --- |
| `result-master` | `obligationClaimId` |
| `stage-master` | `stageId + scopeCode` |
| `capability-master` | `capabilityId` |
| `closure-master` | `closureId` |
| `codegen-stub` | `ownerSubjectId + capabilityId + routeCode [+ profileCode]` |

### 13.3 Path Key 规则

`Path Key` 只允许使用稳定维度，例如：

- `closureKind`
- `scopeCode`
- `ownerSubjectId`
- `capabilityId`
- `routeCode`
- `profileCode`

不得把以下维度带入 formal 路径：

- `runId`
- `sessionId`
- `attemptId`
- `actualDeviceId`

### 13.4 Latest 覆盖语义

`latest` 的语义是“同一 stable key 的当前正式快照”，因此允许覆盖。它不是历史归档目录。

### 13.5 运行标识的存放边界

`runId/sessionId/attemptId/actualDeviceId` 可以存在于：

- `artifacts/` 临时目录
- execution facts 原始记录
- evidence payload 的事实字段

但不能进入：

- `master` merge key
- 正式目录层级
- closure/stage/capability formal path

---

## 14. 状态与判定

### 14.1 Verification State

叶子层的 `verificationState` 固定为：

| 状态 | 含义 |
| --- | --- |
| `passed` | 已拿到满足要求的正式证据 |
| `failed` | 已执行且结果不满足要求 |
| `blocked` | 结构或前置条件阻断，未进入正常验证 |
| `missing` | 本应有证据，但当前没有拿到 |
| `planned` | 已生成 claim，但尚未执行 |
| `not_required` | 当前 closure 明确不要求 |

运行中态如 `queued`、`running`、`retrying` 只允许出现在临时执行层，不进入 formal archive。

### 14.2 Support State

`supportState` 与 `verificationState` 分离：

- 叶子层只允许：
  - `supported`
  - `unsupported`
  - `not_applicable`
- 聚合层允许额外出现：
  - `partial`

### 14.3 Partial 只在聚合层出现

`partial` 只能表示“聚合后的覆盖不完整”，例如：

- 某 capability 在 `managed` 有证据，但 `aot` 尚未闭合
- 某 benchmark 只覆盖部分 device profile
- 某 release scope 只覆盖部分平台

叶子 claim 不能直接写成 `partial`。

### 14.4 Unsupported 的表达

`unsupported` 必须通过：

- `supportState = unsupported`
- `blockerCodes = [...]`

共同表达。它不是普通 `failed` 的别名。

### 14.5 聚合判定

formal 聚合时，`verificationState` 判定顺序固定为：

1. 只要存在 mandatory `failed`，聚合结果为 `failed`
2. 否则只要存在 mandatory `blocked`，聚合结果为 `blocked`
3. 否则只要存在 mandatory `missing`，聚合结果为 `missing`
4. 否则如果所有 mandatory 项为 `passed` 或 `not_required`，聚合结果为 `passed`
5. 否则为 `planned`

### 14.6 Closure Kind 的附加判定

不同 closure kind 的 mandatory 集合不同：

- `completed`
  - 必须闭合该 capability 的 correctness 链路；perf 仅在 fragment 要求时参与。
- `release`
  - 必须闭合 release scope 内所有 mandatory route/platform 的 correctness 与 packaging。
- `platform_gate`
  - 必须闭合指定平台/宿主下的 correctness、lifecycle、packaging。
- `perf_gate`
  - 必须闭合指定 device profile 下的 perf claim，且 correctness 不能是失败或阻断。

---

## 15. 上传与清理

### 15.1 正式上传顺序

每次 closure 完成后，正式上传顺序固定为：

1. 从 `artifacts/` 收集 execution facts
2. 规范化为 formal evidence kernel + typed payload
3. 更新 `latest`
4. 归并 `master`
5. 生成 stage reports
6. 生成下游 projection
7. 上传/提交正式产物
8. 清理临时过程产物

如果本轮改动触及 formal report / projection contract，例如：

- `latest/master/reports` 字段或聚合口径
- `Program / DLL / Verification Project / Artifact` 报告对象
- `testing-inventory`、`benchmark` 或其他 projection 页面字段
- 证据链接规则
- `capability-family ledger` 的分母条目或 family 状态

则不能只改 schema、模板或页面读取逻辑；仍必须按上述顺序刷新正式数据与下游 projection。

foundation-dll-audit 投影的 refresh 顺序：

1. 确认 capability-family-ledger.json 是最新冻结快照
2. 读取所有 audit input / surface ledger / semantic ledger 的最新版本
3. 生成 program.json（含三轴进度）
4. 生成 dll-matrix.json（含 gate 列 + Closure/Progress 列）
5. 生成 dlls/\<assembly\>.json（含 capability families、waiver records、source links）
6. 从 projection JSON 派生 HTML 页面（dashboard.html + DLL detail pages + artifact-index.html）
7. 提交正式产物并更新 archive

其中 step 6 由 generator 在 projection JSON 就绪后自动执行。

### 15.2 无人工复核闸门

正常路径不要求人工复核。只要：

- preflight 自审通过
- formal 归并成功
- stage reports 成功生成

系统就应自动完成上传与清理。

### 15.3 必须暂停并确认的情况

只有以下情况需要停下来和用户确认：

- authority 冲突无法自动裁决
- closure scope 被改动
- 仍存在未冻结的 open question
- formal merge key 或 path key 将发生破坏性变化
- 将覆盖不兼容 schema 的正式产物

### 15.4 清理边界

清理只允许作用于当前 closure 的临时过程产物。不得清理：

- `verification/archive/master`
- `verification/archive/reports`
- `verification/evidence/owners/<OwnerPack>/codegen-stubs`

---

## 16. 迁移策略

### 16.1 迁移目标

迁移的目标不是与旧验证管线长期双写，而是逐步完成 authority 切换，最终废弃旧的 run-oriented 正式管线。

### 16.2 建议迁移阶段

建议按以下五步迁移：

1. `Authority Freeze`
   - 固定 closure kind、evidence kind、主线 lane、merge key、path key。
2. `Graph Introduction`
   - 引入 `Authority Graph`、`Claim Graph` 与 formal schema，但允许执行器先沿用现有实现。
3. `Fact Normalization`
   - 让 managed/aot/hotupdate 执行器统一回写 execution facts，再由规范化层生成 evidence。
4. `Projection Cutover`
   - 让 `verification/projections/testing-inventory/*` 与 `verification/projections/benchmark/*` 改为从 `verification/archive/latest` 和 `verification/archive/master` 派生。
5. `Legacy Retirement`
   - 停止旧 formal 路径写入，删除 run-id 式正式归档与重复 report 逻辑。

### 16.3 现有目录的迁移方向

现有目录建议按以下方式收敛：

| 现有目录/文件 | 新角色 |
| --- | --- |
| `docs/testing-inventory/unit-test-inventory.*` | 从 verification master 派生的 unit test 投影 |
| `docs/testing-inventory/benchmark-inventory.*` | 从 verification master 派生的 benchmark 投影 |
| `docs/testing-inventory/capability-inventory.*` | 从 verification master 派生的 capability 投影 |
| `verification/projections/benchmark/*` | 从 perf evidence 派生的 perf 展示层 |
| `subjects/*/baselines/codegen` | 迁移期只读参考；正式存根迁移到 `verification/evidence/owners/*/codegen-stubs` |

### 16.4 Cutover 完成判据

只有在以下条件全部满足时，才视为迁移完成：

- closure、obligation、evidence 三层对象全部稳定
- 主线 lane 的权责图审核已进入正式阶段报告
- `verification/archive/latest`、`verification/archive/master`、`verification/archive/reports` 已成为唯一 formal source
- `verification/projections/testing-inventory/*` 和 `verification/projections/benchmark/*` 已完全从 formal source 派生
- 旧 run-oriented 正式目录不再接收新写入

### 16.5 长期维护规则

迁移完成后，后续新增 capability、closure 或 route 时，必须优先回答四个问题：

1. owner subject 是谁
2. 命中哪些 mandatory lane
3. obligation claim 如何展开
4. formal 结果与 codegen 存根落在哪里

如果本轮还改动了 formal report / projection contract，还必须补答第五个问题：

5. 哪些 `verification/archive/{latest,master,reports}` 与 `verification/projections/**` 产物需要重生成

如果这些问题回答不清，任务不能直接进入执行阶段。

---

## 17. Foundation DLL Capability Verification Closure

### 17.1 Goal

foundation-dll translation verification is a formal closure system rather than a single proof or dashboard view.

Any DLL that enters translation-verification scope must have:

- complete DLL / family / method capability authority
- complete functional / performance / hotupdate obligations
- archivable, reviewable, solution-executable generated result
- stable formal aggregates consumable by projections

### 17.2 Four-Layer Model

#### Truth Layer

Formal objects:

- `DllCapabilityManifest`
- `CapabilityFamilyVerificationContract`
- `MethodCapabilityContract`

Truth Layer defines authority only. It does not execute, aggregate, or infer truth from runtime results.

#### Evidence Layer

Formal objects:

- `ManagedFactRecord`
- `NativeFactRecord`
- `ManagedBenchmarkRecord`
- `NativeBenchmarkRecord`
- `HotUpdateFactRecord`
- `HotUpdateBenchmarkRecord`
- `ReviewBundle`
- `MethodValidationRecord`
- `MethodCaseIndex`

#### Execution Layer

Formal objects:

- `FoundationDllTranslationSolution.sln`
- generated family `test/native/benchmark/host/patch` projects
- execution contract

#### Projection Layer

Formal objects:

- `DllAggregateRecord`
- `FamilyAggregateRecord`
- `MethodAggregateRecord`
- dashboard / detail / method projections

Projection Layer may only consume Truth + Evidence + Execution formal objects. It may not invent new truth.

### 17.3 Onboarding Freeze

Once a DLL enters translation-verification scope, all four layers must be generated together.

Minimum inputs:

- `assemblyName`
- `ownerSubjectId`
- `capabilityFamilies[]`
- `methodUniverse[]`
- `requiredRoutes`
- `benchmark obligations`
- `hotupdate obligations`
- `review targets`
- `solution entries`

### 17.4 Directory Contract

Per family, fixed directories are:

- `verification/foundation-dll/<assembly>/<family>/test/`
- `verification/foundation-dll/<assembly>/<family>/native/`
- `verification/foundation-dll/<assembly>/<family>/benchmark/`
- `verification/foundation-dll/<assembly>/<family>/host/`
- `verification/foundation-dll/<assembly>/<family>/patch/`
- `verification/foundation-dll/<assembly>/<family>/review/`

Formal indexes:

- `verification/foundation-dll/<assembly>/<family>/method-test-case-index.json`
- `verification/foundation-dll/<assembly>/<family>/method-benchmark-case-index.json`
- `verification/foundation-dll/<assembly>/<family>/method-hotupdate-case-index.json`

### 17.5 A2 Source-Annotation-First

foundation-dll case mapping truth uses generated source / metadata annotations.

The annotation scanner authority boundary is fixed:

- it may only read generated source / metadata
- it may only produce formal case index
- it may not infer truth from runtime results

Annotations should prefer enum-backed attributes. String is reserved for:

- `methodSubjectId`
- a small number of non-enumerable path-like fields

### 17.6 completed-before Gate

foundation-dll translation verification uses `G2 reviewable-bundle` as the completed-before gate.

Each in-scope family must satisfy:

- generated source snapshot archived
- case index generated
- latest execution results archived
- `ReviewBundle` generated
- solution executable

### 17.7 Execution Entry

The formal main entry is fixed to:

- `FoundationDllTranslationSolution.sln`

The system must also support local accelerated execution:

- by DLL
- by family
- by family `test/benchmark/host/patch`

These local entries are development accelerators only. They are not new formal main entries and cannot replace the main `.sln` in formal aggregation.

### 17.8 Display Contract

Navigation is fixed as:

- level-1 dashboard: DLL
- level-2 detail: family
- level-3 detail: method
- tooltip: method test details
