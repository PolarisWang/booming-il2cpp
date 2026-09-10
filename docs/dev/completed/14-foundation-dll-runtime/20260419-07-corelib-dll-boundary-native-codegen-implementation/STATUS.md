---
task_id: 20260419-07-corelib-dll-boundary-native-codegen-implementation
title: CoreLib DLL Boundary Native Codegen Implementation
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-19 13:55:00 +08:00
updated_at: 2026-04-24 08:40:00 +08:00
latest_stop_point: 已完成窄边界 CoreLib-first assembly-bound runtime-skeleton widening、subject review bundle 接线与重新验证；source task `20260419-03` 仍因 `truthBoundary.fullCoreLibTranslated = false` 保持 completed-blocked
current_dir: docs/dev/completed/20260419-07-corelib-dll-boundary-native-codegen-implementation
parent_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_task_id: 20260419-03-system-private-corelib-full-verification
source_relation: unblocker-for-corelib-full-verification
design_doc: docs/dev/completed/20260419-07-corelib-dll-boundary-native-codegen-implementation/design-v1-01.md
plan_doc: docs/dev/completed/20260419-07-corelib-dll-boundary-native-codegen-implementation/plan-v1-01.md
total_tasks: 6
active: false
---

# 20260419-07 CoreLib DLL Boundary Native Codegen Implementation

## Outcome

- `plan-v1-01.md` Step 5/6 已完成。
- 新增真实 assembly-bound bool lowering/emitter family，覆盖：
  - `bool identity forwarder`
  - `bool producer -> bool identity forwarder -> ret`
- 关闭 `subject-visible-artifact-review-gap`：
  - `build/toolchains/run/testing/subject_reporting.py` 现在会 materialize subject-level `review-bundle.json`
  - `build/toolchains/run/commands/test.py` 已接入真实 `run.py test subject` / `run.py test declared-unit-test` 主链
- `declared-hotupdate-single-run-file-lock` 不再作为本 child 的活动 blocker：
  - 最新串行 direct declared-unit run 已返回 `ok`

## Completion Evidence

- arch_review: ok
- authority_review: ok
- verification:
  - canonicalCommand: `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
  - formalObject: `subject/SolutionCorePack::windows-corelib-reference-native-hotupdate-proof`
  - summaryPath: `artifacts/subjects/SolutionCorePack/runs/20260424-083326-windows-9222/run-report/summary.json`
  - eventsPath: `artifacts/subjects/SolutionCorePack/runs/20260424-083326-windows-9222/run-report/events.jsonl`
  - consolePath: `artifacts/subjects/SolutionCorePack/runs/20260424-083326-windows-9222/run-report/console.log`
- verification_data:
  - refreshCommand: `n/a`
  - verificationOutputRoot: `artifacts/subjects/SolutionCorePack/runs/20260424-083326-windows-9222`
  - latestResultPath: `artifacts/subjects/SolutionCorePack/runs/20260424-083326-windows-9222/run-report/summary.json`
  - masterResultPath: `n/a`
  - reportSummaryPath: `artifacts/subjects/SolutionCorePack/runs/20260424-083326-windows-9222/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/native-hotupdate-audit.json`
  - codegenStubPath: `verification/evidence/owners/SolutionCorePack/codegen-stubs/`
- tests:
  - `python -m pytest tests/unit/reporting -q`
  - `python -m pytest tests/unit/run/test_test_command_workspace.py -q`
  - `python -m pytest tests/unit/compatibility/test_solution_core_pack_subject_core.py tests/unit/compatibility/test_solution_core_pack_subject_matrices.py tests/unit/compatibility/test_solution_core_pack_subject_scenarios.py tests/unit/compatibility/test_solution_core_pack_subject_runtime.py tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_validators.py tests/unit/compatibility/test_compiled_subject_catalog_solution_assemblies.py -q`
  - `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - `python build/toolchains/run/run.py test declared-unit-test --id "declared-unit-test/SolutionCorePack::GoldenCoreLibReference.HotUpdatePatch::GoldenCoreLibReference.HotUpdatePatch.CoreLibHotUpdateProof::Run()" --json`
  - `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`
- test_result: passed

## Key Evidence

- canonical audit:
  - `translationUnitMethodCount = 167`
  - `runtimeSkeletonReservedStubCount = 0`
  - `runtimeSkeletonUncoveredMethodCount = 0`
  - `fullCoreLibTranslated = false`
- canonical coverage:
  - `requestedMethodCount = 167`
  - `emittedMethodCount = 158`
  - `uncoveredMethodCount = 0`
- subject review bundle:
  - `artifacts/subjects/SolutionCorePack/runs/20260424-083326-windows-9222/declared/unit/corelib-reference-hotupdate-proof/review-bundle.json`
- direct declared-unit summary:
  - `artifacts/subjects/SolutionCorePack/runs/20260424-083206-windows-1e3e/run-report/summary.json`

## Boundary

- 本 child 已完成其 narrower unblocker scope。
- source task `20260419-03` 仍保持 `completed-blocked`：
  - 当前 canonical truth boundary 仍是 narrow CoreLib proof packet
  - `truthBoundary.fullCoreLibTranslated = false`
  - 因此仍不能启动 Complex BCL 13 DLL lane

## Next

- handed back to parent roadmap / source task blocker
- wiki: n/a
