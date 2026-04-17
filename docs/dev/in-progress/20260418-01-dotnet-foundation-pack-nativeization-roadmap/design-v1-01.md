# DotNet Foundation Pack Nativeization Design v1.01

Date: 2026-04-18 01:19:47 +08:00
Status: draft-approved-for-roadmap

## 1. 问题重述

当前仓库的 native AOT 主线是项目/入口驱动模型，它回答的是“某个 subject 的入口闭包能否生成 native”，而不是“官方基础框架程序集本身是否已经完整 native 化”。

新的目标必须改成：

- 以官方 `Microsoft.NETCore.App.Ref` 与 `Microsoft.NETCore.App` 为唯一主输入
- 先完成 `.NET 8`，并保留 `.NET 10` 的同构执行路径
- 认证单位是单 DLL，而不是单个 proof 或单个入口
- 完成标准是“单 DLL 的全部公开 API / IL 语义都必须可 native 化”
- 项目、mixed execution、hotupdate 只作为消费者验证层，不再承担底层能力发现职责

## 2. 设计目标

- 建立一条与现有 `dotnet-project` 主线并行存在的 `framework-pack-driven` 主线
- 用 `ref pack + runtime pack` 双输入定义单 DLL 的公开 surface 与真实实现语义
- 将 `Microsoft.NETCore.App` 内的程序集分为 contract/facade、shim、implementation、runtime-backed 几类
- 用机器可审计的 ledger 证明“公开 surface 无漏项、语义分类无漏项、native 化无漏项”
- 为每个阶段定义可监控的 native proof、benchmark 与进度指标
- 最终以 hotupdate 消费已认证基础层，而不是让 hotupdate 反向驱动基础层设计

## 3. 架构

### 3.1 总体结构

建议新增如下正式主线：

`FrameworkPackResolve -> FrameworkCatalog -> AssemblyClassifier -> SurfaceLedger -> LayerPlanner -> AssemblySemanticIR -> NativeizationPlanner -> PerAssemblyNativeEmitter -> AssemblyCertification -> ConsumerValidation`

### 3.2 分层

#### L0: Asset Source Layer

负责解析官方 packs：

- `Microsoft.NETCore.App.Ref/<version>/ref/<tfm>/*.dll`
- `Microsoft.NETCore.App/<version>/*.dll`

职责：

- 定位 ref/runtime 资产
- 记录版本、来源、校验信息
- 产出标准化 catalog 输入

#### L1: Catalog And Classification Layer

负责产出：

- `framework-catalog.json`
- `assembly-classification.json`
- `public-surface-manifest.json`
- `assembly-dependency-graph.json`

职责：

- 列出每个程序集
- 区分 facade/shim/implementation/runtime-backed
- 汇总公开 types、methods、properties、fields、events
- 汇总 type forward 与公开依赖

#### L2: Planning And IR Layer

负责产出：

- `layer-plan.json`
- `assembly-semantic-ir/<assembly>.json`
- `assembly-nativeization-plan/<assembly>.json`

职责：

- 确定 DLL 的执行层级
- 对单 DLL 全部公开 surface 建立语义账本
- 标记每个公开成员属于：
  - `contract-only`
  - `abstract/interface`
  - `il-lowerable`
  - `runtime-backed`
  - `pinvoke-backed`
  - `deferred-blocked`

#### L3: Native Emission Layer

负责产出 per-assembly native 产物：

- `generated/native/<framework>/<version>/<assembly>/...`
- `native-proof-manifest.json`
- `native-build-manifest.json`

职责：

- 不再围绕单入口生成单一大 TU
- 改为以 DLL 为边界生成 native 产物
- 显式输出 ABI、helper、link dependency、surface coverage 信息

#### L4: Certification And Consumer Validation Layer

负责两类验证：

- `assembly certification`
- `consumer validation`

前者证明 DLL 本身完成；后者证明上层系统可以消费已认证底座。

## 4. 组件与职责边界

### 4.1 `FrameworkPackResolver`

做什么：

- 定位 `.NET 8` / `.NET 10` 的 ref/runtime packs
- 读取来源与版本元信息

如何被使用：

- 作为新 source model `framework-pack` 的第一个 stage

依赖：

- 本地 packs 或仓库冻结快照

是否可独立测试：

- 可以，使用固定 pack 目录做 unit/contracts 测试

### 4.2 `FrameworkCatalogBuilder`

