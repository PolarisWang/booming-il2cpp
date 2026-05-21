# Legacy To Retained Mapping v1.01

Date: 2026-04-13 08:57:38 +08:00

## 1. 目标

给出方案 A 下当前 28 个顶层 subject 的长期归宿。

本文件冻结的是：

- 迁移目标
- 迁移形式
- 为什么不再保留原 root

## 2. Mapping

| 当前 subject | 当前形态 | 目标 retained subject | 目标形式 | 备注 |
| --- | --- | --- | --- | --- |
| `AutoBridgeProof` | manifestless hot-update proof | `HotUpdateHostPack` | `ChaosUnitTest` | 先补薄 manifest 过渡，再并回 hot-update proof |
| `BenchDelegate` | benchmark root | `SolutionCorePack` | `ChaosBenchmark` | 委托调度类 runtime benchmark |
| `BenchException` | benchmark root | `SolutionCorePack` | `ChaosBenchmark` | 异常流 benchmark |
| `BenchGameLoop` | benchmark root | `SolutionCorePack` | `ChaosBenchmark` | 主循环类 benchmark |
| `BridgeRoundtripProof` | bridge proof root | `SolutionCorePack` | `ChaosUnitTest` | bridge / interop slice |
| `CompatibilityMatrixProof` | compatibility root | `SolutionCorePack` | engineering validation | 不是长期独立 proof root |
| `EngineHostProof` | engine host proof root | `SolutionCorePack` | engineering validation + `ChaosUnitTest` | 以 host/engine slice 承接 |
| `EngineLifecycleCallbackLite` | engine slice | `SolutionCorePack` | `ChaosUnitTest` | engine host slice |
| `EngineLogWriteLite` | engine slice | `SolutionCorePack` | `ChaosUnitTest` | engine host slice |
| `EngineObjectHandleLite` | engine slice | `SolutionCorePack` | `ChaosUnitTest` | engine host slice |
| `GenericEcho` | benchmark / backend 边界项 | `SolutionCorePack` | `ChaosBenchmark` 或 engineering workload | 实施阶段逐条重判 |
| `GenericSupplementProof` | generic / metadata proof | `SolutionCorePack` | `ChaosUnitTest` | metadata slice |
| `HelloWorld` | onboarding root | `SolutionCorePack` | `ChaosUnitTest` | onboarding / smoke slice |
| `HelloWorldObject` | onboarding root | `SolutionCorePack` | `ChaosUnitTest` | onboarding / object slice |
| `HostEmbeddingLite` | host embedding slice | `SolutionCorePack` | `ChaosUnitTest` | host/engine slice |
| `HotUpdateHostPack` | retained 基础候选 | `HotUpdateHostPack` | retained subject | 升级为显式 `.sln` |
| `MainlineFeaturePack` | retained 基础候选 | `SolutionCorePack` | retained slice + `ChaosUnitTest` | 不再作为单独顶层 retained |
| `MethodReplacementProof` | manifestless hot-update proof | `HotUpdateHostPack` | `ChaosUnitTest` | replacement slice |
| `MixedExecutionFeaturePack` | retained 基础候选 | `MixedExecutionFeaturePack` | retained subject | 升级为显式 `.sln` |
| `MobileHelloWorldProof` | mobile root | `SolutionCorePack` | engineering validation + `ChaosUnitTest` | mobile/device slice，不再独立保留 |
| `PerformanceFeaturePack` | retained 基础候选 | `SolutionCorePack` | retained slice + `ChaosBenchmark` | core benchmark project |
| `PInvokeLite` | interop slice | `SolutionCorePack` | `ChaosUnitTest` | interop slice |
| `ReflectionLite` | reflection slice | `SolutionCorePack` | `ChaosUnitTest` | reflection / interop slice |
| `SharedContractProof` | shared contract proof | `HotUpdateHostPack` | `ChaosUnitTest` | hot-update shared contract |
| `SolutionMultiProject` | archetype solution-like root | `SolutionCorePack` | retained slice + engineering validation/workload | archetype slice |
| `SolutionPackageReference` | archetype solution-like root | `SolutionCorePack` | retained slice + engineering validation/workload | archetype slice |
| `SolutionSimpleLib` | archetype solution-like root | `SolutionCorePack` | retained slice + engineering validation/workload | archetype slice |
| `VersionRollbackProof` | manifestless hot-update proof | `HotUpdateHostPack` | `ChaosUnitTest` | rollback slice |

## 3. Phase 0 冻结结论

### 3.1 长期 retained 只剩三类

- `SolutionCorePack`
- `HotUpdateHostPack`
- `MixedExecutionFeaturePack`

### 3.2 其余 root 全部是迁移源

除了上面三类 retained 目标外，其余现有顶层 root 全部只作为：

- 迁移源
- 兼容过渡源
- 待删除对象

### 3.3 compatibility alias 最终清零

对于上表中的非 retained root：

- 迁移期间允许短期 alias
- 但最终阶段必须一次性全部删除

这不是建议，而是方案 A 的强约束。
