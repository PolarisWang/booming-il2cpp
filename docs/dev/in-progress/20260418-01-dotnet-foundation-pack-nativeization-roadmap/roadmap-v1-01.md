# DotNet Foundation Pack Nativeization Roadmap v1.01

Date: 2026-04-18 01:19:47 +08:00
Status: in-progress

## 1. 目标

建立一条新的 `framework-pack-driven` nativeization 主线，以官方 `.NET 8 / .NET 10` 的 `Microsoft.NETCore.App.Ref + Microsoft.NETCore.App` 为输入，按单 DLL 认证方式完成基础框架程序集的逐层 native 化，并把 hotupdate 降为消费者验证层。

本 roadmap 的成功标准不是“多跑几条 proof”，而是：

- 单 DLL 的全部公开 API / IL 语义都必须可 native 化
- 单 DLL 认证结果可审计、可追溯
- 共享 proof / benchmark / engineering gate 证据必须可回填为逐 DLL `full certified` 结论
- 每个阶段都有明确的 native proof / benchmark / monitor signals
- `.NET 8` 与 `.NET 10` 保持同构执行路径

## 2. 范围边界

本 roadmap 负责：

- `Microsoft.NETCore.App` 的 ref/runtime packs 目录化与分类
- foundation DLL 的单 DLL 认证模型
- per-assembly native proof / conformance / benchmark 路线
- `DotNetFoundationPack` owner 设计
- hotupdate 作为 foundation 消费者验证的接线方式

本 roadmap 不负责：

- 第一阶段接入 `Microsoft.AspNetCore.App`
- 第一阶段接入 `Microsoft.WindowsDesktop.App`
- 把仓库业务 DLL 作为底层认证对象
- 在父 roadmap 内直接完成所有实现

## 3. 非目标

- 不继续沿用 `entry-driven closure` 作为基础框架能力发现入口
- 不用 `SolutionCorePack` proof 结果替代单 DLL completion
- 不把 facade/shim 与 implementation 混为同一类完成标准
- 不把 hotupdate 直接并入 foundation 认证阶段
- 不接受“绝大多数公开 API 可用”这种非完整完成口径

## 4. 阶段列表

### Framework Pack Catalog And Asset Policy

- goal:
  - 固定 `.NET 8` 与 `.NET 10` foundation 资产来源
  - 建立 `framework catalog / pack diff / asset policy`
- exit_criteria:
  - `framework-pack` source model 明确
  - `.NET 8` 与 `.NET 10` 的 ref/runtime 目录模型可重现
  - `.NET 10` 资产策略固定为“仓库冻结快照”或“外部预装”其一
- deliverables:
  - `framework-catalog.json`
  - `asset-source-policy.md`
  - `pack-diff-summary.json`
  - `phase-0-native-proof-and-benchmark-matrix.md`
- status:
  - completed

### Contract Facade And Shim Certification Lane

- goal:
  - 收口 `mscorlib.dll`、`netstandard.dll`、`System.Runtime.dll`、`System.dll`、`System.Core.dll` 及其 facade/shim 的公开 surface 认证
- exit_criteria:
  - facade/shim 分类规则稳定
  - type forward / canonical owner 映射完整
  - contract-only DLL 的 completion gate 稳定
- deliverables:
  - `assembly-classification.json`
  - `public-surface-manifest.json`
  - `surface-proof-report.json`
  - `native-binding-proof-report.json`
  - `manifest-benchmark.json`
- status:
  - completed

### System.Private.CoreLib Certification Substrate

- goal:
  - 搭建 `System.Private.CoreLib` 整 DLL nativeization 所需的最低认证底座
- exit_criteria:
  - `System.Private.CoreLib` 公开 surface 全量枚举
  - semantic ledger 结构、helper contract、runtime-backed 分类规则稳定
  - corelib 不再依赖 entry-driven proof 发现能力缺口
- deliverables:
  - `assembly-semantic-ir/System.Private.CoreLib.json`
  - `runtime-helper-contracts.json`
  - `corelib-surface-proof-report.json`
  - `corelib-semantic-proof-report.json`
- status:
  - completed

### System.Private.CoreLib Semantic Family Nativeization

- goal:
  - 按语义族完成 `System.Private.CoreLib` 全公开 API / IL 语义 native 化
- exit_criteria:
  - `System.Private.CoreLib` 达到单 DLL completed 标准
  - 无未分类公开成员
  - 无未实现 runtime-backed 公开契约
- deliverables:
  - corelib family batch child tasks
  - per-family `native-proof-report.json`
  - per-family `conformance-report.json`
  - per-family `semantic-family-benchmark.json`
- status:
  - completed

### Dependency-Driven Core BCL Layer Nativeization

- goal:
  - 基于 dependency graph 自动分层，逐层完成核心 `System.*` DLL 认证
- exit_criteria:
  - 当前层内每个 DLL 均产出 machine-readable `assembly-certification-report` 且 `finalStatus = ok`
  - 当前层内全部 DLL 均已 certified
  - 下一层所依赖的 foundation DLL 已无 blocker
