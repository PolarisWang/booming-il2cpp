---
task_id: 20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization
title: Phase 3 NativeAot Planner And Entry ABI Generalization
task_type: plan
lifecycle_status: in_progress
phase: executing
created_at: 2026-04-16 10:36:50 +08:00
updated_at: 2026-04-16 23:15:27 +08:00
latest_stop_point: ResourceLifecycle catch+finally EH shape is the current blocker after collection interface bridge closure
current_dir: docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization
parent_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_relation: roadmap-child-phase-3
design_doc: docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/design-v1-01.md
plan_doc: docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/plan-v1-01.md
total_tasks: 6
current_task: batch-h-aggregate-owner-reprobe-after-testframework-closure
active: true
---

## 2026-04-16 23:15 Stop Point

- collection external-runtime frontier 的第一层已收口：
  - `System.Collections.Generic.IReadOnlyCollection<int>::get_Count()`
  - `System.Collections.Generic.IReadOnlyList<int>::get_Item(int)`
  - `System.Collections.Generic.IReadOnlyCollection<string>::get_Count()`
  - `System.Collections.Generic.IReadOnlyList<string>::get_Item(int)`
- focused verification:
  - `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
    - 结果: 成功
  - `python -m pytest tests/unit/compatibility/test_phase3_required_instantiation_collection_interface_native_aot.py -q`
    - 结果: `1 passed`
  - `python -m pytest tests/unit/compatibility/test_phase3_required_instantiation_collection_interface_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_phase3_generic_collection_proof_native_aot.py tests/unit/compatibility/test_phase3_span_memory_native_aot.py tests/unit/compatibility/test_phase3_ref_struct_stackalloc_native_aot.py -q`
    - 结果: `7 passed`
- 当前新的真实 blocker：
  - `CoreRuntimeFeatures/ResourceLifecycleProofEntry::RunScenario(System.Boolean)` does not support current exception region shape
- 根因已经定位：
  - catch region: `tryOffset=6, tryLength=57, handlerOffset=63, handlerLength=14`
  - inner finally region: `tryOffset=13, tryLength=38, handlerOffset=51, handlerLength=10`
  - `IL_0006` / `IL_0007` / `IL_000C` 是资源获取前缀，位于 outer catch 内、inner finally 外
  - 现有 `TryCreateCatchAndFinallyExceptionMethodShape(...)` 强要求 `innerFinallyRegion.TryOffset == rootTryOffset`
- 已冻结 RED：
  - `python -m pytest tests/unit/compatibility/test_phase3_resource_lifecycle_native_aot.py -q`
    - 结果: `1 failed`
    - 失败点: `native-aot method 'CoreRuntimeFeatures/ResourceLifecycleProofEntry::RunScenario(System.Boolean)' does not support current exception region shape`
- 下一个直接执行项：
  - 在 `CatchAndFinallyExceptionMethodShape` 中加入 `PreInnerTryInstructions`
  - 放宽 inner finally 起点约束，允许 `innerFinallyRegion.TryOffset > rootTryOffset`
  - 调整 emitter，让 pre-inner 段在 outer catch try 内先执行，再建立 `chaos_inner_finally_guard`，然后跳入 inner try
  - focused regression 转绿后，再重新 probe 聚合 owner `CoreRuntimeFeatures/ProofEntry::Run()`

## 关键文档

- parent_status: `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/STATUS.md`
- parent_roadmap: `docs/dev/in-progress/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/roadmap-v1-01.md`
- design: `docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/design-v1-01.md`
- plan: `docs/dev/in-progress/20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization/plan-v1-01.md`

## 当前结论

- `Phase 3` 既有 batch A-D 已完成:
  - batch A: `ldc.i8 + and / or / xor`
  - batch B: `shl / shr / shr.un`
  - batch C: `not`
  - batch D: `beq / bgt / ble / bge / bge.un`
- generic constraint dispatch 收口已完成:
  - loader placeholder generic instantiation 不再错误折叠回 open definition
  - planner 已补 `ldarga` 最小 lowering, 并在 direct / virtual 调用点标准化 inbound native-int 参数
  - linker 已补 interface dispatch implementation retention
  - `GenericConstraintProofEntry::Run()` 已可完整 `emit-native-aot`
- async/state-machine external-runtime family 已收口:
  - `tests/unit/compatibility/test_phase3_async_task_builder_native_aot.py`
  - catch-only EH shape 放宽, 支持 async `MoveNext()` try 体内多处 `leave`
  - structured EH linear lowering 已补 `ldarg` / `ldfld`
  - planner 已补 `brfalse`
  - `callvirt` 的 `HybridDispatchKind.ExternalRuntime` 已走 direct helper bridge
  - `AsyncAwaitProofEntry::Run()` 已可完整 `emit-native-aot`
- 本轮 batch G 已完成 `ldtoken` / reflection metadata frontier 的第一段收口:
  - 新增 focused evidence: `tests/unit/compatibility/test_phase3_aot_closure_ldtoken_native_aot.py`
  - planner 已补 `ldtoken(type)` lowering
  - native-aot object model 已补最小 reflection object helpers:
    - `System.Type::GetTypeFromHandle(System.RuntimeTypeHandle)`
    - `System.Type::get_TypeHandle()`
    - `System.Type::GetMethod(System.String,System.Reflection.BindingFlags)`
    - `System.Reflection.MethodBase::get_MethodHandle()`
    - `System.Reflection.MethodInfo::MakeGenericMethod(System.Type[])`
  - synthetic native-aot surface 已可稳定 `emit-native-aot`
- `Chaos.TestFramework` generic assert / native closure frontier 已在真实 owner proof 上完成收口:
  - loader 已补跨程序集 closed generic method materialization pass, 可将 caller-discovered `MethodSpec` / `MemberRef` 回灌到定义程序集 methods 集合
  - `Chaos.TestFramework/Assert.Equal<T>` 已改为 AOT-friendly object/null equality 路径, 不再依赖 `EqualityComparer<T>.Default`
  - external-runtime helper 已补:
    - `System.Object::Equals(System.Object)`
    - `System.Reflection.MethodBase::Invoke(System.Object,System.Object[])`
  - `CoreRuntimeFeatures/AotClosureValidationProofEntry::Run()` 已可完整 `emit-native-aot`
  - 下一步应回到聚合 owner `CoreRuntimeFeatures/ProofEntry::Run()` 重新 probe, 暴露下一层真实 frontier

## 最近摘要

- 2026-04-16 10:36:50 +08:00: `20260416-04` 归档完成, 正式激活 `20260416-05`, 第一批锁定 `ldc.i8 + bitwise` planner legalization.
- 2026-04-16 11:00:55 +08:00: 完成 `ldc.i8 + and/or/xor + shl/shr/shr.un` widening, 并新增 `ShiftOpsBenchmark` focused evidence.
- 2026-04-16 11:19:03 +08:00: 完成 `not + beq/bgt/ble/bge/bge.un` widening, 并在 branch proof 回补 `LoaderStage` 的 `Bge_un / Bge_un_s` decode.
- 2026-04-16 13:07:19 +08:00: 完成 generic constraint dispatch 收口, 并把 aggregate `ProofEntry::Run()` 推进到 async/state-machine external-runtime helper blocker.
- 2026-04-16 14:01:08 +08:00: 完成 async task builder native-aot 收口与最小 `ldstr` runtime, 主线新阻塞前推到 `ldtoken`.
- 2026-04-16 14:39:09 +08:00: 完成 `ldtoken(type)` 与最小 reflection method-info helper family 收口; synthetic native-aot surface 转绿; 真实 owner proof 首阻塞前推到 `Chaos.TestFramework` generic `Assert.Equal<T>`.
- 2026-04-16 15:12:42 +08:00: 完成跨程序集 generic assert specialization retention、`System.Object::Equals(System.Object)`、最小 `MethodBase::Invoke(System.Object,System.Object[])` 收口; `AotClosureValidationProofEntry::Run()` 已可完整 `emit-native-aot`; 下一步转向聚合 owner `ProofEntry::Run()` 重新探测真实 frontier.

## 下一步

- next_action: 重新 build 并 probe 聚合 owner `CoreRuntimeFeatures/ProofEntry::Run()`, 确认在 `AotClosureValidationProofEntry::Run()` 收口之后的下一个真实 native-aot frontier
- owner: codex
- trigger: 用户继续推进当前 AOT core roadmap

## 风险 / 阻塞

### risks

- 如果继续把 TestFramework generic assert、reflection helper、metadata token 问题混成一批, `Phase 3` 会再次退化成按报错文本逐条补洞, 而不是 capability-first 收口
- `MethodBase::Invoke(System.Object,System.Object[])` 当前仍是最小 bridge, 仅覆盖本轮 owner proof 用到的 static + single-arg + generic method reflect-invoke 形态
- 聚合 owner 重新 probe 后很可能暴露更宽的 reflection invoke / entry ABI / object-model frontier
- 需要继续避免把 `Chaos.TestFramework` 行为逻辑混入 AOT Core IR 规则本体

### blockers

- 当前稳定 blocker: 无
- 现状: `AotClosureValidationProofEntry::Run()` 已可完整 `emit-native-aot`; 下一阻塞待聚合 owner `ProofEntry::Run()` 重新 probe 暴露

## 验证

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

## 2026-04-16 18:39 Update

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
- 原因: 当前 frontier 已重新回到聚合 owner audit, 下一层 reflection invoke / entry ABI 结论尚未稳定
