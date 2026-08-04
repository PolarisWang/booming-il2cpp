# HybridCLR 对齐的 AOT 与热更开发计划 Roadmap v1.02

Date: 2026-04-15 17:18:00 +08:00
Status: completed

> 本版 roadmap 已完成归档。最终状态以本文件与 `STATUS.md` 为准。

## 1. 目标

继续把这条 roadmap 推进到可真正关闭的状态，最终满足：

- `typed-il -> AotCoreIr -> native` 能承载真实而非最小子集的 AOT translation surface。
- AOT、hot update、mixed execution 共用同一套 identity / body availability / metadata contract。
- owner subjects 上的 proof / benchmark / archetype gate 反映真实能力面，而不是复用历史 gate。
- full-solution、dashboard、unsupported report、release evidence 与新的 AOT 覆盖面对齐。

## 2. 范围边界

本 roadmap 负责：

- 完成 `AotCoreIr` 后续 contract 扩展与 native codegen widening。
- 把 translation surface ledger 中尚未真正落地的能力面映射回 owner subjects / proof / support-state。
- 在新的 AOT coverage 基线之上重做 engineering gate、dashboard 与 unsupported report。

本 roadmap 不负责：

- 一次性补齐全部 C# feature。
- 继续扩大 string 协议驱动的 subject 内部调度面。
- 通过新增临时 subject 逃避 owner-subject 收口。

## 3. 全局硬约束

- `Chaos.TestFramework` 继续作为真实 managed dependency，位于 `src/reference/Chaos.TestFramework`。
- `AotCoreIr`、`NativeAotLoweringPlanner`、`NativeAotEmitter` 不混入测试框架专用逻辑。
- 新增 contract 优先使用紧凑枚举与小载体，不继续扩大 string protocol。
- owner subject proof 统一使用 `ChaosAssertState` + `Assert`，不依赖 `Console.WriteLine` 外置判定。
- Native AOT 生成链固定为 `NativeAotLoweringPlanner -> NativeAotEmitter -> Templates/NativeAot*.scriban`。

## 4. 阶段结果

### Phase 0: Contract Freeze And Capability Ledger

- 状态：completed
- 子任务：`20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers`
- 结果：冻结 reference bundle、能力 ledger、Phase 0 contract 边界。

### Phase 1: Subject Source Reshape And Test Metadata Cutover

- 状态：completed
- 子任务：`20260415-02-phase-1-subject-source-reshape-and-test-metadata-cutover`
- 结果：owner subject 与 `Chaos.TestFramework` metadata 声明主线完成切换。

### Phase 2: Typed-IL Shared Contract And Hybrid Dispatch Foundation

- 状态：completed
- 子任务：`20260415-03-phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation`
- 结果：typed-il shared contract、hybrid dispatch foundation 与统一 identity 基线完成。

### Phase 3: HotUpdate Runtime And Supplemental Metadata Foundation

- 状态：completed
- 子任务：`20260415-04-phase-3-hotupdate-runtime-and-supplemental-metadata-foundation`
- 结果：supplemental metadata、hot update runtime 基础能力与生成链接入主线。

### Phase 4A: AotCoreIr Backbone And Minimal Native Path

- 状态：completed
- 子任务：`20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening`
- 结果：AotCoreIr backbone 与最小 native path 成立。

### Phase 4B: AotCoreIr Object Model And Runtime Surface

- 状态：completed
- 子任务：`20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface`
- 结果：
  - object model / field / array / cast / boxing / runtime-service carrier 正式接入 `AotCoreIr`
  - Native AOT 生成链升级为 `planner + emitter + Scriban`

### Phase 4C: Generic Sharing And Metadata Closure Contract

- 状态：completed
- 子任务：`20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure`
- 结果：
  - generic context / sharing contract / instantiation closure / metadata closure stitching 正式纳入 `AotCoreIr`
  - 基于 Phase 4B 的 planner + emitter 结构继续扩展 Native AOT codegen

### Phase 4D: Exception Dispatch And ABI Widening

- 状态：completed
- 子任务：`20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening`
- 结果：
  - exception / dispatch / ABI widening typed contract 与 planner emission 闭环
  - `callvirt` / hidden `this` ABI / virtual type-id dispatch 接入 Native AOT planner
  - retained `DispatchProofEntry`、`AbiWideningProofEntry` 在新链路闭环

