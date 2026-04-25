---
task_id: 20260419-03-system-private-corelib-full-verification
title: System.Private.CoreLib Full Verification
task_type: plan
lifecycle_status: completed
phase: completed-blocked
created_at: 2026-04-19 12:45:00 +08:00
updated_at: 2026-04-24 21:20:00 +08:00
latest_stop_point: `20260419-20` 已完成归档并把真实 `System.Private.CoreLib.dll` supplemental full-closure evidence 接入 canonical audit；但 full verification 仍 blocked，因为 `truthBoundary.fullCoreLibTranslated = false`，且 supplemental runtime-skeleton 仍有 `54310` 个 uncovered methods，下一步切换到 coverage widening child。
current_dir: docs/dev/completed/20260419-03-system-private-corelib-full-verification
parent_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_task_id:
source_relation: roadmap-child-phase-1
active: false
---

# 20260419-03 System.Private.CoreLib Full Verification

## Outcome

- CoreLib audit skeleton 与 blocker normalization 已完成。
- scoped unblocker `20260419-07-corelib-dll-boundary-native-codegen-implementation` 已完成：
  - 关闭新的 assembly-bound runtime-skeleton widening 切片
  - 关闭 subject-facing review bundle 缺口
  - 证明窄 CoreLib proof packet 的 auditable `native -> hotupdate` bridge
- Complex BCL 13 DLL lane 仍不能启动。

## Current Blocker

- 最新 canonical run:
  - `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/run-report/summary.json`
- 最新 canonical audit:
  - `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/matrices/windows-corelib-reference-native-hotupdate-proof/pipeline-report/report/native-hotupdate-audit.json`
- 当前 gate 仍未关闭：
  - `truthBoundary.coreLibScope = narrow-proof-packet-plus-supplemental-full-closure-evidence`
  - `truthBoundary.fullCoreLibTranslated = false`
  - supplemental `nativeReferenceTranslationUnitMethodCount = 59863`
  - supplemental `runtimeSkeletonUncoveredMethodCount = 54310`
- 当前已不再只有窄 `GoldenCoreLibReference.NativeProofApp` proof packet；现在已有真实 `System.Private.CoreLib.dll` supplemental full-closure evidence，但仍远未达到完整 `System.Private.CoreLib` DLL-body translated coverage。

## Supporting Evidence

- child closeout:
  - `docs/dev/completed/20260419-07-corelib-dll-boundary-native-codegen-implementation/STATUS.md`
- child closeout:
  - `docs/dev/completed/20260419-20-corelib-supplemental-full-closure-audit-integration/STATUS.md`
- narrow canonical coverage:
  - `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/analysis/generated/generated/runtime/native-reference.runtime-skeleton.coverage.json`
  - `requestedMethodCount = 167`
  - `emittedMethodCount = 158`
  - `uncoveredMethodCount = 0`
- supplemental canonical coverage:
  - `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/native-reference.runtime-skeleton.coverage.json`
  - `requestedMethodCount = 59863`
  - `emittedMethodCount = 495`
  - `uncoveredMethodCount = 54310`
- direct declared-unit success:
  - `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/declared/unit/corelib-reference-hotupdate-proof/summary.json`
- subject-facing review bundle:
  - `artifacts/subjects/SolutionCorePack/runs/20260424-200729-windows-23a1/declared/unit/corelib-reference-hotupdate-proof/review-bundle.json`

## Next

- 执行新的 phase-1 blocker child，针对真实 CoreLib supplemental runtime-skeleton coverage 做 widening / uncovered-family classification。
- 在 `fullCoreLibTranslated = false` 被实证关闭前，不得启动 Complex BCL lane。
