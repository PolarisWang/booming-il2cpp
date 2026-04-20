# System.Private.CoreLib Full Verification Design v1.01

## 1. Goal

Verify `System.Private.CoreLib` as a full DLL translation target, not as a representative semantic-family sample.

The result must be a CoreLib audit packet that can answer:

- What official runtime DLL was used as input.
- Which public/runtime surface was enumerated.
- Which members are semantically classified.
- Which runtime helpers are required and bound.
- Which managed/native/hotupdate proofs ran.
- Which generated native files were reviewed.
- Whether any blocker remains.

## 2. Completion Model

`System.Private.CoreLib` is complete only if all required reports exist and the final completion report is `completed`.

If a gap exists, this task must produce a blocker ledger and split follow-up children. It must not downgrade the gate to the old semantic-family batch result.

## 3. Audit Packet Target

Target location:

```text
docs/dev/in-progress/20260419-03-system-private-corelib-full-verification/audit/System.Private.CoreLib/
```

Expected files:

- `input-manifest.json`
- `surface-ledger.json`
- `semantic-ledger.json`
- `nativeization-plan.json`
- `managed-proof-report.json`
- `native-proof-report.json`
- `hotupdate-proof-report.json`
- `benchmark-report.json`
- `codegen-review.json`
- `codegen-review.md`
- `completion-report.json`

## 4. Evidence Sources

Existing evidence may be imported or referenced from:

- `docs/dev/completed/20260418-04-phase-2-system-private-corelib-certification-substrate/`
- `docs/dev/completed/20260418-05-phase-3-system-private-corelib-semantic-family-nativeization/`
- `subjects/SolutionCorePack/source/Proofs/CoreRuntimeFeatures/`
- `subjects/HotUpdateHostPack/`
- `artifacts/tests/registry/current/index.json`

Any imported evidence must be normalized into the Phase 0 audit schema.

## 5. Non-Goals

- Do not claim full completion from existing phase2/phase3 artifacts alone.
- Do not silently mark hotupdate as not applicable.
- Do not generate codegen review Markdown without the paired JSON report.

