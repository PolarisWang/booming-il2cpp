---
task_id: 20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization
title: Phase 3 NativeAot Planner And Entry ABI Generalization
task_type: plan
lifecycle_status: abandoned
phase: archived
created_at: 2026-04-16 10:36:50 +08:00
updated_at: 2026-04-18 01:30:50 +08:00
latest_stop_point: plan canceled with parent roadmap after foundation-pack-driven redesign; latest active blocker remained ResourceLifecycle catch+finally EH shape
current_dir: docs/dev/abandoned/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization
parent_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_relation: roadmap-child-phase-3
design_doc: docs/dev/abandoned/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/design-v1-01.md
plan_doc: docs/dev/abandoned/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/plan-v1-01.md
total_tasks: 6
current_task: archived-cancelled-with-parent-roadmap
active: false
---

## 2026-04-18 01:30 Cancel Point

- 用户明确取消父 roadmap `20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap`。
- 本计划完全依附旧 entry-driven AOT 主线，因此与父任务一并转入 `docs/dev/abandoned/`。
- 截止取消时，最近稳定技术 blocker 仍是 `CoreRuntimeFeatures/ResourceLifecycleProofEntry::RunScenario(System.Boolean)` 的 catch+finally EH shape。
- 该计划中的 planner / entry ABI / EH widening 结论保留为历史参考，但不再是当前执行入口。

## 关键文档

- parent_status: `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`
- successor_roadmap: `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/roadmap-v1-01.md`
- design: `docs/dev/abandoned/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/design-v1-01.md`
- plan: `docs/dev/abandoned/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/plan-v1-01.md`

## 当前判断

- current_focus: 该 `Phase 3` 子任务已废弃归档，不再继续沿旧主线推进 EH / collection / aggregate owner 收口。
- why_now: 上游 roadmap 已被 foundation-pack-driven redesign 取代；继续执行会把后续 nativeization 再次绑定到项目入口驱动。
- done_definition: 保留 batch A-H 期间关于 planner legalization、entry ABI 与 EH shape 的历史发现；若未来仍需这些能力，应从 `20260418-01` 新主线重新派生。

## 最近摘要

- 2026-04-18 01:30:50 +08:00: 随父 roadmap 一并转入 `docs/dev/abandoned/`，旧 `Phase 3` 不再作为有效执行入口。
- 2026-04-16 23:15:27 +08:00: 最后 active stop point 锁定 `ResourceLifecycleProofEntry::RunScenario(System.Boolean)` EH shape，focused RED 为 `tests/unit/compatibility/test_phase3_resource_lifecycle_native_aot.py`。
- 2026-04-16 10:36:50 +08:00: 从 `20260416-04` 派生并进入 `Phase 3`，开始 planner / entry ABI widening 收口。

## 下一步

- next_action: 无。若未来仍需继续 `NativeAotLoweringPlanner / entry ABI` 相关能力，应从 `20260418-01-dotnet-foundation-pack-nativeization-roadmap` 重新切 phase。
- owner: unassigned
- trigger: 用户需要复盘旧 `Phase 3` 历史，或把其中的技术发现映射到新 foundation roadmap 时查看。

## 风险 / 阻塞

### risks

- 旧计划里的 owner、proof 与 benchmark 绑定 `SolutionCorePack` / project-entry assumptions，不适合直接沿用到 foundation-pack nativeization。
- 若不重新拆 phase 就继续执行，会把旧 planner 收口目标与新 DLL-first certification 目标混在一起。

### blockers

- 无。该计划已按用户指令废弃归档。

## 历史验证

- `python -m pytest tests/unit/compatibility/test_phase3_testframework_generic_assert_bundle.py tests/unit/compatibility/test_phase3_aot_closure_validation_owner_native_aot.py tests/unit/compatibility/test_phase3_aot_closure_ldtoken_native_aot.py -q`
  - 结果: `6 passed`
- `python -m pytest tests/unit/compatibility/test_phase3_aot_closure_ldtoken_native_aot.py -q`
  - 结果: `3 passed`
- `python -m pytest tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py tests/unit/compatibility/test_phase3_aot_closure_ldtoken_native_aot.py tests/unit/compatibility/test_phase3_testframework_generic_assert_bundle.py tests/unit/compatibility/test_phase3_aot_closure_validation_owner_native_aot.py tests/unit/compatibility/test_phase4_generic_constraint_dispatch_native_aot.py tests/unit/compatibility/test_phase4_generic_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_phase2_aot_core_ir_ldtoken_target_reference.py -q`
  - 结果: `29 passed`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果: 成功
- `dotnet ... emit-native-aot <latest phase3-aot-closure-validation bundle> <probe emit dir>`
  - 结果: 成功

## 2026-04-16 18:39 Historical Update

- `System.Threading/Interlocked::CompareExchange<T>` 已补最小 reference-type bridge，并转绿 `tests/unit/compatibility/test_phase3_event_callback_flow_native_aot.py`
- `chaos_resolve_native_int_slot(...)` 已提升到通用 runtime prelude，避免 helper 仅在 async bundle 内可见
- `System.GC::Collect()` / `System.GC::WaitForPendingFinalizers()` 已补显式 hard-fail bridge，并新增 `tests/unit/compatibility/test_phase3_gc_sensitive_flow_native_aot.py`
- 相邻 focused 回归:
  - `python -m pytest tests/unit/compatibility/test_phase3_bcl_delegate_invoke_native_aot.py tests/unit/compatibility/test_phase3_multicast_delegate_native_aot.py tests/unit/compatibility/test_phase3_instance_delegate_method_group_native_aot.py tests/unit/compatibility/test_phase3_event_callback_flow_native_aot.py tests/unit/compatibility/test_phase3_delegate_callback_interop_native_aot.py tests/unit/compatibility/test_phase3_function_pointer_native_aot.py tests/unit/compatibility/test_phase3_string_composite_helpers_native_aot.py tests/unit/compatibility/test_phase3_string_concat_native_aot.py tests/unit/compatibility/test_phase3_filter_only_eh_native_aot.py tests/unit/compatibility/test_phase3_nested_finally_catch_native_aot.py tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py tests/unit/compatibility/test_phase3_aot_closure_validation_owner_native_aot.py tests/unit/compatibility/test_phase3_aot_closure_ldtoken_native_aot.py tests/unit/compatibility/test_phase3_testframework_generic_assert_bundle.py tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py -q`
  - 结果: `20 passed`
- 聚合 owner `CoreRuntimeFeatures/ProofEntry::Run()` frontier 已继续前推到 collection external-runtime
- 当前首个真实 blocker:
  - `System.Collections/System.Collections.Generic.List<System.Int32>::Add(System.Int32)`
- 判断:
  - 这层不是 method retention 漏洞，而是 `dispatchKindCode = ExternalRuntime` 下的 BCL collection helper family 缺口

## wiki

- 本轮暂不写入 wiki
- 原因: 该计划仅作为历史执行记录保留，当前 authority 已切换到新 foundation 主线。
