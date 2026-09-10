---
task_id: 20260421-08-legacy-generic-path-purge
title: Legacy Generic Path Purge
task_type: task
lifecycle_status: completed
phase: completed
created_at: 2026-04-22 22:41:45 +08:00
updated_at: 2026-04-23 12:02:21 +08:00
latest_stop_point: follow-up cleanup removed ManagedMethodIdentityResolver string overload and migrated all handwritten subject/fixture identities to explicit ManagedMethodIdentitySpec; only SupplementalMetadataRegistry subject/definition fallback remains as compatibility debt, and formal proof/codegen evidence stays green
current_dir: docs/dev/completed/20260421-08-legacy-generic-path-purge
parent_task_id: 20260421-02-canonical-generic-runtime-kernel-roadmap
source_task_id: 20260421-02-canonical-generic-runtime-kernel-roadmap
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
preflight_review: warn
preflight_summary: parent roadmap 的边界、AOT obligation 与自动续跑条件仍成立；本轮继续限定在 loader/runtime generic authority 与 HotUpdate compatibility facade 收口，不新增 capability、owner、formal object 或新的治理入口。
auto_execution_decision: continue
design_doc: docs/dev/completed/20260421-02-canonical-generic-runtime-kernel-roadmap/design-v1-02.md
roadmap_or_plan: docs/dev/completed/20260421-08-legacy-generic-path-purge/plan-v1-01.md
arch_review_mode: critical
active: false
---

# 20260421-08 Legacy Generic Path Purge

## 当前状态
- 状态: `completed`
- 完成阶段: `Step 5 CLOSEOUT`
- 当前目标: `20260421-08` 已收口，向父 roadmap `20260421-02` 回写最终 closeout

## 当前结论
- `20260421-08` 已完成六个 purge 批次:
  - loader 首批 purge：`ProjectCrossAssemblyMethodInstantiations` / `projected*` 语义已替换旧的 materialization 入口。
  - HotUpdate bridge contract purge：bridge plan 与 dispatcher 已切到 identity / authority-only 合同。
  - CodeGen authority purge：`AotCoreIrLowering`、`MetadataWriterStage`、generic matrix observation 已停止从 `subjectId` 隐式派生 runtime generic authority。
  - HotUpdate authority contract purge：`ManagedMethodIdentityArtifact` 新增显式 `ExecutionAuthorityKey`，bridge / auto-bridge 链路优先消费显式 authority。
  - HotUpdate binding/runtime authority purge：binding record 现已显式承载可选 `ExecutionAuthorityKey`，`RuntimeManager.ApplyBindings(...)` 与 `HotUpdateMethodRegistry` 统一支持 authority-first 注册。
  - loader-projected runtime generic context cutover：`ManagedMethodModel` 显式携带 `RuntimeGenericContext`，loader 在 assembly loading / generic instantiation projection 阶段稳定回填，`AutoBridgeGenerator` 直接消费 `method.RuntimeGenericContext?.InstantiationStubId.Value`，HotUpdate 侧不再本地调用 `ManagedNaming.TryCreateRuntimeGenericContext(...)`。
- 收口审计结论:
  - `HotUpdateMethodRegistry` / `RuntimeManager` / `BridgeDispatcher` / `BridgeGenerator` 中残留的 `ResolveExecutionAuthorityKey(identity)` 仅是 identity -> authority 的兼容 facade，不再重新合成 generic execution authority。
  - `SupplementalMetadataRegistry` 保留 `TryGetMethod(...)` 的 subject/definition fallback 以兼容 metadata supplement proof；authority-aware lookup 已由 `TryGetMethodByExecutionAuthority(...)` 提供。
  - `ResolvePackageExecutionAuthorityKey(...)` 在 manifest 缺省时回退 `EntryPoint` 只是 package compatibility 默认值，不改变 runtime 最终 authority model。
- 结合 regression、formal proof、generated review 与 codegen benchmark 证据，本 child 已满足 completed gate；当前不存在未解决 blocker。

