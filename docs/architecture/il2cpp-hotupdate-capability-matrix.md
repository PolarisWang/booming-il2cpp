# IL2CPP + 热更能力矩阵

## 目的

这份文档定义仓库当前用于描述 IL2CPP、热更与 mixed execution 能力闭环的长期 contract。它回答四类问题：

1. 某个 C# 运行时能力点归谁负责验证。
2. 它在 `native`、`hot-update`、`mixed` 三条执行路径中的主验证载体是什么。
3. 哪些信息放在 `Chaos.TestFramework` attribute 内，哪些信息必须留在外层 matrix / manifest / reporting。
4. 当能力未支持或只部分支持时，如何统一公开表达。

## 契约分层

### Subject 内的内在元数据

由 `src/reference/Chaos.TestFramework` 提供，允许 subject 作者直接在 C# 方法上声明：

- `ChaosUnitTestAttribute.Category`
- `ChaosBenchmarkAttribute.Category`
- `Requires`
- `Archetype`
- `HotUpdateCapability`
- benchmark 的 `Modes`、`Metrics`、`WarmupCount`、`IterationCount`、`InvocationCount`

这些字段只表达“条目本身是什么”，不表达“整个系统在所有平台上是否完全支持”。

### 外层支持状态

以下信息必须留在外层 matrix / manifest / reporting / docs：

- `supported / partial / unsupported / not-applicable`
- support state 的 reason code
- 某个能力在特定 host/target/platform 下的可用性
- 产品化边界，例如 iOS distribution policy、compatibility band、rollback gate

这是为了避免把平台状态和产品状态硬编码进 subject source attribute。

## Retained Subject 分工

| Subject | 主职责 | 主要 archetype / slice | 主验证问题 |
| --- | --- | --- | --- |
| `SolutionCorePack` | IL2CPP native 主线、完整 solution 闭包、reference/corelib 依赖图 | `SimpleLibrarySolution`、`MultiProjectSolution`、`PackageReferenceSolution`、`ReferenceAssemblySolution`、`CoreLibReferenceSolution`、`MixedReferenceClosureSolution`、`CoreRuntimeFeatures`、`CoreRuntimeBenchmarks` | “能否把完整 solution ingest、分析、翻译成 native，并保持语义正确” |
| `HotUpdateHostPack` | host + patch 主线、shared contracts、metadata supplement、rollback / compatibility | `SkeletonPatchSolution`、`FullProjectHotUpdateSolution`、`VersionCompatibilitySolution`、`ReferenceAssemblyPatchSolution` | “热更工程怎么组织、怎么加载、怎么回滚、怎么维持 host/patch 契约” |
| `MixedExecutionFeaturePack` | managed / native / interpreter 混合执行、lowering、bridge 语义 | `MixedBridgeSolution`、`ManagedBridge`、`Interpreter`、`Lowering` | “native / interpreter / managed 混合边界是否保持同一语义” |

## 支持状态词汇

所有外层支持状态统一使用：

| 状态 | 含义 |
| --- | --- |
| `supported` | 该能力在当前目标矩阵内有稳定证据，且无已知降级。 |
| `partial` | 有证据，但只覆盖部分平台、部分模式或存在已知限制。必须携带 reason code。 |
| `unsupported` | 当前明确不支持。必须携带 reason code。 |
| `not-applicable` | 对该 subject / archetype / mode 不适用，不代表失败。 |

## 能力矩阵

### Primitives And Ops

| level-2 capability | native owner | hot-update owner | mixed owner | 主要证据 |
| --- | --- | --- | --- | --- |
| arithmetic / bitwise / branch / conversion / overflow | `SolutionCorePack/CoreRuntimeFeatures/PrimitivesAndOps` | `not-applicable` | `MixedBridgeSolution` 的算术桥接可复用为对照 | proof + benchmark |
| arrays / boxing / basic object ops | `SolutionCorePack/CoreRuntimeFeatures/PrimitivesAndOps` | `not-applicable` | `MixedExecutionFeaturePack` 仅在 mixed 需要时补充 | proof |

### Object Model, Dispatch And Delegates

| level-2 capability | native owner | hot-update owner | mixed owner | 主要证据 |
| --- | --- | --- | --- | --- |
| interface / virtual / dispatch | `SolutionCorePack/CoreRuntimeFeatures/ObjectModelAndDispatch` | `HotUpdateHostPack` 在 shared contract 调用链上复用 | `MixedExecutionFeaturePack/ManagedBridge` | proof + benchmark |
| delegate chain / callback / closure | `SolutionCorePack/CoreRuntimeFeatures/ObjectModelAndDispatch` | `PatchCallbackFlow` | `MixedExecutionFeaturePack/ManagedBridge` | proof |

### Generics And Collections

| level-2 capability | native owner | hot-update owner | mixed owner | 主要证据 |
| --- | --- | --- | --- | --- |
| generic layout / instantiation / collection semantics | `SolutionCorePack/CoreRuntimeFeatures/GenericsAndCollections` | `SharedContractBinding` 与 patch generic roundtrip | `MixedExecutionFeaturePack` 跨 interpreter/native 的泛型边界 | proof + benchmark |
| generic virtual / interface dispatch | `ChaosRuntimeFeature.GenericVirtualDispatch` 的 proof 条目归 `SolutionCorePack` | host/patch 边界按需补充 | mixed bridge 边界补充 | proof |