### Phase 5: Feature Coverage Expansion On Owner Subjects

- 状态：completed
- 子任务：`20260415-09-phase-5-feature-coverage-expansion-on-owner-subjects`
- 结果：
  - `capability_coverage.py` 成为 owner subject / support-state / proof-required / benchmark-required 的单一事实源
  - declared discovery 与 compiled catalog freshness 对齐
  - `SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack` 的 retained owner proof 收口
  - diagnostics `47/48/49/50` 冻结为 reporting-owned `proofOptional`

### Phase 6: Engineering Archetype Benchmark And Release Gates

- 状态：completed
- 子任务：`20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates`
- 结果：
  - benchmark dashboard / perf dashboard / unsupported report / release evidence contract 收口到 Phase 5 capability truth
  - `releaseEvidenceSummary` 输出闭环
  - 补齐 `Conv_ovf_u1` 的 loader/native-aot lowering 缺口
  - 更新陈旧的 registry / phase4 / workload bundle 断言
  - 项目级 `python -m pytest -q` 恢复为 `699 passed, 40 skipped`

## 5. 阶段退出标准完成情况

- Phase 4D：已满足。exception / dispatch / ABI widening contract、planner 消费与 Scriban emitter 均已闭环。
- Phase 5：已满足。目标 capability item 不再停留在 gap ledger；owner subject / proof / support-state / unsupported 结论已刷新。
- Phase 6：已满足。engineering gate、dashboard、unsupported report、release evidence 与新 coverage 一致，且通过项目级回归。

## 6. 子任务状态

| task_id | phase | status | owner | summary | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening` | `Phase 4A` | `completed` | `codex` | AotCoreIr backbone + minimal native path | `Phase 3` |
| `20260415-06-phase-4b-aot-core-ir-object-model-and-runtime-surface` | `Phase 4B` | `completed` | `codex` | object model / array / cast / box / runtime-service carrier + planner/emitter/Scriban cutover | `20260415-05` |
| `20260415-07-phase-4c-aot-core-ir-generic-sharing-and-metadata-closure` | `Phase 4C` | `completed` | `codex` | generic / sharing / metadata closure contract | `20260415-06` |
| `20260415-08-phase-4d-aot-core-ir-exception-dispatch-and-abi-widening` | `Phase 4D` | `completed` | `codex` | exception / dispatch / ABI widening 收口 | `20260415-07` |
| `20260415-09-phase-5-feature-coverage-expansion-on-owner-subjects` | `Phase 5` | `completed` | `codex` | retained owner subject coverage 与 proof-optional reporting contract 收口 | `20260415-08` |
| `20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates` | `Phase 6` | `completed` | `codex` | Phase 6 contract、遗留 Phase 4/registry/workload blocker 与项目级回归均已收口；`pytest` 结果为 `699 passed, 40 skipped` | `20260415-09` |

## 7. 验证

- `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py tests/unit/registry/test_declared_registry_matrix_selection.py tests/unit/compatibility/test_declared_metadata_discovery_contract.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/compatibility/test_metadata_closure_proof_subject.py tests/unit/compatibility/test_async_await_proof_subject.py tests/unit/compatibility/test_threading_gc_proof_subject.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_hot_update_productization_gates.py tests/unit/reporting/test_subject_reporting.py tests/unit/selection/test_selectors.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_benchmark_command.py -q`
  - 结果：`85 passed`
- `python -m pytest tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/performance/test_native_aot_workload_entry_bundle.py tests/integration/registry/test_declared_metadata_discovery.py -q`
  - 结果：`14 passed`
- `python -m pytest tests/unit/compatibility -k "phase4" -q`
  - 结果：`66 passed, 162 deselected`
- `python -m pytest -q`
  - 结果：`699 passed, 40 skipped`

## 8. 风险与后续

- 当前无阻塞归档的新风险。
- 后续若继续扩展 typed-il / AOT / hotupdate 能力，仍需坚持：
  - `AotCoreIr -> NativeAotLoweringPlanner -> NativeAotEmitter + Scriban`
  - `capability_coverage.py` 作为 release/reporting/dashboard 的单一真值
  - `Chaos.TestFramework` 与 AOT core / planner / emitter 严格隔离
