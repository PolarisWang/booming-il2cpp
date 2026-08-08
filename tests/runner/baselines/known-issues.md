# Known Infrastructure Issues (not test failures)

This file documents known infrastructure gaps in the test toolchain that are tracked
but NOT test failures — tests/suites affected are "not run, not skipped" (excluded from
the unified test entry until the underlying gap is closed). See also the per-layer
`known-failures.{layer}.yaml` baseline files for test-level known failures.

## Current known infrastructure gaps

| # | Component | Gap | Status | Tracker |
|---|-----------|-----|--------|---------|
| 1 | `tests/unit/managed/Chaos.IL2CPP.Tests.MetadataWriter` | Declares `[Fact]`s but lacks `Microsoft.NET.Test.Sdk`, so `dotnet test` cannot actually execute it. Excluded from the `unit` layer ("not run, not skipped"). | Open (records no coverage) | `tests/suite_contract.yaml` unit-layer note |

## Resolution policy

- A gap that is closed (SDK added, or project retired) should be removed from this file.
- Keep entries short and reference the owning config/suite so it is discoverable.
