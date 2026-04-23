---
task_id: 20260421-02-canonical-generic-runtime-kernel-roadmap
title: Canonical Generic Runtime Kernel Roadmap
task_type: roadmap
lifecycle_status: completed
phase: completed
created_at: 2026-04-21 23:02:19 +08:00
updated_at: 2026-04-23 12:02:21 +08:00
latest_stop_point: roadmap closeout remains valid, and a 2026-04-23 follow-up cleanup removed the last ManagedMethodIdentityResolver string-overload compatibility path; handwritten subject/fixture identities now use explicit ManagedMethodIdentitySpec, leaving only SupplementalMetadataRegistry subject/definition fallback as residual compatibility debt
current_dir: docs/dev/completed/20260421-02-canonical-generic-runtime-kernel-roadmap
parent_task_id:
source_task_id: 20260421-01-aot-generic-task-lowering-normalization
source_relation: broadened-from-task-lowering-to-loader-ir-first-canonical-generic-hard-cut-roadmap
brainstorm_doc: docs/dev/completed/20260421-02-canonical-generic-runtime-kernel-roadmap/brainstorm-v1-01.md
design_doc: docs/dev/completed/20260421-02-canonical-generic-runtime-kernel-roadmap/design-v1-02.md
roadmap_or_plan: docs/dev/completed/20260421-02-canonical-generic-runtime-kernel-roadmap/roadmap-v1-02.md
child_execution_mode: auto
auto_continue: false
auto_stop_policy: blocking-only
recommended_next_child:
active: false
---

# 20260421-02 Canonical Generic Runtime Kernel Roadmap

## 当前状态
- 状态: `completed`
- 完成阶段: `Roadmap CLOSEOUT`
- 当前目标: 整个 canonical generic runtime kernel 主路线已收口，归档到 `docs/dev/completed/`

## 当前结论
- 整个 roadmap 已完成，`loader/IR-first hard cut` 主路线闭环成立，最终不再回退到 clone/materialization-driven generic execution。
- 七个 child 均已完成:
  - `20260421-03-canonical-generic-contracts-and-parser-unification`
  - `20260421-04-loader-instantiation-graph-hard-cut`
  - `20260421-05-shared-generic-aot-ir-hard-cut`
  - `20260421-06-generic-matrix-and-ir-gates`
  - `20260421-01-aot-generic-task-lowering-normalization`
  - `20260421-07-dispatch-reflection-hotupdate-cutover`
  - `20260421-08-legacy-generic-path-purge`
- 终态架构结论:
  - canonical generic authority 已稳定拆分并收口到 `GenericInstantiationKey`、`SharedGenericBodyId`、`InstantiationStubId`、`RuntimeGenericContextArtifact`。
  - loader / AOT IR / generated matrix / runtime / HotUpdate 均转入 explicit authority model；`subjectId`、metadata token 与 legacy slot naming 不再充当最终 execution authority。
  - roadmap 尾项 `20260421-08` 最初把残余 fallback 压缩到兼容 facade；其后 2026-04-23 follow-up cleanup 已删除 identity overload，当前只剩 `SupplementalMetadataRegistry` 的 subject/definition lookup 作为兼容 debt。
- 所有 required formalVerificationObjects 已留证完成:
  - `generic matrix`
  - `aot-core-ir generic contracts`
  - `native generic proofs`
  - `hotupdate generic boundary proofs`
  - `generated code review`
  - `generated cpp size/memory benchmark`

## 本轮产物
- `brainstorm-v1-01.md`
- `design-v1-02.md`
- `roadmap-v1-02.md`
- `docs/dev/completed/20260421-03-canonical-generic-contracts-and-parser-unification/STATUS.md`
- `docs/dev/completed/20260421-04-loader-instantiation-graph-hard-cut/STATUS.md`
- `docs/dev/completed/20260421-05-shared-generic-aot-ir-hard-cut/STATUS.md`
- `docs/dev/completed/20260421-06-generic-matrix-and-ir-gates/STATUS.md`
- `docs/dev/completed/20260421-01-aot-generic-task-lowering-normalization/STATUS.md`
- `docs/dev/completed/20260421-07-dispatch-reflection-hotupdate-cutover/STATUS.md`
- `docs/dev/completed/20260421-08-legacy-generic-path-purge/STATUS.md`

## AOT/IL2CPP/Test Governance Intake
- capabilityFamily: `native-generic-runtime-kernel`
- capabilityItem: `loader-ir-first-canonical-generic-hard-cut`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `Proof`
- formalVerificationObjects:
  - `generic matrix`
  - `aot-core-ir generic contracts`
  - `native generic proofs`
  - `hotupdate generic boundary proofs`
  - `generated code review`
  - `generated cpp size/memory benchmark`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`
  - `native-host`
  - `hotupdate-host`

## 子任务闭环
- `20260421-03`: contracts / parser authority 收口完成
- `20260421-04`: loader instantiation demand graph hard cut 完成
- `20260421-05`: shared generic AOT IR / stub authority hard cut 完成
- `20260421-06`: generic matrix / boundary case / gate 体系完成
- `20260421-01`: async generic family runtime cutover 完成
- `20260421-07`: dispatch / reflection / HotUpdate cutover 完成
- `20260421-08`: legacy generic path purge 完成

## 架构审视
- arch_review_mode: `critical`
- 结论: `ok`
- authority:
  - loader/IR-first 路线现已统一收口到显式 generic authority 合同；`InstantiationStubId + RuntimeGenericContext` 是最终 runtime 执行核心。
  - HotUpdate runtime、bridge、metadata query 的残留 wrapper 已降级为兼容 facade，不再破坏 authority 主线。
- upstream:
  - `ManagedNaming`
  - contracts / loader / metadata writer / AOT IR lowering
  - owner proof / benchmark registry
- downstream:
  - native generated artifact
  - generic capability matrix / optimization facts
  - dispatch / reflection / HotUpdate runtime
  - owner native proof / hotupdate proof / engineering workload
- allowed_deps:
  - 仅沿既有 contracts -> loader -> IR -> codegen/runtime -> proof/benchmark 主线推进，不新增新的 capability/owner/formal object 边界。
- shim_debt:
  - 若未来继续削减兼容层，首要入口仅剩 `SupplementalMetadataRegistry` 的 subject/definition lookup。
  - 当前 debt 已全部降级为 `warn`，没有未解决 blocker。
- proof:
  - roadmap 最后一轮 closeout 已通过 child `20260421-08` 的 regression + formal object + codegen benchmark 证据完成闭环。

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
- follow-up cleanup 已删除 `ManagedMethodIdentityResolver.Create(string subjectId, ...)` 旧入口，并把 retained handwritten identities 迁移到显式 `ManagedMethodIdentitySpec`。
- 因此 roadmap `20260421-08` 中提到的 “identity overload facade” 已不再是残留 debt。
- 当前 residual compatibility debt 只剩 `SupplementalMetadataRegistry.TryGetMethod(...)` 的 subject/definition fallback。

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
- `python -m build.toolchains.run.run test declared-unit-test --id "declared-unit-test/SolutionCorePack::GoldenCoreLibReference.HotUpdatePatch::GoldenCoreLibReference.HotUpdatePatch.CoreLibHotUpdateProof::Run()" --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - 结果: `ok`
  - run id: `20260423-011137-windows-3241`
- `python -m build.toolchains.run.run test engineering-workload --id engineering-workload/SolutionCorePack/codegen --json`
  - 结果: `ok`
  - run id: `20260423-011232-windows-82c9`

## 下一步
- `n/a`
