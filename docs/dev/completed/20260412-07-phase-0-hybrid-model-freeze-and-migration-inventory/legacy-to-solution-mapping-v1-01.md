# Legacy To Solution Mapping v1.01

**日期：** 2026-04-12 17:55 +08:00

## 1. 目标

给出现有 legacy root 的第一版长期归宿建议。

说明：

- 本文件冻结的是 **迁移方向**
- 不是一次性物理搬迁清单
- 个别 root 的最终 canonical 名称仍可能在 Phase 4-6 微调

## 2. archetype / onboarding

| 当前 root | 当前语义 | 长期归宿 |
| --- | --- | --- |
| `GoldenSimpleLib` | golden archetype | `SolutionSimpleLib` |
| `GoldenMultiProject` | golden archetype | `SolutionMultiProject` |
| `GoldenWithPackage` | golden archetype | `SolutionPackageReference` |
| `HelloWorld` | minimal canonical/onboarding | 保留为 onboarding archetype 候选 |
| `HelloWorldObject` | minimal canonical/onboarding | 保留为 onboarding archetype 候选或并入 HelloWorld family |

## 3. mainline family

| 当前 root | 长期归宿建议 |
| --- | --- |
| `ArrayOpsProof` | 并入 `MainlineFeaturePack` family |
| `AsyncAwaitProof` | 并入 `MainlineFeaturePack` family |
| `BitwiseOpsProof` | 并入 `MainlineFeaturePack` family |
| `BranchOpsProof` | 并入 `MainlineFeaturePack` family |
| `ConversionOpsProof` | 并入 `MainlineFeaturePack` family |
| `CrossBoundaryExceptionProof` | 并入 `MainlineFeaturePack` family |
| `DelegateChainProof` | 并入 `MainlineFeaturePack` family |
| `GenericCollectionProof` | 并入 `MainlineFeaturePack` family |
| `InterfaceDispatchProof` | 并入 `MainlineFeaturePack` family |
| `LinkerStrippingProof` | 并入 `MainlineFeaturePack` family 或独立 linker root 候选 |
| `MarshalingProof` | 并入 interop/mainline 交叉 family |
| `NestedExceptionProof` | 并入 `MainlineFeaturePack` family |
| `ObjectOpsProof` | 并入 `MainlineFeaturePack` family |
| `OverflowOpsProof` | 并入 `MainlineFeaturePack` family |
| `ThreadingProof` | 并入 `MainlineFeaturePack` family |
| `VTableDispatchProof` | 并入 `MainlineFeaturePack` family |
| `MainlineFeaturePack` | 保留为 canonical mainline root 候选 |

## 4. engine / interop family

| 当前 root | 长期归宿建议 |
| --- | --- |
| `EngineHostProof` | 保留为 engine-facing canonical root 候选 |
| `EngineLifecycleCallbackLite` | 并入 engine host family |
| `EngineLogWriteLite` | 并入 engine host family |
| `EngineObjectHandleLite` | 并入 engine host family |
| `BridgeRoundtripProof` | 并入 engine/interop family |
| `GenericSupplementProof` | 并入 engine/interop or metadata family |
| `SharedContractProof` | 并入 engine/interop family |
| `HostEmbeddingLite` | 并入 host/engine family |
| `PInvokeLite` | 并入 interop family |
| `ReflectionLite` | 并入 reflection/interop family |
| `CompatibilityMatrixProof` | 从 proof root 改为 engineering compatibility item |

## 5. interpreter / mixed execution family

| 当前 root | 长期归宿建议 |
| --- | --- |
| `InterpreterArithmeticProof` | 并入 execution-model family |
| `InterpreterLoweringProof` | 并入 execution-model family |
| `MixedExecutionProof` | 并入 execution-model family |

## 6. hot-update family

| 当前 root | 长期归宿建议 |
| --- | --- |
| `HotUpdateSkeletonProof` | 保留为 hot-update canonical root 候选 |
| `AutoBridgeProof` | 并入 hot-update family，需补薄 manifest |
| `MethodReplacementProof` | 并入 hot-update family，需补薄 manifest |
| `VersionRollbackProof` | 并入 hot-update family，需补薄 manifest |

## 7. mobile family

| 当前 root | 长期归宿建议 |
| --- | --- |
| `MobileHelloWorldProof` | 保留为 mobile host / package canonical root 候选 |

## 8. benchmark family

| 当前 root | 长期归宿建议 |
| --- | --- |
| `BenchAllocation` | 默认改为 canonical root 上的 benchmark/workload |
| `BenchArithmetic` | 默认改为 canonical root 上的 benchmark/workload |
| `BenchDelegate` | 默认改为 canonical root 上的 benchmark/workload |
| `BenchDispatch` | 默认改为 canonical root 上的 benchmark/workload |
| `BenchException` | 默认改为 canonical root 上的 benchmark/workload |
| `BenchGameLoop` | 默认改为 canonical root 上的 benchmark/workload |
| `BenchGeneric` | 默认改为 canonical root 上的 benchmark/workload |
| `BenchHotUpdateDispatch` | 并入 hot-update family 的 benchmark/workload |
| `BenchHotUpdateLoad` | 并入 hot-update family 的 benchmark/workload |
| `BenchHotUpdateRoundtrip` | 并入 hot-update family 的 benchmark/workload |
| `BenchMixed` | 并入 execution-model family 的 benchmark/workload |
| `GenericEcho` | 默认改为 canonical root 上的 benchmark/workload |

## 9. Phase 0 结论

Phase 0 的 mapping 冻结结论：

- 所有 legacy root 都已给出第一版长期归宿方向
- `golden` 的重命名方向已冻结
- benchmark-like root 默认不再保留为顶层 source taxonomy
- manifestless hot-update root 被明确保留为有效迁移对象，而不是清理噪音
