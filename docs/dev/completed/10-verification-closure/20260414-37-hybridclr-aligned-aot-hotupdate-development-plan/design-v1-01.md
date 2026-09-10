# HybridCLR 对齐的 AOT 与热更开发计划 Design v1.01

Date: 2026-04-14 23:34:39 +08:00
Status: draft-approved

## 1. 设计目标

建立一条长期可扩展的 IL2CPP + 热更主线，使仓库同时满足以下目标：

- `typed-il` 能逐步承接更完整的 C# feature 翻译能力。
- 全部目标 AOT translation surface 都能被显式追踪，包含 `supported`、`partial`、`unsupported` 与明确 deferred 的边界。
- 热更路线与 HybridCLR 的核心思路对齐，而不是额外旁挂。
- `subjects/` 继续保持 solution 模式，并成为 capability proof、benchmark、full-solution gate 的统一样例源。
- 新增测试项时，只需要新增或扩展 owner subject 内的 capability slice，而不是再发明一套新目录协议或手工注册逻辑。

## 2. 非目标

- 不把当前任务定义成“一次性补齐所有 C# feature”。
- 不继续扩大 `subject` 内部字符串协议面。
- 不回退到大量 `validation/`、`scenario/`、`workload/` 外部配置目录来描述 proof/benchmark。
- 不为了短期 native coverage，牺牲后续 hybrid execution / hot update 的统一契约。

## 3. 核心设计原则

### 3.1 `typed-il` 是共享契约，不只是 AOT 前端中间产物

`typed-il` 后续必须同时服务：

- AOT lowering
- interpreter-ready lowering
- bridge/thunk generation
- supplemental metadata production
- capability evidence 与 diagnostics 定位

这意味着后续不能再允许 AOT、热更、benchmark 各自维护一套相互脱节的方法 identity 和 body 归属模型。

### 3.2 执行模型采用统一的 body availability 分类

每个方法或入口在系统内都应该能被归入以下一种状态：

- `AotNative`
- `HybridInterpreted`
- `BridgeThunk`
- `MetadataOnly`
- `Unsupported`

这组状态需要以枚举形式公开，不使用字符串常量。

### 3.3 热更主线对齐 HybridCLR

运行时总体结构采用三层：

- `AOT host runtime`
- `hybrid execution layer`
- `hot update assembly runtime`

其中 dispatch、补充元数据、桥接桩、泛型共享和 body fallback 都围绕这三层设计，而不是先做一套纯 AOT，再在末端补热更例外。

### 3.4 subject 继续收敛为 solution owner pack

顶层 `subjects/` 继续只保留少量 owner pack。每个 pack 内：

- 源码保持 solution 模式。
- 历史语法点和能力点按单文件机械拆分。
- 多个单文件 slice 根据领域归并到合适 project。
- proof / benchmark 通过 C# attribute 声明，不额外引入目录协议驱动。

### 3.5 测试框架放在 `src/reference/Chaos.TestFramework`

- `src/managed` 保持 IL2CPP 阶段核心代码。
- C# 层项目引用的测试框架统一放在 `src/reference/Chaos.TestFramework`。
- 该框架提供 `ChaosUnitTest`、`ChaosBenchmark`、Assert 套件以及必要的 metadata contract。

### 3.6 公共契约优先用枚举和稳定数值标识

- 公共 API、metadata、registry、dashboard 和 benchmark identity 优先使用枚举、稳定 integer id、紧凑结构体。
- 所有公开枚举都必须带注释。
- string 仅保留展示层或诊断文本，不再继续承担调度协议。

## 4. 目标工程结构

```text
subjects/
  SolutionCorePack/
    source/
      SolutionCorePack.sln
      src/
        CoreRuntime.Features/
        CoreRuntime.Benchmarks/
        CoreRuntime.Tests/
  HotUpdateHostPack/
    source/
      HotUpdateHostPack.sln
      src/
        HotUpdateHost/
        HotUpdate.Features/
        HotUpdate.Tests/
  MixedExecutionFeaturePack/
    source/
      MixedExecutionFeaturePack.sln
      src/
        MixedExecution.Features/
        MixedExecution.Tests/

src/
  managed/
    Chaos.IL2CPP.*
  reference/
    Chaos.TestFramework/
```

说明：

- 顶层 subject 数量保持少量且稳定。
- 每个 feature 用单文件承载核心样例，方便能力映射与迁移。
- project 仍按领域聚合，避免“一个 feature 一个 project”导致 solution 爆炸。

## 5. capability taxonomy

后续 capability ledger 统一采用两级结构：

- Level 1: capability family
- Level 2: capability item

当前不继续往 Level 3 或更深层级扩展，原因是：

- 两级足够支撑 owner subject、owner archetype、proof、benchmark、support-state 映射。
- 再细化会过早把实现内部细节固化到对外 taxonomy 中。

每个 capability item 至少要绑定：

- stable capability id
- family enum
- item enum
- owner subject
- owner archetype
- required proof kind
- optional benchmark kind
- body availability expectation

### 5.1 AOT translation surface 覆盖规则

后续不能只维护 capability family/item 的抽象分层，还必须维护一份显式的 AOT 翻译面台账。原因是：

