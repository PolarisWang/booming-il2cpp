#!/usr/bin/env python3
"""ci_test.py — single CI test entry point.

Delegates every tier to the unified test driver (tests/runner/test_driver.py),
which consolidates the previously-duplicated `dotnet build` + `dotnet test`
commands across the GitHub Actions workflows and correctly gates on known
failures (NEVER skipping — failures surface, only unexpected ones fail).

A workflow invokes this with a preset to run the tier it needs, replacing N
copies of the same build+test steps in the YAML with one step.

Usage:
    python scripts/ci_test.py --preset managed-codegen
    python scripts/ci_test.py --preset managed-full
    python scripts/ci_test.py --preset native-contracts
    python scripts/ci_test.py --preset foundation-smoke
    python scripts/ci_test.py --preset all

Exit 0 on success (known failures surfaced but allowed), nonzero unexpectedly.
"""
from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def _run(args: list[str], label: str) -> int:
    print(f"\n[ci_test] {label}: python tests/runner/test_driver.py {' '.join(args)}")
    p = subprocess.run(["python", "tests/runner/test_driver.py"] + args, cwd=ROOT)
    print(f"[ci_test] {'ok' if p.returncode == 0 else 'FAILED'} (rc={p.returncode}): {label}")
    return p.returncode


PRESETS = {
    # unit tier: snapshot + codegen (the historical codegen-regression core)
    "managed-codegen": [
        (["--layer", "unit", "--group", "snapshot"], "unit/snapshot"),
        (["--layer", "unit", "--group", "codegen"], "unit/codegen"),
    ],
    # full unit tier: all three dotnet test projects
    "managed-full": [(["--layer", "unit"], "unit")],
    # native integration tier (CTest under build/native; CI preconfigures)
    "native-contracts": [(["--layer", "integration", "--group", "contracts-native", "--quick"],
                          "integration/contracts-native")],
    # python e2e smoke (foundation-dll)
    "foundation-smoke": [(["--layer", "e2e", "--group", "foundation-dll-smoke"],
                          "e2e/smoke")],
}


def main() -> int:
    ap = argparse.ArgumentParser(description="Chaos IL2CPP CI test entry")
    ap.add_argument("--preset", required=True, choices=list(PRESETS) + ["all"])
    ar = ap.parse_args()

    presets = list(PRESETS) if ar.preset == "all" else [ar.preset]
    failures = 0
    for p in presets:
        for args, label in PRESETS[p]:
            failures += _run(args, f"{p}/{label}")
    print(f"\n[ci_test] OVERALL: {'OK' if failures == 0 else '%d step(s) failed' % failures}")
    return 0 if failures == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
