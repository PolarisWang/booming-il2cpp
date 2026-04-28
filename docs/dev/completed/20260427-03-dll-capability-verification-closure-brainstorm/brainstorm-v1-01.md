# DLL Capability Verification Closure Brainstorm v1

Date: 2026-04-27 02:35:00 +08:00
Updated: 2026-04-27 04:18:00 +08:00
Status: in-progress

## 1. 目标

重做 DLL 翻译验证体系，不只是补 dashboard，而是建立一套可以回答以下问题的正式系统：

1. 每个进入翻译验证的 DLL，它的全部信息是否完整可见
2. 每个 CapabilityFamily 的功能 / 性能 / 热更验证是否形成闭环
3. 这些测试项是否在 DLL onboarding 时一次性规划
4. 最新生成结果是否可归档、可 review、可在 solution 中直接执行
5. dashboard 只消费 formal truth，不再自己发明状态

## 2. 已拍板内容

### 2.1 总体方向

采用 `Hybrid Layer` 分层方案。

### 2.2 导航结构

- 一级 dashboard：看 DLL
- 二级详情：看 family
- 三级详情：看 method
- tooltip：展示 method 对应测试详情，不再展示 run 汇总

### 2.3 闭环主线

功能闭环：

- `managed fact -> native fact -> hotupdate fact`

性能闭环：

- `managed benchmark -> native benchmark -> hotupdate benchmark`

### 2.4 Subject Validation authority

必须一步到位采用 method 级全映射：

- `methodSubjectId -> test case`
- `methodSubjectId -> benchmark case`
- `methodSubjectId -> hotupdate case`

不允许先走弱 authority。

### 2.5 方案 A 选择

在方案 A 的 case 实现里，用户已拍板采用：

- `A2 Source-Annotation-First`

### 2.6 新增 DLL 时的要求

当新增一个 DLL 进入翻译验证时，必须一次性生成四层内容，尤其包括：

- Evidence Layer 的各种 case 代码
- Execution Layer 的项目与 solution 接线

### 2.7 目录结构

按 family 固定目录：

- test case
  - `verification/foundation-dll/<assembly>/<family>/test/`
  - `verification/foundation-dll/<assembly>/<family>/native/`
- benchmark case
  - `verification/foundation-dll/<assembly>/<family>/benchmark/`
- hotupdate case
  - `verification/foundation-dll/<assembly>/<family>/host/`
  - `verification/foundation-dll/<assembly>/<family>/patch/`

### 2.8 Annotation 约束

- Attribute 尽量用枚举，少用 string
- string 只保留真正不可枚举化的标识，例如 `methodSubjectId`

### 2.9 执行性能要求

- 必须支持按 family 局部测试
- 不能每次都要求全 DLL / 全 solution 全量跑

## 3. 四层分别做什么

## 3.1 Truth Layer

### 回答的问题

- 这个 DLL 到底是什么
- 包含哪些 CapabilityFamily
- 每个 family 包含哪些 method
- 每个 method 需要哪些 functional / benchmark / hotupdate / review obligations

### 准备怎么做

建立三个正式对象：

#### A. `DllCapabilityManifest`

负责冻结 DLL 的能力全貌。

建议字段：

- `assemblyName`
- `ownerSubjectId`
- `scopePhase`
- `sourceAssemblies`
- `capabilityFamilies[]`
- `methodUniverse[]`
- `implementationFamilies[]`
- `sourceGroups[]`
- `requiredRoutes`
- `requiredBenchmarks`
- `hotupdateImpactMatrix`
- `reviewTargets`
- `solutionEntries`

#### B. `CapabilityFamilyVerificationContract`

负责冻结 family 级 obligations。

建议字段：

- `familyId`
- `displayName`
- `methodSubjectIds`
- `functionalObligation`
- `performanceObligation`
- `hotupdateFunctionalObligation`
- `hotupdatePerformanceObligation`
- `reviewBundleRequired`
- `formalVerificationObjects`

#### C. `MethodCapabilityContract`

负责冻结 method 级 obligations。

建议字段：

