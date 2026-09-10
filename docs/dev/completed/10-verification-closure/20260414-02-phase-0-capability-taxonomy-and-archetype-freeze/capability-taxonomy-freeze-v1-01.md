# Capability Taxonomy Freeze v1.01

Date: 2026-04-14 01:42:47 +08:00

## 1. 冻结目标

本文件冻结本轮 roadmap 使用的 capability vocabulary。后续阶段可以补实现和补覆盖，但不能随意更改命名、归属和层级结构。

冻结原则：

- capability taxonomy 固定到两级
- 历史语法点按“单文件 proof/benchmark 条目”整理
- 同一 capability family 下的条目尽量合并到同一项目
- 只有在 solution graph、依赖模型或 executable plan 明显不同的情况下，才允许新开项目
- subject 内部调度继续收紧，不用 string 协议继续扩大

## 2. 支持状态 Vocabulary

统一支持状态：

- `supported`
  - 该能力在当前 subject / matrix / mode 下有完整证据闭环
- `partial`
  - 该能力部分可用，但存在明确边界，必须附带 `reason_code`
- `unsupported`
  - 该能力当前不支持，必须附带 `reason_code`
- `not-applicable`
  - 该能力对当前 subject / mode 不适用

约束：

- `partial` 与 `unsupported` 必须带 `reason_code`
- `reason_code` 由外层 matrix/manifest/reporting 维护，不放进 proof 方法名

## 3. 编排规则

### 3.1 单文件规则

- 一个历史语法点或一个明确 runtime 断言，对应一个 `.cs` 文件
- 文件内可含多个断言，但必须围绕单一 level-2 capability

### 3.2 合并规则

- 同一 capability family 的 proof 尽量合并到同一 `FeatureSlice` 项目
- benchmark 同理，尽量按 family 合并到共享 benchmark 项目
- 不因新增一个 proof 就新开一个 `.csproj`

### 3.3 新开项目的允许条件

仅在以下情况允许新开项目：

- 需要不同的 solution archetype
- 需要不同的 dependency model
- 需要不同的 executable plan
- 需要隔离独立的 reference/package/corelib graph

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

Current seeds:

- `ArrayOpsProof.cs`
- `ArrayBoxingProof.cs`
- `BitwiseOpsProof.cs`
- `BranchOpsProof.cs`
- `ConversionOpsProof.cs`
- `OverflowOpsProof.cs`

### 4.2 `ObjectModelAndDispatch`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `ObjectAllocationAndIdentity`
- `VirtualDispatch`
- `InterfaceDispatch`
- `DelegateInvocation`
- `DelegateChaining`

Current seeds:

- `ObjectOpsProof.cs`
- `DispatchProof.cs`
- `VTableDispatchProof.cs`
- `InterfaceDispatchProof.cs`
- `DelegateProof.cs`
- `DelegateChainProof.cs`

### 4.3 `GenericsAndCollections`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `GenericMethodFlow`
- `GenericCollections`
- `GenericLayout`
- `GenericDispatch`

Current seeds:

- `GenericCollectionProof.cs`
- `GenericLayoutProof.cs`
- `GenericBenchmark.cs`

### 4.4 `ExceptionsAndControlFlow`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `ExceptionBasic`
- `NestedException`
- `FinallyAndFilter`
- `CrossBoundaryException`

Current seeds:

- `ExceptionProof.cs`
- `NestedExceptionProof.cs`
- `CrossBoundaryExceptionProof.cs`

### 4.5 `AsyncAndThreading`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `AsyncAwait`
- `Threading`
- `Synchronization`
- `ThreadLocalState`

Current seeds:

- `AsyncAwaitProof.cs`
- `ThreadingProof.cs`

### 4.6 `ReflectionAndMetadata`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `ReflectionLookup`
- `MetadataClosure`
- `LinkerStripping`
- `PreserveContract`

Current seeds:

- `ReflectionInteropClosureProof.cs`
- `LinkerStrippingProof.cs`

### 4.7 `InteropAndMarshaling`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `NativeCallInterop`
- `StringAndUtf8Marshaling`
- `StructMarshaling`
- `DelegateCallbackInterop`

Current seeds:

- `MarshalingProof.cs`

### 4.8 `RuntimeLowLevel`

Owner subject:

- `SolutionCorePack`

Level-2 capabilities:

- `RuntimeHelpers`
- `SpanMemory`
- `RefStructAndStackalloc`
- `UnsafePointer`
- `FunctionPointer`

Current state:

- 作为本轮 roadmap 的新增补齐目标，当前 retained subject 中缺少系统性 seed

### 4.9 `HotUpdateWorkflow`

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

Current seeds:

- `HotUpdateSkeletonProofEntry.cs`
- `SharedContractProofEntry.cs`
- `MethodReplacementProofEntry.cs`
- `VersionRollbackProofEntry.cs`
- `HotUpdatePackageSupport.cs`

### 4.10 `MixedExecution`

Owner subject:

- `MixedExecutionFeaturePack`

Level-2 capabilities:

- `InterpreterArithmetic`
- `InterpreterLowering`
- `MixedBridgeDispatch`
- `MixedGenericFlow`
- `MixedExceptionFlow`
- `MixedDelegateFlow`

Current seeds:

- `MixedExecutionProofEntry.cs`
- `InterpreterLoweringProofEntry.cs`
- `InterpreterArithmeticProof/*`

## 5. Benchmark Families

benchmark 不单独建第三层 taxonomy，直接挂在上述 capability families 上；同时保留以下 benchmark category 作为跨 family 的性能视角：

- `RuntimeDispatch`
- `Startup`
- `Allocation`
- `HotUpdate`

规则：

- benchmark 文件仍然单文件化
- 同 family benchmark 尽量合并在同一 benchmark 项目
- 不为每个 capability 强制补 benchmark

## 6. Subject Ownership Freeze

- `SolutionCorePack`
  - 负责 `PrimitivesAndOps`
  - 负责 `ObjectModelAndDispatch`
  - 负责 `GenericsAndCollections`
  - 负责 `ExceptionsAndControlFlow`
  - 负责 `AsyncAndThreading`
  - 负责 `ReflectionAndMetadata`
  - 负责 `InteropAndMarshaling`
  - 负责 `RuntimeLowLevel`
- `HotUpdateHostPack`
  - 负责 `HotUpdateWorkflow`
- `MixedExecutionFeaturePack`
  - 负责 `MixedExecution`

## 7. 结论

本轮 roadmap 以 level-2 capability 为组织单位，而不是以旧 subject 名称或零散语法点名称为组织单位。

后续所有实现阶段都必须遵守：

- 历史语法点单文件化
- 能力族内尽量同项目合并
- 用 capability vocabulary 做归属，不再靠临时字符串命名撑调度面
