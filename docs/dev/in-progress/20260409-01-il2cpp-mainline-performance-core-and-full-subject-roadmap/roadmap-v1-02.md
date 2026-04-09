# IL2CPP Mainline 性能优先核心与完整 Subject Roadmap v1.02

Date: 2026-04-09
Status: active-roadmap-phase-5-brainstorming

## 1. 目标

本轮 roadmap 的目标不是把当前仓库过早描述成“完整 `C# -> C++` 总方案已经成立”，而是把它收敛成一条边界清晰、可验证、性能导向的主线：

- 当前主线是 `Phase A performance-first core mainline roadmap`。
- 一切核心决策优先服从 generated native 工程的 `C++` 性能上限。
- 在 `Phase A` 内，先把 core contract、完整 subject、capability batch 与性能证据链收口，再处理 `engine binding` 尾阶段接入。

## 2. 范围边界

本 roadmap 负责：

- 冻结当前主线的 `DefinitionOfComplete` 与 `PerformanceGovernance`
- 冻结 performance-first core contract 与 `OptimizationFacts`
- 把 `SemanticWorld -> Linker -> CodeGen -> native proof/runtime` 切到正式 contract
- 维护完整 subject `MainlineFeaturePack`
- 建立 `native output / trace / perf` 的统一验证主线
- 以 `dispatch`、`closed generics/layout`、`arrays/boxing`、`delegates`、`exceptions` 等 capability batch 继续补全
- 把 `engine binding` 放在 roadmap 最后一个尾阶段，不允许提前污染 core mainline

本 roadmap 当前不负责：

- 把当前路线扩写成完整 total solution roadmap
- `.NET 10` 输入兼容
- `macOS parity`
- Android / iOS runtime subset
- 完整高阶 `BCL` 兼容

## 3. 非目标

当前不允许把下面这些表述当成既成事实：

- “当前 roadmap 已经覆盖完整 `C# -> C++` 总方案”
- “当前每一个实现决策都已经被完整证明为 `C++` 性能全局最优”

当前也不做：

- 在 complete 定义仍含糊时继续推进 child implementation
- 在没有 objective function 与 tradeoff order 的情况下泛化“performance-first”
- 把 `exceptions` 再次降级成以后再决定
- 在 `Phase 6` 前提前卷入 `engine binding`

## 4. 阶段列表

### Phase 0: Completeness Definition And Performance Governance

- 冻结“当前主线的 complete 到底是什么意思”
- 冻结“`C++` 性能最优如何裁决、如何举证、允许牺牲什么”
- 明确把 `exceptions` 写成 roadmap 内强制交付项

### Phase 1: `OptimizationFacts` And Native Perf Contracts

- 在 `Phase 0` 约束下冻结 performance-first core contract
- 冻结 `MainlineFeaturePack` 第一版 validation/profile/matrix/perf evidence 边界

### Phase 2: Core Contract Cutover

- 把 `SemanticWorld -> Linker -> CodeGen` 的 sample-driven 暂存逻辑替换为正式 contract 落点
- 在代码层引入 `OptimizationFacts` 的最小闭环

### Phase 3: Capability-Driven Lowering And Arbitration

- 把 lowering family 选择从 emitter 内部样例识别中迁出
- 建立“合法性过滤 + 性能裁决”的 codegen 主线

### Phase 4: `MainlineFeaturePack` Onboarding

- 把独立完整 subject 正式接入仓库主线
- 建立 `output / trace / perf` 三条统一入口
- 当前状态：已完成

### Phase 5: Capability Batch A

- 优先落首批最影响 generated native 质量上限的能力：
  - `dispatch`
  - `closed generics/layout`
  - `arrays/boxing`
- 当前状态：design review 中

### Phase 6: Capability Batch B And Phase A Closure

- 落第二批能力：
  - `delegates/callbacks`
  - `exceptions`
  - 必要的 `reflection/interop` 补位
- 完成 `Phase A` closure，明确剩余缺口与 follow-up

### Phase 7: Engine Binding Tail Roll-In

- 在 core performance-first 主线与完整 subject 稳定后，再把 `engine binding` 正式收口进 roadmap
- 复用既有 `subject + matrix + artifact-chain` 主线，不允许新造平行系统

## 5. 阶段完成定义

### Phase 0

- 完成 `DefinitionOfComplete` 与 `PerformanceGovernance` 冻结
- 明确 `exceptions` 是 roadmap 内交付项

### Phase 1

- `OptimizationFacts`、native perf evidence/report 与 `MainlineFeaturePack` 第一版 contract 冻结

### Phase 2

- `SemanticWorld`、`Linker`、`CodeGen` 已切到 performance-first contract

### Phase 3

- `LoweringFamily` legality filter 生效
- performance arbitration 成为主裁决器
- emitter 不再承担长期 lowering discovery 职责

