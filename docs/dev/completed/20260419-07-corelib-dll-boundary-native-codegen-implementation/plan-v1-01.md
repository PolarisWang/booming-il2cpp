# CoreLib DLL Boundary Native Codegen Implementation Plan v1.01

Status: completed

## Plan

- [x] Step 1: Inspect current codegen/planner/tooling architecture.
- [x] Step 2: Add failing tests for the first DLL-boundary reachability slice (`full-assembly-closure` convert mode).
- [x] Step 3: Implement `full-assembly-closure` propagation through request, loader, semantic world, linker, preserve descriptor, and closure manifest.
- [x] Step 4: Run real convert evidence on subject/CoreLib-facing inputs and capture artifact review outputs.
- [x] Step 5: Split the next blocker slice for assembly-bound native lowering/emitter conversion into executable translation-unit implementation.
- [x] Step 6: Re-run expanded audit/schema/subject verification after the next blocker slice lands.

## Acceptance

- There is a real CoreLib DLL-boundary inventory source.
- CoreLib codegen review no longer relies only on entry-driven artifact inspection.
- CoreLib completion remains blocked unless native proof and hotupdate proof are actually closed.

## Current Note

- The completed slice in this task is narrower than full DLL-boundary nativeization: it upgrades linker/preserve analysis from entry-driven closure to optional full input-assembly closure.
- The completed follow-up slice upgrades full-assembly-closure codegen output from placeholder entry plans to assembly-bound audit plans carrying translation-unit method inventories.
- This plan is complete as a narrower unblocker:
  - actual assembly-bound lowering/emission is no longer only `EntrySubjectId`-anchored
  - the real subject execution path now emits a consolidated review bundle
  - source task `20260419-03` still remains `completed-blocked` because current truth boundary is still a narrow CoreLib proof packet rather than full DLL-body coverage
