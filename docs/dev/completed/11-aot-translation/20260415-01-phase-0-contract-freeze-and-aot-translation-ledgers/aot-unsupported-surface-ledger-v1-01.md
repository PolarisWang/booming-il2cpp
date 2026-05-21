# AOT Unsupported Surface Ledger v1.01

Date: 2026-04-15 00:11:06 +08:00
Status: draft-freeze

## 1. 目标

把当前明确未闭环的能力点正式记入台账，避免出现：

- 实际未支持，但没有公开说法；
- 只有局部证据，却被误读成“默认已支持”；
- dashboard / release gate / benchmark 缺数据时无法区分“unsupported”和“missing evidence”。

本台账的 state 与 reason code 词汇均冻结到：

- `supported`
- `partial`
- `unsupported`
- `not-applicable`

reason code 只允许来自：

- `platform_host_gap`
- `toolchain_gap`
- `runtime_gap`
- `metadata_gap`
- `compatibility_gap`
- `policy_blocked`
- `scope_not_targeted`

## 2. 当前 partial / unsupported baseline

| capability item | state | reason code | 受影响模式 | 当前依据 | next action |
| --- | --- | --- | --- | --- | --- |
| `GenericConstraint` | `unsupported` | `toolchain_gap` | `native` | retained subjects 中没有 dedicated proof / benchmark，也没有统一 constraint-binding contract | 在 Phase 5 补 capability slice，并把 constraint 绑定纳入 typed-il / AOT Core IR |
| `GenericVirtualDispatch` | `partial` | `toolchain_gap` | `native` `mixed` | 当前只有非泛型 `InterfaceDispatchProof` / `VTableDispatchProof`，缺 dedicated generic dispatch surface | 在 Phase 5 增加 generic dispatch proof，并在 Phase 4 打通 codegen 支持 |
| `GenericInterfaceDispatch` | `partial` | `toolchain_gap` | `native` `mixed` | 同上，缺 dedicated generic-interface dispatch 闭环 | 同上 |
| `AsyncAwait` | `partial` | `scope_not_targeted` | `native` | 当前保留了 `TaskAndValueTaskFlow` / `IteratorStateMachine`，但没有独立 async/await capability owner 证据 | 在 Phase 5 判断是否补 dedicated proof，或明确继续由 task/iterator item 代理 |
| `Threading` | `unsupported` | `scope_not_targeted` | `native` | 当前 retained proof 聚焦 task scheduling / lock / thread-local，没有显式 thread creation capability item 闭环 | Phase 5 决定是否增加真实 thread proof，或继续排除出主线目标面 |
| `MetadataSupplement` | `partial` | `metadata_gap` | `native` `hot-update` | 现有 hot-update proof 只覆盖样例级 supplement witness，还没有统一 registry / diagnostics contract | 在 Phase 3 建立 `SupplementalMetadataRegistry` 并补 diagnostics |
| `AotClosureValidation` | `partial` | `metadata_gap` | `native` | 已有 `RequiredInstantiationClosure` / `GenericSharingBoundary` 样例，但还没有统一 closure ledger -> gate 闭环 | 在 Phase 4-6 接到 `AotCoreIr`、unsupported report 和 release gate |
| `RuntimeTraceOutput` | `unsupported` | `scope_not_targeted` | `native` `mixed` `hot-update` | 当前没有 retained diagnostics owner source，也没有稳定 trace evidence contract | 在 Phase 6 作为 diagnostics/reporting surface 落地 |
| `NativeSymbolEvidence` | `unsupported` | `scope_not_targeted` | `native` | 当前没有稳定 native symbol capture / compare evidence | 在 Phase 6 与 native generate gate 一并落地 |
| `CrashEvidence` | `unsupported` | `scope_not_targeted` | `native` `mixed` `hot-update` | 当前没有统一 crash evidence 流程 | 在 Phase 6 diagnostics gate 处理 |
| `FailureDiagnostics` | `unsupported` | `scope_not_targeted` | `native` `mixed` `hot-update` | 仍缺统一 failure diagnostics contract | 在 Phase 6 reporting / release gate 处理 |
| `PackageLoad` | `partial` | `runtime_gap` | `interpreter` `mixed` | `HotUpdateLoadBenchmark` 与 skeleton proof 仅证明样例路径，不是统一 runtime foundation | 在 Phase 3 建立 `HotUpdateAssemblyLoader` |
| `SharedContractBinding` | `partial` | `runtime_gap` | `interpreter` `mixed` | `SharedContractProofEntry` 已存在，但 host/patch identity 和 dispatch 还未统一 | 在 Phase 2-3 打通 shared identity 和 dispatch |
| `MethodReplacement` | `partial` | `runtime_gap` | `interpreter` `mixed` | proof entry 已存在，但还没有明确 bridge/thunk 与替换策略 contract | 在 Phase 3 明确 compile-time first 的 bridge/thunk 策略 |
| `HotUpdateMetadataSupplement` | `partial` | `metadata_gap` | `interpreter` `mixed` | 有 proof 样例，无 registry / query / diagnostics 闭环 | 同 `MetadataSupplement` |
| `PatchIntegrity` | `partial` | `metadata_gap` | `interpreter` `mixed` | 缺统一 package integrity 与 evidence contract | 在 Phase 3-6 接入 metadata/reporting 体系 |
| `VersionRollback` | `partial` | `compatibility_gap` | `interpreter` `mixed` | 有 proof entry，但 compatibility band / rollback gate 还没有固定到 release 证据链 | 在 Phase 6 接入 release gate |
| `PatchCallbackFlow` | `partial` | `runtime_gap` | `interpreter` `mixed` | host -> patch / patch -> host 的 callback flow 还没有统一 dispatch foundation | 在 Phase 2-3 与 hybrid dispatch 一并处理 |
| `MixedBridgeDispatch` | `partial` | `runtime_gap` | `native` `interpreter` | MixedExecutionFeaturePack 已有 proof / benchmark，但 shared dispatch contract 还未从 runtime 侧固定 | 在 Phase 2 建立 `HybridDispatchResolver` |
| `InterpreterLowering` | `partial` | `toolchain_gap` | `interpreter` | 现有 lowering proof 仍未升级为母 roadmap 定义的 shared `typed-il -> interpreter IR` contract | 在 Phase 2 冻结 thin interpreter IR |
| `InterpreterArithmetic` | `partial` | `runtime_gap` | `interpreter` | 当前只有样例级 interpreter arithmetic witness | 在 Phase 2-5 纳入 shared contract 与 capability owner |
| `MixedGenericFlow` | `partial` | `runtime_gap` | `native` `interpreter` | proof entry 已有，但 generic/sharing contract 仍未统一 | 在 Phase 2 先冻结 generic/sharing contract |
| `MixedExceptionFlow` | `partial` | `runtime_gap` | `native` `interpreter` | proof entry 已有，但 mixed boundary exception dispatch 尚未统一 | 在 Phase 2-3 接入 shared dispatch |
| `MixedDelegateFlow` | `partial` | `runtime_gap` | `native` `interpreter` | proof entry 已有，但 mixed delegate/callback 边界仍属样例级 | 在 Phase 2-3 接入 bridge/thunk 基线 |

## 3. 使用规则

1. 本台账里的 `partial` / `unsupported` 不是永久结论，而是 Phase 0 基线。
2. 后续 child task 关闭某项缺口时，必须同步更新这里，而不是只改代码或测试。
3. dashboard 遇到无数据时，必须先判断：
  - 是否在本台账里被标记为 `unsupported` / `partial`；
  - 若不在，则应归类为 `missing evidence`，而不是自动降成 `unsupported`。

## 4. 阶段出口

当且仅当以下条件满足时，对应条目才能移出本台账：

- 有明确 owner subject / archetype。
- 有稳定 proof 证据。
- 若属于高价值 runtime path，则有 benchmark。
- 外层 reporting / gate 能正确消费对应 state，不再误报。
