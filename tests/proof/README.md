# Proof Tree

`tests/proof/` no longer stores subject-owned proof inputs or native reference hosts.

Canonical locations:

- `subjects/<subject-id>/source/`
- `subjects/<subject-id>/validation/proof/`

If a proof flow still depends on a path under `tests/proof/`, that path is stale and must be migrated into the owning `subjects/<subject-id>/...` tree.