做什么：

- 构建基础框架程序集目录

如何被使用：

- 为下游 classifier、planner、certifier 提供统一输入

依赖：

- resolver 输出

是否可独立测试：

- 可以，验证程序集数量、版本、路径、哈希与稳定顺序

### 4.3 `AssemblyClassifier`

做什么：

- 将程序集分类为：
  - `contract-facade`
  - `compat-shim`
  - `implementation`
  - `runtime-backed`

如何被使用：

- 决定单 DLL completion gate 与后续 phase 归属

依赖：

- ref/runtime 对照信息
- type forward 与实现体统计

是否可独立测试：

- 可以，使用已知样本如 `System.Runtime.dll`、`mscorlib.dll`、`System.Private.CoreLib.dll`

### 4.4 `SurfaceLedgerBuilder`

做什么：

- 枚举单 DLL 全部公开 surface
- 建立公开成员账本

如何被使用：

- 作为“单 DLL 全公开 API 0 漏项”认证基础

依赖：

- ref assembly metadata
- runtime assembly metadata

是否可独立测试：

- 可以，验证公开 surface 枚举稳定性与 ref/runtime 映射

### 4.5 `LayerPlanner`

做什么：

- 根据公开依赖、实现依赖、type forward、runtime helper 生成分层计划

如何被使用：

- 决定哪个 DLL 可以进入当前批次

依赖：

- classification
- dependency graph
- semantic ledger

是否可独立测试：

- 可以，验证 layer 生成规则与循环依赖处理

### 4.6 `AssemblySemanticIRBuilder`

做什么：

- 为单 DLL 生成“整程序集语义账本”

如何被使用：

- 替代当前“以 entry method 为中心的 AotCoreIr 消费方式”

依赖：

- surface ledger
- metadata reader
- lowerability / runtime helper 规则

是否可独立测试：

- 可以，验证公开成员分类与 helper 需求识别

### 4.7 `PerAssemblyNativeEmitter`

做什么：

- 按 DLL 输出 native 产物

如何被使用：

- 为 per-assembly build/proof/conformance 提供输入

依赖：

- assembly nativeization plan
- templates / helper contracts

是否可独立测试：

- 可以，验证输出路径、manifest、模板 shape 与 linker input

### 4.8 `AssemblyCertificationRunner`

做什么：

- 对单 DLL 执行 completion gate

如何被使用：

- 将 DLL 标记为 `certified / blocked / partial / deferred`

依赖：

- public surface ledger
- semantic IR
- native build result
- conformance result

是否可独立测试：

- 可以，验证 gate 计算与失败报告

### 4.9 `ConsumerValidationOrchestrator`

做什么：

- 使用已认证基础层做上层验证

如何被使用：

- 驱动 managed/native/hotupdate 消费者 proof 与 benchmark

依赖：

- certified assembly set
- collection / registry / workspace

是否可独立测试：

- 可以，验证“只消费已认证底座”的选择逻辑

## 5. 数据流与控制流

### 5.1 基础数据流

1. 解析 `.NET 8` / `.NET 10` packs
2. 为每个 DLL 建立 catalog
3. 枚举 ref/runtime 公开 surface
4. 生成程序集分类与依赖图
5. 生成 layer plan
6. 对当前层内每个 DLL 生成 semantic ledger
7. 为当前 DLL 生成 nativeization plan 与 native 输出
8. 执行单 DLL certification
9. 将 certified 集合作为上层 layer 或 consumer validation 的输入

### 5.2 Phase 闭环

每个 phase 的闭环固定为：

`unit/contracts -> surface proof -> native proof -> phase benchmark -> certification update`

只有当本阶段目标 DLL 集全部达到 certification gate，才允许进入下一阶段。

## 6. 单 DLL 完成模型

### 6.1 Contract/Facade/Shim

完成标准：

- 全部公开 types/members 已被枚举
- 全部公开 surface 已归属 canonical owner
- type forward 与 facade 映射无漏项
- 所有公开语义都能在 native 世界被正确绑定

### 6.2 Implementation

完成标准：

- 全部公开 surface 已被枚举
- 每个公开成员都已完成语义分类
- 每个 `il-lowerable` 公开方法都已可 native 化
- 每个 `runtime-backed` / `pinvoke-backed` 公开方法都有实现契约
- 无未分类、无未覆盖、无未归属依赖