### Phase 4

- `MainlineFeaturePack` 具备真实 `source / validation / baselines`
- subject-scoped proof build 已落地
- `correctness.dev` / `correctness.platform` / `perf.profile` 三条入口已通过验证

### Phase 5

- `dispatch`、`closed generics/layout`、`arrays/boxing` 在完整 subject 上具备明确 proof slice 与 perf evidence
- 对应能力不再依赖样例特化 emitter 分支
- 可以观察 native perf 的 capability regression

### Phase 6

- `delegates/callbacks`、`exceptions` 与必要 `reflection/interop` 补位完成
- 明确 `Phase A` 剩余缺口及 follow-up

### Phase 7

- `engine binding` 以正式 child task 接入，不再只停留在 discuss 文档
- 继续复用 `subject + matrix + artifact-chain`

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260409-03-phase-0-completeness-definition-and-performance-governance` | `Phase 0` | `completed` | `codex` | 冻结 `DefinitionOfComplete`、`PerformanceGovernance` 与 `exceptions` 的强制交付地位 | `-` |
| `20260409-02-phase-1-optimization-facts-and-native-perf-contracts` | `Phase 1` | `completed` | `codex` | 冻结 `OptimizationFacts`、native perf evidence 与完整 subject 第一版 contract | `20260409-03-phase-0-completeness-definition-and-performance-governance` |
| `20260409-04-phase-2-core-contract-cutover` | `Phase 2` | `completed` | `codex` | 把 performance-first contract 切进 `SemanticWorld -> Linker -> CodeGen` | `20260409-02-phase-1-optimization-facts-and-native-perf-contracts` |
| `20260409-05-phase-3-capability-driven-lowering-and-arbitration` | `Phase 3` | `completed` | `codex` | 迁出 sample-driven lowering 发现逻辑，建立 family + arbitration 主线 | `20260409-04-phase-2-core-contract-cutover` |
| `20260409-06-phase-4-mainline-feature-pack-onboarding` | `Phase 4` | `completed` | `codex` | 接入完整 subject，并贯通 `output / trace / perf` 三条统一主线 | `20260409-05-phase-3-capability-driven-lowering-and-arbitration` |
| `20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing` | `Phase 5` | `in_progress` | `codex` | 为 Batch A 设计 proof slice、entry override 与 capability 补全落点 | `20260409-06-phase-4-mainline-feature-pack-onboarding` |
| `phase-6-capability-batch-b-delegate-exception-and-phase-a-closure` | `Phase 6` | `planned` | `codex` | 落地第二批能力，并完成 `Phase A` closure | `20260409-07-phase-5-capability-batch-a-dispatch-generic-layout-and-array-boxing` |
| `phase-7-engine-binding-tail-roll-in` | `Phase 7` | `planned` | `codex` | 把 `engine binding` 作为尾阶段正式接入成熟主线 | `phase-6-capability-batch-b-delegate-exception-and-phase-a-closure` |

## 7. 依赖顺序

全局依赖顺序固定为：

1. 先冻结 `DefinitionOfComplete` 与 `PerformanceGovernance`
2. 再冻结 `OptimizationFacts` 与 native perf contract
3. 再切进 core stages
4. 再把 lowering arbitration 从 sample-driven emitter 中迁出
5. 再接入完整 subject
6. 再按批次补 capability
7. 再完成 `Phase A` closure
8. 最后收口 `engine binding`

## 8. 当前风险

### 风险 1: performance-first 继续停留在口号层

如果脱离 `Phase 0` 的 objective function 与 tradeoff order，后续 child task 仍会不断把“性能优先”退化成局部判断。

### 风险 2: roadmap 再次被误读成 total solution

如果在 `Phase 6/7` 完成前重新宣称“主线已闭环”，会继续混淆 `Phase A mainline complete` 与 `C# -> C++ total solution complete`。

### 风险 3: `MainlineFeaturePack` 退化成巨型 smoke

如果 `Phase 5` 不采用 capability slice，而是改成 mega proof entry 或重新拆回多个 probe subject，后续 perf regression 与故障定位都会退化。

### 风险 4: `arrays/boxing` 缺口被低估

当前 loader/runtime 还缺 `newarr`、`box` 以及相关 array store/unbox 路径；这是 Batch A 中最容易拖慢实现节奏的部分。

## 9. 当前推进建议

1. `Phase 4` 已完成，不再回退到 planning-only `MainlineFeaturePack`。
2. `Phase 5` 推荐继续沿用“一个完整 subject + 多个 proof slice”的方向，而不是重回多 subject 分裂方案。
3. `Phase 5` 实施前先完成 design review；确认后再写 `plan-v1-01.md` 并进入 TDD/implementation。
