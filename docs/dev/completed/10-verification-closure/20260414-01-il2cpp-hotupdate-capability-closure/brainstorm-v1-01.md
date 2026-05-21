# IL2CPP 与热更全能力收口 Brainstorm v1.01

Date: 2026-04-14 01:06:01 +08:00

## 1. 用户目标重述

本轮目标不是继续做局部 subject 清理，而是启动一条新的长期主线，覆盖以下四类结果：

- C# 所有需要支持的 feature，都要能正确落到 IL2CPP native 与热更执行路径，并在 `subjects/` 中有对应测试用例。
- 完整 solution 要能被完整验证，覆盖 `ProjectReference`、显式 `Reference`、`PackageReference` 等不同依赖形态。
- 新增一个代表性的完整 solution，引用若干 dotnet 核心 DLL，要求既能完整翻译成 native，也能给出全工程热更用法。
- 输出配套文档，明确能力矩阵、使用方式、边界和热更方案说明。

## 2. 当前基线

### 2.1 retained subject 基线

当前仓库已经收口到 3 个 retained subject：

- `SolutionCorePack`
- `HotUpdateHostPack`
- `MixedExecutionFeaturePack`

这说明顶层 subject 收口本身已经基本完成，新的问题不应再用“增加更多顶层 subject”来解决。

### 2.2 声明式测试框架基线

当前已经存在：

- `src/reference/Chaos.TestFramework`
- `ChaosUnitTestAttribute`
- `ChaosBenchmarkAttribute`
- `Assert`
- `ChaosRuntimeFeature` / `ChaosMetric` / `ChaosExecutionMode` / `ChaosSubjectEntryKind` / `ChaosSubjectSlice`

这意味着“subject 内使用 C# attribute 声明 proof/benchmark，而不是继续堆自定义 validation/scenario 文件”的方向已经成立，本轮应沿这条线继续扩展，而不是发明第二套声明机制。

### 2.3 现有 subject 覆盖基线

- `SolutionCorePack`
  - 已覆盖部分 mainline 语言特性 proof
  - 已覆盖 `single project` / `multi-project` / `package reference` archetype
  - 已具备 managed/native benchmark 基础
- `HotUpdateHostPack`
  - 已覆盖 skeleton / replacement / shared contract / rollback
  - 已具备 hot-update benchmark 基础
- `MixedExecutionFeaturePack`
  - 已覆盖 mixed proof / interpreter lowering proof
  - 已具备 managed/native/interpreter benchmark 基础

### 2.4 当前最大缺口

缺的不是 subject 机制，而是“能力边界没有被系统表达”：

- 没有统一的语言能力矩阵
- 没有统一的完整 solution 形态矩阵
- 没有“native 支持”和“hot-update 支持”之间的显式差异模型
- 没有“dotnet 核心 DLL 引用 solution”与“全工程热更方案”的 canonical archetype
- 没有把这些能力与文档、gate、发布边界收口成一套完整 contract

## 3. 这轮任务真正要解决的问题

### 3.1 “C# 所有 feature”不能直接按语法关键字列清单

如果直接按“C# 语法点”列测试，会出现两个问题：

- 范围失控。C# 版本、编译器糖、BCL 行为和 runtime 能力会混在一起。
- 测试价值失真。很多语法只是编译期糖，真正需要验证的是 IL/runtime 语义是否正确。

所以本轮必须把“all features”重新定义成：

- 以 `net8.0` + 仓库实际支持的语言版本为边界
- 以 runtime 语义能力族为主轴
- 以“native 支持 / hot-update 支持 / mixed 支持 / 明确不支持”四态来表达

### 3.2 完整 solution 验证不能只看 source 目录能不能编译

完整 solution 验证至少要回答：

- 多 project 图能不能被完整 ingest
- transitive reference 能不能被完整解析
- 显式 `Reference` 的 DLL 能不能正确进入分析和 codegen
- package reference 的闭包能不能稳定工作
- host/patch/shared contracts 的 solution 能不能形成全工程热更方案

换句话说，这不是“再加几个 proof 文件”，而是要建立 solution archetype contract。

