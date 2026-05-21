# Capability Taxonomy Freeze v1.01

Date: 2026-04-14 10:12:00 +08:00

## 1. 冻结目标

本文档冻结本轮 capability vocabulary 与 level-2 item 边界。后续阶段可以补实现和补覆盖，但不能随意修改命名、归属和层级结构。

冻结原则：

- capability taxonomy 固定到两级
- 历史语法点按“单文件 proof/benchmark 条目”整理
- 同一 capability family 下的条目尽量合并到同一个共享项目
- 新 benchmark 不是按语法点机械扩张，而是按高价值 runtime path 选择性补充
- support state 默认不进入 attribute，而是由外部 `matrix/manifest/reporting` 维护

## 2. Support State Vocabulary

统一支持状态：

- `supported`
  - 当前 subject / matrix / mode 下有完整证据闭环
- `partial`
  - 部分可用，但存在明确边界；必须附带 `reason_code`
- `unsupported`
  - 当前不支持；必须附带 `reason_code`
- `not-applicable`
  - 对当前 subject / mode 不适用

冻结结论：

- OQ2 采用混合模型
- 工程落地按“外部 `matrix/manifest` 为主”执行
- attribute 只承载 capability、archetype、execution mode 这类内在 metadata

## 3. Benchmark Policy

冻结结论：

- 所有 capability item 先要求至少一个 proof
- benchmark 只覆盖高价值 runtime path
- benchmark 默认以 managed baseline 作为对比基线

优先 benchmark family：

- `RuntimeDispatch`
- `GenericInstantiation`
- `AllocationAndStartup`
- `InteropAndMarshaling`
- `HotUpdateLoadAndDispatch`
- `MixedBridgeAndInterpreter`

默认只保留 proof 的类型：

- 纯 correctness 断言
- 低价值语法糖展开
- 不具独立性能意义的异常或边界场景

## 4. Capability Families

### 4.1 `PrimitivesAndOps`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `ArithmeticOps`
- `BitwiseOps`
- `ComparisonAndBranch`
- `NumericConversion`
- `OverflowBehavior`
- `ArrayBasics`
- `ArrayBoxing`

### 4.2 `TypeSystemAndGenerics`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `GenericInstantiation`
- `GenericConstraint`
- `GenericCollections`
- `GenericLayout`
- `GenericVirtualDispatch`
- `GenericInterfaceDispatch`

### 4.3 `AsyncAndIterators`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `AsyncAwait`
- `TaskAndValueTaskFlow`
- `IteratorStateMachine`

### 4.4 `DelegatesAndClosures`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `DelegateInvocation`
- `DelegateChaining`
- `LambdaClosureCapture`
- `EventCallbackFlow`

### 4.5 `ExceptionsAndControlFlow`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `ExceptionBasic`
- `NestedException`
- `FinallyAndFilter`
- `CrossBoundaryException`

### 4.6 `ReflectionAndMetadata`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `CustomAttributeLookup`
- `MemberMetadataLookup`
- `MetadataClosure`
- `MetadataSupplement`
- `LinkerPreserveContract`
- `LinkerStripping`

### 4.7 `InteropAndMarshaling`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `NativeCallInterop`
- `StringAndUtf8Marshaling`
- `StructMarshaling`
- `DelegateCallbackInterop`

### 4.8 `ThreadingAndSynchronization`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `Threading`
- `TaskScheduling`
- `MonitorAndLocking`
- `ThreadLocalState`

### 4.9 `RuntimeServices`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `RuntimeHelpers`
- `GcSensitiveFlow`
- `ResourceLifecycle`
- `LoaderContract`

### 4.10 `UnsafeAndLowLevel`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `SpanAndMemory`
- `RefStructAndStackalloc`
- `UnsafePointer`
- `FunctionPointer`

### 4.11 `DiagnosticsAndTracing`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `RuntimeTraceOutput`
- `NativeSymbolEvidence`
- `CrashEvidence`
- `FailureDiagnostics`

### 4.12 `LinkerAndAotClosure`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `RequiredInstantiationClosure`
- `GenericSharingBoundary`
- `AotClosureValidation`

### 4.13 `HotUpdateWorkflow`

Owner subject:

- `HotUpdateHostPack`

Level-2 capabilities:

- `PackageLoad`
- `SharedContractBinding`
- `MethodReplacement`
- `MetadataSupplement`
- `PatchIntegrity`
- `VersionRollback`
- `PatchCallbackFlow`

### 4.14 `MixedExecution`

Owner subject:

- `MixedExecutionFeaturePack`

Level-2 capabilities:

- `MixedBridgeDispatch`
- `InterpreterLowering`
- `InterpreterArithmetic`
- `MixedGenericFlow`
- `MixedExceptionFlow`
- `MixedDelegateFlow`

## 5. 机械拆分规则

- 一个历史语法点或一个明确 runtime 断言，对应一个 `.cs` 文件
- 同一 capability family 下的 proof 尽量合并到同一个共享项目
- 不因新增一个 proof 就新增一个 `.csproj`
- 只有 solution graph、dependency model 或 executable plan 明显不同，才允许新增项目或 archetype

## 6. 结论

Phase 0 之后，所有新增 proof / benchmark / archetype 都必须先回答：

- 属于哪个 capability family
- 属于哪个 level-2 capability
- 归哪个 retained subject
- 是否真有 benchmark 价值
