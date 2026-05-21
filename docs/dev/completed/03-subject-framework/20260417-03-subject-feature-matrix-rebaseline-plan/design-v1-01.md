# Subject Feature Matrix Rebaseline Design v1.01

Date: 2026-04-17
Status: proposed

## 1. 目标

把当前 `subjects/` 中已经完成的 AOT / Mixed Execution / HotUpdate 能力，按照现行统一测试规范重新收敛成一套可持续扩展的验证体系：

- feature 有清晰的 owner subject
- feature 有统一的 capability matrix 身份
- 用例只通过 `Chaos.TestFramework.Sdk` 声明
- collector 统一产出 collection file
- managed/native/hotupdate 统一消费 collection file
- 删除手工 proof/benchmark 聚合入口与旧验证残留
- 最终可以对“已完成能力”做一次完整、可重复的全链路回归

## 2. 范围边界

### 2.1 In Scope

- 重新定义 subject 内测试内容的归类方式
- 盘点并冻结已经完成的 feature inventory
- 把已完成 feature 逐条映射到 owner subject + capability matrix
- 按当前规范补齐或重写：
  - managed proof
  - native proof
  - hotupdate proof
  - benchmark
- 删除旧入口和旧验证路径
- 建立最终全量回归入口与 coverage 报表

### 2.2 Out of Scope

- 本轮不扩展新的 AOT Core IR 能力本身
- 本轮不扩展新的 HotUpdate 语义能力本身
- 本轮不重做 subject 数量层面的再拆分
- 本轮不追求一次性重写全部 architecture，只聚焦测试主线与 subject 内容收敛

## 3. 固定原则

### 3.1 owner subject 固定为三个 canonical pack

- `SolutionCorePack`
  - AOT 主线默认 owner
  - solution/project/reference/corelib/reference-closure 工程级样例 owner
- `MixedExecutionFeaturePack`
  - mixed execution / bridge / interpreter 协同 owner
- `HotUpdateHostPack`
  - patch/host/binding/metadata/hotupdate workflow owner

结论：

- 这轮不再新建更多 subject
- 重点是把 feature 放回正确 owner，并去掉重复或漂移的验证资产

### 3.2 feature = capability matrix 的一行

每个已完成能力都必须进入统一矩阵，最少包含：

- `stableId`
- `ownerSubjectId`
- `capabilityFamily`
- `capabilityItem`
- `executionModes`
  - `managed`
  - `native`
  - `hotupdate: none|smoke|proof`
- `benchmarkMode`
  - `none|managed|native|interpreter|mixed`
- `archetypeCoverage`
- `status`
  - `completed`
  - `partial`
  - `missing`

能力矩阵是后续：

- 补用例
- 跑 collector
- 生成回归清单
- 产出 coverage gap

的唯一真源。

### 3.3 正式验证资产只有四类

每个 feature 只允许通过以下正式资产体现：

- `tests/unit` / `tests/contracts`
- owner subject managed proof
- owner subject native proof
- 按触发规则补的 hotupdate proof
- 按需 benchmark

不再接受：

- 手工 `Program.cs` 聚合 proof
- 手工 benchmark 入口列表
- 旧 `validation/*` 旁路验证
- 依赖输出文本的外置判定

### 3.4 subject 只保留 authoring source

subject 目录下正式保留的，是：

- managed solution source
- `Sdk` attribute 声明的 proof / benchmark
- 少量工程 archetype

不再保留：

- 旧 proof/native-reference 验证工程
- 与 collection 主线重复的旧 runner
- 手工维护的 proof/benchmark 聚合表

## 4. 当前问题归纳

### 4.1 入口仍有手工聚合

当前已确认：

- `SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/Program.cs`
- `MixedExecutionFeaturePack/source/ManagedBridge/Program.cs`
- `HotUpdateHostPack/source/Host/Program.cs`

仍然承担“proof 入口聚合器”的角色。

这与当前 “attribute -> collector -> collection -> generated hosts” 模式冲突。