- staged_acceptance:
  - `gameplay-core-ready`
    - goal:
      - 证明当前 foundation DLL 已足以支撑基础游戏业务逻辑，不必等待整个 `core-bcl-layer-01` 全量收口后才给出阶段结论
    - scope:
      - 来自 `20260418-10-core-bcl-layer-01-priority-batch-execution` 的 `8` 个基线 DLL:
        - `System.Collections`
        - `System.Collections.Concurrent`
        - `System.Collections.NonGeneric`
        - `System.Memory`
        - `System.Threading`
        - `System.Threading.Channels`
        - `System.Threading.Tasks.Dataflow`
        - `System.Console`
      - 当前 dependency-layer 核心补齐 `7` 个 DLL:
        - `System.ComponentModel`
        - `System.Runtime.Numerics`
        - `System.Text.Encodings.Web`
        - `System.Text.Encoding.CodePages`
        - `System.IO.Compression`
        - `System.Reflection.TypeExtensions`
        - `System.Runtime.Serialization.Primitives`
    - acceptance:
      - 上述 `15` 个 DLL 都必须存在各自的 `assembly-certification-report/<assembly>.json`
      - 上述 `15` 个 DLL 都必须 `finalStatus = ok`
      - 每个 DLL 都必须同时满足“完整 native 化四层判定”：`surface complete -> semantic complete -> native executable evidence complete -> per-DLL certified complete`
    - note:
      - `gameplay-core-ready` 只是 `dependency-layer` 内的阶段性收口目标，不等于 `core-bcl-layer-01 completed`
- deliverables:
  - `layer-plan.json`
  - `assembly-nativeization-plan/<assembly>.json`
  - `assembly-certification-report/<assembly>.json`
  - `layer-certification-summary.json`
  - per-layer `native-proof-summary.json`
  - per-layer `nativeization-throughput-benchmark.json`
- status:
  - in-progress

### Complex BCL And Common External DLL Lane

- goal:
  - 收口复杂 BCL 家族，并验证常用外部 DLL 可建立在已认证 foundation 之上
- exit_criteria:
  - 选定的复杂 BCL DLL 达到单 DLL certified
  - 选定的常用外部 DLL 达到单 DLL certified
- deliverables:
  - `complex-bcl-certification-matrix.json`
  - external DLL `conformance-report.json`
  - external DLL `benchmark-report.json`
- status:
  - planned

### HotUpdate Consumer Validation

- goal:
  - 证明已认证 foundation DLL 可被 hotupdate host/patch 正确消费
- exit_criteria:
  - bind/invoke/semantic 三档 hotupdate proof 可执行
  - hotupdate 不再承担 foundation 能力发现职责
- deliverables:
  - `hotupdate-consumer-binding-proof-report.json`
  - `hotupdate-consumer-semantic-proof-report.json`
  - `consumer-overhead-benchmark.json`
- status:
  - planned

### .NET 10 Delta Closure And Release Gates

- goal:
  - 在 `.NET 8` 已稳定后，完成 `.NET 10` delta 分析与收口
- exit_criteria:
  - `.NET 10` 新增/变化程序集完成 catalog、classification、certification
  - version-diff gate 稳定
- deliverables:
  - `framework-diff/net8-vs-net10/*.json`
  - `.NET 10` `native-proof-summary.json`
  - `.NET 8 vs .NET 10` regression benchmark
- status:
  - planned

## 5. 每阶段可监控的 Native Proof / Benchmark

