# Subject Cutover Mapping v1.01

Date: 2026-04-24 00:00:00 +08:00
Status: draft
Parent: `design-v1-01.md`

## 1. Scope

Current hard-cut owner packs:

- `subjects/HotUpdateHostPack`
- `subjects/MixedExecutionFeaturePack`
- `subjects/SolutionCorePack`

All three are treated as verification-framework migration input. They do not remain as long-term authority roots after cutover.

## 2. Final Landing

```text
verification/
  catalog/
    owners/
      <owner-id>/
        owner.manifest.json
        owner.features.json
        proofs/
        benchmarks/
        support/
          host/
          interpreter/
          patch/
          shared-contracts/
        benchmark-baselines/
    scenarios/
      <owner-id>/
        <scenario-id>/
  evidence/
    owners/
      <owner-id>/
        codegen-stubs/
  workspaces/
    subjects/
      <owner-id>/
        owner-workspace.manifest.json
        generated/
```

## 3. Physical Mapping

- `subjects/<owner>/subject.manifest.json`
  - -> `verification/catalog/owners/<owner>/owner.manifest.json`
- `subjects/<owner>/subject.features.json`
  - -> `verification/catalog/owners/<owner>/owner.features.json`
- `subjects/<owner>/source/Proofs/**`
  - -> `verification/catalog/owners/<owner>/proofs/**`
- `subjects/<owner>/source/Host/Proofs/**`
  - -> `verification/catalog/owners/<owner>/proofs/**`
- `subjects/<owner>/source/Benchmarks/**`
  - -> `verification/catalog/owners/<owner>/benchmarks/**`
- `subjects/<owner>/source/Host/Benchmarks/**`
  - -> `verification/catalog/owners/<owner>/benchmarks/**`
- `subjects/<owner>/source/EngineeringScenarios/**`
  - -> `verification/catalog/scenarios/<owner>/**`
- `subjects/<owner>/source/Host/**`
  - -> `verification/catalog/owners/<owner>/support/host/**`
- `subjects/<owner>/source/Interpreter/**`
  - -> `verification/catalog/owners/<owner>/support/interpreter/**`
- `subjects/<owner>/source/Patch/**`
  - -> `verification/catalog/owners/<owner>/support/patch/**`
- `subjects/<owner>/source/SharedContracts/**`
  - -> `verification/catalog/owners/<owner>/support/shared-contracts/**`
- `subjects/<owner>/baselines/perf/**`
  - -> `verification/catalog/owners/<owner>/benchmark-baselines/**`
- `subjects/<owner>/baselines/codegen/**`
  - -> `verification/evidence/owners/<owner>/codegen-stubs/**`
- `subjects/<owner>/verification/codegen-stubs/**`
  - -> `verification/evidence/owners/<owner>/codegen-stubs/**`

## 4. Benchmark Records

`benchmark-records` does not remain under `subjects/*`:

- `subjects/<owner>/benchmark-records/*`
  - raw execution records -> `.artifact/verification/benchmark-records/<owner>/*`
  - merged latest -> `verification/archive/latest/*`
  - merged master -> `verification/archive/master/*`
  - human-readable benchmark projection -> `verification/projections/benchmark/*`

## 5. Removed Instead Of Moved

These paths are deleted during cutover and do not get archived as canonical sources:

- `subjects/<owner>/source/bin/**`
- `subjects/<owner>/source/obj/**`
- `subjects/<owner>/source/Host/bin/**`
- `subjects/<owner>/source/Host/obj/**`
- all other build intermediates and cache directories

These paths no longer remain as public entry:

- `subjects/<owner>/source/*.sln`
- `subjects/<owner>/source/EngineeringScenarios/**/*.sln`

Replacement entry points:

- owner-level IDE facade -> `verification/workspaces/subjects/<owner>/generated/*`
- total IDE facade -> `verification/workspaces/verification-all.sln`

## 6. Pack-By-Pack Result

- `HotUpdateHostPack`
  - manifest / features / proofs / benchmarks / engineering scenarios / host support / patch support / shared contracts / perf baselines all move out
- `MixedExecutionFeaturePack`
  - manifest / features / proofs / benchmarks / engineering scenarios / host support / interpreter support / perf baselines all move out
- `SolutionCorePack`
  - manifest / features / proofs / benchmarks / engineering scenarios / host support / perf baselines / legacy codegen baselines / existing verification stubs all move out

## 7. End State

The end state is not a slimmed `subjects/*`.

- If no non-verification responsibility remains, the entire `subjects/<owner>/` directory exits mainline.
- For the three current owner packs, the default expectation is full directory removal after hard cut.
