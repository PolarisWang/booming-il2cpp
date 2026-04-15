---
task_id: 20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
title: HybridCLR 对齐的 AOT 与热更开发计划
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-14 23:34:39 +08:00
updated_at: 2026-04-15 06:42:00 +08:00
current_dir: docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan
parent_task_id:
source_task_id: 20260414-12-il2cpp-core-capability-development-roadmap
source_relation: follow-up-after-core-capability-roadmap
active: false
---

## 关键文档

- brainstorm: docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/brainstorm-v1-01.md
- design: docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/design-v1-01.md
- roadmap_or_plan: docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-01.md
- review: docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/review-v1-01.md
- phase_5_6_reuse_audit: docs/dev/completed/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/phase-5-6-reuse-audit-v1-01.md
- phase_0_status: docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/STATUS.md
- phase_1_status: docs/dev/completed/20260415-02-phase-1-subject-source-reshape-and-test-metadata-cutover/STATUS.md
- phase_2_status: docs/dev/completed/20260415-03-phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation/STATUS.md
- phase_3_status: docs/dev/completed/20260415-04-phase-3-hotupdate-runtime-and-supplemental-metadata-foundation/STATUS.md
- phase_4_status: docs/dev/completed/20260415-05-phase-4-aot-core-ir-and-native-codegen-strengthening/STATUS.md

## 当前结论

- 本 roadmap 已完成并归档。
- 最终收口结果如下：
  - Phase 0 完成 contract freeze、capability taxonomy、stable identity、body availability 与 translation/unsupported ledgers。
  - Phase 1 完成 subject source reshape、`ChaosUnitTest` / `ChaosBenchmark` 声明入口收口与 Assert proof 化。
  - Phase 2 完成 typed-il shared contract、`BodyAvailabilityResolver` 与 hybrid dispatch foundation。
  - Phase 3 完成 `HotUpdateAssemblyLoader`、`SupplementalMetadataRegistry` 与最小 hot update runtime foundation。
  - Phase 4 完成 `AotCoreIr` 与 `typed-il -> AotCoreIr -> native` 主线切换，并冻结当前 native codegen gap。
  - Phase 5 / Phase 6 通过 `phase-5-6-reuse-audit-v1-01.md` 完成对账：复用 `20260414-13` 到 `20260414-19` 这组已完成任务中的 owner-subject capability batch、full-solution gate、mixed execution / hot update / dashboard / release-gate 闭环，并在当前代码线上完成最新回归确认。
- 结论：当前 roadmap 已同时满足“建立新的 shared contract / runtime / AOT core IR 基线”和“把 owner-subject / engineering gate 闭环重新映射回当前基线”这两个目标，不再需要继续派生 Phase 5 / Phase 6 的平行实现任务。

## 最近摘要

- 2026-04-14 23:34:39 +08:00: 创建 follow-up roadmap，定义 HybridCLR 对齐的 AOT + 热更长期主线。
- 2026-04-15 00:29:10 +08:00: Phase 0 完成并归档。
- 2026-04-15 01:52:44 +08:00: Phase 1 完成并归档。
- 2026-04-15 03:37:41 +08:00: Phase 2 完成并归档，public string compatibility cleanup 完成。
- 2026-04-15 04:18:56 +08:00: Phase 3 完成并归档，hot update runtime foundation 落地。
- 2026-04-15 06:24:00 +08:00: Phase 4 完成并归档，`AotCoreIr` 与 native codegen 主线收口。
- 2026-04-15 06:28:00 +08:00: 完成 `phase-5-6-reuse-audit-v1-01.md`，确认 Phase 5 / Phase 6 通过复用既有 owner-subject / engineering-gate 闭环即可完成。
- 2026-04-15 06:42:00 +08:00: 跑完最终项目级验证，parent roadmap 正式归档到 completed。

## 下一步

- next_action: 无。本 roadmap 已完成并归档；后续若继续扩张 generic / exception / object model / metadata closure 的 AOT codegen 能力，请从当前 `AotCoreIr` gap ledger 与 owner-subject evidence 基线派生新的 follow-up。
- owner: codex
- trigger: 无。

## 风险 / 阻塞

### risks

- `AotCoreIr` 之后的 generic / exception / object model / metadata closure 扩张，必须继续围绕 owner subject、translation surface ledger 与 gap ledger 做增量切片，不能脱离外层证据链。
- 若后续对 retained subjects、dashboard 或 release gates 做大改动，需要重新验证 `phase-5-6-reuse-audit-v1-01.md` 的成立前提。

### blockers

- 当前无 blocker。

## 验证

1. python -m pytest tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_multi_assembly_entry_override.py::MultiAssemblyEntryOverrideTests::test_driver_supports_multi_assembly_entry_override_for_closure_and_native_aot -q
   - 结果：5 passed
2. python -m pytest tests/unit/compatibility/test_managed_closure_contract_bundle.py tests/unit/compatibility/test_multi_assembly_entry_override.py tests/unit/performance/test_native_aot_workload_entry_bundle.py -q
   - 结果：9 passed
3. python -m pytest tests/unit/compatibility -q
   - 结果：179 passed
4. python -m pytest tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/compatibility/test_mainline_feature_pack_onboarding.py tests/unit/compatibility/test_subject_manifest_schema.py tests/tooling/run/test_subject_command.py tests/unit/compatibility/test_hot_update_productization_gates.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/execution/test_subject_workers.py -q
   - 结果：100 passed
5. python -m pytest tests/unit tests/tooling/run tests/integration/registry -q
   - 结果：599 passed, 34 skipped
6. dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1
   - 结果：Build succeeded
7. dotnet build src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj -c Release -m:1
   - 结果：Build succeeded
8. dotnet build subjects/SolutionCorePack/source/Launcher/SolutionCorePack.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-roadmap-closeout/solutioncore/
   - 结果：Build succeeded（保留既有 nullable warnings）
9. dotnet build subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-roadmap-closeout/hotupdate/
   - 结果：Build succeeded
10. dotnet build subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-roadmap-closeout/mixed/
   - 结果：Build succeeded

## wiki

- 本轮继续仅在 docs/dev 沉淀执行状态。
- 如后续需要把 `typed-il` / `AotCoreIr` / hot update shared runtime 的长期结构补入 `docs/architecture`，请另起 follow-up，避免把归档 roadmap 再次拉回执行态。
