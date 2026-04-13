# Tests Tree

Canonical test-owned roots:

- `tests/contracts/`: formal shared contracts and schemas
- `tests/fixtures/`: generic repository fixtures and registry manifests
- `tests/unit/`: mechanism-level unit coverage
- `tests/integration/`: cross-module and host integration coverage
- `tests/gate/`: temporary compatibility wrappers that forward to canonical integration hosts
- `tests/perf/`: temporary engineering workload baselines that have not been migrated yet

Subject-owned proof inputs, native validation hosts, and benchmark source do not live under `tests/`.
Those assets must stay under the owning `subjects/<subject-id>/...` tree.

Retired roots:

- `tests/proof/`
- `tests/registry/`
- `tests/platform/`
