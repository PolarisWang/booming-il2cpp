# CoreLib DLL Boundary Native Codegen Implementation Design v1.01

## 1. Goal

Create the first real DLL-boundary implementation path for `System.Private.CoreLib` full verification.

The minimum viable implementation should produce:

- a CoreLib nativeization/codegen inventory,
- generated file or manifest evidence at the DLL boundary,
- a native proof report input,
- a hotupdate proof packet input or a concrete implementation blocker.

## 2. Current Known Gap

Existing generated native artifacts are entry-driven:

- sample run `20260418-100526-windows-01a7`
- `assemblyName = CoreRuntimeFeatures`
- `entrySubjectId = CoreRuntimeFeatures/InterfaceDispatchProofEntry::Run()`

Therefore they cannot close CoreLib DLL-boundary codegen review.

## 3. Required First Slice

The first implementation slice should be small:

- add or extend a tooling command that reads the CoreLib semantic/nativeization inputs,
- emits a DLL-boundary inventory/report,
- updates CoreLib `codegen-review.json` from real generated/inventory data,
- leaves native execution and hotupdate execution as explicit next blockers if not yet implemented.

## 4. Non-Goals

- Do not claim CoreLib full verification complete in this task unless all three blockers are genuinely closed.
- Do not accept entry-driven native artifacts as DLL-boundary proof.

