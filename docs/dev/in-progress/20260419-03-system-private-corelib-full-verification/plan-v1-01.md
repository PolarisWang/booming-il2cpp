# System.Private.CoreLib Full Verification Plan v1.01

Status: completed-blocked

## Plan

- [x] Step 1: Inventory existing CoreLib substrate and semantic-family artifacts.
- [x] Step 2: Generate CoreLib audit packet skeleton from existing evidence.
- [x] Step 3: Add tests that validate CoreLib audit packet shape against Phase 0 schemas.
- [x] Step 4: Fill managed/native/hotupdate proof report links or blockers.
- [x] Step 5: Generate codegen review JSON and Markdown skeletons.
- [x] Step 6: Run required registry/workspace/proof/build validations available in current repo.
- [x] Step 7: Mark completion only if no blockers remain; otherwise create blocker child tasks.

## Blocker Child Tasks

- [ ] `20260419-04-corelib-codegen-review-inventory`: produce a real generated-file inventory and codegen review input set for CoreLib.
- [ ] `20260419-05-corelib-full-native-proof-normalization`: normalize full native proof requirements and execution evidence.
- [ ] `20260419-06-corelib-hotupdate-proof-normalization`: normalize CoreLib-specific hotupdate proof evidence.

## Acceptance

- CoreLib audit packet exists and is schema-valid.
- CoreLib completion report does not overclaim.
- Every missing proof/codegen/hotupdate requirement is either satisfied or recorded as a blocker.
