# Generated Workspace Contract Freeze v1.01

Date: 2026-04-14 16:34:34 +08:00
Status: frozen-for-phase-1

## 1. 目的

本文件冻结 `Subject Generated Test Workspace And Unified Test Entry` roadmap 的 Phase 0 contract。
从本文件开始，后续 Phase 1-5 不再重新讨论以下边界：

- `workspace.manifest.json` v2 的目标字段与职责
- declared catalog v2 与 generated host 的调度 contract
- proof / benchmark / engineering validation / engineering workload 的职责分层
- 允许短期存在的兼容层，以及明确禁止继续扩张的 legacy surface

## 2. 单一真源

### 2.1 Subject 真源

- `subjects/<SubjectId>/source/` 是 subject source solution / project 的唯一真源
- `subjects/<SubjectId>/subject.manifest.json` 只负责 subject 级别的声明、matrix/pipeline 选择与迁移期兼容信息
- `solutions/subjects/<SubjectId>/` 是生成产物目录，不反向成为手工维护真源

### 2.2 Proof / Benchmark 真源

- proof 只由 `ChaosUnitTestAttribute` 声明
- benchmark 只由 `ChaosBenchmarkAttribute` 声明
- 共享声明面位于 `src/reference/Chaos.TestFramework/Chaos.TestFramework.cs`
- `src/managed` 仍是 IL2CPP 核心实现，不承担 subject 侧测试 API 的放置职责

### 2.3 Catalog / Workspace 真源

- declaration discovery 真源：
  - `src/managed/Chaos.IL2CPP.DeclarationDiscovery/Program.cs`
- compiled catalog 真源：
  - `build/toolchains/run/testing/compiled_catalog.py`
- workspace 真源：
  - 生成后的 `workspace.manifest.json` v2

## 3. Workspace Manifest v2 Freeze

### 3.1 根对象

`workspace.manifest.json` v2 固定为：

```json
{
  "workspaceVersion": 2,
  "kind": "subject-workspace",
  "subjectId": "SolutionCorePack",
  "variant": "CHECK",
  "defaultMatrixId": "windows-native-check",
  "managedSolutionPath": "solutions/subjects/SolutionCorePack/SolutionCorePack.sln",
  "managedProjects": [],
  "managedTestProjects": [],
  "nativeProjects": [],
  "nativeTestProjects": [],
  "matrices": []
}
```

冻结要求：

- `workspaceVersion` 必须显式写 `2`
- `kind` 固定为 `subject-workspace`
- `subjectId`、`variant`、`defaultMatrixId`、`managedSolutionPath` 为根级必备字段
- 四类工程数组是 v2 的一等概念，不能再折叠回单个 `nativeProjectPath` 或 `nativeTargets`

### 3.2 Project 记录 contract

四类工程记录统一遵循“闭集字段 + 闭集枚举”的原则。内部 Python/C# API 应优先使用枚举；JSON 只是枚举序列化边界。

#### 3.2.1 `managedProjects[]`

最小字段：

- `projectId`
- `projectPath`
- `assemblyName`
- `isPrimary`

#### 3.2.2 `managedTestProjects[]`

最小字段：

- `projectId`
- `projectPath`
- `assemblyName`
- `hostKind`
  - `proof-host`
  - `benchmark-host`
- `catalogPath`
- `generatedSourcePath`

#### 3.2.3 `nativeProjects[]`

最小字段：

- `projectId`
- `matrixId`
- `projectPath`
- `configureRoot`
- `targetPlatform`
- `toolchainProfile`
- `deliveryKind`

#### 3.2.4 `nativeTestProjects[]`

最小字段：

- `projectId`
- `matrixId`
- `projectPath`
- `configureRoot`
- `targetPlatform`
- `toolchainProfile`
- `deliveryKind`
- `hostKind`
  - `proof-host`
  - `benchmark-host`
- `managedTestProjectId`

### 3.3 Matrix 记录 contract

`matrices[]` 固定承担“矩阵到工程集合的路由”，而不是继续承载旧 proof target 别名。

最小字段：

- `matrixId`
- `goalIds`
- `hostPlatform`
- `targetPlatform`
- `toolchainProfile`
- `managedProjectIds`
- `managedTestProjectIds`
- `nativeProjectIds`
- `nativeTestProjectIds`

### 3.4 明确禁止继续写入的旧字段

