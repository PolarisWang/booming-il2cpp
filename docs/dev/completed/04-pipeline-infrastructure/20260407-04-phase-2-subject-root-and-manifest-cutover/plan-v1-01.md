# Phase 2 Subject Root And Manifest Cutover Plan v1.01

Date: 2026-04-07
Status: completed-first-slice

## Tasks

- [x] Add subject content and perf baseline resolver helpers
- [x] Extend subject manifests with typed validation/profile/expected/baselines fields
- [x] Land subject-local xUnit and perf harness samples
- [x] Update focused repo-layout / manifest / perf tests

## Verification

- [x] `python -m unittest tests.unit.run.test_path_resolver`
- [x] `python -m unittest tests.unit.run.test_subject_manifest_schema`
- [x] `python -m unittest tests.unit.run.test_subject_perf_policy`
- [x] `python -m unittest tests.unit.run.test_subject_workers_perf`
- [x] `python -m unittest tests.unit.run.test_subject_reporting`
- [x] `python -m unittest tests.unit.run.test_repo_layout`
- [x] `python -m unittest tests.tooling.run.test_subject_command`