| phase | primary native proof | primary benchmark | monitor signals |
| --- | --- | --- | --- |
| `catalog-baseline` | pack asset integrity proof | none or catalog build time | pack count, hash count, version diff count |
| `contract-lane` | facade/shim native binding proof | facade manifest generation time | assemblies classified, public surface mapped |
| `corelib-substrate` | corelib surface + semantic proof | corelib ledger build time | public members total, semantic coverage |
| `semantic-family` | corelib per-family native proof + conformance | string/array/generic/dispatch/reflection family benchmark | runtime-backed implemented, native pass rate |
| `dependency-layer` | per-layer DLL native proof + per-assembly certification closeout | nativeization throughput + layer compile time | certified assemblies, layer completion rate, assembly certification pass rate |
| `complex-bcl-and-external` | complex DLL / external DLL conformance proof | representative external DLL benchmark | external pass rate, performance delta |
| `hotupdate-consumer` | hotupdate bind/invoke/semantic proof | patch load/bind/call overhead benchmark | hotupdate pass rate, overhead trend |
| `net10-delta` | `.NET 10` delta proof | net8 vs net10 regression benchmark | delta resolved count, cross-version regressions |

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260418-02-phase-0-framework-pack-catalog-and-asset-policy` | `catalog-baseline` | `completed` | `codex` | 固定 `.NET 8/.NET 10` pack 资产来源、catalog 与快照策略 | `20260418-01` |
| `20260418-03-phase-1-contract-facade-and-shim-certification-lane` | `contract-lane` | `completed` | `codex` | 收口 facade/shim 公开 surface 与 canonical owner 映射 | `20260418-02` |
| `20260418-04-phase-2-system-private-corelib-certification-substrate` | `corelib-substrate` | `completed` | `codex` | 搭建 corelib 整 DLL 认证底座与 helper contract | `20260418-03` |
| `20260418-05-phase-3-system-private-corelib-semantic-family-nativeization` | `semantic-family` | `completed` | `codex` | 已完成首批 corelib semantic-family execution entry 与 proof / conformance / benchmark 实跑闭环 | `20260418-04` |
| `20260418-06-dependency-driven-core-bcl-layer-nativeization` | `dependency-layer` | `completed` | `codex` | 已冻结 dependency-layer authority、layer plan、proof/benchmark lane 与 per-assembly execution plan | `20260418-05-phase-3-system-private-corelib-semantic-family-nativeization` |
| `20260418-10-core-bcl-layer-01-priority-batch-execution` | `dependency-layer` | `completed` | `codex` | 已完成 `System.Collections` / `System.Memory` / `System.Threading` / `System.Console` representative batch 的真实 proof / benchmark / engineering gate 闭环 | `20260418-06-dependency-driven-core-bcl-layer-nativeization` |
| `20260418-11-core-bcl-layer-01-remaining-ready-dll-execution` | `dependency-layer` | `in-progress` | `codex` | 执行剩余 `21` 个 ready DLL 的 formal objects，并把共享 run evidence 回填为逐 DLL certification closeout | `20260418-10-core-bcl-layer-01-priority-batch-execution` |
| `20260418-07-complex-bcl-and-common-external-dll-lane` | `complex-bcl-and-external` | `planned` | `codex` | 收口复杂 BCL 与常用外部 DLL | `20260418-06-dependency-driven-core-bcl-layer-nativeization` |
| `20260418-08-hotupdate-consumer-validation` | `hotupdate-consumer` | `planned` | `codex` | 用 hotupdate host/patch 验证已认证 foundation 底座 | `20260418-06-dependency-driven-core-bcl-layer-nativeization` |
| `20260418-09-dotnet10-delta-closure-and-release-gates` | `net10-delta` | `planned` | `codex` | 完成 `.NET 10` delta 闭环与跨版本 gate | `20260418-06-dependency-driven-core-bcl-layer-nativeization` |

## 7. 依赖

- 官方 `.NET 8` `Microsoft.NETCore.App.Ref` / `Microsoft.NETCore.App`
- 官方 `.NET 10` `Microsoft.NETCore.App.Ref` / `Microsoft.NETCore.App`
- `docs/architecture/managed-native-hotupdate-test-pipeline.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- 现有 `Loader / Contracts / CodeGen / NativeAotEmitter / subject` 主线实现

## 8. 风险

- 如果 `.NET 10` 资产策略不先固定，后续 catalog 与 CI 可重复性会漂移
- 如果 facade/shim 不先拆开，后续 DLL 认证会反复误判
- 如果 `System.Private.CoreLib` 仍按 proof 驱动补洞，会导致 semantic-family 主线失控
- dynamic-code-heavy DLL 可能需要新 runtime service，若不提前承认，会产生伪完成
- hotupdate 若过早进入主线，会重新污染 foundation contract

## 9. 备选路径

### 备选路径 A：继续 entry-driven 主线，只新增更多基础 proof

- 不推荐
- 原因：无法证明单 DLL 完整性

### 备选路径 B：先拿业务 DLL 做整链，再反推 foundation

- 不推荐
- 原因：目标错位，且会把业务语义与 foundation 语义混杂

### 备选路径 C：直接对 `System.Private.CoreLib` 开工，不先做 facade/shim catalog

- 不推荐
- 原因：会在 assembly ownership、type forward、public surface completeness 上反复返工

### 备选路径 D：先做 `framework-pack-driven` 主线，再逐层认证

- 推荐
- 原因：唯一能稳定满足“单 DLL 全部公开 API / IL 语义都必须可 native 化”的路线

## 10. 当前建议推进顺序

1. `20260418-02-phase-0-framework-pack-catalog-and-asset-policy` 已完成并归档。
2. `20260418-03-phase-1-contract-facade-and-shim-certification-lane` 已完成并归档。
3. `20260418-04-phase-2-system-private-corelib-certification-substrate` 已完成并归档。
4. `20260418-05-phase-3-system-private-corelib-semantic-family-nativeization` 已完成并归档。
5. `20260418-06-dependency-driven-core-bcl-layer-nativeization` 已完成并归档
6. `20260418-10-core-bcl-layer-01-priority-batch-execution` 已完成并归档。
7. `20260418-11-core-bcl-layer-01-remaining-ready-dll-execution` 进行中；先把 `gameplay-core-ready` 作为 `dependency-layer` 阶段性验收目标固化下来，再在剩余 `21` 个 ready DLL 上推进 shared execution evidence 与逐 DLL `assembly-certification-report`。
8. 只有当 `gameplay-core-ready` 达成且 layer-01 剩余 DLL 继续完成逐 DLL full-certified closeout 后，再推进更高 layer、复杂 BCL、外部 DLL、hotupdate consumer 与 `.NET 10` delta closure。