- `methodSubjectId`
- `familyId`
- `functionalRequired`
- `benchmarkRequired`
- `hotupdateFunctionalRequired`
- `hotupdateBenchmarkRequired`
- `routeCodes[]`
- `benchmarkProfiles[]`
- `hotupdateProfiles[]`
- `reviewRequired`

### 这层的边界

负责：

- denominator
- obligation
- completed gate 输入边界

不负责：

- 跑测试
- 记录运行结果
- 输出 dashboard

## 3.2 Evidence Layer

### 回答的问题

- 实际跑出来了什么
- 最新生成了什么
- 什么可以被 review

### 准备怎么做

建立 7 类正式证据对象：

#### A. `ManagedFactRecord`

记录 managed 侧 method/family 事实：

- method 存在性
- signature
- declaring type
- family 归属
- required routes / benchmark / hotupdate impact

#### B. `NativeFactRecord`

记录 native 侧事实：

- generated native symbol / emitted fact
- covered / uncovered / blocked
- unsupported reason
- generated artifact path

#### C. `BenchmarkRecord`

分三种：

- `ManagedBenchmarkRecord`
- `NativeBenchmarkRecord`
- `HotUpdateBenchmarkRecord`

统一字段建议：

- `benchmarkId`
- `methodSubjectId`
- `familyId`
- `routeCode`
- `profileCode`
- `metricName`
- `value`
- `baselineValue`
- `variancePercent`
- `passRule`
- `artifactPath`

#### D. `HotUpdateFactRecord`

记录热更功能事实：

- host -> patch
- patch -> host
- binding / metadata visibility
- activation / rollback
- applicable methods / families

#### E. `ReviewBundle`

这是强制可审对象。

至少包含：

- latest generated sources snapshot
- method mappings
- unsupported / blocked reasons
- representative generated artifacts
- benchmark artifacts
- hotupdate artifacts
- review notes placeholder
- solution path
- execution instructions

#### F. `MethodValidationRecord`

这是给三级 method 页面和 tooltip 用的。

建议字段：

- `methodSubjectId`
- `familyId`
- `managedFactStatus`
- `nativeFactStatus`
- `subjectValidationStatus`
- `managedBenchmarkStatus`
- `nativeBenchmarkStatus`
- `hotupdateFactStatus`
- `hotupdateBenchmarkStatus`
- `testCases[]`
- `benchmarkCases[]`
- `hotupdateCases[]`
- `generatedArtifacts[]`

#### G. `MethodCaseIndex`

这是 A2 的核心 formal index。

拆成三份：

- `method-test-case-index.json`
- `method-benchmark-case-index.json`
- `method-hotupdate-case-index.json`

每条记录至少包含：

- `methodSubjectId`
- `familyId`
- `caseKind`
- `projectPath`
- `sourceFile`
- `className`
- `memberName`
- `routeCode`
- `profileCode`
- `annotationDigest`

### 这层的边界

负责：

- 沉淀真实证据
- 让 latest result 可归档、可审计、可 review

不负责：

- 决定 DLL 是否 usable
- 决定 completed / release-ready

## 3.3 Execution Layer

### 回答的问题

- 这些验证怎么被执行
- 在仓库里从哪里运行

### 准备怎么做

建立统一执行入口：

#### A. `FoundationDllTranslationSolution.sln`

这是正式主入口。

包含：

- generated family test projects
- generated benchmark projects
- hotupdate host / patch projects
- review helper / audit projects

#### B. Family / DLL 执行项目

固定四类：

- `functional test project`
- `native proof project`
- `benchmark project`
- `hotupdate proof project`

#### C. 局部执行入口

必须支持：

- 只跑某个 DLL
- 只跑某个 family
- 只跑某个 family 的 test / benchmark / host / patch

推荐方式：

- 保持单一正式主 `.sln`
- 同时支持 family filter

例如：

- `run foundation-dll test --assembly <assembly> --family <family>`
- `run foundation-dll benchmark --assembly <assembly> --family <family>`
- `run foundation-dll hotupdate --assembly <assembly> --family <family>`

#### D. Execution Contract

定义：

- 用哪个 runner / host
- 读哪些 manifest / collection
- 结果回写到哪里
- 哪些 artifact 是 primary evidence

### 这层的边界

负责：

- 给所有 truth object 提供可执行入口
- 保证 `.sln`、`.csproj`、host/patch 项目都能落地

