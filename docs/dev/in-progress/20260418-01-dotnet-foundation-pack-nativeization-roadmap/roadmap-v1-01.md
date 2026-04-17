# DotNet Foundation Pack Nativeization Roadmap v1.01

Date: 2026-04-18 01:19:47 +08:00
Status: in-progress

## 1. 目标

建立一条新的 `framework-pack-driven` nativeization 主线，以官方 `.NET 8 / .NET 10` 的 `Microsoft.NETCore.App.Ref + Microsoft.NETCore.App` 为输入，按单 DLL 认证方式完成基础框架程序集的逐层 native 化，并把 hotupdate 降为消费者验证层。

该 roadmap 的成功标准不是“少量 proof 跑通”，而是：

- 单 DLL 全公开 API / IL 语义可 native 化
- 单 DLL 认证结果可审计
- 各阶段有明确 native proof / benchmark / 监控指标
- `.NET 8` 与 `.NET 10` 具备同构执行路径

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

- 不继续沿用 entry-driven closure 作为基础框架能力发现主入口
- 不用 `SolutionCorePack` proof 结果替代单 DLL completion
- 不把 facade/shim 与 implementation 混为同一类完成标准
- 不把 hotupdate 直接并入底层 foundation 认证阶段
- 不接受“绝大多数公开 API 可用”这种非完整完成口径

## 4. 阶段列表

### Phase 0: Framework Pack Catalog And Asset Policy

- goal:
  - 固定 `.NET 8` 与 `.NET 10` foundation 资产来源
  - 建立 `framework catalog / pack diff / asset policy`
- exit_criteria:
  - `framework-pack` source model 明确
  - `.NET 8` 与 `.NET 10` 的 ref/runtime 目录模型可重现
  - `.NET 10` 资产策略固定为“仓库冻结快照”或“外部预装”其中一种
- deliverables:
  - `framework-catalog.json`
  - `asset-source-policy.md`
  - `pack-diff-summary.json`
  - Phase 0 native proof/benchmark matrix
- dependencies:
  - 官方 `.NET 8` packs
  - 官方 `.NET 10` packs 或其冻结快照
- open_questions:
  - `.NET 10` 最终是否采用仓库冻结快照

### Phase 1: Contract Facade And Shim Certification Lane

- goal:
  - 收口 `mscorlib.dll`、`netstandard.dll`、`System.Runtime.dll`、`System.dll`、`System.Core.dll` 与其他 facade/shim 的公开 surface 认证
- exit_criteria:
  - facade/shim 分类规则稳定
  - type forward / canonical owner 映射完整
  - contract-only DLL 的 completion gate 稳定
- deliverables:
  - `assembly-classification.json`
  - `public-surface-manifest.json`
  - facade/shim `surface-proof-report.json`
  - facade/shim `native-binding-proof-report.json`
- dependencies:
  - `Phase 0`
- open_questions:
  - 哪些 facade 允许标记为 contract-complete 但不要求实现性 emit

### Phase 2: System.Private.CoreLib Certification Substrate

- goal:
  - 搭建 `System.Private.CoreLib` 整 DLL native 化所需的最低认证底座
- exit_criteria:
  - `System.Private.CoreLib` 公开 surface 全量枚举
  - semantic ledger 结构、helper contract、runtime-backed 分类规则稳定
  - corelib 不再依赖 entry-driven proof 发现能力缺口
- deliverables:
  - `assembly-semantic-ir/System.Private.CoreLib.json`
  - `runtime-helper-contracts.json`
  - `corelib-surface-proof-report.json`
  - `corelib-semantic-proof-report.json`
- dependencies:
  - `Phase 1`
- open_questions:
  - 是否需要把 corelib 的 native helper contract 切分为更细粒度子族

### Phase 3: System.Private.CoreLib Semantic Family Nativeization

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
- dependencies:
  - `Phase 2`
- open_questions:
  - 是否需要把 `Expressions/Emit` 相关桥接 helper 单独后移

### Phase 4: Dependency-Driven Core BCL Layer Nativeization

- goal:
  - 基于 dependency graph 自动分层，逐层完成核心 `System.*` DLL 认证
- exit_criteria:
  - 当前层内全部 DLL 均已 certified
  - 下一层所依赖的 foundation DLL 已无 blocker
- deliverables:
  - `layer-plan.json`
  - `assembly-nativeization-plan/<assembly>.json`
  - per-layer `native-proof-summary.json`
  - per-layer `nativeization-throughput-benchmark.json`
- dependencies:
  - `Phase 3`
- open_questions:
  - 是否需要单独划出 `System.Net.*` / `System.Security.*` / `System.Xml.*`

### Phase 5: Complex BCL And Common External DLL Lane

- goal:
  - 收口复杂 BCL 家族，并验证常用外部 DLL 可建立在已认证 foundation 之上
- exit_criteria:
  - 选定的复杂 BCL DLL 达到单 DLL certified
  - 选定的常用外部 DLL 达到单 DLL certified
- deliverables:
  - `complex-bcl-certification-matrix.json`
  - external DLL `conformance-report.json`
  - external DLL `benchmark-report.json`
- dependencies:
  - `Phase 4`
- open_questions:
  - 第一批外部 DLL 是仅 `Newtonsoft.Json`，还是再加入 `System.Collections.Immutable` 风格样本

### Phase 6: HotUpdate Consumer Validation

- goal:
  - 证明已认证 foundation DLL 可被 hotupdate host/patch 正确消费
- exit_criteria:
  - bind/invoke/semantic 三档 hotupdate proof 可执行
  - hotupdate 不再承担 foundation 能力发现职责
