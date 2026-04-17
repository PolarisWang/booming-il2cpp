---
task_id: 20260417-02-il2cpp-core-structure-and-scriban-governance-roadmap
title: IL2CPP Core Structure And Scriban Governance Roadmap
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-17 13:20:00 +08:00
updated_at: 2026-04-17 11:22:33 +08:00
latest_stop_point: Completed roadmap after splitting NativeAot object/runtime helper families plus Loader/Linker/Contracts hotspots; governance contracts and compatibility suites were updated to aggregated truth sources and all required validation passed
current_dir: docs/dev/completed/20260417-02-il2cpp-core-structure-and-scriban-governance-roadmap
parent_task_id:
source_task_id: 20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap
source_relation: companion-governance-roadmap-for-il2cpp-core-refactor
active: false
---

## 2026-04-17 11:22 Completion

- 已完成本 roadmap 的全部 Phase 0-6。
- 本轮补齐的结构切片：
  - `Emission/NativeAotLoweringPlanner.ObjectEqualityEmission.cs`
  - `Emission/NativeAotLoweringPlanner.ReflectionObjectEmission.cs`
  - `Emission/NativeAotLoweringPlanner.ObjectModelUtilities.cs`
  - `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.StringAndPlatform.cs`
  - `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.CollectionAndReflection.cs`
  - `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.TypeResolution.cs`
  - `RuntimeSupport/NativeAotLoweringPlanner.ExternalRuntimeHelpers.InvocationAbi.cs`
  - `LoaderStage.CrossAssemblyInstantiation.cs`
  - `LoaderStage.AssemblyLoading.cs`
  - `LoaderStage.InstructionDecoding.cs`
  - `LoaderStage.MetadataResolution.cs`
  - `LoaderStage.GenericMaterialization.cs`
  - `LinkerStage.Reachability.cs`
  - `LinkerStage.OptimizationFacts.cs`
  - `LinkerStage.DispatchResolution.cs`
  - `LinkerStage.OutputProjection.cs`
  - `ManagedNaming.cs`
  - `ManagedClosureModels.cs`
  - `ManagedMethodIdentityContracts.cs`
  - `ManagedSemanticWorldContracts.cs`
  - `TypedIlAndAotCoreIrContracts.cs`
  - `ManagedClosureArtifactModels.cs`
- 关键热点文件尺寸已明显收口：
  - `LoaderStage.cs`: `119131` bytes -> `2894` bytes
  - `LinkerStage.cs`: `59442` bytes -> `2632` bytes
  - `ManagedClosureContracts.cs`: `59505` bytes -> `1119` bytes
  - `NativeAotLoweringPlanner.ObjectModelEmission.cs`: `182710` bytes -> `36374` bytes
  - `NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs`: `162863` bytes -> `2999` bytes
- 文本级 compatibility / governance 断言已切换为读取 `root + split files` 的聚合真相源，后续继续拆 partial file 时不应回退到“单个 root file 即真相源”的旧模式。

## 关键文档

- brainstorm: `docs/dev/completed/20260417-02-il2cpp-core-structure-and-scriban-governance-roadmap/brainstorm-v1-01.md`
- design: `docs/dev/completed/20260417-02-il2cpp-core-structure-and-scriban-governance-roadmap/design-v1-01.md`
- roadmap: `docs/dev/completed/20260417-02-il2cpp-core-structure-and-scriban-governance-roadmap/roadmap-v1-01.md`
- wiki: `wiki/04-工具与集成/il2cpp-core-structure-and-scriban-governance.md`

## 当前结论

- `Chaos.IL2CPP.CodeGen` 已完成从超大 root planner 文件向 `Planning/RuntimeSupport/Emission/ReferenceProof/Templating` 的结构收口。
- `Scriban` 已稳固为核心 codegen 基础设施；后续新增文件级生成规则应优先通过模板模型、模板函数与 shared renderer 扩展。
- `Loader / Linker / Contracts` 的热点文件已完成按职责拆分，后续新增逻辑必须继续沿着 split file 边界落位。
- 当前剩余的主要技术债不是结构债，而是既有 nullable warnings；它们不阻塞本 roadmap 完成。

## 最近摘要

- 2026-04-17 13:20:00 +08:00: 冻结 `CodeGen` 分层、文件尺寸阈值、Scriban 核心层定位与 wiki 长期规则。
- 2026-04-17 10:32:05 +08:00: 完成 `NativeAotLoweringPlanner` 的 runtime prelude、metadata support、invocation planning 三批结构切片，同时修正 `NativeReference` catalog alias 在 `switch case` 下的编译期常量约束。
- 2026-04-17 10:55:40 +08:00: 完成 `NativeAotLoweringPlanner` 的 method emission / exception emission / object model / external runtime helper 主切片，并通过 `40 passed` compatibility 回归与 `CodeGen` Release build。
- 2026-04-17 11:22:33 +08:00: 完成 `ObjectModelEmission` / `ExternalRuntimeHelpers` 二次切片，以及 `LoaderStage` / `LinkerStage` / `ManagedClosureContracts` 的结构拆分；同步更新 compatibility 聚合真相源辅助函数并完成 `65 passed` 回归。

## 下一步

- next_action: 无；roadmap 已完成并转入 `completed`。
- owner: codex
- trigger: 无

## 风险 / 阻塞

### risks

- 工作树当前仍然是脏状态，后续执行必须只增量修改目标文件，不能误回滚用户已有改动。
- `Chaos.IL2CPP.CodeGen` 现在已有 `Planning/`、`RuntimeSupport/`、`Emission/`、`Templating/`、`ReferenceProof/` 多个方向，后续 feature 接入需要继续保证目录归属一致，不要回流到根目录。
- 若后续再次通过已编译 DLL 回收源码级切片，必须显式使用 `ilspycmd -p`，避免引入 compiler-generated helper 名称污染。
- 后续新增文本级结构断言时，应继续复用聚合 source helper，而不是退回单文件断言。

### blockers

- 当前无阻塞；任务已完成。

## 验证

- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
- `python -m pytest tests/unit/compatibility/test_il2cpp_codegen_structure_governance.py tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_capability_driven_lowering.py tests/unit/compatibility/test_async_await_proof_subject.py tests/unit/compatibility/test_engine_host_proof_subject.py tests/unit/compatibility/test_engine_binding_subjects.py tests/unit/compatibility/test_mainline_capability_batch_a.py tests/unit/compatibility/test_mainline_capability_batch_b.py tests/unit/compatibility/test_marshaling_proof_subject.py tests/unit/compatibility/test_nested_exception_proof_subject.py tests/unit/compatibility/test_threading_gc_proof_subject.py tests/unit/compatibility/test_loader_switch_opcode.py tests/unit/compatibility/test_phase1_loader_typed_reference_decode.py tests/unit/compatibility/test_phase1_loader_vararg_decode.py tests/unit/compatibility/test_phase3_linker_preserve_contract_native_aot.py tests/unit/compatibility/test_phase3_loader_contract_native_aot.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_managed_closure_contract_bundle.py tests/unit/compatibility/test_phase7_loader_function_pointer_decode.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py -q`
- 结果：`65 passed`
