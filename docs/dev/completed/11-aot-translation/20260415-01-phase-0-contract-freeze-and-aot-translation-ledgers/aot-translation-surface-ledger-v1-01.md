# AOT Translation Surface Ledger v1.01

Date: 2026-04-15 00:11:06 +08:00
Status: draft-freeze

## 1. 目标

冻结母 roadmap 中的 `AOT translation surface ledger`，把目标翻译面统一整理成：

- language surface
- compiler-lowered surface
- runtime service surface
- metadata / closure surface
- interop / ABI surface

本台账不负责列出所有缺口细节；`partial / unsupported / deferred` 的具体原因统一进入 `aot-unsupported-surface-ledger-v1-01.md`。

## 2. 读法

每一行都回答 6 个问题：

1. 这类 surface 属于什么语义域。
2. 它覆盖哪些 capability item。
3. 当前 owner subject / owner archetype 是谁。
4. 当前正确性证据应落在哪类 proof。
5. 是否必须有 benchmark。
6. 目标 `BodyAvailability` 是什么。

## 3. Ledger

| surface group | capability items | owner subject | owner archetype / source anchor | 正确性证据 | benchmark 策略 | target body availability |
| --- | --- | --- | --- | --- | --- | --- |
| Language: numeric / branch / array semantics | `ArithmeticOps` `BitwiseOps` `ComparisonAndBranch` `NumericConversion` `OverflowBehavior` `ArrayBasics` `ArrayBoxing` | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/PrimitivesAndOps` | proof 以单文件 capability slice 为主 | `ArithmeticOps` 必带 benchmark，其余按需 | `NativeGenerated` |
| Language: object model / dispatch / delegates | `DelegateInvocation` `DelegateChaining` `LambdaClosureCapture` `EventCallbackFlow` `GenericVirtualDispatch` `GenericInterfaceDispatch` | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/ObjectModelAndDispatch` | proof 为主；mixed / hot-update 复用同一语义基线 | `Dispatch` / callback 高价值路径带 benchmark | `NativeGenerated` |
| Language: generics and type-system core | `GenericInstantiation` `GenericConstraint` `GenericCollections` `GenericLayout` | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/GenericsAndCollections` | proof 为主 | `GenericInstantiation`、sharing 相关路径带 benchmark | `NativeGenerated` |
| Compiler-lowered: async / iterator / task flow | `AsyncAwait` `TaskAndValueTaskFlow` `IteratorStateMachine` | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | proof 为主 | `TaskAndValueTaskFlow` 带 benchmark | `NativeGenerated` |
| Compiler-lowered: exception lowering and control flow | `ExceptionBasic` `NestedException` `FinallyAndFilter` `CrossBoundaryException` | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/ExceptionsAndControlFlow` | proof 为主 | correctness 优先；默认不强制 benchmark | `NativeGenerated` |
| Runtime service: threading and synchronization | `Threading` `TaskScheduling` `MonitorAndLocking` `ThreadLocalState` | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` + `Benchmarks/CoreRuntimeBenchmarks/ThreadingAndSynchronization` | proof 为主 | `TaskScheduling`、`MonitorAndLocking` 必带 benchmark | `NativeGenerated` |
| Runtime service: loader / helpers / lifecycle | `RuntimeHelpers` `GcSensitiveFlow` `ResourceLifecycle` `LoaderContract` | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/RuntimeServices` | proof 为主 | 默认不强制 benchmark | `NativeGenerated` / `MetadataOnly` |
| Metadata / closure: reflection and member lookup | `CustomAttributeLookup` `MemberMetadataLookup` `MetadataClosure` | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/ReflectionAndMetadata` | proof 为主 | `MemberMetadataLookup` 带 benchmark | `MetadataOnly` + `NativeGenerated` |
| Metadata / closure: linker and AOT closure | `LinkerPreserveContract` `LinkerStripping` `RequiredInstantiationClosure` `GenericSharingBoundary` `AotClosureValidation` | `SolutionCorePack` | `ReflectionAndMetadata`、`GenericsAndCollections`、`Archetypes/CoreLibReferenceSolution` | proof + full-solution archetype evidence | 只为高价值 closure path 带 benchmark | `MetadataOnly` + `NativeGenerated` |
| Interop / ABI: native calls and marshaling | `NativeCallInterop` `StringAndUtf8Marshaling` `StructMarshaling` `DelegateCallbackInterop` | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/InteropAndMarshaling` | proof 为主 | 这组高价值 runtime path 默认带 benchmark | `NativeGenerated` + `ExternalRuntime` |
| Interop / ABI: unsafe and low-level | `SpanAndMemory` `RefStructAndStackalloc` `UnsafePointer` `FunctionPointer` | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/InteropAndMarshaling` + `Benchmarks/CoreRuntimeBenchmarks/UnsafeAndLowLevel` | proof 为主 | `SpanAndMemory` `UnsafePointer` `FunctionPointer` 带 benchmark | `NativeGenerated` |
| Hot-update surface | `PackageLoad` `SharedContractBinding` `MethodReplacement` `HotUpdateMetadataSupplement` `PatchIntegrity` `VersionRollback` `PatchCallbackFlow` | `HotUpdateHostPack` | `Host/Proofs`、`Host/Benchmarks`、`Archetypes/FullProjectHotUpdateSolution` | proof + full-project hot-update archetype | `Load` / `Dispatch` / `Roundtrip` 高价值路径必带 benchmark | `BridgeDispatch` / `InterpreterReady` |
| Mixed-execution surface | `MixedBridgeDispatch` `InterpreterLowering` `InterpreterArithmetic` `MixedGenericFlow` `MixedExceptionFlow` `MixedDelegateFlow` | `MixedExecutionFeaturePack` | `ManagedBridge`、`Lowering`、`Archetypes/MixedBridgeSolution` | proof 为主 | mixed dispatch / native benchmark 必带 benchmark | `BridgeDispatch` / `InterpreterReady` / `NativeGenerated` |
| Diagnostics and reporting surface | `RuntimeTraceOutput` `NativeSymbolEvidence` `CrashEvidence` `FailureDiagnostics` | `SolutionCorePack` owner，Phase 6 reporting/gate 落地 | reporting / dashboard / release gate | 不以 subject proof 为唯一证据，需外层 evidence chain | 默认不以 benchmark 为主 | `MetadataOnly` / `Unsupported` |

## 4. 工程级 surface

除了 capability item 级 surface，本 roadmap 还要求保留 solution / archetype 级翻译面：

| engineering surface | owner subject | archetype |
| --- | --- | --- |
| `ProjectReference` 最小闭包 | `SolutionCorePack` | `SimpleLibrarySolution` |
| 多 project 传递闭包 | `SolutionCorePack` | `MultiProjectSolution` |
| `PackageReference` 闭包 | `SolutionCorePack` | `PackageReferenceSolution` |
| repo-owned explicit DLL reference | `SolutionCorePack` | `ReferenceAssemblySolution` |
| repo-owned corelib closure | `SolutionCorePack` | `CoreLibReferenceSolution` |
| project + package + reference 混合闭包 | `SolutionCorePack` | `MixedReferenceClosureSolution` |
| host + patch 完整图 | `HotUpdateHostPack` | `FullProjectHotUpdateSolution` |
| managed/native/interpreter 混合桥接图 | `MixedExecutionFeaturePack` | `MixedBridgeSolution` |

这些 engineering surface 不取代 capability ledger，而是作为 capability 的 solution-level gate。

## 5. 维护规则

1. 新增 capability item 时，必须同步更新本台账。
2. 新增 benchmark 不是默认要求，只在高价值 runtime path 上强制。
3. 任何 surface 若当前并未闭环，必须进入 unsupported ledger，而不是在本台账里“静默缺失”。
4. diagnostics / release 相关 surface 可以暂由外层 evidence chain 承载，但 owner 仍然必须明确。
