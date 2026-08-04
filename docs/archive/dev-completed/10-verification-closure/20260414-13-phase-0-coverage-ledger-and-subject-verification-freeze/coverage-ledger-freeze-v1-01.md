# Coverage Ledger Freeze v1.01

Date: 2026-04-14 12:52:36 +08:00
Status: frozen

## 1. 目的

这份文档冻结新的母 roadmap 所使用的 capability ledger。后续所有 child task 都必须基于这份台账回答：

- 这个能力点属于哪个 `capability_family` / `capability_item`
- 谁是 `owner_subject`
- 谁是 `owner_archetype`
- proof 应该落在哪里
- benchmark 是否必要、若必要应以什么粒度存在
- support-state 与文档证据应挂在哪里

## 2. 台账字段

| 字段 | 含义 |
| --- | --- |
| `capability_family` | 一级能力族，用于 dashboard、docs、阶段分组 |
| `capability_item` | 二级能力项，是 owner 和验收的最小单位 |
| `owner_subject` | 负责长期承接该能力点的 retained subject |
| `owner_archetype` | 工程级主验证载体 |
| `proof_shape` | proof 应以哪类 slice / entry 方式承接 |
| `benchmark_policy` | `required` / `selective` / `none` |
| `support_state_anchor` | 外层 support-state 与 reason code 的主挂点 |
| `target_phase` | 当前母 roadmap 的目标推进阶段 |

## 3. 冻结规则

1. 新能力点必须先映射到已有 `capability_item`；只有跨越现有边界时才新增项。
2. 每个 `capability_item` 必须只有一个 `owner_subject`。
3. 每个 `capability_item` 必须有一个明确的 `owner_archetype`；若 proof 落在 feature slice，仍需有工程级 archetype 兜底。
4. benchmark 不是 proof 的机械副产物，只在高价值 runtime path 上出现。
5. support-state 不写回 attribute，统一挂在 architecture matrix / reporting / dashboard 层。

## 4. 冻结台账

### 4.1 Core Runtime Semantics

