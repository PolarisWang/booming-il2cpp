# Legacy Subject Inventory v1.01

**日期：** 2026-04-12 17:55 +08:00

## 1. 目标

本文件用于完成 Phase 0 的第一版 `subjects/` 盘点：

- 识别当前顶层 subject root 的真实语义
- 区分 canonical solution root、proof-like root、benchmark-like root、异常 root
- 为后续 `legacy-to-solution-mapping` 提供输入

## 2. 当前结构概览

当前 `subjects/` 顶层 root 可分为以下几类：

### 2.1 archetype / golden roots

- `subjects/golden/GoldenSimpleLib`
- `subjects/golden/GoldenMultiProject`
- `subjects/golden/GoldenWithPackage`

结论：

- 这三者是第一批工程 archetype subject 的历史来源
- 长期不再保留 `golden` 作为 canonical bucket

### 2.2 benchmark-like roots

- `BenchAllocation`
- `BenchArithmetic`
- `BenchDelegate`
- `BenchDispatch`
- `BenchException`
- `BenchGameLoop`
- `BenchGeneric`
- `BenchHotUpdateDispatch`
- `BenchHotUpdateLoad`
- `BenchHotUpdateRoundtrip`
- `BenchMixed`
- `GenericEcho`

结论：

- 这类 root 默认不再视为长期顶层 source root
- 需要在后续阶段区分：
  - 哪些是 engineering workload
  - 哪些是 `ChaosBenchmark`
  - 哪些拥有独立 source bundle，值得保留

### 2.3 mainline proof-like roots

- `ArrayOpsProof`
- `AsyncAwaitProof`
- `BitwiseOpsProof`
- `BranchOpsProof`
- `ConversionOpsProof`
- `CrossBoundaryExceptionProof`
- `DelegateChainProof`
- `GenericCollectionProof`
- `InterfaceDispatchProof`
- `LinkerStrippingProof`
- `MarshalingProof`
- `NestedExceptionProof`
- `ObjectOpsProof`
- `OverflowOpsProof`
- `ThreadingProof`
- `VTableDispatchProof`
- `MainlineFeaturePack`

结论：

- 这一组大概率应围绕 `MainlineFeaturePack` 或更少数 canonical solution root 收敛
- proof-like root 不再天然代表长期顶层 source root

### 2.4 engine / interop / diagnostic roots

- `EngineHostProof`
- `EngineLifecycleCallbackLite`
- `EngineLogWriteLite`
- `EngineObjectHandleLite`
- `BridgeRoundtripProof`
- `GenericSupplementProof`
- `SharedContractProof`
- `HostEmbeddingLite`
- `PInvokeLite`
- `ReflectionLite`
- `CompatibilityMatrixProof`

结论：

- 这些 root 多数表达的是 engine-facing / interop-facing capability，不应该长期各自独立膨胀
- 长期应收敛成较少的 canonical solution root，并把验证切片下沉为 engineering item 或 `ChaosUnitTest`

### 2.5 onboarding / minimal canonical roots

- `HelloWorld`
- `HelloWorldObject`

结论：

- 二者具备 archetype / onboarding 候选属性
- 后续需要决定是保留独立 onboarding subject，还是并入更大 canonical solution root

### 2.6 interpreter / mixed execution roots

- `InterpreterArithmeticProof`
- `InterpreterLoweringProof`
- `MixedExecutionProof`

结论：

- 这组 root 表达的是 runtime mode / execution model，而不是长期 source taxonomy
- 后续应归并到更稳定的 canonical solution root

### 2.7 hot-update roots

- `HotUpdateSkeletonProof`
- `AutoBridgeProof`
- `MethodReplacementProof`
- `VersionRollbackProof`

结论：

- 这组 root 明确属于 hot-update 语义域
- 长期应收敛到更少数 hot-update canonical solution root

### 2.8 mobile root

- `MobileHelloWorldProof`

结论：

- 长期更像 mobile host / device package 的 canonical solution root 候选
- 不适合继续作为 proof-like 根目录扩张

## 3. 异常项

以下顶层 root 当前没有 `subject.manifest.json`：

- `AutoBridgeProof`
- `MethodReplacementProof`
- `VersionRollbackProof`

判断：

- 这三者不是噪音目录，也不是应直接删除的残留
- 它们来自 hot-update E2E 线，具备明确业务语义
- 在新模型中应被视为：
  - `hot-update canonical candidate`
  - 或 `hot-update canonical root` 下的 source slice

Phase 0 冻结结论：

- “无 manifest” 不等于“无效 subject”
- 但它们在进入新模型前必须先补齐薄 manifest 或被并入已有 canonical root

## 4. 第一版分类结论

| 分类 | 当前 root | 长期结论 |
| --- | --- | --- |
| archetype | `GoldenSimpleLib` `GoldenMultiProject` `GoldenWithPackage` | 升级为 `Solution*` archetype |
| benchmark-like | `Bench*` `GenericEcho` | 默认不再作为顶层 root 增长 |
| mainline proof-like | 各类 `*Proof` + `MainlineFeaturePack` | 向更少数 canonical mainline root 收敛 |
| engine/interop | `Engine*` `*Lite` `*Contract*` `*Supplement*` | 向 engine/interop canonical root 收敛 |
| onboarding | `HelloWorld` `HelloWorldObject` | 保留为 onboarding archetype 或并入大 root |
| execution model | `Interpreter*` `MixedExecutionProof` | 从“执行模式 root”转为“canonical root 上的能力切片” |
| hot-update | `HotUpdateSkeletonProof` `AutoBridgeProof` `MethodReplacementProof` `VersionRollbackProof` | 收敛为 hot-update canonical root |
| mobile | `MobileHelloWorldProof` | 收敛为 mobile canonical root |

## 5. Phase 0 结论

Phase 0 对 `subjects/` 的冻结结论是：

- 顶层 `subject` 当前仍被历史 proof/benchmark root 污染
- 但现有 root 已足够支撑第一版迁移盘点
- 下一步不应继续新增 `*Proof` / `Bench*`
- 下一步应按 `legacy-to-solution-mapping-v1-01.md` 明确每个 root 的长期归宿