### 4.2 feature 已存在，但不等于 feature 已完成规范化接入

虽然现在已有大量 proof / benchmark 文件，但这不等于：

- capability matrix 已冻结
- collector / registry / workspace 已统一消费
- managed/native/hotupdate 已全部正式重验

所以本轮必须把“已有文件”与“已完成规范化接入”分开看待。

### 4.3 旧验证目录仍污染 subject

`subjects/SolutionCorePack/validation/proof/native-reference/` 说明旧的验证旁路还在仓库里。  
这会让后续新增 feature 时继续走双轨。

## 5. 目标结构

### 5.1 Subject 内容组织

#### `SolutionCorePack`

保留并明确三类内容：

- `source/Archetypes/*`
  - 工程级 archetype 验证
  - 例如 multi-project/reference/package/corelib/mixed-reference
- `source/FeatureSlices/CoreRuntimeFeatures/*`
  - AOT correctness canonical proof
- `source/Benchmarks/CoreRuntimeBenchmarks/*`
  - AOT performance canonical benchmark

建议补充一个显式的 matrix 清单文件，用于把“哪些 proof/benchmark 属于哪些 capability”沉淀下来。

#### `MixedExecutionFeaturePack`

保留：

- `source/Archetypes/*`
- `source/ManagedBridge/Proofs/*`
- `source/ManagedBridge/Benchmarks/*`
- `source/Lowering/*`

重点不是再扩目录，而是把 mixed execution feature 明确映射到 capability matrix。

#### `HotUpdateHostPack`

保留：

- `source/Archetypes/*`
- `source/Host/Proofs/*`
- `source/Host/Benchmarks/*`
- `source/PatchModules/*`
- `source/SharedContracts/*`

重点是把 hotupdate capability 逐条转成 collection 驱动的 proof / smoke / benchmark。

### 5.2 工程级 archetype 的定位

工程级样例需要保留，但应重新定义角色：

- 它们是工程装配与 solution/reference/patch-host 场景的正式工程级验证样例
- 它们不再承担“控制台输出再外部判定”的旧式 smoke 角色
- 只要进入正式测试主线，就应尽量改为 `Chaos.TestFramework.Sdk + Assert + collection` 驱动

结论：

- archetype 的“工程拓扑价值”保留
- archetype 的“旧 Program + Console 验证方式”不应继续保留为目标形态
- 这轮重构不应原样保留现有全部 archetype，而应压缩成最小必要集合

### 5.3 archetype 最小必要集合

当前 `SolutionCorePack` 的 6 个 archetype 中，至少有一部分只是“同类场景的层级变体”，不需要全部长期保留。

建议压缩为以下最小集合：

- `MultiProjectSolution`
  - 保留
  - 代表最基本、最通用的 `solution + project-to-project reference` 工程装配能力
  - 可以覆盖 `SimpleLibrarySolution` 的主体价值
- `ReferenceAssemblySolution`
  - 保留
  - 代表显式 `Reference`/外部 DLL bundle 装配能力
- `CoreLibReferenceSolution`
  - 保留
  - 代表显式 corelib/reference bundle 装配能力
  - 这是 AOT 主线里相对独立、不能被普通 `ProjectReference` 替代的场景
- `MixedReferenceClosureSolution`
  - 保留
  - 代表 `ProjectReference + external Reference + PackageReference` 的混合闭包
  - 它是最接近真实工程闭包复杂度的一个样例

建议合并/删除：

- `SimpleLibrarySolution`
  - 删除
  - 与 `MultiProjectSolution` 高度重叠，只是更简单的两项目变体
- `PackageReferenceSolution`
  - 删除
  - 其“纯包引用”价值已被 `MixedReferenceClosureSolution` 覆盖

对于另外两个 subject：

- `MixedExecutionFeaturePack`
  - `MixedBridgeSolution` 保留
  - 当前只有这一个 archetype，且其 bridge/interpreter 协同价值独立
