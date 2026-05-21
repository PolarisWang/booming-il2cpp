# Phase 0 Full Rebirth Authority Freeze Design v1.01

Date: 2026-04-17
Status: in-progress

## 1. 目标

本阶段不做“局部修修补补”，而是冻结后续全量重生的权威边界，明确:

- 什么属于 capability coverage truth source
- 什么属于 subject/source/pipeline truth source
- 什么属于 discovered test declaration truth source
- canonical subject 内部到底保留哪些目录和场景
- Phase 1 必须先改哪些 contract / generator / business pipeline

## 2. 当前仓库的权威冲突

### 2.1 `subject.manifest.json` 过载

以 [subjects/SolutionCorePack/subject.manifest.json](/D:/agent/booming-il2cpp/subjects/SolutionCorePack/subject.manifest.json) 为代表，当前 manifest 同时承载了:

- subject 身份与 source 路径
- `source.entry` / `source.entrySelection`
- `workloadEntry`
- `validationProfiles` / `validation`
- `executionPipelines` / `environmentMatrices`
- 特定 archetype 与 proof/benchmark 入口选择

这意味着 manifest 同时是:

- subject 配置
- test selection 协议
- benchmark summary 协议
- planner / registry / dashboard 的输入

这与目标架构冲突。

### 2.2 collection 契约分裂

[ChaosTestCollection.cs](/D:/agent/booming-il2cpp/src/reference/Chaos.TestFramework.Runtime/Collection/ChaosTestCollection.cs) 目前只声明:

- `DeclaredUnitTests`
- `DeclaredBenchmarks`

但 Python 工作流在 workspace collection JSON 中还额外使用:

- `subjectId`
- `frameworkReferenced`
- `subjectKind`
- `warningCodes`

运行时 loader 与 Python 管线并没有共享同一份版本化 schema，只是靠 JSON 容错“凑巧兼容”。

### 2.3 capability coverage 仍是硬编码

[capability_coverage.py](/D:/agent/booming-il2cpp/build/toolchains/run/testing/capability_coverage.py) 仍通过:

- `_OWNER_SUBJECT_BY_FAMILY`
- `_CAPABILITY_FAMILY_BY_ITEM`
- `_SUPPORT_STATES_BY_ITEM`
- `_BENCHMARK_REQUIRED_ITEMS`

硬编码维护 capability obligation。  
这会导致 registry、dashboard、benchmark completeness 与真实 subject 漂移。

### 2.4 tests 仍直接绑定历史布局

当前大量测试直接引用:

- `Archetypes`
- `FeatureSlices`
- 真实 subject 目录路径
- 真实 `subject.manifest.json` 字段

这意味着重构 subject 结构时，tests 不是验证 contract，而是在锁死旧实现细节。

## 3. 冻结后的单轨真源链

本 roadmap 的唯一正式链路冻结为:

`managed source with Chaos.TestFramework.Sdk attributes -> dotnet 8 collector -> collection file -> generated managed/native/hotupdate hosts -> report/coverage`

其中:

- managed source 是唯一声明源
- collection 是唯一跨执行形态的执行 contract
- report/coverage 必须从 collection + features 权威文件出发，而不是从历史 manifest 推导

## 4. 三类权威文件的职责冻结

## 4.1 `subject.features.json`

它是 capability obligation 的权威文件，只负责:

- subject 对哪些 capability item 负责
- 每条 capability 的 required modes
- benchmark / hotupdate 要求
- capability 关联到哪些 engineering scenario
- feature 的完成状态

它不负责:

- 具体 proof/benchmark 方法入口
- 运行时 host 选择
- C# 项目路径
- build pipeline 阶段细节

### 冻结格式

Phase 0 先冻结最小骨架:

- `schemaVersion: int`
- `subjectId: string`
- `engineeringScenarios: [{ code:int, id:string, sourcePath:string }]`
- `features: []`

后续在 Phase 1/4 扩展 `features[]` 的完整条目。

## 4.2 `subject.manifest.json`

它是 subject/source/pipeline 配置文件，只负责:

- `subjectId`
- `displayName`
- source roots
- default pipeline / matrix
- toolchain / platform defaults

它在最终状态中不再负责:

- proof 入口选择
- benchmark 工作负载选择
- validation profile truth source
- capability ownership

Phase 0 保留旧字段只是为了不打断现有主线，不代表它们仍是长期设计。

## 4.3 collection

collection 是 discovered concrete declarations 的执行 contract，只负责:

- discovered unit test entries
- discovered benchmark entries
- 这些 entry 的 typed metadata
- host 运行所需的最小必要信息

collection 最终必须:

- versioned
- enum-first
- 同时被 managed/native/hotupdate host 消费
- 被 dashboard/report 直接消费

## 5. canonical subject 与最小 engineering scenarios 冻结

## 5.1 canonical subjects

保持 3 个 owner subject:

- `SolutionCorePack`
- `MixedExecutionFeaturePack`
- `HotUpdateHostPack`

不再增加新的顶层 owner。

## 5.2 最小 scenario 集合

`SolutionCorePack`

- `solution-multi-project`
- `solution-reference-assembly`
- `solution-corelib-reference`
- `solution-mixed-reference-closure`

`MixedExecutionFeaturePack`

- `mixed-bridge-solution`

`HotUpdateHostPack`

- `full-project-hotupdate-solution`

明确移除目标:

- `SimpleLibrarySolution`
- `PackageReferenceSolution`

## 6. subject 目录命名冻结

最终命名模型冻结为:

- `Proofs/`
- `Benchmarks/`
- `EngineeringScenarios/`
- `Host/`
- `Patch/`
- `SharedContracts/`

需要被淘汰的历史命名:

- `Archetypes`
- `FeatureSlices`
- `Launcher`
- `validation/*`

另外，subject 内的 `bin/obj/artifacts` 不是正式资产，后续应从仓库结构治理中清理。

## 7. Phase 1 的明确输入

Phase 1 必须从以下 4 个入口开始:

1. collection schema 重建  
   目标是把 Python 与 Runtime 看到的 collection 统一成同一个 versioned contract。

2. features file 消费接入  
   目标是让 registry / dashboard / coverage 不再依赖 `capability_coverage.py` 硬编码。

3. generator contract 重建  
   目标是让 generated managed/native/hotupdate hosts 统一消费 collection，并把 file-level 生成从内联字符串转成模板化资产。

4. collector 归位  
   目标是把当前 `Chaos.IL2CPP.DeclarationDiscovery` 的测试声明分析能力，收敛为明确的 test-framework collector 责任边界，避免继续与 IL2CPP 核心能力混杂。

## 8. 本阶段验收标准

Phase 0 结束必须能回答下面 5 个问题:

- capability ownership 的权威来源是什么
- subject/source/pipeline 的权威来源是什么
- discovered proof/benchmark entry 的权威来源是什么
- canonical subject 最小 scenario 集合是什么
- Phase 1 的第一批重建入口是什么

此外，仓库中必须至少已经存在:

- 3 个 canonical subject 的 `subject.features.json`
- contract test 锁住这些权威种子文件