| capability_family | capability_item | owner_subject | owner_archetype | proof_shape | benchmark_policy | support_state_anchor | target_phase |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `CoreRuntimeSemantics` | `TaskAndValueTaskFlow` | `SolutionCorePack` | `MultiProjectSolution` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 2` |
| `CoreRuntimeSemantics` | `IteratorStateMachine` | `SolutionCorePack` | `SimpleLibrarySolution` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 2` |
| `CoreRuntimeSemantics` | `FinallyAndFilter` | `SolutionCorePack` | `SimpleLibrarySolution` | `FeatureSlices/CoreRuntimeFeatures/ExceptionsAndControlFlow` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 2` |
| `CoreRuntimeSemantics` | `TaskScheduling` | `SolutionCorePack` | `MultiProjectSolution` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 2` |
| `CoreRuntimeSemantics` | `MonitorAndLocking` | `SolutionCorePack` | `MultiProjectSolution` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 2` |
| `CoreRuntimeSemantics` | `ThreadLocalState` | `SolutionCorePack` | `MultiProjectSolution` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 2` |
| `CoreRuntimeSemantics` | `RuntimeHelpers` | `SolutionCorePack` | `SimpleLibrarySolution` | `FeatureSlices/CoreRuntimeFeatures/ObjectModelAndDispatch` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 2` |
| `CoreRuntimeSemantics` | `GcSensitiveFlow` | `SolutionCorePack` | `MultiProjectSolution` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 2` |
| `CoreRuntimeSemantics` | `ResourceLifecycle` | `SolutionCorePack` | `SimpleLibrarySolution` | `FeatureSlices/CoreRuntimeFeatures/ExceptionsAndControlFlow` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 2` |
| `CoreRuntimeSemantics` | `LoaderContract` | `SolutionCorePack` | `MixedReferenceClosureSolution` | `Archetypes/* + FeatureSlices/CoreRuntimeFeatures` | `none` | `docs/architecture/solution-archetype-matrix.md` | `Phase 2` |

### 4.2 Low-Level Runtime And Interop

| capability_family | capability_item | owner_subject | owner_archetype | proof_shape | benchmark_policy | support_state_anchor | target_phase |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `LowLevelRuntimeAndInterop` | `SpanAndMemory` | `SolutionCorePack` | `CoreLibReferenceSolution` | `FeatureSlices/CoreRuntimeFeatures/InteropAndMarshaling` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 3` |
| `LowLevelRuntimeAndInterop` | `RefStructAndStackalloc` | `SolutionCorePack` | `CoreLibReferenceSolution` | `FeatureSlices/CoreRuntimeFeatures/InteropAndMarshaling` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 3` |
| `LowLevelRuntimeAndInterop` | `UnsafePointer` | `SolutionCorePack` | `CoreLibReferenceSolution` | `FeatureSlices/CoreRuntimeFeatures/InteropAndMarshaling` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 3` |
| `LowLevelRuntimeAndInterop` | `FunctionPointer` | `SolutionCorePack` | `CoreLibReferenceSolution` | `FeatureSlices/CoreRuntimeFeatures/InteropAndMarshaling` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 3` |
| `LowLevelRuntimeAndInterop` | `NativeCallInterop` | `SolutionCorePack` | `ReferenceAssemblySolution` | `FeatureSlices/CoreRuntimeFeatures/InteropAndMarshaling` | `required` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 3` |
| `LowLevelRuntimeAndInterop` | `StringAndUtf8Marshaling` | `SolutionCorePack` | `ReferenceAssemblySolution` | `FeatureSlices/CoreRuntimeFeatures/InteropAndMarshaling` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 3` |
| `LowLevelRuntimeAndInterop` | `StructMarshaling` | `SolutionCorePack` | `ReferenceAssemblySolution` | `FeatureSlices/CoreRuntimeFeatures/InteropAndMarshaling` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 3` |
| `LowLevelRuntimeAndInterop` | `DelegateCallbackInterop` | `SolutionCorePack` | `ReferenceAssemblySolution` | `FeatureSlices/CoreRuntimeFeatures/ObjectModelAndDispatch` | `required` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 3` |

### 4.3 Metadata And AOT Closure

| capability_family | capability_item | owner_subject | owner_archetype | proof_shape | benchmark_policy | support_state_anchor | target_phase |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `MetadataAndAotClosure` | `CustomAttributeLookup` | `SolutionCorePack` | `CoreLibReferenceSolution` | `FeatureSlices/CoreRuntimeFeatures/ReflectionAndMetadata` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 4` |
| `MetadataAndAotClosure` | `MemberMetadataLookup` | `SolutionCorePack` | `CoreLibReferenceSolution` | `FeatureSlices/CoreRuntimeFeatures/ReflectionAndMetadata` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 4` |
| `MetadataAndAotClosure` | `MetadataClosure` | `SolutionCorePack` | `CoreLibReferenceSolution` | `Archetypes/CoreLibReferenceSolution + ReflectionAndMetadata` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 4` |
| `MetadataAndAotClosure` | `LinkerPreserveContract` | `SolutionCorePack` | `CoreLibReferenceSolution` | `FeatureSlices/CoreRuntimeFeatures/ReflectionAndMetadata` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 4` |
| `MetadataAndAotClosure` | `LinkerStripping` | `SolutionCorePack` | `CoreLibReferenceSolution` | `FeatureSlices/CoreRuntimeFeatures/ReflectionAndMetadata` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 4` |
| `MetadataAndAotClosure` | `RequiredInstantiationClosure` | `SolutionCorePack` | `MixedReferenceClosureSolution` | `Archetypes/MixedReferenceClosureSolution + GenericsAndCollections` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 4` |
| `MetadataAndAotClosure` | `GenericSharingBoundary` | `SolutionCorePack` | `MixedReferenceClosureSolution` | `FeatureSlices/CoreRuntimeFeatures/GenericsAndCollections` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 4` |
| `MetadataAndAotClosure` | `AotClosureValidation` | `SolutionCorePack` | `CoreLibReferenceSolution` | `Archetypes/CoreLibReferenceSolution` | `none` | `docs/architecture/solution-archetype-matrix.md` | `Phase 4` |

### 4.4 Full Solution And Reference Closure

| capability_family | capability_item | owner_subject | owner_archetype | proof_shape | benchmark_policy | support_state_anchor | target_phase |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `FullSolutionClosure` | `ProjectReferenceGraph` | `SolutionCorePack` | `MultiProjectSolution` | `Archetypes/MultiProjectSolution` | `none` | `docs/architecture/solution-archetype-matrix.md` | `Phase 5` |
| `FullSolutionClosure` | `PackageReferenceGraph` | `SolutionCorePack` | `PackageReferenceSolution` | `Archetypes/PackageReferenceSolution` | `none` | `docs/architecture/solution-archetype-matrix.md` | `Phase 5` |
| `FullSolutionClosure` | `ReferenceAssemblyGraph` | `SolutionCorePack` | `ReferenceAssemblySolution` | `Archetypes/ReferenceAssemblySolution` | `none` | `docs/architecture/reference-assembly-and-corelib-policy.md` | `Phase 5` |
| `FullSolutionClosure` | `CoreLibReferenceGraph` | `SolutionCorePack` | `CoreLibReferenceSolution` | `Archetypes/CoreLibReferenceSolution` | `none` | `docs/architecture/reference-assembly-and-corelib-policy.md` | `Phase 5` |
| `FullSolutionClosure` | `MixedReferenceClosureGraph` | `SolutionCorePack` | `MixedReferenceClosureSolution` | `Archetypes/MixedReferenceClosureSolution` | `none` | `docs/architecture/solution-archetype-matrix.md` | `Phase 5` |
| `FullSolutionClosure` | `FullProjectHotUpdateGraph` | `HotUpdateHostPack` | `FullProjectHotUpdateSolution` | `Archetypes/FullProjectHotUpdateSolution` | `selective` | `docs/architecture/solution-archetype-matrix.md` | `Phase 5` |

### 4.5 HotUpdate Productization

| capability_family | capability_item | owner_subject | owner_archetype | proof_shape | benchmark_policy | support_state_anchor | target_phase |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `HotUpdateProductization` | `PackageLoad` | `HotUpdateHostPack` | `SkeletonPatchSolution` | `PatchModules + Host` | `required` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |
| `HotUpdateProductization` | `SharedContractBinding` | `HotUpdateHostPack` | `FullProjectHotUpdateSolution` | `SharedContracts + PatchModules` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |
| `HotUpdateProductization` | `MethodReplacement` | `HotUpdateHostPack` | `FullProjectHotUpdateSolution` | `PatchModules` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |
| `HotUpdateProductization` | `MetadataSupplement` | `HotUpdateHostPack` | `FullProjectHotUpdateSolution` | `Archetypes/FullProjectHotUpdateSolution` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |
| `HotUpdateProductization` | `PatchIntegrity` | `HotUpdateHostPack` | `VersionCompatibilitySolution` | `Archetypes/VersionCompatibilitySolution` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |
| `HotUpdateProductization` | `VersionRollback` | `HotUpdateHostPack` | `VersionCompatibilitySolution` | `Archetypes/VersionCompatibilitySolution` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |
| `HotUpdateProductization` | `PatchCallbackFlow` | `HotUpdateHostPack` | `FullProjectHotUpdateSolution` | `Host + PatchModules + SharedContracts` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |
| `HotUpdateProductization` | `ReferenceAssemblyPatchClosure` | `HotUpdateHostPack` | `ReferenceAssemblyPatchSolution` | `Archetypes/ReferenceAssemblyPatchSolution` | `none` | `docs/architecture/reference-assembly-and-corelib-policy.md` | `Phase 6` |

### 4.6 Mixed Execution

| capability_family | capability_item | owner_subject | owner_archetype | proof_shape | benchmark_policy | support_state_anchor | target_phase |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `MixedExecution` | `ManagedNativeBridge` | `MixedExecutionFeaturePack` | `MixedBridgeSolution` | `ManagedBridge` | `required` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |
| `MixedExecution` | `InterpreterLowering` | `MixedExecutionFeaturePack` | `MixedBridgeSolution` | `Lowering` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |
| `MixedExecution` | `InterpreterSemanticParity` | `MixedExecutionFeaturePack` | `MixedBridgeSolution` | `Interpreter` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |
| `MixedExecution` | `CrossBoundaryExceptionBridge` | `MixedExecutionFeaturePack` | `MixedBridgeSolution` | `ManagedBridge + Lowering` | `none` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |
| `MixedExecution` | `GenericBridgeBoundary` | `MixedExecutionFeaturePack` | `MixedBridgeSolution` | `ManagedBridge + Interpreter` | `selective` | `docs/architecture/il2cpp-hotupdate-capability-matrix.md` | `Phase 6` |

## 5. Benchmark Policy Freeze

- `required`
  - 该能力本身就是高价值性能路径，没有 benchmark 就不能判定闭环。
- `selective`
  - 先有 proof；benchmark 只覆盖最有代表性的热路径。
- `none`
  - 只要 correctness 闭环，不强求独立 benchmark。

阈值治理冻结为：

- 默认按 `capability_family + metric` 治理；
- 允许外层 reporting 对个别 case 做显式例外；
- 不允许把阈值直接写入 subject source 或 attribute。

## 6. 接入要求

后续任一 child task 新增 capability 时，必须同步更新：

1. 这份 coverage ledger。
2. 对应的 architecture matrix / archetype docs。
3. 外层 reporting 或 support-state 锚点。

否则视为 capability 未闭环。