## 本轮修改文件
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureModels.cs`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.AssemblyLoading.cs`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.GenericInstantiationProjection.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/AutoBridgeGenerator.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/HotUpdateMethodBindings.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/HotUpdateMethodRegistry.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/RuntimeManager.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/SupplementalMetadataRegistry.cs`
- `subjects/HotUpdateHostPack/source/Host/Proofs/VersionRollbackProofEntry.cs`
- `tests/fixtures/subjects/VersionRollbackProof/source/Program.cs`
- `tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py`

## 文件审计
- `src/managed/Chaos.IL2CPP.HotUpdate/BridgeDispatcher.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/BridgeGenerator.cs`
- `subjects/HotUpdateHostPack/source/Host/Proofs/MetadataSupplementProofEntry.cs`

## 架构审视
- arch_review_mode: `critical`
- 结论: `ok`
- authority:
  - loader / managed closure 现已端到端显式承载 `RuntimeGenericContext`；generic execution authority 由 `InstantiationStubId` / 显式 `ExecutionAuthorityKey` 收口，不再由 `subjectId`、materialized clone 命名或 HotUpdate 本地推导承担。
  - HotUpdate auto-bridge、binding、runtime 路径只消费既有显式 authority；HotUpdate 代码内已不存在新的 runtime generic context 本地合成入口。
- upstream:
  - `ManagedMethodModel.RuntimeGenericContext`
  - `LoaderStage.AssemblyLoading.cs`
  - `LoaderStage.GenericInstantiationProjection.cs`
  - `ManagedMethodIdentityResolver.ResolveExecutionAuthorityKey(...)`
- downstream:
  - `AutoBridgeGenerator`
  - `BridgeDispatcher` / `BridgeGenerator`
  - `RuntimeManager` / `HotUpdateMethodRegistry`
  - `SupplementalMetadataRegistry` / `MetadataSupplementProof`
- allowed_deps:
  - 仅在既有 contracts / loader / HotUpdate authority 边界内清理 fallback 与显式 authority carrier。
  - 不新增 capability、owner、formal object 或新的治理入口，不回退到 clone/materialization-driven generic execution。
- shim_debt:
  - `SupplementalMetadataRegistry.TryGetMethod(...)` 仍保留 subject/definition fallback 作为 metadata supplement compatibility 查询面；authority-first 查询入口已由 `TryGetMethodByExecutionAuthority(...)` 承担。
  - 2026-04-23 follow-up cleanup 已删除 `ManagedMethodIdentityResolver` 的 string overload；HotUpdate runtime/bridge 侧不再保留这条 compatibility facade。
  - `ResolvePackageExecutionAuthorityKey(...)` 仍允许 manifest 未填 authority 时回退 `EntryPoint`，该 debt 已降级为 `warn`，不构成 closeout blocker。
- proof:
  - RED 先证明 binding/runtime 尚未支持 binding-level authority，且 HotUpdate 尚未消费 loader-projected runtime generic context。
  - GREEN 通过 phase2 / loader / phase3 / hotupdate e2e regression、generic matrix / AOT IR gates、owner native proof、hotupdate proof 与 codegen workload 完成闭环。

## 完成证据
- arch_review: ok
- authority_review: ok
- verification.native.canonicalCommand: `python -m build.toolchains.run.run test declared-unit-test --id "declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.GenericSharingBoundaryProofEntry::Run()" --matrix windows-native-check --json`
- verification.native.formalObject: `[native generic proofs, generated code review]`
- verification.native.summaryPath: `artifacts/subjects/SolutionCorePack/runs/20260423-010946-windows-ba41/declared/unit/generic-sharing-boundary-proof/summary.json`
- verification.native.eventsPath: `artifacts/subjects/SolutionCorePack/runs/20260423-010946-windows-ba41/run-report/events.jsonl`
- verification.native.consolePath: `artifacts/subjects/SolutionCorePack/runs/20260423-010946-windows-ba41/run-report/console.log`
- verification.hotupdate.canonicalCommand: `python -m build.toolchains.run.run test declared-unit-test --id "declared-unit-test/SolutionCorePack::GoldenCoreLibReference.HotUpdatePatch::GoldenCoreLibReference.HotUpdatePatch.CoreLibHotUpdateProof::Run()" --matrix windows-corelib-reference-native-hotupdate-proof --json`
- verification.hotupdate.formalObject: `[hotupdate generic boundary proofs, generated code review]`
- verification.hotupdate.summaryPath: `artifacts/subjects/SolutionCorePack/runs/20260423-011137-windows-3241/declared/unit/corelib-reference-hotupdate-proof/summary.json`
- verification.hotupdate.eventsPath: `artifacts/subjects/SolutionCorePack/runs/20260423-011137-windows-3241/run-report/events.jsonl`
- verification.hotupdate.consolePath: `artifacts/subjects/SolutionCorePack/runs/20260423-011137-windows-3241/run-report/console.log`
- verification.codegen.canonicalCommand: `python -m build.toolchains.run.run test engineering-workload --id engineering-workload/SolutionCorePack/codegen --json`
- verification.codegen.formalObject: `[generated cpp size/memory benchmark]`
- verification.codegen.summaryPath: `artifacts/subjects/SolutionCorePack/runs/20260423-011232-windows-82c9/engineering/workloads/codegen/summary.json`
- verification.codegen.eventsPath: `artifacts/subjects/SolutionCorePack/runs/20260423-011232-windows-82c9/run-report/events.jsonl`
- verification.codegen.consolePath: `artifacts/subjects/SolutionCorePack/runs/20260423-011232-windows-82c9/run-report/console.log`
- test_result: passed
- wiki: n/a
- next: n/a
 
## 2026-04-23 Follow-up Cleanup
- 已删除 `ManagedMethodIdentityResolver.Create(string subjectId, ...)` 与其 parser helper；手写 identity 统一改为显式 `ManagedMethodIdentitySpec`。
- 该 follow-up 不改写此前 closeout 的历史验证记录；它只补充说明旧 identity overload facade 已经清理完成。
- 当前仍保留的兼容 debt 仅剩 `SupplementalMetadataRegistry.TryGetMethod(...)` 的 subject/definition fallback。

## 验证
- `python -m pytest tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py tests/unit/compatibility/test_loader_instantiation_demand_graph_contract.py tests/unit/compatibility/test_loader_instantiation_demand_graph_pipeline_contract.py tests/unit/compatibility/test_phase3_hotupdate_runtime_foundation.py tests/unit/compatibility/test_hot_update_e2e_flows.py -q`
  - 结果: `20 passed`
- `python -m pytest tests/unit/compatibility/test_phase4c_generic_matrix_gates.py tests/unit/compatibility/test_phase5_virtual_dispatch_native_aot.py -q`
  - 结果: `12 passed`
- `python -m pytest tests/unit/compatibility/test_loader_instantiation_demand_graph_aot_core_ir_authority_contract.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_phase7_loader_generic_method_reference_decode.py -q`
  - 结果: `7 passed`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果: `Build succeeded. 0 Warning(s), 0 Error(s)`
- `python -m build.toolchains.run.run test declared-unit-test --id "declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.GenericSharingBoundaryProofEntry::Run()" --matrix windows-native-check --json`
  - 结果: `ok`
  - run id: `20260423-010946-windows-ba41`
  - 关键指标: `generatedCppTotalBytes = 23087`，`generatedSymbolCount = 7`，`peakWorkingSetBytes = 68743168`，`proofArtifactCount = 7`
- `python -m build.toolchains.run.run test declared-unit-test --id "declared-unit-test/SolutionCorePack::GoldenCoreLibReference.HotUpdatePatch::GoldenCoreLibReference.HotUpdatePatch.CoreLibHotUpdateProof::Run()" --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - 结果: `ok`
  - run id: `20260423-011137-windows-3241`
  - 关键指标: `generatedCppTotalBytes = 27291`，`generatedSymbolCount = 0`，`peakWorkingSetBytes = 67059712`，`proofArtifactCount = 8`
- `python -m build.toolchains.run.run test engineering-workload --id engineering-workload/SolutionCorePack/codegen --json`
  - 结果: `ok`
  - run id: `20260423-011232-windows-82c9`
  - 关键指标: `generatedCppTotalBytes = 23087`，`generatedSymbolCount = 7`，`peakWorkingSetBytes = 68730880`

## 残留告警
- `warn`: metadata supplement 的 subject/definition fallback 仍保留为兼容 facade，但本轮审计与 proof 已确认它不再承担新的 execution authority 合成职责。
- `warn`: 当前 worktree 仍包含大量用户已有并行修改；本 child 仅在已批准边界内完成收口与归档，不回滚外部差异。

## 下一步
- `n/a`
