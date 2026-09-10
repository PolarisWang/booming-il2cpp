---
task_id: 20260415-09-phase-5-feature-coverage-expansion-on-owner-subjects
title: Phase 5 Feature Coverage Expansion On Owner Subjects
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-15 16:55:00 +08:00
updated_at: 2026-04-15 20:20:00 +08:00
current_dir: docs/dev/completed/20260415-09-phase-5-feature-coverage-expansion-on-owner-subjects
parent_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
source_relation: roadmap-child
active: false
---

## 关键文档

- parent_status: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-02.md`
- phase_0_capability_taxonomy: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/capability-taxonomy-freeze-v1-01.md`
- phase_0_identity_body_availability: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/stable-identity-and-body-availability-v1-01.md`
- phase_0_translation_surface: `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/aot-translation-surface-ledger-v1-01.md`
- brainstorm: `docs/dev/completed/20260415-09-phase-5-feature-coverage-expansion-on-owner-subjects/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260415-09-phase-5-feature-coverage-expansion-on-owner-subjects/design-v1-01.md`
- plan: `docs/dev/completed/20260415-09-phase-5-feature-coverage-expansion-on-owner-subjects/plan-v1-01.md`

## 当前结论

- Phase 5 已完成并归档；本轮把 frozen capability ledger 重新接回 `SolutionCorePack` / `HotUpdateHostPack` / `MixedExecutionFeaturePack`，不再依赖过期 assembly 或隐式 gap 记录。
- tooling / metadata 收口已完成：
  - 新增 `build/toolchains/run/testing/capability_coverage.py` 作为 owner subject / support-state / proof-required / benchmark-required 的单一事实源
  - registry declared item 显式暴露 capability family/item、labels、`ownerSubjectId`、`supportStates`、`proofRequired`、`benchmarkRequired`
  - compiled catalog 会在 assembly 过期时自动重建，避免 declared discovery 假绿
- owner proof 第二批已闭环：
  - `SolutionCorePack` 新增 `MetadataSupplement` 与 `AotClosureValidation`
  - `tests/unit/compatibility/test_metadata_closure_proof_subject.py` 锁定真实 proof 源文件与 declared discovery
  - `tests/unit/compatibility/test_phase5_owner_subject_coverage.py` 显式冻结 diagnostics `47/48/49/50` 为 reporting-owned `proofOptional` contract
- 当前 Phase 5 的最终结论是：
  - retained owner subjects 的本轮目标 capability items 已全部有 owner/support-state truth
  - `RuntimeTraceOutput` / `NativeSymbolEvidence` / `CrashEvidence` / `FailureDiagnostics` 不再视为 subject proof gap，而是带 `proofRequired = false` 的 reporting/evidence-chain contract，转入 Phase 6 消费

## 最近摘要

- 2026-04-15 16:55:00 +08:00: 创建 `20260415-09`，承接 parent roadmap 的 Phase 5。
- 2026-04-15 16:55:00 +08:00: 初步审计确认 current owner subjects 已能提供 declared proofs/benchmarks，但 registry 元数据与 discovery freshness 仍未收口。
- 2026-04-15 18:48:00 +08:00: Phase 5 RED tests 转绿；新增 capability coverage 聚合、registry owner/support-state metadata 和 stale assembly rebuild。
- 2026-04-15 18:59:00 +08:00: 真实仓库审计确认 `SolutionCorePack` 正常 discovery 已包含 item `67/68`；registry 已能正确暴露 interop item 的 owner/support-state。
- 2026-04-15 19:12:00 +08:00: 缺口盘点完成；当前缺失 item 为 `SolutionCorePack: 8/9/14/19/20/23/35/47/48/49/50/53`，`HotUpdateHostPack: 58`，`MixedExecutionFeaturePack: 0`。
- 2026-04-15 19:34:00 +08:00: 第一批 owner proof 补洞完成；真实 discovery 已把缺口收缩到 `SolutionCorePack: 23/47/48/49/50/53`，`HotUpdateHostPack: 0`，`MixedExecutionFeaturePack: 0`。
- 2026-04-15 20:20:00 +08:00: 第二批 owner proof 补洞完成；`SolutionCorePack` 新增 `MetadataSupplement` 与 `AotClosureValidation`，diagnostics `47/48/49/50` 明确冻结为 Phase 6 reporting/evidence-chain contract，`20260415-09` 达到退出标准并归档。

## 下一步

- next_action: 返回父 roadmap，创建并激活 `20260415-10-phase-6-engineering-archetype-benchmark-and-release-gates`
- owner: codex
- trigger: retained owner subject coverage 已完成收口，主线切入 Phase 6

## 风险 / 阻塞

### risks

- 如果继续使用过期 assembly 结果，Phase 5 的 owner coverage 会出现假绿。
- 更大范围的 all-item benchmark/proof 对齐仍存在历史不一致，后续 Phase 6 需要决定哪些纳入新的 engineering gate，哪些继续保持 deferred。
- diagnostics 四项虽然已冻结为 proof-optional contract，但 dashboard / unsupported report / release evidence 还没有完全消费这套新真值。
- `tests/fixtures/registry/modules/**` 仍有历史 missing-member 错误，但属于既有 registry fixture 噪声，不阻塞本轮 Phase 5 tooling 收口。

### blockers

- 当前无外部 blocker。

## 验证

- `python -m pytest tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/registry/test_declared_registry_matrix_selection.py tests/unit/compatibility/test_compiled_subject_catalog.py -q`
  - 结果：`13 passed`
- `python -m pytest tests/unit/compatibility/test_declared_metadata_discovery_contract.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/reporting/test_subject_reporting.py tests/unit/selection/test_selectors.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_benchmark_command.py -q`
  - 结果：`35 passed`
- `python -m pytest tests/unit/compatibility/test_metadata_closure_proof_subject.py -q`
  - 结果：`2 passed`
- `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py tests/unit/registry/test_declared_registry_matrix_selection.py tests/unit/compatibility/test_declared_metadata_discovery_contract.py tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_phase5_owner_subject_coverage.py tests/unit/compatibility/test_metadata_closure_proof_subject.py tests/unit/compatibility/test_async_await_proof_subject.py tests/unit/compatibility/test_threading_gc_proof_subject.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/reporting/test_subject_reporting.py tests/unit/selection/test_selectors.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_benchmark_command.py -q`
  - 结果：`70 passed`
- 真实仓库审计：
  - `SolutionCorePack`：`56` unit tests / `17` benchmarks
  - `HotUpdateHostPack`：`6` unit tests / `3` benchmarks
  - `MixedExecutionFeaturePack`：`6` unit tests / `2` benchmarks
  - registry probe：`NativeCallInterop` 已暴露 `ownerSubjectId=SolutionCorePack`、`supportStates=[1,5]`
  - Phase 5 目标项收口：
    - `SolutionCorePack`：`MetadataSupplement`、`AotClosureValidation` 已进入 declared coverage
    - `SolutionCorePack`：`RuntimeTraceOutput`、`NativeSymbolEvidence`、`CrashEvidence`、`FailureDiagnostics` 已冻结为 `proofRequired = false` reporting contract
    - `HotUpdateHostPack`：当前 owner items 已全部有 declared coverage
    - `MixedExecutionFeaturePack`：当前 owner items 已全部有 declared coverage
- `python -m pytest tests/unit/compatibility/test_async_await_proof_subject.py tests/unit/compatibility/test_threading_gc_proof_subject.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_hot_update_skeleton_subject.py -q`
  - 结果：`20 passed`

## wiki

- 当前先只在 `docs/dev` 维护执行状态。
- 等 Phase 5 把 owner/support-state contract 固化后，再判断是否沉淀到长期 wiki。
