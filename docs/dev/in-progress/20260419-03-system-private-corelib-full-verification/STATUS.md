---
task_id: 20260419-03-system-private-corelib-full-verification
title: System.Private.CoreLib Full Verification
task_type: plan
lifecycle_status: completed
phase: completed-blocked
created_at: 2026-04-19 12:40:00 +08:00
updated_at: 2026-04-19 13:55:00 +08:00
latest_stop_point: completed CoreLib audit skeleton and blocker normalization; full verification remains blocked pending real DLL-boundary native/codegen/hotupdate implementation
current_dir: docs/dev/in-progress/20260419-03-system-private-corelib-full-verification
parent_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_task_id: 20260419-01-foundation-dll-translation-audit-roadmap
source_relation: roadmap-child-phase-1-system-private-corelib-full-verification
design_doc: docs/dev/in-progress/20260419-03-system-private-corelib-full-verification/design-v1-01.md
plan_doc: docs/dev/in-progress/20260419-03-system-private-corelib-full-verification/plan-v1-01.md
total_tasks: 7
current_task: completed-blocked
active: false
---

## Start Point

- Phase 0 is complete: audit/codegen schemas exist and `FoundationDllTranslationSolution` builds.
- This task must verify `System.Private.CoreLib` under scheme C.
- Current known state: previous CoreLib work covered substrate and first semantic-family batch, but not the strong full-verification gate.

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `FrameworkPackNativeization`
- capabilityItem: `SystemPrivateCoreLibFullVerification`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `Proof`
- formalVerificationObjects:
  - `System.Private.CoreLib` audit packet
  - CoreLib surface ledger
  - CoreLib semantic ledger
  - CoreLib runtime helper coverage
  - CoreLib managed proof report
  - CoreLib native proof report
  - CoreLib hotupdate proof report
  - CoreLib codegen review JSON
  - CoreLib codegen review Markdown
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`
  - `managed-proof`
  - `native-proof`
  - `hotupdate-proof`
  - `benchmark`
  - `codegen-json-review`
  - `codegen-markdown-review`

## Immediate Next Step

- Split blocker remediation tasks for full native proof, codegen review, and CoreLib-specific hotupdate proof.

## Current Evidence Summary

- Surface ledger evidence exists from `20260418-04`:
  - `net8.0`: `2045` public types, `26154` public members, `0` unresolved public types.
  - `net10.0`: `2276` public types, `32489` public members, `0` unresolved public types.
- Semantic ledger evidence exists from `20260418-04` and `20260418-05`:
  - `net8.0`: `26154/26154` public members classified.
  - `net10.0`: `32489/32489` public members classified.
  - Families: `managed-cil`, `internal-call`, `runtime-special`, `pinvoke`, `field-metadata`, `abstract`.
- Existing semantic-family execution entry is only `corelib-semantic-family-batch-01`, covering selected families `managed-cil`, `internal-call`, and `field-metadata`.

## Current Blockers

- `corelib-full-native-proof-missing`: no normalized full CoreLib native proof report exists.
- `corelib-codegen-review-missing`: no full generated native file inventory and codegen review exists.
- `corelib-hotupdate-proof-missing`: no CoreLib-specific hotupdate proof packet exists.

## Blocker Child Split

- `20260419-04-corelib-codegen-review-inventory`: produce a real generated-file inventory and codegen review input set for CoreLib.
- `20260419-05-corelib-full-native-proof-normalization`: normalize full native proof requirements and execution evidence.
- `20260419-06-corelib-hotupdate-proof-normalization`: normalize CoreLib-specific hotupdate proof evidence.

## Closeout Conclusion

- This task completed the audit skeleton and blocker normalization required by the adjusted roadmap.
- `System.Private.CoreLib` is not full-verified yet.
- The current authoritative result is `completion-report.json.finalStatus = blocked`.
- The next implementation task must build a real DLL-boundary path for CoreLib generated code, native proof, and CoreLib-specific hotupdate proof before Complex BCL 13 DLL execution can start.

## Verification

- `python -m pytest tests/contracts/shared/test_system_private_corelib_audit_packet.py tests/contracts/shared/test_foundation_dll_translation_audit_schema.py tests/contracts/shared/test_subject_authority_freeze.py tests/unit/compatibility/test_solution_core_pack_subject.py -q`
  - Result: `17 passed`
- `dotnet build subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/FoundationDllTranslationSolution.sln -c Release`
  - Result: build succeeded, `0` warnings, `0` errors