- deliverables:
  - `hotupdate-consumer-binding-proof-report.json`
  - `hotupdate-consumer-semantic-proof-report.json`
  - `consumer-overhead-benchmark.json`
- dependencies:
  - `Phase 4`
  - `Phase 5`（如需验证外部 DLL 消费）
- open_questions:
  - hotupdate 第一批是否只验证 foundation API 消费，不验证外部 DLL patch 消费

### Phase 7: .NET 10 Delta Closure And Release Gates

- goal:
  - 在 `.NET 8` 已稳定后，完成 `.NET 10` delta 分析与收口
- exit_criteria:
  - `.NET 10` 新增/变化程序集完成 catalog、classification、certification
  - version-diff gate 稳定
- deliverables:
  - `framework-diff/net8-vs-net10/*.json`
  - `.NET 10` `native-proof-summary.json`
  - `.NET 8 vs .NET 10` regression benchmark
- dependencies:
  - `Phase 0`
  - `Phase 4`
- open_questions:
  - `.NET 10` 是否需要与 `.NET 8` 共用同一 certification runner 还是仅共享 contract 层

## 5. 每阶段可监控的 Native Proof / Benchmark

| phase | primary native proof | primary benchmark | monitor signals |
| --- | --- | --- | --- |
| `Phase 0` | pack asset integrity proof | none or catalog build time | pack count, hash count, version diff count |
| `Phase 1` | facade/shim native binding proof | facade manifest generation time | assemblies classified, public surface mapped |
| `Phase 2` | corelib surface + semantic proof | corelib ledger build time | public members total, semantic coverage |
| `Phase 3` | corelib per-family native proof + conformance | string/array/generic/dispatch/reflection family benchmark | runtime-backed implemented, native pass rate |
| `Phase 4` | per-layer DLL native proof | nativeization throughput + layer compile time | certified assemblies, layer completion rate |
| `Phase 5` | complex DLL / external DLL conformance proof | representative external DLL benchmark | external pass rate, performance delta |
| `Phase 6` | hotupdate bind/invoke/semantic proof | patch load/bind/call overhead benchmark | hotupdate pass rate, overhead trend |
| `Phase 7` | `.NET 10` delta proof | net8 vs net10 regression benchmark | delta resolved count, cross-version regressions |

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260418-02-phase-0-framework-pack-catalog-and-asset-policy` | `Phase 0` | `in_progress` | `codex` | 固定 `.NET 8/.NET 10` pack 资产来源、catalog 与快照策略 | `20260418-01` |
| `20260418-03-phase-1-contract-facade-and-shim-certification-lane` | `Phase 1` | `planned` | `codex` | 收口 facade/shim 公开 surface 与 canonical owner 映射 | `20260418-02` |
| `20260418-04-phase-2-system-private-corelib-certification-substrate` | `Phase 2` | `planned` | `codex` | 搭建 corelib 整 DLL 认证底座与 helper contract | `20260418-03` |
| `20260418-05-phase-3-system-private-corelib-semantic-family-nativeization` | `Phase 3` | `planned` | `codex` | 按语义族完成 corelib 单 DLL 全公开 API native 化 | `20260418-04` |
| `20260418-06-phase-4-dependency-driven-core-bcl-layer-nativeization` | `Phase 4` | `planned` | `codex` | 依赖驱动分层推进核心 `System.*` DLL 认证 | `20260418-05` |
| `20260418-07-phase-5-complex-bcl-and-common-external-dll-lane` | `Phase 5` | `planned` | `codex` | 收口复杂 BCL 与常用外部 DLL | `20260418-06` |
| `20260418-08-phase-6-hotupdate-consumer-validation` | `Phase 6` | `planned` | `codex` | 用 hotupdate host/patch 验证已认证 foundation 底座 | `20260418-06` |
| `20260418-09-phase-7-dotnet10-delta-closure-and-release-gates` | `Phase 7` | `planned` | `codex` | 完成 `.NET 10` delta 闭环与跨版本 gate | `20260418-06` |

## 7. 依赖

- 官方 `.NET 8` `Microsoft.NETCore.App.Ref` / `Microsoft.NETCore.App`
- 官方 `.NET 10` `Microsoft.NETCore.App.Ref` / `Microsoft.NETCore.App`
- `docs/architecture/managed-native-hotupdate-test-pipeline.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- 现有 `Loader / Contracts / CodeGen / NativeAotEmitter / subject` 主线实现

## 8. 风险

- 如果 `.NET 10` 资产策略不先固定，后续 catalog 与 CI 可重复性会漂移
- 如果 facade/shim 不先分开，后续 DLL 认证会反复误判
- `System.Private.CoreLib` 若仍按 proof 驱动补洞，会导致阶段 3 失控
- dynamic-code-heavy DLL 可能需要新 runtime service，若不提前承认，会产生伪完成
- hotupdate 若过早进入主线，会重新污染 foundation contract

## 9. 备选路径

### 备选路径 A：继续 entry-driven 主线，只新增更多基础库 proof

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
- 原因：唯一能稳定满足“单 DLL 全公开 API / IL 语义都必须可 native 化”的路线

## 10. 当前建议推进顺序

1. 立即启动 `20260418-02-phase-0-framework-pack-catalog-and-asset-policy`
2. 先固定 `.NET 10` 资产策略，并完成 `.NET 8/.NET 10` pack catalog
3. 再推进 facade/shim lane，冻结 contract-complete 规则
4. 随后单开 corelib 底座与 corelib 语义族路线
5. corelib certified 后，再切入依赖驱动的核心 `System.*` 分层
6. foundation 稳定后，最后推进复杂 BCL、外部 DLL 与 hotupdate 消费者验证
