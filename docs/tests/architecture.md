# Chaos IL2CPP Test Architecture

> Unified test entry for the whole repo. Single command, three tiers, one report.
> Updated 2026-08-04 as part of the test-pyramid consolidation (方案 C).

## The pyramid

The suite is organized as a three-tier pyramid. Every tier exposes a uniform
"run a group, get a SuiteResult" contract and reports into one driver.

| Tier | Adapter | Contents | Where | Speed |
|------|---------|----------|-------|-------|
| **unit** | dotnet | CodeGen.Tests (2143), Driver.Tests (26), SnapshotTests (108) | tests/managed/, tests/snapshots/ | fast |
| **integration** | native | CTest targets: abi/bridge/common/engine-binding/gc/hot-update/interpreter/runtime-core | tests/contracts/native/** | slow (cmake+ctest) |
| **e2e** | python | foundation-dll chunk translation + ci_smoke | testing/foundation-dll/verification/ | very slow |

Separate "managed/unit" Python tests live under `tests/unit/` (pytest-style);
they are a fourth internal zone not yet under the dotnet/native/python adapters.

## Entry points

Developer — unified driver (recommended):
```bash
# all three tiers (unit is fast, integration/e2e slow)
python tests/runner/test_driver.py --layer all --quick

# just the unit tier (fast, ~50s)
python tests/runner/test_driver.py --layer unit

# a single group
python tests/runner/test_driver.py --layer unit --group snapshot
```

CI — `ci_test.py` preset wrapper (delegates to the driver):
```bash
python scripts/ci_test.py --preset managed-codegen
python scripts/ci_test.py --preset managed-full
python scripts/ci_test.py --preset native-contracts
python scripts/ci_test.py --preset foundation-smoke
python scripts/ci_test.py --preset all
```

Both produce `tests/runner/test-report.json` (and `--junit` for CI).

## Contract

`tests/suite_contract.yaml` is the single source of truth: it declares layers,
each layer's adapter, and each group (path / cmake root / script). The driver
reads it; add a test by adding a group entry, not by touching driver code.

## No-skip rule (project requirement)

There is **no "skip" state**. Every test either PASSES or FAILS. Tests that are
currently failing for a known reason are listed in
`tests/runner/baselines/known-failures.{layer}.yaml`. The driver RUNS them and
reports them as **FAIL**, labeled `[KNOWN-FAIL]`; a group whose only failures are
known ones still returns exit 0 for gating, but the failures are fully visible
in the report. When a known failure is fixed, delete its baseline entry and the
gate fails if it regresses.

Current known failures (unit):
- 28 in CodeGen.Tests AotCoreIrLoweringTests (`ResolveAbiSlot_*`, `UnknownOpWarning_*`)
- order-dependent flakes: fixture 70 methods.json (`defaultStringId`) and
  `CreatePseudoMetadataHandle_ReturnsNonZero` — pass in isolation, fail in full
  suite due to shared static codegen state.

## Adapters

`tests/runner/adapters/{dotnet,native,python}.py` each implement
`run(group_or_contract, timeout, quick) -> SuiteResult`:
- **dotnet**: `dotnet test --logger trx`; parses summary + failing test names.
- **native**: `cmake --build <dir>` + `ctest --output-on-failure`; parses Test # lines.
- **python**: spawns the pipeline script; exit code = pass/fail.

`SuiteResult` (model.py) = { layer, group, passed, failed, total, cases, error }.

## How to add a test

1. Put the test in the right tier dir (a new xunit project under
   `tests/managed/`, a new CTest target under `tests/contracts/native/`, or a
   new verification script).
2. Register it: add a `groups:` entry in `tests/suite_contract.yaml`.
3. If it's currently red for a known reason, add its name to the matching
   `baselines/known-failures.{layer}.yaml`.

## Notes / exclusions

- `testing/src/managed/Chaos.IL2CPP.Tests.MetadataWriter` declares `[Fact]`s but
  lacks a test SDK package — not runnable via `dotnet test` (infra gap; not run,
  not skipped).
- `testing/project-test/**` is a manual-orchestration integration tier (CMake +
  codegen + hot-update) with no standalone script; fold into the driver once a
  stable run entry exists.
- Test-FRAMEWORK support projects are NOT tests and are excluded from the
  driver: `src/reference/Chaos.TestFramework.{Runtime,Sdk}`,
  `src/tools/*.{AutoTestGenerator,TestFramework.CollectionGen,TestProjectGenerator}`.
- `third_party/scriban/src/Scriban.Tests*` are vendored upstream tests, excluded.