- capability taxonomy 解决的是 owner、证据、工程归属问题。
- AOT translation surface ledger 解决的是“到底哪些语言面、编译器生成面、运行时服务面被纳入目标”。

Phase 0 至少要产出两本台账：

- `aot-translation-surface-ledger`
  - 记录当前目标翻译面、owner、proof、可选 benchmark、support-state。
- `aot-unsupported-surface-ledger`
  - 记录明确不支持、暂缓支持或策略上不进入本轮范围的翻译面，并附 reason code 与下一步。

每个 translation surface 条目至少属于以下一种面向：

- language surface
  - 例如基本语法、泛型约束、模式匹配、nullable 语义。
- compiler-lowered surface
  - 例如 async state machine、iterator、lambda / closure、local function。
- runtime service surface
  - 例如 type init、exception、reflection、delegate、generic runtime semantics。
- metadata / closure surface
  - 例如 metadata lookup、linker preserve、AOT closure、generic sharing closure。
- interop / ABI surface
  - 例如 marshaling、function pointer、unsafe、ref-like、stack-only 语义。

规则：

- 不能因为某项能力暂不支持，就不把它写进台账。
- `unsupported` / `partial` 项必须按 `docs/architecture/unsupported-feature-policy.md` 携带稳定 reason code。
- 只有当 translation surface 已全部归档到 supported/partial/unsupported/not-applicable 范畴后，才允许声称“目标 AOT 翻译面已完整覆盖”。

## 6. reference bundle 模型

reference bundle 采用 repository-owned 模式：

- 由仓库统一维护固定位置、固定刷新规则、固定版本标识。
- 仅特定 archetype 消费，例如引用核心 DLL、reference assembly 或补丁式 reference 输入的方案。
- Phase 0 先冻结 bundle 边界、刷新策略、校验方式和 archetype 使用边界。

## 7. proof / benchmark 声明模型

### 7.1 proof

- proof 统一通过 `ChaosUnitTest` 标注。
- 验证结果统一通过 Assert 套件表达，不再依赖 `Console.WriteLine` 后置判定。
- 每个 proof entry 直接映射 capability item 或工程级 gate。

### 7.2 benchmark

- benchmark 统一通过 `ChaosBenchmark` 标注。
- dashboard 的基准线固定为 managed latency。
- native、mixed、hot update 结果都围绕 managed baseline 对比展示。
- benchmark 只服务高价值 runtime path，不追求为所有 feature 都补性能样例。

## 8. 运行时核心组件

后续实现建议围绕以下组件推进：

- `TypedIlContract`
  - 稳定的方法、类型、token、generic context 表达。
- `BodyAvailabilityResolver`
  - 决定某个入口是 `AotNative`、`HybridInterpreted` 还是其他状态。
- `HybridDispatchResolver`
  - 统一宿主调用到 native body、解释器 body、bridge thunk 的分发。
- `SupplementalMetadataRegistry`
  - 管理热更所需的补充元数据。
- `HotUpdateAssemblyLoader`
  - 装载热更程序集并建立 identity 对齐。
- `BridgeThunkGenerator`
  - 负责 AOT/解释器/热更之间的桥接桩。
- `AotCoreIr`
  - 作为 `typed-il -> native codegen` 的稳定中层，而不是直接从 ad-hoc loader model 发射 C++。

## 9. 验证体系

测试体系至少包含五层：

- capability proof
  - 验证单项能力是否正确。
- selective benchmark
  - 验证高价值路径的 managed vs native vs mixed 性能表现。
- engineering archetype gate
  - 验证 project/reference/package/corelib 等典型 solution 形态。
- full-solution gate
  - 验证完整 solution ingestion、translation、build、run 和报告链路。
- hot update / mixed execution gate
  - 验证宿主、补充元数据、桥接调用、fallback 和回滚能力。
- unsupported feature report
  - 验证未支持能力是否被正确记录、正确分类，并能进入 dashboard / gate / 发布文档。

后续若要覆盖移动平台，还应补以下能力：

- host-platform matrix
- package/strip/link 差异
- crash diagnostics / symbol / trace
- 发布工件一致性
- 版本回退与兼容性边界

## 10. 关键风险与缓解

### 风险 1: AOT 扩展再次退化为零散 opcode patch

- 缓解
  - 先冻结 `typed-il -> AotCoreIr` 方向。
  - 所有新能力先映射 capability item，再进入具体实现。

### 风险 2: 热更与 AOT 仍然各自维护一套 identity

- 缓解
  - Phase 0 先冻结 stable id、bundle policy、taxonomy 和 body availability。

### 风险 3: subject 虽然继续 solution 化，但内部又长出新的目录协议

- 缓解
  - 统一用 `ChaosUnitTest` / `ChaosBenchmark` 标注。
  - `validation/`、`scenario/`、`workload/` 仅允许保留为执行层概念，不再要求 subject 目录显式维护。

### 风险 4: benchmark 持续与真实能力状态脱节

- 缓解
  - dashboard 固定使用 managed baseline。
  - 缺失结果必须能明确归因到 `Unsupported` 或对应 reason code。

### 风险 5: 计划只覆盖“能跑通的能力”，遗漏明确不支持面

- 缓解
  - Phase 0 强制输出 supported 与 unsupported 两本 translation surface ledger。
  - Phase 5 与 Phase 6 的收口标准必须包含 unsupported feature report。
