---
task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
title: AOT Core IR Gap Audit And Next Expansion Roadmap
task_type: roadmap
lifecycle_status: in_progress
phase: roadmap
created_at: 2026-04-16 08:54:25 +08:00
updated_at: 2026-04-16 23:15:27 +08:00
latest_stop_point: phase-3 blocker moved from collection helper family to ResourceLifecycle catch+finally EH shape
current_dir: docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
parent_task_id:
source_task_id: 20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap
source_relation: follow-up-on-remaining-aot-core-ir-capability-gaps
active: true
---

## 2026-04-16 23:15 Stop Point

- `20260416-05` 当前主线没有停在 collection helper family。
- `IReadOnlyCollection<T>` / `IReadOnlyList<T>` interface bridge 已落地，并通过 focused 回归。
- parent roadmap 当前首个真实 blocker 已改为：
  - `CoreRuntimeFeatures/ResourceLifecycleProofEntry::RunScenario(System.Boolean)` 的 catch+finally EH shape
- 根因：
  - outer catch try 从 `IL_0006` 开始
  - inner finally try 从 `IL_0013` 开始
  - 存在一段 pre-inner 资源获取前缀落在 outer catch 内、inner finally 外
  - 现有 `NativeAotLoweringPlanner` 还不支持这个 structured EH lowering 形状
- 当前建议的收口顺序：
  - 先扩 `CatchAndFinallyExceptionMethodShape` / emitter
  - 转绿 `tests/unit/compatibility/test_phase3_resource_lifecycle_native_aot.py`
  - 再重新回到聚合 owner `CoreRuntimeFeatures/ProofEntry::Run()`

## 关键文档

- source_roadmap: `docs/dev/completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/roadmap-v1-01.md`
- source_status: `docs/dev/completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/STATUS.md`
- previous_core_roadmap: `docs/dev/completed/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/roadmap-v1-01.md`
- brainstorm: `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/design-v1-01.md`
- roadmap_or_plan: `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`

## 当前结论

- `20260416-02` 已完成 capability-first gap ledger 与 owner evidence matrix 冻结
- `20260416-03` 与 `20260416-09` 已完成 `Phase 1` loader / typed-il 输入收口
- `20260416-04` 已完成 `Phase 2` 的 `ldtoken target reference + Int64/UInt64 carrier` 收口
- `20260416-05` 仍在执行, 但主线 frontier 已继续前推:
  - generic constraint dispatch family 已收口
  - async/state-machine external-runtime helper family 已收口
  - 最小 `ldstr` literal runtime 已落地
  - `ldtoken(type)` 与最小 reflection method-info helper family 已收口
  - `Chaos.TestFramework` generic assert / native closure frontier 也已在真实 owner proof 上收口:
    - 跨程序集 closed generic method retention 已落地
    - `Assert.Equal<T>` 已去掉 `EqualityComparer<T>.Default` 依赖
    - `System.Object::Equals(System.Object)` 与最小 `MethodBase::Invoke(System.Object,System.Object[])` helper 已落地
    - `CoreRuntimeFeatures/AotClosureValidationProofEntry::Run()` 已可完整 `emit-native-aot`
- parent roadmap 当前下一步重新回到聚合 owner `CoreRuntimeFeatures/ProofEntry::Run()` 审计, 需要让它暴露新的真实 frontier

## 最近摘要

