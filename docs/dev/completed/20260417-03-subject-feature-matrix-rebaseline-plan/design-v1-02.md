# Subject/Test Framework Full Rebirth Design v1.02

Date: 2026-04-17
Status: proposed

## 1. 目标

按用户最新决策，本轮不再走“在现有 subject 体系上持续修补”的路线，而是直接采用方案 C：

`整个 subject 体系全量重生`

并把范围升级到：

- 整个测试框架大架构
- 为完成这套大架构所必须调整的非测试主线业务功能

最终目标是把仓库重构成一套单轨、清晰、可自动校验、可长期扩展的验证体系：

- 不允许“新旧双轨并存”的长期兼容状态
- 不允许 `Program::Main + Console.WriteLine` 继续作为正式 correctness 判定方式
- 不允许 `subject.manifest.json` 与 collection / feature matrix 同时承担测试真源
- 不允许新增 feature 仍然依赖手工 proof/benchmark 聚合入口

## 2. 新方向带来的根本变化

相比上一版设计，这一版有 4 个根本变化：

### 2.1 范围从“subject 测试内容整理”升级为“测试框架整体重生”

这轮不只是重写 proof / benchmark / hotupdate proof，也包括：

- subject model
- capability coverage model
- collection schema
- generated hosts
- planner / registry / workspace
- runner / report / dashboard
- 需要配合这套主线收口的业务管线

### 2.2 不接受长期双轨

开发过程中可以在分支内短暂存在“新结构构建中的过渡形态”，但合入目标必须是：

- 仓库里只保留新 subject 体系
- 仓库里只保留新测试真源
- 仓库里不保留长期 compatibility alias
- 仓库里不保留旧 `validation/*` 旁路

### 2.3 不接受 console 作为 correctness 真源

`Program::Main()` 可以作为某些业务宿主或装配入口继续存在，但它不能再承担：

- proof 结果判定
- benchmark 是否有效的判定
- hotupdate 行为正确性的判定

正式 correctness 只能来自：

- `Chaos.TestFramework.Sdk` attribute
- `Assert`
- collection 驱动的 managed/native/hotupdate host

### 2.4 subject 体系不是“迁移”，而是“重生”

不是在老 subject 上持续堆修补规则，而是：

- 先定义新 subject 模型
- 再定义新测试框架契约
- 然后用新模型一次性重建 canonical subject
- 最后删除旧体系

## 3. 设计原则

### 3.1 单轨真源

最终只允许存在一条正式真源链：

`managed source with Sdk attributes -> collector -> collection -> generated managed/native/hotupdate hosts -> report/coverage`

### 3.2 权责拆分

最终权责应固定为：

- `subject.features.json`
  - owner feature obligations
  - required coverage
  - benchmark / hotupdate requirements
- `subject.manifest.json`
  - subject 身份
  - source solution / project 边界
  - engineering scenario 集合
  - pipeline / matrix / platform 默认信息
- collection file
  - 由 collector 从 managed source discovery 出来的具体 proof / benchmark / hotupdate 声明

三者不能继续混职。

### 3.3 subject 内部只保留新结构

subject 目录里最终只保留：

- owner managed source
- owner proof / benchmark source
- 必要 engineering scenario
- patch / host / shared contract 等业务样例
- 与新框架一致的 subject contracts

不保留：

- 手工 proof 聚合器
- 手工 benchmark 聚合器
- 旧 `validation/*`
- 旧 console correctness 旁路

### 3.4 工程级验证也要纳入新框架

工程级 scenario 不是“额外附带 demo”，而是正式验证资产的一部分。  
因此它们也要尽量通过：

- `Chaos.TestFramework.Sdk`
- `Assert`
- collection

来接入。

如果少数业务宿主场景确实暂时只能通过 `Program::Main()` 存在，也必须在 feature matrix 中标成临时 engineering smoke，而不能伪装成正式 correctness。

### 3.5 必要时允许调整非测试主线业务功能

如果新测试框架要落地，发现以下业务面必须调整，也纳入本 roadmap：

- subject discovery / planner / registry / workspace
- generated host / runner / driver
- reporting / benchmark dashboard
- build/run orchestration
- 与 patch/host/native generation 直接相关的业务装配点

本轮不主动扩展 AOT/HotUpdate 新能力，但会为测试框架架构调整必要的业务入口。

## 4. 目标架构

## 4.1 测试框架总结构

推荐目标结构：

```text
src/reference/
  Chaos.TestFramework.Sdk/
  Chaos.TestFramework.Runtime/

src/tools/
  Chaos.TestFramework.CollectionGen/

subjects/
  SolutionCorePack/
  MixedExecutionFeaturePack/
  HotUpdateHostPack/

tests/
  unit/
  contracts/
  integration/
  templates/
```

### 4.2 Subject 新结构

推荐结构：

```text
subjects/
  <SubjectId>/
    subject.manifest.json
    subject.features.json
    source/
      Proofs/
      Benchmarks/
      EngineeringScenarios/
      Host/
      Patch/
      SharedContracts/
```

注意：

- 不是每个 subject 都必须有全部目录
- 但命名模型应统一
- `Archetypes/` 建议统一改为 `EngineeringScenarios/`
- `FeatureSlices/` 这类历史命名如果继续存在，会降低整体清晰度；更推荐直接收敛成 `Proofs/`

