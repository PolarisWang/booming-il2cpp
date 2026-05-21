# Progress v1.02

Date: 2026-04-07 11:25:00 +08:00

## Summary

User clarified that the migration scope is all projects under `tests/smoke/input/`, not just `GenericEcho`.

Completed in this update:
- Added canonical subject/source trees for `HelloWorld`, `GenericEcho`, `ReflectionLite`, `PInvokeLite`, and `HostEmbeddingLite`.
- Added shared `subjects/Directory.Build.props` so these migrated subjects build into `artifacts/smoke/`.
- Switched smoke suite manifests and smoke command entries in `build/toolchains/run/run_manifest.json` to `subjects/*/source`.
- Updated subject workers and registry/planner tests to understand the migrated subject set.

## Verification

- `python -m unittest tests.unit.run.test_subject_manifest_schema tests.unit.run.test_subject_planner tests.unit.run.test_subject_workers tests.integration.run.test_registry_scan tests.tooling.run.test_command_manifest tests.tooling.run.test_dotnet_bootstrap`
- `python -m unittest tests.tooling.run.test_cmake_bootstrap tests.tooling.run.test_subject_command`
- `dotnet build` succeeded for:
- `subjects/HelloWorld/source/HelloWorld.csproj`
- `subjects/GenericEcho/source/GenericEcho.csproj`
- `subjects/ReflectionLite/source/ReflectionLite.csproj`
- `subjects/PInvokeLite/source/PInvokeLite.csproj`
- `subjects/HostEmbeddingLite/source/HostEmbeddingLite.csproj`
- Direct DLL execution matched expected smoke output for all five migrated subjects.

## Remaining

- The original Stage F perf/reporting items are still open and should be handled as follow-up work, not as part of the completed smoke-input migration correction.
