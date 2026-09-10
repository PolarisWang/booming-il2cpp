# Progress v1.03

Date: 2026-04-07 01:42:14 +08:00

## Summary

Completed the remaining Stage F closeout work:

- Fixed the last `run` suite regressions after the smoke-subject migration.
- Normalized fingerprint path output to stable POSIX text.
- Updated `verify-roadmap-0.py` to build smoke inputs from `subjects/*/source` and kept canonical trace-contract literals in both Python and PowerShell wrappers.
- Fixed `tests/proof/input/HelloWorldObject/HelloWorldObject.csproj` to reference the canonical `subjects/HelloWorldObject/source` path correctly.
- Stabilized Stage 5A/5B Windows proof-run tests by preferring the existing `VsDevCmd + cl` fallback path when available, avoiding Visual Studio CMake path-length overflow.
- Fixed Windows collection of `tests/tooling/run/test_wrapper_bootstrap_unix.py` so non-POSIX hosts skip it instead of failing at import time.

## Verification

- `python -m unittest discover -s tests/unit/run -p test_*.py`
- `python -m unittest discover -s tests/integration/run -t . -p test_*.py`
- `python -m unittest discover -s tests/tooling/run -t . -p test_*.py`

## Outcome

- `Stage F` is complete.
- `GenericEcho` is the second formal subject.
- All projects under `tests/smoke/input/` now have canonical homes under `subjects/*/source`.
- Subject-aware perf baseline/reporting is live for the new subject flow.