- `HotUpdateHostPack`
  - `FullProjectHotUpdateSolution` 保留
  - 当前只有这一个 archetype，且其 host/patch/shared-contract 拓扑不可替代

因此，这轮目标 archetype 集合建议收敛为：

- `SolutionCorePack`
  - `MultiProjectSolution`
  - `ReferenceAssemblySolution`
  - `CoreLibReferenceSolution`
  - `MixedReferenceClosureSolution`
- `MixedExecutionFeaturePack`
  - `MixedBridgeSolution`
- `HotUpdateHostPack`
  - `FullProjectHotUpdateSolution`

总计从当前 8 个 archetype 压缩到 6 个。

### 5.4 更贴合现测试框架的最终形态

如果目标是“最清晰结构 + 最贴合当前测试框架”，仅做 capability 归类还不够，还需要再补三条收口原则：

#### 5.4.1 capability matrix 不能只是文档，必须是可校验契约

理想形态不是写一份说明文档然后人工对照，而是：

- 每个 subject 维护一份 machine-readable 的 owner feature matrix
- collection discovery 结果可以自动对照这份 matrix
- CI 或回归可以直接报告：
  - declared but not covered
  - covered but not registered
  - required benchmark missing
  - required hotupdate proof missing

否则 matrix 很快会再次漂移。

#### 5.4.2 subject manifest 不应继续承担 proof/benchmark 具体入口真源

当前 `subject.manifest.json` 中仍有较多 `entry`、`workloadEntry`、`entrySelection` 这类手工入口信息。  
这在“attribute -> collector -> collection” 新框架下已经不是理想形态。

理想形态应收敛为：

- manifest 只负责：
  - subject 基本身份
  - source solution/project 边界
  - pipeline / matrix / platform 信息
  - engineering scenario 或默认执行集合选择
- 具体 proof / benchmark 项由 collection discovery 成为唯一真源

这一步是本设计在重审后新增的关键修正。

#### 5.4.3 `Archetypes` 这个命名本身不够直观

如果追求最清晰结构，`Archetypes` 更适合改成更直观的名字，例如：

- `EngineeringScenarios`
- `SolutionScenarios`

我更推荐 `EngineeringScenarios`，因为它强调“工程装配级验证样例”，而不是抽象模板概念。

这不是功能性 blocker，但从长期可维护性看，属于值得在本轮一起收口的命名优化。

## 6. 推荐执行阶段

### Phase 0: Inventory Freeze

目标：

- 冻结一份 completed feature inventory
- 建立 capability matrix 初版
- 标出每个 feature 的 owner、执行模式、benchmark 需求、hotupdate 需求
- 冻结一份 archetype inventory，并明确“保留/删除/合并”决策
- 冻结 capability matrix / collection / manifest 三者的真源边界

产物：

- feature inventory 文档
- capability matrix 数据文件
- archetype inventory 与压缩决策表
- legacy 残留清单

验收：

- 所有当前已完成 feature 都能落到唯一 matrix 行
- 不再存在“这个 feature 到底算哪个 subject”的模糊地带

### Phase 1: Subject Authoring Cutover

目标：

- 把 subject 作者侧彻底对齐到 `Sdk` 声明模式
- 删除手工 proof/benchmark 入口聚合
- 删除已判定不需要保留的 archetype，并修正 manifest / planner / registry / tests
- 尽量把保留下来的 engineering scenario 也切到 `Sdk + Assert + collection`
- 收缩 `subject.manifest.json` 中对具体 proof / benchmark 入口的手工声明

产物：

- 精简后的 `Program.cs`
- 删除的 `DefaultProofEntries` / 手工 `switch`
- 对齐后的 proof / benchmark 声明方式
- 精简后的 archetype 集合

验收：

- collector 能从 attribute 发现全部正式测试项
- 新增 feature 不再需要修改手工入口

### Phase 2: SolutionCorePack AOT Rebaseline

目标：