### Async, Threading And Synchronization

| level-2 capability | native owner | hot-update owner | mixed owner | 主要证据 |
| --- | --- | --- | --- | --- |
| async / await state machine | `SolutionCorePack/CoreRuntimeFeatures/AsyncAndThreading` | `partial`，仅在热更需要时补条目 | mixed 场景按 lowering / bridge 需要扩展 | proof |
| thread / synchronization primitives | `SolutionCorePack/CoreRuntimeFeatures/AsyncAndThreading` | `not-applicable` 默认不纳入 patch contract | mixed runtime 有需要再补 | proof |

### Exceptions And Control Flow

| level-2 capability | native owner | hot-update owner | mixed owner | 主要证据 |
| --- | --- | --- | --- | --- |
| throw / catch / finally / nested exception | `SolutionCorePack/CoreRuntimeFeatures/ExceptionsAndControlFlow` | `PatchCallbackFlow` 上的异常传播 | `MixedExecutionFeaturePack/Lowering` | proof |
| cross-boundary exception | `SolutionCorePack/CoreRuntimeFeatures/ExceptionsAndControlFlow` | `HotUpdateHostPack` host-patch 边界 | `MixedBridgeSolution` managed/native/interpreter 边界 | proof |

### Reflection, Metadata And Linker / AOT Closure

| level-2 capability | native owner | hot-update owner | mixed owner | 主要证据 |
| --- | --- | --- | --- | --- |
| reflection / metadata lookup | `SolutionCorePack/CoreRuntimeFeatures/ReflectionAndMetadata` | `MetadataSupplement` | mixed runtime 仅承接必要桥接 | proof |
| stripping / preserve / closure completeness | `SolutionCorePack/CoreRuntimeFeatures/ReflectionAndMetadata` + `CoreLibReferenceSolution` | `MetadataSupplement` / `PatchIntegrity` | `not-applicable` 默认不单独建 mixed archetype | proof + registry evidence |
| repository-owned corelib closure | `SolutionCorePack/CoreLibReferenceSolution` | `not-applicable` | `not-applicable` | managed output + native ingest |

### Interop And Low-Level Runtime

| level-2 capability | native owner | hot-update owner | mixed owner | 主要证据 |
| --- | --- | --- | --- | --- |
| marshaling / ABI boundary | `SolutionCorePack/CoreRuntimeFeatures/InteropAndMarshaling` | `not-applicable` | `MixedExecutionFeaturePack/ManagedBridge` 视桥接边界补充 | proof |
| span / memory / unsafe / function pointer / ref struct | 优先归 `SolutionCorePack`，按能力族继续扩 slice | `not-applicable`，除非 patch contract 必须覆盖 | mixed runtime 仅补实际桥接所需点 | proof |

### Hot-Update Productization

| level-2 capability | native owner | hot-update owner | mixed owner | 主要证据 |
| --- | --- | --- | --- | --- |
| package load | `not-applicable` | `ChaosHotUpdateCapability.PackageLoad` | `not-applicable` | proof + benchmark |
| shared contract binding | `not-applicable` | `ChaosHotUpdateCapability.SharedContractBinding` | mixed bridge 复用共享契约时可参照 | proof |
| method replacement | `not-applicable` | `ChaosHotUpdateCapability.MethodReplacement` | `not-applicable` | proof |
| metadata supplement | `not-applicable` | `ChaosHotUpdateCapability.MetadataSupplement` | mixed runtime 只消费，不成为 owner | proof |
| patch integrity | `not-applicable` | `ChaosHotUpdateCapability.PatchIntegrity` | `not-applicable` | proof |
| version rollback / compatibility band | `not-applicable` | `ChaosHotUpdateCapability.VersionRollback` | `not-applicable` | proof + release gate |
| host-to-patch / patch-to-host callback flow | `not-applicable` | `ChaosHotUpdateCapability.PatchCallbackFlow` | mixed bridge 可复用边界语义 | proof |

## 产品化维度

能力闭环不能只看 proof 是否为真，还必须叠加以下维度：

- host platform matrix：`windows` / `macos` / `linux`
- execution mode matrix：`managed` / `native` / `interpreter`
- solution archetype matrix：完整依赖图如何进入 IL2CPP
- compatibility matrix：AOT 主包版本与 patch 版本带宽
- distribution policy：尤其是 iOS 的逻辑热更边界
- diagnostics matrix：benchmark、trace、metadata、symbol、rollback evidence

这些维度由 `subject.manifest.json`、外层 runner、dashboard 与 release 文档共同承载。

## 当前 canonical 证据锚点

- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures`
- `subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks`
- `subjects/SolutionCorePack/source/Archetypes/CoreLibReferenceSolution`
- `subjects/HotUpdateHostPack/source/Archetypes/FullProjectHotUpdateSolution`
- `subjects/MixedExecutionFeaturePack/source/Archetypes/MixedBridgeSolution`

## 维护规则

1. 新能力先映射到已有 level-2 capability；只有确实跨出边界时才新增 capability 项。
2. 新验证点优先落到现有 retained subject，而不是新增顶层 subject。
3. support state 不写进 attribute；attribute 只描述条目自身。
4. benchmark 只给高价值运行时路径，不机械为每个 proof 派生 benchmark。
