# Legacy Subject Inventory v1.01

Date: 2026-04-13 08:57:38 +08:00

## 1. 目标

盘点当前 `subjects/` 顶层 root 的真实状态，为方案 A 的 retained subject 收敛提供唯一 inventory 输入。

本文件只回答三个问题：

- 当前有哪些顶层 subject
- 它们现在处于什么形态
- 它们为什么不能继续长期并列存在

## 2. 当前顶层 subject 清单

当前 `subjects/` 共 28 个顶层目录：

- `AutoBridgeProof`
- `BenchDelegate`
- `BenchException`
- `BenchGameLoop`
- `BridgeRoundtripProof`
- `CompatibilityMatrixProof`
- `EngineHostProof`
- `EngineLifecycleCallbackLite`
- `EngineLogWriteLite`
- `EngineObjectHandleLite`
- `GenericEcho`
- `GenericSupplementProof`
- `HelloWorld`
- `HelloWorldObject`
- `HostEmbeddingLite`
- `HotUpdateHostPack`
- `MainlineFeaturePack`
- `MethodReplacementProof`
- `MixedExecutionFeaturePack`
- `MobileHelloWorldProof`
- `PerformanceFeaturePack`
- `PInvokeLite`
- `ReflectionLite`
- `SharedContractProof`
- `SolutionMultiProject`
- `SolutionPackageReference`
- `SolutionSimpleLib`
- `VersionRollbackProof`

## 3. 按当前结构形态分类

### 3.1 retained 基础候选

这批对象已经具备 pack 化或 solution-like 基础，但还没收敛到方案 A 的最终 retained 结构：

| subject | 当前形态 | 主要问题 |
| --- | --- | --- |
| `MainlineFeaturePack` | 主线 proof pack | 仍然是单 `.csproj`，不是显式 `.sln` |
| `PerformanceFeaturePack` | benchmark pack | 尚未并回 core solution |
| `HotUpdateHostPack` | hot-update pack | 尚未显式 `.sln` 化 |
| `MixedExecutionFeaturePack` | mixed proof + benchmark pack | 尚未显式 `.sln` 化 |
| `SolutionSimpleLib` | archetype solution-like subject | 仍以 app project 为入口，不是显式 `.sln` |
| `SolutionMultiProject` | archetype solution-like subject | 同上 |
| `SolutionPackageReference` | archetype solution-like subject | 同上 |

### 3.2 benchmark legacy roots

这批对象都是单点 benchmark root，不适合继续长期作为顶层 subject：

| subject | 当前特征 | 当前证据 |
| --- | --- | --- |
| `BenchDelegate` | 单 benchmark root | `workloadEntry = Program::RunWorkload()` |
| `BenchException` | 单 benchmark root | `workloadEntry = Program::RunWorkload()` |
| `BenchGameLoop` | 单 benchmark root | `workloadEntry = Program::RunWorkload()` |
| `GenericEcho` | 运行时 perf / backend 边界项 | benchmark 类，但是否保留为 runtime benchmark 需重判 |

### 3.3 correctness / interop / host legacy roots

这批对象本质上是单点 proof 或单点 host/interop slice，不应继续占用顶层 root：

| subject | 当前特征 |
| --- | --- |
| `BridgeRoundtripProof` | bridge correctness root |
| `CompatibilityMatrixProof` | compatibility 验证 root |
| `EngineHostProof` | engine host proof root |
| `EngineLifecycleCallbackLite` | engine callback slice |
| `EngineLogWriteLite` | engine log slice |
| `EngineObjectHandleLite` | engine object handle slice |
| `GenericSupplementProof` | generic / metadata proof |
| `HelloWorld` | onboarding smoke root |
| `HelloWorldObject` | onboarding object root |
| `HostEmbeddingLite` | host embedding slice |
| `MobileHelloWorldProof` | mobile correctness root |
| `PInvokeLite` | interop slice |
| `ReflectionLite` | reflection slice |
| `SharedContractProof` | shared contract root |

### 3.4 manifestless hot-update legacy roots

这三类对象当前甚至还没有 `subject.manifest.json`，只能视为待并回 retained subject 的过渡残留：

| subject | 当前状态 | 问题 |
| --- | --- | --- |
| `AutoBridgeProof` | 只有 `source/*.csproj` | manifest 缺失，不适合作为长期顶层 subject |
| `MethodReplacementProof` | 只有 `source/*.csproj` | 同上 |
| `VersionRollbackProof` | 只有 `source/*.csproj` | 同上 |

## 4. 当前结构共性问题

### 4.1 顶层 subject 粒度过细

很多目录表达的是：

- 单个 proof
- 单个 benchmark
- 单个 host/interop slice

而不是一个独立的 executable plan 或独立 solution source bundle。

### 4.2 “solution 模式” 仍未显式落地

现有 retained 基础都还是：

- `source/` 下一个或多个 `.csproj`
- 通过 manifest 指向某个 app project 入口

还没有真正升级成显式 `.sln`。

### 4.3 compatibility 心智仍由 legacy root 驱动

当前仓库打开 `subjects/` 后，第一感知仍然是：

- proof root 很多
- bench root 很多
- feature pack 只是其中一部分

这与方案 A 的目标完全相反。

## 5. 方案 A 下的 inventory 结论

Phase 0 对当前 inventory 的冻结结论如下：

- 顶层 root 长期只保留极少数 retained subject
- 现有 28 个 root 中，长期 retained 的目标只剩三类：
  - `SolutionCorePack`
  - `HotUpdateHostPack`
  - `MixedExecutionFeaturePack`
- 其余 root 全部视为：
  - 迁移源
  - 兼容过渡源
  - 待删除对象

这意味着：

- 以后不再把 “一个新测试点” 理解成 “一个新顶层 subject”
- 而是默认理解成 retained subject 里的新 slice、新工程级条目或新声明条目