不负责：

- 自己定义 obligation
- 自己计算最终聚合状态

## 3.4 Projection Layer

### 回答的问题

- 用户现在应该看到什么
- 当前开发到哪里
- 能不能真正用于 IL2CPP

### 准备怎么做

建立三层展示对象：

#### A. DLL Aggregate

一级 dashboard 用。

建议字段：

- `dllStage`
- `functionalReadiness`
- `performanceReadiness`
- `hotupdateReadiness`
- `reviewReadiness`
- `il2cppUsable`
- `releaseReadiness`
- `mainBlocker`
- `nextAction`

#### B. Family Aggregate

二级详情页用。

建议字段：

- `familyStage`
- `functionalProgress`
- `performanceProgress`
- `hotupdateProgress`
- `reviewStatus`
- `methodCount`
- `blockedReason`

#### C. Method Aggregate

三级详情页用。

建议字段：

- `methodSubjectId`
- `displayName`
- `managedFact`
- `nativeFact`
- `subjectValidation`
- `managedBenchmark`
- `nativeBenchmark`
- `hotupdateFact`
- `hotupdateBenchmark`
- `reviewArtifacts`

### 这层的边界

负责：

- 聚合
- 展示
- 导航

不负责：

- 自己发明 truth
- 直接从 HTML 反推状态

## 4. dashboard 应该怎么显示四层信息

## 4.1 一级：DLL dashboard

只显示 DLL Aggregate，不显示 family 和 method。

建议列：

| DLL | Stage | Functional | Performance | HotUpdate | Review | IL2CPP Usable | Main Blocker | Next Action |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |

## 4.2 二级：Family 详情页

只显示 family aggregate，不直接展开 method。

建议列：

| Family | Functional | Performance | HotUpdate | Review | Family Stage | Blocker |
| --- | --- | --- | --- | --- | --- | --- |

额外显示：

- denominator authority
- family obligations
- family required routes
- executable project / benchmark / hotupdate entry

## 4.3 三级：Method 页面

建议列：

| Method | Managed Fact | Native Fact | Subject Validation | Managed Benchmark | Native Benchmark | HotUpdate Fact | HotUpdate Benchmark | Review |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |

目标：

- method 级追责
- method 级 debug
- method 级 review

## 4.4 Tooltip：Method 测试详情

tooltip 只展示 method 相关测试详情：

- method 短名
- 所属 family
- test case 列表
- benchmark case 列表
- hotupdate case 列表
- project 路径
- latest execution status
- latest artifact links

truth source 推荐：

1. `MethodValidationRecord`
2. `ReviewBundle` 中的 method section

不推荐直接从 run 结果反推。

## 5. A2 Source-Annotation-First：具体怎么做

### 5.1 核心原则

A2 的 truth 分成两部分：

- DLL / family / method obligations：来自 Truth Layer
- case 映射 truth：来自 generated source annotations

scanner 读取 generated source，产出 formal case index。

### 5.2 三类 annotation

#### functional test

```csharp
[MethodSubjectId("System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)")]
[CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ConvertChar)]
[VerificationRoute(VerificationRoute.Native)]
public void Method_System_Convert_ToChar_System_Boolean()
{
}
```

#### benchmark

```csharp
[BenchmarkSubjectId("System.Private.CoreLib/System.Buffer::BlockCopy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
[CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_BufferMemory)]
[BenchmarkRoute(BenchmarkRoute.Native)]
[BenchmarkProfile(BenchmarkProfile.Default)]
public void Method_System_Buffer_BlockCopy()
{
}
```

#### hotupdate

```csharp
[HotUpdateSubjectId("System.Private.CoreLib/System.Enum::Parse:System.Object(System.Type,System.String)")]
[CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_EnumParsing)]
[HotUpdateDirection(HotUpdateDirection.HostToPatch)]
public void Method_System_Enum_Parse_HostToPatch()
{
}
```

### 5.3 为什么要尽量枚举化

优点：

- 减少 typo
- scanner 更容易严格校验
- projection 更容易稳定消费

string 只保留：

- `methodSubjectId`
- artifact path

### 5.4 新增 DLL 时要生成什么

#### Truth Layer

