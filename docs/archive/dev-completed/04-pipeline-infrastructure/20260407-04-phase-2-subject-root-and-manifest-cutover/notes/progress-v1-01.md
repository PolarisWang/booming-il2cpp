# Progress v1.01

Date: 2026-04-07 14:55:56 +08:00

## Completed

- Added subject-local validation/profile/expected/baselines manifest fields.
- Added subject-local perf baseline resolution.
- Added HelloWorldObject unit-test canonical path and GenericEcho perf harness/baselines.

## Verification

- `python -m unittest tests.unit.run.test_path_resolver`
- `python -m unittest tests.unit.run.test_subject_manifest_schema`
- `python -m unittest tests.unit.run.test_subject_perf_policy`
- `python -m unittest tests.unit.run.test_subject_workers_perf`
- `python -m unittest tests.unit.run.test_subject_reporting`
- `python -m unittest tests.unit.run.test_repo_layout`
- `python -m unittest tests.tooling.run.test_subject_command`
