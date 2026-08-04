---
task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
title: AOT Core IR Gap Audit And Next Expansion Roadmap
task_type: roadmap
lifecycle_status: abandoned
phase: archived
created_at: 2026-04-16 08:54:25 +08:00
updated_at: 2026-04-18 01:30:50 +08:00
latest_stop_point: roadmap canceled after the foundation-pack-driven redesign superseded the entry-driven AOT mainline; latest active blocker remained ResourceLifecycle catch+finally EH shape
current_dir: docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
parent_task_id:
source_task_id: 20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap
source_relation: follow-up-on-remaining-aot-core-ir-capability-gaps
active: false
---

## 2026-04-18 01:30 Cancel Point

- 用户明确要求取消该 roadmap。
- 当前仓库后续主线已切换到 `20260418-01-dotnet-foundation-pack-nativeization-roadmap`，以 `Microsoft.NETCore.App` foundation pack 为 nativeization 起点。
- 本任务与未完成子任务 `20260416-05` 一并转入 `docs/dev/abandoned/`，旧 entry-driven AOT gap audit 仅保留为历史参考。
- 截止取消时，最近稳定技术 stop point 仍是 `CoreRuntimeFeatures/ResourceLifecycleProofEntry::RunScenario(System.Boolean)` 的 catch+finally EH shape。

## 关键文档

- source_roadmap: `docs/archive/dev-completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/roadmap-v1-01.md`
- source_status: `docs/archive/dev-completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/STATUS.md`
- previous_core_roadmap: `docs/archive/dev-completed/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/roadmap-v1-01.md`
- successor_roadmap: `docs/dev/in-progress/20260418-01-dotnet-foundation-pack-nativeization-roadmap/roadmap-v1-01.md`
- brainstorm: `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`

## 当前判断

- current_focus: 这条 entry-driven AOT capability gap roadmap 已废弃归档，不再作为当前 native 主线入口。
- why_now: 用户明确取消本任务；同时已启动 foundation-pack-driven 的 `20260418-01` 新 roadmap。
- done_definition: 保留旧 gap audit、phase 切分与 stop point 作为历史资产；若未来需要相关能力，应从新主线重新派生。

## 最近摘要

- 2026-04-18 01:30:50 +08:00: 用户明确取消本任务；父 roadmap 与未完成子任务 `20260416-05` 已转入 `docs/dev/abandoned/`，由 `20260418-01` 承接后续设计。
- 2026-04-16 23:15:27 +08:00: 旧主线最后一次 active stop point 锁定到 `ResourceLifecycleProofEntry::RunScenario(System.Boolean)` 的 catch+finally EH shape。
- 2026-04-16 08:54:25 +08:00: 完成新一轮 AOT core gap audit，并形成 capability-first child-task roadmap 骨架。

## 下一步

- next_action: 无。若继续 foundation-pack nativeization 主线，请转到 `20260418-01-dotnet-foundation-pack-nativeization-roadmap`。
- owner: codex
- trigger: 用户需要查看旧 entry-driven AOT gap audit 历史，或核对新旧路线继承关系时查看。

## 风险 / 阻塞

### risks

- 旧文档中的 `SolutionCorePack` owner、entry-driven owner probe 与 project DLL 假设，已经不再是当前默认架构答案。
- 若直接沿用这条旧 roadmap 的 phase 切分继续实现，会把 foundation-pack-driven 目标再次拉回到项目入口驱动。

### blockers

- 无。该 roadmap 已按用户指令废弃归档。

## 历史验证

- `python -m pytest tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py tests/unit/compatibility/test_phase3_aot_closure_ldtoken_native_aot.py tests/unit/compatibility/test_phase3_testframework_generic_assert_bundle.py tests/unit/compatibility/test_phase3_aot_closure_validation_owner_native_aot.py tests/unit/compatibility/test_phase4_generic_constraint_dispatch_native_aot.py tests/unit/compatibility/test_phase4_generic_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_phase2_aot_core_ir_ldtoken_target_reference.py -q`
  - 结果: `29 passed`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果: 成功
- `dotnet ... emit-native-aot <latest phase3-aot-closure-validation bundle> <probe emit dir>`
  - 结果: 成功

## 2026-04-16 18:39 Historical Update

- `20260416-05` 本轮继续沿聚合 owner `CoreRuntimeFeatures/ProofEntry::Run()` 前推 frontier
- 已新增收口:
  - `System.Threading/Interlocked::CompareExchange<T>` 最小 reference-type bridge
  - `System.GC::Collect()` / `System.GC::WaitForPendingFinalizers()` 显式 hard-fail bridge
- 已新增 focused evidence:
  - `tests/unit/compatibility/test_phase3_event_callback_flow_native_aot.py`
  - `tests/unit/compatibility/test_phase3_gc_sensitive_flow_native_aot.py`
- 相邻 compatibility 回归结果:
  - `20 passed`
- parent roadmap 当前新的真实 blocker:
  - `System.Collections/System.Collections.Generic.List<System.Int32>::Add(System.Int32)`
- 结论:
  - 下一阶段不应再沿报错文本逐条补洞，而应按 capability-first 收口 collection external-runtime family

## wiki

- 本轮暂不写入 wiki
- 原因: 这是旧主线归档动作，不是新增长期 authority；当前 authority 已切换到 `20260418-01` 及其后续 phase。