### 6.3 Runtime-Backed

完成标准：

- 不能只写“未来补 runtime helper”
- helper contract 未落地前，该 DLL 不能进入 certified

## 7. Phase 级 proof / benchmark / 监控模型

### 7.1 Proof 类型

#### `surface-proof`

证明：

- 单 DLL 公开 surface 已完整枚举

核心产物：

- `public-surface-manifest.json`
- `surface-proof-report.json`

#### `semantic-proof`

证明：

- 全部公开成员已被语义分类

核心产物：

- `semantic-ledger.json`
- `semantic-proof-report.json`

#### `native-proof`

证明：

- 单 DLL native 输出可构建、可链接、可调度

核心产物：

- `native-build-manifest.json`
- `native-proof-report.json`

#### `conformance-proof`

证明：

- managed 语义与 native 语义对齐

核心产物：

- `conformance-cases.json`
- `conformance-report.json`

#### `consumer-proof`

证明：

- certified foundation APIs 可被 managed/native/hotupdate 消费者使用

核心产物：

- `consumer-proof-report.json`

### 7.2 Benchmark 类型

#### `nativeization-throughput-benchmark`

观测：

- per-DLL lowering、emit、build 时间

#### `semantic-family-benchmark`

观测：

- string、array、delegate、generic、reflection、net、io 等族的 native 性能

#### `consumer-overhead-benchmark`

观测：

- hotupdate bind/load/call overhead
- external DLL 场景性能损耗

### 7.3 监控指标

每阶段至少监控：

- `assembliesDiscovered`
- `assembliesClassified`
- `assembliesCertified`
- `publicMembersTotal`
- `publicMembersCertified`
- `runtimeBackedMembersImplemented`
- `nativeProofPassRate`
- `conformancePassRate`
- `phaseBenchmarkTrend`

## 8. 错误处理与风险边界

### 8.1 `.NET 10` 资产缺失

当前本机没有 `.NET 10` packs。

默认假设：

- 后续执行前将官方 `.NET 10` `Microsoft.NETCore.App.Ref` / `Microsoft.NETCore.App` 冻结到仓库资产目录

如果改为外部预装：

- 只影响资产获取，不影响认证模型

### 8.2 动态代码与不可静态 lower 的 API

像下列程序集可能天然需要更强 runtime 能力：

- `System.Linq.Expressions`
- `System.Reflection.Emit*`
- `Microsoft.CSharp`

设计规则：

- 未具备所需 runtime service 前，不允许把整个 DLL 标为 completed

### 8.3 Facade 与 Implementation 混用风险

如果不先区分 facade/shim/implementation，容易把“公开身份可绑定”误判成“实现已完整 native 化”。

必须分别认证。

### 8.4 dotnet 编译崩溃门禁

在任何测试/验证阶段，只要 `dotnet build` / `dotnet test` / `msbuild` 崩溃：

- 当前阶段立即失败
- 必须保留 stderr/binlog/dump
- 必须先查根因并修复，再继续后续验证

## 9. 测试与验证

### 9.1 上游 gate

基础框架阶段必须先过：

- `tests/unit/**`
- `tests/contracts/**`
- catalog/classification/layer/ledger contract

### 9.2 单 DLL gate

每个 DLL 的认证顺序固定为：

1. `surface-proof`
2. `semantic-proof`
3. `native-proof`
4. `conformance-proof`
5. `phase benchmark`（按需）

### 9.3 下游消费者 gate

完成基础框架层后，再进入：

- managed consumer proof
- native consumer proof
- hotupdate consumer proof
- benchmark

### 9.4 owner subject 设计

建议新增 canonical owner：

- `DotNetFoundationPack`

职责：

- 承载 `Microsoft.NETCore.App` 基础程序集认证
- 承载 phase benchmark 与 release gate

现有 owner 保持：

- `SolutionCorePack`: 项目/solution 消费者验证
- `MixedExecutionFeaturePack`: mixed execution 消费者验证
- `HotUpdateHostPack`: hotupdate 消费者验证

## 10. 设计结论

这次重设计的本质不是“把更多 proof 接到现有 entry-driven 主线上”，而是把主线本身改成：

`官方 framework pack 驱动 + 单 DLL 认证驱动`

只有这样，才能满足“从 `.NET 8 / .NET 10` 最底层基础框架程序集开始，一层层确认能否 native 化更复杂 DLL 类型”的目标。
