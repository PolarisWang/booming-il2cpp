# Repo-Hygiene Dashboard

> Last run: 2026-08-28T03:25:31Z · overall: **FAIL**

| check | status | note |
|-------|--------|------|
| root-clean | FAIL | [repo-clean] new untracked file at repo root: analyze_r19.py �� add to .gitignore, commit  |
| disk-health | PASS | === [repo-clean --disk] ignored-disk usage (surfacing silent accumulation) === |
| generated-drift | PASS | [PASS] all registered generated files are up to date |
| completion-audit | PASS | [PASS] no suspected-incomplete functions found |
| scripts-hygiene | PASS | [PASS] scripts/ is free of one-shot scratch and build artifacts |

Run `python scripts/cleanliness/chaos_hygiene.py --report` to refresh.