### 3.3 dotnet 核心 DLL 引用方案需要工程化，而不是临时 `HintPath`

用户要的是“新增一个完整 solution，引用 dotnet 核心 DLL，并能完整 native 翻译与热更说明”。这里真正要解决的是：

- 引用源从哪里来
- 如何保证机器间可重复
- 如何避免把本机 SDK 路径硬编码进测试工程

这在 roadmap 里必须单列为 open question。

## 4. 方案对比

### 方案 A: 单一超级 subject

做法：

- 把所有 native、hot-update、mixed execution 都塞进一个顶层 subject
- 所有 solution archetype 和 feature slice 都放在同一 solution 下

优点：

- 顶层结构最少
- 新人从目录表面看最简单

缺点：

- executable plan 被混掉
- host-plus-patch、generated-native、mixed runtime 三类链路边界不清
- launcher、artifact、benchmark、报告都会再次变脏

结论：

- 不推荐。

### 方案 B: 保留 3 个 retained subject，在内部扩展 archetype 与 capability slice

做法：

- 继续保留：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- 所有新增测试点尽量并入这 3 个 subject 内部
- 新增完整 solution archetype，而不是新增顶层 subject
- 用 `Chaos.TestFramework` 扩充声明语义，统一 proof / benchmark / capability 标注

优点：

- 与当前仓库状态连续
- 顶层 subject 数量稳定
- executable plan 边界仍然清晰
- 最容易把“能力矩阵”和“solution archetype”同时做干净

缺点：

- `SolutionCorePack` 会继续变大，需要更清晰的内部切片命名
- 需要在 `Chaos.TestFramework` 上继续补齐少量声明枚举

结论：

- 推荐。

### 方案 C: 按能力域继续拆成更多 pack

做法：

- 新增 `LanguageFeaturePack`、`ReferenceAssemblyPack`、`CoreLibSolutionPack`、`HotUpdateWorkflowPack` 等更多 subject

优点：

- 每个 subject 职责更纯
- 单个 solution 规模更小

缺点：

- 顶层 subject 再次膨胀
- 新增测试点时又会回到“是否再新建 subject”的讨论
- 不符合当前仓库已经完成的 retained-subject 收口方向

结论：

- 不推荐。

## 5. 推荐方向

推荐采用方案 B，并加上以下明确落地原则：

- 顶层 `subjects/` 继续只保留 3 个 retained subject。
- 新的“dotnet 核心 DLL 引用 solution”不新建顶层 subject，归入 `SolutionCorePack` 的新 archetype。
- “全工程热更用法”不新建顶层 subject，归入 `HotUpdateHostPack` 的新 archetype。
- `MixedExecutionFeaturePack` 继续只负责 mixed/interpreter 语义链路，不吞并完整 host-plus-patch 工作流。
- proof / benchmark 的公开声明入口继续只走 `src/reference/Chaos.TestFramework`。

## 6. 本轮应该新增的能力维度

除了现有的 proof / benchmark 闭环，本轮还需要把以下更大维度纳入设计：

- 语言与运行时语义矩阵
  - native 支持与 hot-update 支持要分开建模
- solution archetype 矩阵
  - `ProjectReference` / `Reference` / `PackageReference` / corelib reference / host-patch-shared-contract
- 平台与产品化矩阵
  - host 平台、target 平台、device package、版本兼容、rollback、distribution policy
- 可观测性矩阵
  - 输出、trace、symbol、benchmark、crash/rollback 证据
- 文档矩阵
  - capability matrix
  - solution authoring guide
  - full-project hot-update guide
  - unsupported / partial support policy

## 7. 进入 roadmap 的必要性

这个任务必须进入 roadmap，原因很明确：

- 至少会拆成 `Chaos.TestFramework` 扩展、subject source 扩展、pipeline/gate 调整、文档输出四条主线
- 会跨多次会话推进
- 需要先冻结阶段边界，否则很容易再次边做边改方向

本轮 brainstorm/design 通过后，下一步应进入正式 roadmap，而不是直接开改代码。
