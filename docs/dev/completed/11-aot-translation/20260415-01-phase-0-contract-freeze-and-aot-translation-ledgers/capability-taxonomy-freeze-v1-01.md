# Capability Taxonomy Freeze v1.01

Date: 2026-04-15 00:11:06 +08:00
Status: draft-freeze

## 1. 目标

冻结母 roadmap 的 `Q1`:

- `capability taxonomy = 两级结构`
- level-1 family 负责语义 / 运行时域的归类
- level-2 item 负责稳定、细粒度、可追踪的 capability identity

本文件同时回答两个执行问题：

1. 某个 capability item 属于哪个 family。
2. 某个 family 当前由哪个 retained subject 作为 owner。

## 2. Level-1 Family 冻结表

| family enum | 语义域 | owner subject | 当前真实承载根 |
| --- | --- | --- | --- |
| `PrimitivesAndOps` | 基础算术、转换、分支、数组语义 | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/PrimitivesAndOps` |
| `TypeSystemAndGenerics` | 泛型实例化、layout、约束、generic dispatch | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/GenericsAndCollections` |
| `AsyncAndIterators` | async/await、task、iterator lowering | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` |
| `DelegatesAndClosures` | delegate、closure、callback 语义 | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/ObjectModelAndDispatch` |
| `ExceptionsAndControlFlow` | 异常传播与控制流语义 | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/ExceptionsAndControlFlow` |
| `ReflectionAndMetadata` | 反射、元数据查询、preserve/stripping | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/ReflectionAndMetadata` |
| `InteropAndMarshaling` | native interop 与 ABI 边界 | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/InteropAndMarshaling` |
| `ThreadingAndSynchronization` | 线程、任务调度、锁与同步 | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` |
| `RuntimeServices` | loader、runtime helpers、资源与生命周期 | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/RuntimeServices` |
| `UnsafeAndLowLevel` | span、ref struct、unsafe、function pointer | `SolutionCorePack` | `FeatureSlices/CoreRuntimeFeatures/InteropAndMarshaling` + `Benchmarks/CoreRuntimeBenchmarks/UnsafeAndLowLevel` |
| `DiagnosticsAndTracing` | trace、crash、symbol、failure diagnostics | `SolutionCorePack` owner，Phase 6 才落专门证据链 | 当前无 retained source root，先由 reporting / gate 台账承接 |
| `LinkerAndAotClosure` | AOT 闭包、generic sharing、instantiation closure | `SolutionCorePack` | `GenericsAndCollections`、`ReflectionAndMetadata`、`CoreLibReferenceSolution` |
| `HotUpdateWorkflow` | package、shared contract、metadata supplement、rollback | `HotUpdateHostPack` | `Host/Proofs`、`Host/Benchmarks`、`Archetypes/FullProjectHotUpdateSolution` |
| `MixedExecution` | managed/native/interpreter 混合执行 | `MixedExecutionFeaturePack` | `ManagedBridge`、`Lowering`、`Archetypes/MixedBridgeSolution` |

## 3. Level-2 Item 冻结表

### 3.1 `PrimitivesAndOps`

- `ArithmeticOps`
- `BitwiseOps`
- `ComparisonAndBranch`
- `NumericConversion`
- `OverflowBehavior`
- `ArrayBasics`
- `ArrayBoxing`

### 3.2 `TypeSystemAndGenerics`

- `GenericInstantiation`
- `GenericConstraint`
- `GenericVirtualDispatch`
- `GenericInterfaceDispatch`
- `GenericCollections`
- `GenericLayout`

### 3.3 `AsyncAndIterators`

- `AsyncAwait`
- `TaskAndValueTaskFlow`
- `IteratorStateMachine`

### 3.4 `DelegatesAndClosures`

- `DelegateInvocation`
- `DelegateChaining`
- `LambdaClosureCapture`
- `EventCallbackFlow`

### 3.5 `ExceptionsAndControlFlow`

- `ExceptionBasic`
- `NestedException`
- `FinallyAndFilter`
- `CrossBoundaryException`

### 3.6 `ReflectionAndMetadata`

- `CustomAttributeLookup`
- `MemberMetadataLookup`
- `MetadataSupplement`
- `MetadataClosure`
- `LinkerPreserveContract`
- `LinkerStripping`

### 3.7 `InteropAndMarshaling`

- `NativeCallInterop`
- `StringAndUtf8Marshaling`
- `StructMarshaling`
- `DelegateCallbackInterop`

### 3.8 `ThreadingAndSynchronization`

- `Threading`
- `TaskScheduling`
- `MonitorAndLocking`
- `ThreadLocalState`

### 3.9 `RuntimeServices`

- `RuntimeHelpers`
- `GcSensitiveFlow`
- `ResourceLifecycle`
- `LoaderContract`

### 3.10 `UnsafeAndLowLevel`

- `SpanAndMemory`
- `RefStructAndStackalloc`
- `UnsafePointer`
- `FunctionPointer`

### 3.11 `DiagnosticsAndTracing`

- `RuntimeTraceOutput`
- `NativeSymbolEvidence`
- `CrashEvidence`
- `FailureDiagnostics`

### 3.12 `LinkerAndAotClosure`

- `RequiredInstantiationClosure`
- `GenericSharingBoundary`
- `AotClosureValidation`

### 3.13 `HotUpdateWorkflow`

- `PackageLoad`
- `SharedContractBinding`
- `MethodReplacement`
- `HotUpdateMetadataSupplement`
- `PatchIntegrity`
- `VersionRollback`
- `PatchCallbackFlow`

### 3.14 `MixedExecution`

- `MixedBridgeDispatch`
- `InterpreterLowering`
- `InterpreterArithmetic`
- `MixedGenericFlow`
- `MixedExceptionFlow`
- `MixedDelegateFlow`

## 4. 命名与目录规则

冻结 `Q3` 的执行口径：

- 路径或命名空间体现 family / domain。
- 文件名体现 level-2 capability item。
- archetype 回答“solution graph 是什么”，capability item 回答“语义能力是什么”。

因此：

- `GenericsAndCollections/GenericLayoutProof.cs` 是合理表达。
- “为一个 capability item 新开顶层 subject” 不是合理表达。
- “把 capability item 埋进新的 string scenario 名称里” 也不是合理表达。

## 5. 新增 item 的准入条件

只有同时满足以下条件，才允许新增 level-2 item：

1. 现有 item 无法承接其语义边界。
2. 它将被稳定地引用到 proof / benchmark / reporting / release gate。
3. 已明确 owner subject。
4. 已明确它属于哪个 level-1 family。

否则应优先复用现有 item 或只新增 subject 内部实现，不新增 taxonomy。

## 6. 对后续阶段的约束

- Phase 1 只能把历史语法点切到这些 family / item 上，不再发明新的同义层。
- Phase 2-4 的 typed-il、dispatch、AOT Core IR 只能消费这些 family / item 作为外层稳定能力标识。
- Phase 5-6 的 dashboard / unsupported report 也只能围绕这些 family / item 汇总。
