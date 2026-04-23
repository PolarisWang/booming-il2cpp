# Verification

## Authority
- Archive: `verification/archive`
- Catalog: `verification/catalog`
- Evidence: `verification/evidence`
- Workspaces: `verification/workspaces`
- Projections: `verification/projections`

## Main Entries
- Testing inventory: `verification/projections/testing-inventory/inventory.html`
- Benchmark dashboard: `verification/projections/benchmark/dashboard.html`
- Total workspace: `verification/workspaces/verification-all.sln`

## Commands
- `python build/toolchains/run/run.py verify verification-v1 --json`
- `python build/toolchains/run/run.py test inventory --json`
- `python build/toolchains/run/run.py project all-workspaces --json`
