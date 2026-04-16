# Phase 0 Owner Evidence Matrix v1.01

Date: 2026-04-16 09:05:23 +08:00
Status: completed

## 1. 规则

- `owner_subject`
  - 纯 AOT family 默认使用 `SolutionCorePack`
- `proof_requirement`
  - `mandatory`: 必须有至少一个 owner proof
  - `optional`: proof 可由更高层 family 共用
- `benchmark_requirement`
  - `mandatory`: 必须存在 benchmark
  - `optional`: 可以仅 proof 收口
  - `not-required`: 不要求 benchmark
- `gate_requirement`
  - 所有 family 最终都必须进入 gate / reporting / release evidence 口径

## 2. Matrix

| family_id | owner_subject | proof_anchors | benchmark_anchors | proof_requirement | benchmark_requirement | gate_requirement | notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `F01` | `SolutionCorePack` | `RuntimeServices/LoaderContractProof.cs`, `InteropAndMarshaling/FunctionPointerProof.cs`, `GenericsAndCollections/GenericInstantiationProof.cs` | `UnsafeAndLowLevel/FunctionPointerBenchmark.cs` | `mandatory` | `optional` | `mandatory` | Loader family 主要靠 proof 与 compatibility 冻结事实，只有 function pointer 等 hot path 需要 benchmark。 |
| `F02` | `SolutionCorePack` | `GenericsAndCollections/GenericLayoutProof.cs`, `ObjectModelAndDispatch/AbiWideningProof.cs`, `RuntimeServices/RuntimeHelpersProof.cs` | `PrimitivesAndOps/IntegerCarrierBenchmark.cs`, `PrimitivesAndOps/FloatingPointCarrierBenchmark.cs` | `mandatory` | `optional` | `mandatory` | IR typed contract 以 proof 为主，carrier 相关可复用现有 benchmark。 |
| `F03` | `SolutionCorePack` | `ObjectModelAndDispatch/ObjectFieldProof.cs`, `ObjectModelAndDispatch/ObjectReturnProof.cs`, `ObjectModelAndDispatch/ArrayCovarianceProof.cs`, `PrimitivesAndOps/ArrayBoxingProof.cs`, `InteropAndMarshaling/RefStructAndStackallocProof.cs`, `InteropAndMarshaling/SpanAndMemoryProof.cs`, `InteropAndMarshaling/UnsafePointerProof.cs` | `UnsafeAndLowLevel/SpanAndMemoryBenchmark.cs`, `UnsafeAndLowLevel/UnsafePointerBenchmark.cs`, `MemoryAndAllocation/AllocationBenchmark.cs` | `mandatory` | `optional` | `mandatory` | object/reference/value flow family 已有丰富 proof，benchmark 只要求性能敏感子面。 |
| `F04` | `SolutionCorePack` | `GenericsAndCollections/GenericSharingBoundaryProof.cs`, `GenericsAndCollections/RequiredInstantiationClosureProof.cs`, `ObjectModelAndDispatch/DispatchProof.cs`, `ObjectModelAndDispatch/InterfaceDispatchProof.cs`, `ObjectModelAndDispatch/VTableDispatchProof.cs`, `GenericsAndCollections/AotClosureValidationProof.cs` | `GenericsAndCollections/GenericBenchmark.cs`, `GenericsAndCollections/GenericSharingBoundaryBenchmark.cs`, `ObjectModelAndDispatch/DispatchBenchmark.cs` | `mandatory` | `mandatory` | `mandatory` | generic/disptach family 是高价值 runtime path，proof 和 benchmark 都必须存在。 |
| `F05` | `SolutionCorePack` | `ObjectModelAndDispatch/AbiWideningProof.cs`, `InteropAndMarshaling/NativeCallInteropProof.cs`, `InteropAndMarshaling/MarshalingProof.cs`, `InteropAndMarshaling/FunctionPointerProof.cs` | `PrimitivesAndOps/IntegerCarrierBenchmark.cs`, `PrimitivesAndOps/FloatingPointCarrierBenchmark.cs`, `InteropAndMarshaling/NativeCallInteropBenchmark.cs`, `InteropAndMarshaling/StringAndUtf8MarshalingBenchmark.cs`, `InteropAndMarshaling/StructMarshalingBenchmark.cs`, `UnsafeAndLowLevel/FunctionPointerBenchmark.cs` | `mandatory` | `mandatory` | `mandatory` | entry ABI / carrier / workload family 直接影响 benchmark dashboard 与 native workload 输出，benchmark 必需。 |
| `F06` | `SolutionCorePack` | `ExceptionsAndControlFlow/ExceptionProof.cs`, `ExceptionsAndControlFlow/FinallyAndFilterProof.cs`, `ExceptionsAndControlFlow/CrossBoundaryExceptionProof.cs`, `ExceptionsAndControlFlow/NestedExceptionProof.cs` | 无固定 owner benchmark | `mandatory` | `not-required` | `mandatory` | EH/control-flow family 以语义正确性为主，默认不要求 benchmark，除非未来出现明确 hot path。 |
| `F07` | `SolutionCorePack` | `ReflectionAndMetadata/MetadataSupplementProof.cs`, `ReflectionAndMetadata/ReflectionInteropClosureProof.cs`, `ReflectionAndMetadata/MemberMetadataLookupProof.cs`, `ReflectionAndMetadata/CustomAttributeLookupProof.cs`, `ReflectionAndMetadata/LinkerPreserveContractProof.cs`, `ReflectionAndMetadata/LinkerStrippingProof.cs` | `ReflectionAndMetadata/MemberMetadataLookupBenchmark.cs` | `mandatory` | `optional` | `mandatory` | 该 family 还要覆盖 reference bundle / full-solution gate；benchmark 只对 metadata lookup 一类成本路径必需。 |

## 3. 非 Owner Subjects

| subject | role | rule |
| --- | --- | --- |
| `HotUpdateHostPack` | supplemental only | 仅在 family 显式涉及热更补丁、supplemental metadata、rollback 等语义时参与，不承接纯 AOT owner。 |
| `MixedExecutionFeaturePack` | supplemental only | 仅在 family 显式涉及 interpreter/AOT 混合 dispatch 时参与，不承接纯 AOT owner。 |

## 4. Gate 对齐要求

- `test_phase5_owner_subject_coverage.py` 与 `test_solution_core_pack_subject.py` 继续作为 owner coverage 基线。
- Phase 5/6 不允许新增 family 却不更新 gate / reporting 口径。
