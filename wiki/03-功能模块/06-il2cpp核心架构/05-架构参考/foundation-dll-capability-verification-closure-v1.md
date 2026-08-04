# Foundation DLL Capability Verification Closure v1

## 1. 目标

定义 foundation-dll 翻译验证闭环的正式 authority，使新增 DLL 进入验证范围时，必须同时具备：

- 完整的 DLL / family / method capability authority
- 完整的功能、性能、热更验证 obligations
- 可归档、可 review、可 solution 执行的 generated result
- 可由 projection 稳定消费的 formal aggregate

## 2. 四层模型

### 2.1 Truth Layer

回答：

- 这个 DLL 是什么
- 包含哪些 `CapabilityFamily`
- 每个 family 包含哪些 `methodSubjectId`
- 每个 method 需要哪些 functional / benchmark / hotupdate / review obligations

正式对象：

- `DllCapabilityManifest`
- `CapabilityFamilyVerificationContract`
- `MethodCapabilityContract`

### 2.2 Evidence Layer

回答：

- 实际运行出了什么
- 最新生成了什么
- 什么可以被 review

正式对象：

- `ManagedFactRecord`
- `NativeFactRecord`
- `ManagedBenchmarkRecord`
- `NativeBenchmarkRecord`
- `HotUpdateFactRecord`
- `HotUpdateBenchmarkRecord`
- `ReviewBundle`
- `MethodValidationRecord`
- `MethodCaseIndex`

### 2.3 Execution Layer

回答：

- 这些验证怎么执行
- 在仓库里从哪里运行

正式对象：

- `FoundationDllTranslationSolution.sln`
- generated family `test/native/benchmark/host/patch` projects
- execution contract

### 2.4 Projection Layer

回答：

- 用户现在应该看到什么
- 当前开发到哪里
- 能否真正用于 IL2CPP

正式对象：

- `DllAggregateRecord`
- `FamilyAggregateRecord`
- `MethodAggregateRecord`
- dashboard / detail / method projections

## 3. 新增 DLL 的 onboarding freeze

任何 DLL 一旦进入翻译验证范围，必须一次性生成四层内容。

最小输入集合：

- `assemblyName`
- `ownerSubjectId`
- `capabilityFamilies[]`
- `methodUniverse[]`
- `requiredRoutes`
- `benchmark obligations`
- `hotupdate obligations`
- `review targets`
- `solution entries`

## 4. 目录 contract

对每个 family 固定目录：

- `verification/foundation-dll/<assembly>/<family>/test/`
- `verification/foundation-dll/<assembly>/<family>/native/`
- `verification/foundation-dll/<assembly>/<family>/benchmark/`
- `verification/foundation-dll/<assembly>/<family>/host/`
- `verification/foundation-dll/<assembly>/<family>/patch/`
- `verification/foundation-dll/<assembly>/<family>/review/`

formal indexes 固定位置：

- `verification/foundation-dll/<assembly>/<family>/method-test-case-index.json`
- `verification/foundation-dll/<assembly>/<family>/method-benchmark-case-index.json`
- `verification/foundation-dll/<assembly>/<family>/method-hotupdate-case-index.json`

## 5. A2 Source-Annotation-First

case 映射 truth 采用 generated source / metadata annotations。

annotation scanner 的 authority 边界固定为：

- 只允许读取 generated source / metadata
- 只允许产出 formal case index
- 不允许从运行结果反推 truth

annotation 规则：

- 优先使用枚举化 attributes
- 允许保留 string 的字段仅限：
  - `methodSubjectId`
  - 少量不可枚举化的路径类字段

推荐 annotation 类型：

- `[MethodSubjectId]`
- `[BenchmarkSubjectId]`
- `[HotUpdateSubjectId]`
- `[CapabilityFamilyId]`
- `[VerificationRoute]`
- `[BenchmarkRoute]`
- `[BenchmarkProfile]`
- `[HotUpdateDirection]`

## 6. completed-before gate

foundation-dll 翻译验证采用 `G2 reviewable-bundle` 作为 completed-before gate。

每个进入验证范围的 family，至少必须满足：

- generated source snapshot 已归档
- case index 已生成
- latest execution results 已归档
- `ReviewBundle` 已生成
- solution 可执行

## 7. 执行入口

正式主入口固定为：

- `FoundationDllTranslationSolution.sln`

必须支持局部执行：

- 按 DLL
- 按 family
- 按 family 的 `test/benchmark/host/patch`

这些局部入口只作为开发加速入口，不属于新的 formal 主入口。

## 8. 展示 contract

导航固定为：

- 一级 dashboard：DLL
- 二级详情：family
- 三级详情：method
- tooltip：method 测试详情

tooltip 不展示 run 汇总，只展示：

- test cases
- benchmark cases
- hotupdate cases
- latest status
- artifact links

## 9. 非目标

- 不允许只改 dashboard 不改 authority
- 不允许只生成半成品测试工程
- 不允许把 family 局部入口升级为新的 formal 主入口
- 不允许引入新流程后长期保留旧双轨逻辑