### 4.3 Capability Model

每个 owner subject 必须有 machine-readable 的 `subject.features.json`。  
它不是测试声明文件，而是 coverage obligation 文件。

每行至少包含：

- `featureId`
- `ownerSubjectId`
- `capabilityFamily`
- `capabilityItem`
- `requiredModes`
  - `managed`
  - `native`
  - `hotupdate`
- `hotupdateLevel`
  - `none`
  - `smoke`
  - `proof`
- `benchmarkRequirement`
  - `not-required`
  - `required`
- `engineeringScenarios`
- `status`
  - `completed`
  - `partial`
  - `missing`

### 4.4 Manifest Model

`subject.manifest.json` 收敛为 subject 级配置，而不是测试项真源。

它最终只负责：

- `subjectId`
- `displayName`
- `source roots`
- `default pipeline / matrix`
- `engineering scenario groups`
- `platform / toolchain defaults`

它不再负责：

- 具体 proof 入口
- 具体 benchmark 入口
- 手工 `entrySelection`
- 手工 `workloadEntry`

### 4.5 Collection Model

collection file 继续是正式测试声明的唯一执行契约。

来源固定为：

- collector 读取 managed source 中的 `Sdk` attribute

消费方固定为：

- managed host
- native host
- hotupdate host
- coverage/report pipeline

### 4.6 Reporting/Coverage Model

dashboard / reporting 不能只展示执行结果，还必须能对照 capability obligations。

理想结果是可以直接回答：

- 哪些 completed feature 已被正式覆盖
- 哪些 completed feature 缺 native
- 哪些 feature 缺 hotupdate proof
- 哪些 benchmark 是 required-but-missing
- 哪些 engineering scenario 未被纳入正式 collection

## 5. Subject 全量重生策略

### 5.1 顶层 owner 仍收敛为三个 canonical subject

本轮虽然是 subject 体系全量重生，但不建议再扩顶层 owner 数量。

保留：

- `SolutionCorePack`
- `MixedExecutionFeaturePack`
- `HotUpdateHostPack`

原因：

- owner 边界已经基本正确
- 真正脏的是内部结构和测试真源，不是顶层 subject 数量

### 5.2 工程级 scenario 压缩为最小必要集合

全量重生后建议保留：

- `SolutionCorePack`
  - `MultiProjectSolution`
  - `ReferenceAssemblySolution`
  - `CoreLibReferenceSolution`
  - `MixedReferenceClosureSolution`
- `MixedExecutionFeaturePack`
  - `MixedBridgeSolution`
- `HotUpdateHostPack`
  - `FullProjectHotUpdateSolution`

删除：

- `SimpleLibrarySolution`
- `PackageReferenceSolution`

### 5.3 Proof/Benchmark/Scenario 全部重新归档

不是简单挪文件，而是按新结构重新归档：

- owner proof 进 `Proofs/`
- owner benchmark 进 `Benchmarks/`
- 工程级 solution 样例进 `EngineeringScenarios/`
- patch/host/shared contract 等业务装配样例按职责分层

### 5.4 旧结构不做长期兼容

以下内容在 cutover 后必须删除：

- `Archetypes/`
- `FeatureSlices/` 中不再符合新结构的历史层级
- 手工 `Program` proof 聚合
- `DefaultProofEntries`
- 旧 `validation/*`
- compatibility alias

## 6. 这版方案比上一版更合理的地方

### 6.1 不再只是“补 coverage”，而是先重建架构

上一版的缺点是仍偏向在旧 subject 结构上逐条补 feature。  
这版先重建：

- subject model
- truth-source boundary
- business consumption model

再做 feature 重接，更符合“全量重生”。

### 6.2 把业务消费管线正式纳入范围

如果 planner / registry / workspace / report / dashboard 不一起改，新的 subject 体系就落不了地。  
上一版把这部分视为附属；这版把它们正式纳入架构范围。

### 6.3 更接近“完美形态”

我认为如果目标是“最清晰结构”和“最满足现在测试框架”，理想终态应当是：

- subject 结构极简
- truth source 唯一
- coverage 自动可审计
- 工程 scenario 与 feature proof 同框架接入
- 没有手工聚合入口
- 没有 console correctness
- 没有长期 alias

## 7. 风险

### 7.1 这是一次真正的 cutover，而不是小修

会同时牵动：

- subject 目录结构
- manifest / collection / feature matrix
- planner / registry / workspace
- generated hosts
- tests
- reporting / dashboard

### 7.2 如果不把 feature obligations 机器化，会很快再次漂移

只写 wiki 或 design，不落成 `subject.features.json`，很快又会回到人工同步失败。

### 7.3 如果保留 `Program::Main()` correctness，重生就不彻底

只要正式 correctness 仍依赖 console，整个框架就仍然停在旧范式里。

## 8. 推荐结论

采用方案 C，并按下面的思路执行：

- 先冻结新架构的单轨真源边界
- 再重建测试框架 contract 与业务消费管线
- 再全量重生 subject 体系
- 最后把已完成 feature 全部重接到新体系并跑通完整回归