- `DllCapabilityManifest`
- `CapabilityFamilyVerificationContract`
- `MethodCapabilityContract`

#### Evidence Layer

- review bundle skeleton
- annotation source
- formal case index 目标路径

#### Execution Layer

- `test/` project
- `native/` project
- `benchmark/` project
- `host/` project
- `patch/` project
- solution wiring

#### Projection Layer

- DLL aggregate skeleton
- family aggregate skeleton
- method aggregate skeleton

## 6. A2 的落地位置

### 6.1 family 目录布局

对每个 family 固定：

- `verification/foundation-dll/<assembly>/<family>/test/`
- `verification/foundation-dll/<assembly>/<family>/native/`
- `verification/foundation-dll/<assembly>/<family>/benchmark/`
- `verification/foundation-dll/<assembly>/<family>/host/`
- `verification/foundation-dll/<assembly>/<family>/patch/`

### 6.2 formal index 位置

建议统一放在：

- `verification/foundation-dll/<assembly>/<family>/method-test-case-index.json`
- `verification/foundation-dll/<assembly>/<family>/method-benchmark-case-index.json`
- `verification/foundation-dll/<assembly>/<family>/method-hotupdate-case-index.json`

### 6.3 review bundle 位置

建议统一放在：

- `verification/foundation-dll/<assembly>/<family>/review/`

里面至少包含：

- `bundle.json`
- `generated-sources/`
- `artifacts/`
- `notes.md`

## 7. A2 的完整验证流程

1. DLL onboarding freeze
   - 冻结 `DllCapabilityManifest`
   - 冻结 family / method obligations
2. generated source emission
   - 生成 functional / benchmark / hotupdate source
   - 打注解
3. solution wiring
   - 挂进正式 `.sln`
4. annotation scan
   - 产出 3 份 formal case index
5. consistency validation
   - 校验 required methods 是否都有 case
   - 校验 case 是否都进了 solution
6. functional execution
7. benchmark execution
8. hotupdate execution
9. review bundle generation
10. projection aggregation

## 8. completed-before gate 做到什么程度

给 3 档方案：

### G1 `artifact-present`

要求：

- generated source 存在
- case index 存在
- solution entry 存在

优点：

- 最轻

缺点：

- 只能证明“有东西”

### G2 `reviewable-bundle`

要求：

- generated source snapshot 已归档
- case index 已生成
- latest execution results 已归档
- review bundle 已生成
- solution 可执行

优点：

- 满足“可归档、可 review、可执行”
- 不强制人工 sign-off

缺点：

- 不能代表“review 已完成”

### G3 `review-signed-off`

要求：

- 满足 G2
- 还要显式 review verdict / sign-off

优点：

- 最强

缺点：

- 流程最重

### 我的建议

推荐 **G2 reviewable-bundle**

### 用户已拍板

- `Review Bundle` 必须成为 completed-before gate
- completed-before gate 强度采用 `G2 reviewable-bundle`
- annotation scanner 的 authority 只允许从 generated source / metadata 产出 formal index
- annotation scanner 不允许从运行结果反推 truth

## 9. 这套方案的主要问题

### 问题 1

annotation scanner 会成为关键基础设施。

### 问题 2

method 级 benchmark / hotupdate 不一定都天然合理。

### 问题 3

generated source 和 projects 会很多，solution 会变大。

### 问题 4

review bundle 如果定义太弱，会退化成“有链接就算完成”。

## 10. 我的建议

在你已经拍板 A2 的前提下，我建议：

1. `Review Bundle` 必须成为 completed-before gate
2. annotation scanner 的 authority 必须收紧，只允许从 generated source / metadata 产出 formal index
3. 必须保留单一正式主 `.sln`
4. family 局部执行入口允许存在，但只作为开发加速入口

## 11. 当前 open questions

### Q1

solution 执行入口是否必须有单一正式 `.sln` 主入口，而 family 局部入口只作为开发加速入口？

我的建议：

- 是

## 12. 下一步分流条件

在进入正式开发计划前，还需要拍板：

1. `Review Bundle` completed-before gate 是否 mandatory，强度是否用 `G2`
1. family 局部入口是否只作为开发加速入口

在这些问题清零前，不应写 implementation plan。
