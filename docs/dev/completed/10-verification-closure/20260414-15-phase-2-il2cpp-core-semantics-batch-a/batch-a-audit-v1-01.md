# Phase 2 Batch A Capability Audit v1.01

Date: 2026-04-14 13:13:20 +08:00
Status: completed

## 1. 目标

把 Phase 0 coverage ledger 中的 Batch A 10 个 capability item 映射到 `SolutionCorePack` 当前 retained source，明确：

- 哪些能力点已经有可复用切片；
- 哪些只是“语义部分存在，但 capability 粒度未对齐”；
- 哪些仍然没有 proof 或 benchmark；
- 后续 TDD 的补齐顺序应该怎么切。

## 2. Audit 表

| capability_item | owner slice | 当前现状 | 结论 | 下一步 |
| --- | --- | --- | --- | --- |
| `TaskAndValueTaskFlow` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | 已有 `AsyncAwaitProof.cs`，只覆盖 `Task`，Capability 仍为 `AsyncAwait`，没有 `ValueTask`。 | `partial` | 新增或改造为专用 `TaskAndValueTaskFlow` proof，并补 `ValueTask` 路径。 |
| `IteratorStateMachine` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | 当前未发现 `yield` / iterator state-machine 专用 proof。 | `missing` | 新增 `IteratorStateMachine` proof。 |
| `FinallyAndFilter` | `FeatureSlices/CoreRuntimeFeatures/ExceptionsAndControlFlow` | `CrossBoundaryExceptionProof.cs` 已覆盖 `catch when` 和 nested `finally`，但 Capability 标到 `CrossBoundaryException`。 | `partial` | 抽出独立 `FinallyAndFilter` proof，保留 cross-boundary proof 各自职责。 |
| `TaskScheduling` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | 当前未发现调度、continuation、同步原语切片；`ThreadingProof.cs` 只做单线程锁定。 | `missing` | 新增 `TaskScheduling` proof，后续评估 selective benchmark。 |
| `MonitorAndLocking` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | `ThreadingProof.cs` 已使用 `lock`，但仍是旧的 `Threading` 粒度。 | `partial` | 拆出 `MonitorAndLocking` proof，加入更明确的同步语义断言。 |
| `ThreadLocalState` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` | `ThreadingProof.cs` 已使用 `[ThreadStatic]`，但没有跨线程隔离断言，也未标到新 capability。 | `partial` | 拆出 `ThreadLocalState` proof，并验证线程隔离。 |
| `RuntimeHelpers` | `FeatureSlices/CoreRuntimeFeatures/ObjectModelAndDispatch` | `ObjectOpsProof.cs` 已标注 `RuntimeHelpers`，但当前断言更偏对象/接口基础行为。 | `partial` | 增强为更明确的 helper/intrinsic proof，避免语义漂移。 |
| `GcSensitiveFlow` | `FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading` + `Benchmarks/CoreRuntimeBenchmarks` | 已有 `AllocationBenchmark.cs` 标注为 `GcSensitiveFlow`，但没有对应 proof。 | `partial` | 新增 proof，并保留 benchmark 作为 selective runtime path。 |
| `ResourceLifecycle` | `FeatureSlices/CoreRuntimeFeatures/ExceptionsAndControlFlow` | 当前未发现 `IDisposable` / `using` / 生命周期顺序专用 proof。 | `missing` | 新增 `ResourceLifecycle` proof。 |
| `LoaderContract` | `Archetypes/* + FeatureSlices/CoreRuntimeFeatures` | `Program.cs` 的 `ProofEntry` 已标注 `LoaderContract`，但它更像聚合入口，不是专用 loader 语义 proof。 | `partial` | 补一个明确的 loader contract proof，聚合入口保留 summary 角色。 |

## 3. 现有可复用文件

- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading/AsyncAwaitProof.cs`
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/AsyncAndThreading/ThreadingProof.cs`
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/ExceptionsAndControlFlow/CrossBoundaryExceptionProof.cs`
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/ObjectModelAndDispatch/ObjectOpsProof.cs`
- `subjects/SolutionCorePack/source/FeatureSlices/CoreRuntimeFeatures/Program.cs`
- `subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/MemoryAndAllocation/AllocationBenchmark.cs`

## 4. 建议切片顺序

1. 先处理“已有语义、但 capability 粒度不对”的项：
   - `TaskAndValueTaskFlow`
   - `FinallyAndFilter`
   - `MonitorAndLocking`
   - `ThreadLocalState`
   - `RuntimeHelpers`
   - `LoaderContract`
2. 再补“完全缺 proof”的项：
   - `IteratorStateMachine`
   - `TaskScheduling`
   - `GcSensitiveFlow`
   - `ResourceLifecycle`
3. 最后补 selective benchmark：
   - `TaskAndValueTaskFlow`
   - `TaskScheduling`
   - `MonitorAndLocking`
   - `GcSensitiveFlow`

## 5. 当前判断

- `SolutionCorePack` 已经具备承接 Batch A 的结构，不需要新增 project 或 subject。
- Phase 2 的关键不是再扩结构，而是把已有 proof 从“旧 capability 名称”重组到 Batch A taxonomy。
- 最适合的下一步是先补 metadata/discovery RED，用测试锁定新增 alias / capability item 会稳定进入 declared catalog，然后按切片补 proof。