- 2026-04-16 08:54:25 +08:00: 完成新一轮源码审计, 确认 `LoaderStage / AotCoreIr / NativeAotLoweringPlanner / NativeAotEmitter` 仍存在显著 unsupported 边界, 需要新 roadmap 收口
- 2026-04-16 09:09:27 +08:00: `20260416-02-phase-0-gap-ledger-and-owner-evidence-audit` 已完成; 代表性 compatibility 结果 `28 passed`
- 2026-04-16 09:31:14 +08:00: `20260416-03-phase-1-loader-and-typed-il-input-closure` 完成第一批 loader widening; 局部回归结果 `8 passed`
- 2026-04-16 09:43:20 +08:00: `20260416-09-phase-1b-member-reference-parent-and-vararg-loader-closure` 完成第二批 loader widening; 局部回归结果 `10 passed`
- 2026-04-16 10:13:21 +08:00: `20260416-04` 完成 batch A `ldtoken targetReference` 收口; 局部回归结果 `19 passed`
- 2026-04-16 10:36:50 +08:00: `20260416-04` 完成 batch B/C `Int64 + UInt64` ABI carrier widening, 并将相邻 compatibility 压到 `30 passed`; 主线切入 `20260416-05`
- 2026-04-16 13:07:19 +08:00: `20260416-05` 完成 generic constraint dispatch 收口, 并把 aggregate `ProofEntry::Run()` 推进到 async/state-machine external-runtime helper blocker
- 2026-04-16 14:01:08 +08:00: `20260416-05` 完成 async task builder native-aot 收口与最小 `ldstr` runtime, 主线新阻塞前推到 `ldtoken`
- 2026-04-16 14:39:09 +08:00: `20260416-05` 完成 `ldtoken(type)` 与最小 reflection method-info helper family, 新阻塞前推到 `Chaos.TestFramework` generic `Assert.Equal<T>`
- 2026-04-16 15:12:42 +08:00: `20260416-05` 完成跨程序集 generic assert specialization retention、`System.Object::Equals(System.Object)` 与最小 `MethodBase::Invoke(System.Object,System.Object[])` 收口; `AotClosureValidationProofEntry::Run()` 已可完整 `emit-native-aot`; 下一步回到聚合 owner `ProofEntry::Run()` 重新 probe

## 下一步

- next_action: 继续推进 `20260416-05`, 重新 build 并 probe 聚合 owner `CoreRuntimeFeatures/ProofEntry::Run()`, 锁定在 `AotClosureValidationProofEntry::Run()` 收口后的下一个真实 frontier
- owner: codex
- trigger: 用户继续推进 AOT core gap 收口主线

## 风险 / 阻塞

### risks

- 如果不继续按 owner-driven aggregate audit 推进, 主线会再次退化成按异常文本逐条补洞
- `MethodBase::Invoke(System.Object,System.Object[])` 当前只是最小桥接, 聚合 owner 很可能继续暴露更宽的反射调用 ABI / object-model frontier
- 必须继续避免把 `Chaos.TestFramework` 行为逻辑混入 AOT Core IR 本体
- 如果聚合 owner 下一跳直接触及更宽的 reflection invoke / metadata materialization, 可能需要新的 child task 继续 capability-first 收口

### blockers

- 当前稳定 blocker: 无
- 说明: `AotClosureValidationProofEntry::Run()` 已完整转绿, 下一阻塞待聚合 owner `ProofEntry::Run()` 重新 probe 暴露

## 验证

- `python -m pytest tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py tests/unit/compatibility/test_phase3_aot_closure_ldtoken_native_aot.py tests/unit/compatibility/test_phase3_testframework_generic_assert_bundle.py tests/unit/compatibility/test_phase3_aot_closure_validation_owner_native_aot.py tests/unit/compatibility/test_phase4_generic_constraint_dispatch_native_aot.py tests/unit/compatibility/test_phase4_generic_dispatch_native_aot.py tests/unit/compatibility/test_phase4d_dispatch_aot_core_ir.py tests/unit/compatibility/test_phase4d_dispatch_native_aot.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_phase4_aot_core_ir_contract.py tests/unit/compatibility/test_phase2_aot_core_ir_ldtoken_target_reference.py -q`
  - 结果: `29 passed`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果: 成功
- `dotnet ... emit-native-aot <latest phase3-aot-closure-validation bundle> <probe emit dir>`
  - 结果: 成功

## 2026-04-16 18:39 Update

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
- 原因: 当前 frontier 已从 TestFramework owner 收口重新回到聚合 owner 审计, 下一层规则尚未稳定
