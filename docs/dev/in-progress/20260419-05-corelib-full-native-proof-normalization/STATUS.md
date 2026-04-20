---
task_id: 20260419-05-corelib-full-native-proof-normalization
title: CoreLib Full Native Proof Normalization
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-04-19 13:30:00 +08:00
updated_at: 2026-04-19 13:40:00 +08:00
latest_stop_point: completed native proof requirement normalization; CoreLib requires DLL-boundary evidence across six semantic families
current_dir: docs/dev/in-progress/20260419-05-corelib-full-native-proof-normalization
parent_task_id: 20260419-03-system-private-corelib-full-verification
source_task_id: 20260419-03-system-private-corelib-full-verification
source_relation: blocker-child-corelib-full-native-proof-missing
design_doc: docs/dev/in-progress/20260419-05-corelib-full-native-proof-normalization/design-v1-01.md
plan_doc: docs/dev/in-progress/20260419-05-corelib-full-native-proof-normalization/plan-v1-01.md
total_tasks: 5
current_task: completed
active: false
---

## Scope

This task addresses only `corelib-full-native-proof-missing`.

It does not close:

- `corelib-codegen-review-missing`
- `corelib-hotupdate-proof-missing`

## Next Step

- Continue with `20260419-06-corelib-hotupdate-proof-normalization`.

## Closeout

- Updated CoreLib `native-proof-report.json` with `requiredProofKind = dll-boundary`.
- Listed six semantic-family requirements: `managed-cil`, `internal-call`, `runtime-special`, `pinvoke`, `field-metadata`, `abstract`.
- Set `entryDrivenArtifactsAccepted = false`.
- Kept CoreLib completion `blocked`.

## Verification

- `python -m pytest tests/contracts/shared/test_system_private_corelib_audit_packet.py -q`
  - Result: `4 passed`
