---
task_id: 20260419-04-corelib-codegen-review-inventory
title: CoreLib Codegen Review Inventory
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-19 13:15:00 +08:00
updated_at: 2026-04-19 13:30:00 +08:00
latest_stop_point: completed CoreLib codegen review inventory finding; existing generated artifacts are entry-driven and cannot close DLL-boundary codegen review
current_dir: docs/dev/in-progress/20260419-04-corelib-codegen-review-inventory
parent_task_id: 20260419-03-system-private-corelib-full-verification
source_task_id: 20260419-03-system-private-corelib-full-verification
source_relation: blocker-child-corelib-codegen-review-missing
design_doc: docs/dev/in-progress/20260419-04-corelib-codegen-review-inventory/design-v1-01.md
plan_doc: docs/dev/in-progress/20260419-04-corelib-codegen-review-inventory/plan-v1-01.md
total_tasks: 5
current_task: completed
active: false
---

## Scope

This task addresses only `corelib-codegen-review-missing`.

It does not close:

- `corelib-full-native-proof-missing`
- `corelib-hotupdate-proof-missing`

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `FrameworkPackNativeization`
- capabilityItem: `CoreLibCodegenReviewInventory`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `false`
- hotupdateImpact: `Proof`
- formalVerificationObjects:
  - CoreLib `codegen-review.json`
  - CoreLib `codegen-review.md`
  - generated native file inventory
- requiredGates:
  - `tests/contracts/shared/test_system_private_corelib_audit_packet.py`
  - generated file inventory validation

## Next Step

- Run targeted tests and keep CoreLib completion blocked until a real DLL-boundary generated inventory exists.

## Current Finding

- Inspected run: `artifacts/subjects/SolutionCorePack/runs/20260418-100526-windows-01a7`.
- `native-aot.plan.json` has `assemblyName = CoreRuntimeFeatures`.
- `entrySubjectId = CoreRuntimeFeatures/InterfaceDispatchProofEntry::Run()`.
- Conclusion: existing generated artifact is entry-driven and cannot close `System.Private.CoreLib` DLL-boundary codegen review.

## Verification

- `python -m pytest tests/contracts/shared/test_system_private_corelib_audit_packet.py tests/contracts/shared/test_foundation_dll_translation_audit_schema.py -q`
  - Result: `5 passed`