以下字段从 Phase 0 开始被冻结为 legacy，只允许读兼容，不允许继续新增写入：

- `visualStudioStateVersion`
- `generatedRoot`
- `defaultOpenNativeProject`
- `defaultBuildNativeProject`
- `generatedNativeProjectPath`
- `proofNativeProjectPath`
- `nativeProjectPath`
- `nativeTargets`

## 4. Declared Catalog v2 Freeze

### 4.1 Catalog 记录

每条 declared entry 至少包含：

- `stableId`
- `entryIndex`
- `alias`
- `assemblyName`
- `declaringType`
- `methodName`
- `methodSignature`
- `category`
- `capabilityFamily`
- `capabilityItem`
- `archetype`
- `hotUpdateCapability`
- `requires`
- proof 专属：
  - `evidence`
  - `priority`
- benchmark 专属：
  - `metrics`
  - `modes`
  - `warmupCount`
  - `iterationCount`
  - `invocationCount`

### 4.2 `entryIndex` 规则

- `entryIndex` 是 generated host 的内部数值调度键
- `entryIndex` 在每个 family 内独立编号
  - `declared-unit-test`
  - `declared-benchmark`
- 编号规则冻结为：
  - 先按 `stableId` 升序排序
  - 再从 `0` 开始连续编号
- 外部 CLI / registry 仍可使用 `stableId` 或 `alias` 选择对象
- managed/native host 的最终执行边界统一下沉到 `entryIndex`

这条规则的目的不是追求“更短字符串”，而是阻止 subject 内部调度继续扩张自由字符串协议面。

## 5. Generated Host Freeze

### 5.1 Managed host

每个 subject 固定生成两个 managed test host：

- proof host
- benchmark host

固定输出：

- `managed-tests/<Subject>.ProofHost.csproj`
- `managed-tests/<Subject>.BenchmarkHost.csproj`
- `managed-tests/Generated/declared-tests.catalog.json`
- `managed-tests/Generated/ChaosGeneratedDeclaredTests.g.cs`
- `managed-tests/Generated/ChaosGeneratedDeclaredBenchmarks.g.cs`

managed host 的职责：

- 持有 catalog 快照
- 建立 `stableId -> entryIndex` 映射
- 建立 `entryIndex -> wrapper invoke` 映射
- 提供统一的 host 执行入口

明确禁止：

- 每个 retained subject 继续手写 proof/benchmark 注册表
- 在 managed host 内继续靠 `sourceEntry` 文本做主分发

### 5.2 Native test host

native test host 不再重复做一套 declaration parser，也不再维持第二套手写 proof 列表。

固定职责：

- 以 generated managed host 的翻译结果为输入
- 负责 native 启动壳、参数解析、运行期打包与交付
- 最终仍调用 generated host 的同一套 `entryIndex` 调度

## 6. Unified Entry Freeze

### 6.1 Proof / Benchmark

- `run generate/build/test` 的 proof / benchmark 主链全部围绕 workspace manifest v2
- registry 负责“找对象”
- workspace manifest 负责“找工程 / 找 host”
- generated host 负责“执行 entry”

### 6.2 Engineering validation / workload

- engineering validation / workload 继续保留 pipeline object 语义
- 但统一入口、artifact spine、report spine 必须与 workspace manifest v2 对齐
- 不允许为了统一入口，强行把 engineering pipeline 伪装成 declared proof / benchmark

## 7. 迁移期允许保留的 shim

Phase 0 允许存在以下 shim，但只读不扩张：

- `subject.manifest.json` 中的 `source.entry`
- registry item 上的 `sourceEntry` / `workloadEntry`
- `ChaosSubjectEntryArguments`
- `ChaosSourceEntryArguments`
- `ChaosSubjectEntryKind`
- `ChaosSubjectSlice`

约束：

- 不允许新增新的 `ChaosSubjectSlice` 成员
- 不允许新增新的 `--chaos-source-entry=` 生产路径
- 不允许新增新的手写 `Dictionary<string, Func<int>>` 或反射字符串调度壳

## 8. Phase 0 之后的硬性规则

- 新功能必须接到 generated host / workspace manifest v2 主链，不得继续接旧 proof host
- 新 subject 不得再新增手写 proof launcher、benchmark launcher、source-entry 字典或字符串反射入口
- 如果一个能力必须引入兼容读取，必须能明确标注删除阶段，默认最晚 Phase 5 删除