- 按 capability matrix 重写或补齐所有已完成 AOT feature 的 canonical proof / benchmark
- 覆盖 solution/project/reference/corelib archetype 验证

优先能力域：

- `PrimitivesAndOps`
- `ObjectModelAndDispatch`
- `GenericsAndCollections`
- `ExceptionsAndControlFlow`
- `InteropAndMarshaling`
- `ReflectionAndMetadata`
- `RuntimeServices`
- `AsyncAndThreading`

验收：

- 每条 completed AOT feature 都有：
  - unit/contracts 证据
  - managed proof
  - native proof
  - 按需 benchmark

### Phase 3: MixedExecution / HotUpdate Rebaseline

目标：

- 把 mixed execution 和 hotupdate 已完成能力逐条回填到正式 collection 主线

MixedExecution 重点：

- interpreter lowering
- bridge dispatch
- mixed generic flow
- mixed exception flow
- mixed delegate flow

HotUpdate 重点：

- skeleton/load
- metadata supplement
- method replacement
- shared contract
- patch integrity
- rollback/versioning

验收：

- 每条 completed mixed/hotupdate feature 都有明确 owner proof
- 命中 hotupdate 触发规则的能力全部具备 hotupdate smoke 或 proof

### Phase 4: Full Regression And Coverage Report

目标：

- 跑一轮按 capability matrix 驱动的完整回归
- 产出“已完成能力全绿”和“剩余 gap”报表

产物：

- managed/native/hotupdate 回归报告
- benchmark 覆盖报告
- remaining gaps 清单

验收：

- 能明确回答：
  - 哪些 completed feature 已通过 managed/native/hotupdate 验收
  - 哪些 feature 缺 benchmark，且是“按设计不需要”还是“尚未补齐”
  - 哪些 feature 仍因 AOT/HotUpdate 实现缺口而阻塞

## 7. 风险与判断

### 7.1 最大风险不是补用例，而是“继续保留双轨”

如果这轮只补 proof 文件、不删旧入口，那么之后新增 feature 仍会继续改：

- attribute 声明
- 手工 proof 聚合
- 旁路验证脚本

这会让规范再次失效。

### 7.2 archetype console 输出不能再承担 correctness 真源

工程级样例可以继续保留，但要退回 smoke / 装配验证角色。  
正式 correctness 只能由 `Assert` 与 collection 驱动的 host 承担。

重审修正：

- 更理想的终态不是“保留 console smoke”，而是“尽量把工程级样例也接入统一 collection 主线”
- 如果某些样例暂时只能以 `Program::Main()` 存在，也应在 matrix 中明确标记为临时工程 smoke，而不是正式 proof

### 7.3 archetype 压缩会牵连较多测试与 manifest

当前大量单测/兼容性测试直接引用 archetype 名称与路径。  
因此 archetype 压缩不是简单删目录，而是一次显式 cutover：

- subject manifest
- planner / registry / workspace tests
- 兼容性测试中的固定路径断言

都要同步更新。

### 7.4 benchmark 需要“有理由地缺失”

不是每个 completed feature 都必须补 benchmark。  
但每个缺 benchmark 的 feature 都要在 matrix 中明确标记：

- `not-required`
- `required-but-missing`
- `required-and-covered`

### 7.5 这轮最终必须清理旧目录

切换完成后，旧 `validation/*`、旧聚合入口、旧手工注册逻辑都必须删除。  
否则仓库不会真的变干净。

## 8. 推荐结论

推荐直接进入 roadmap，并采用以下执行顺序：

1. `Inventory Freeze`
2. `Subject Authoring Cutover`
3. `SolutionCorePack AOT Rebaseline`
4. `MixedExecution / HotUpdate Rebaseline`
5. `Full Regression And Coverage Report`

这个顺序的原因是：

- 先冻结 inventory，避免边补边漂移
- 先切掉手工入口，避免后面 feature 继续写到旧轨道上
- 先打通 `SolutionCorePack`，再推进 mixed/hotupdate
- 最后再跑全量回归，才能看到真实 coverage
