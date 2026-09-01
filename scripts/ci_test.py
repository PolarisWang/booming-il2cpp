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
    python scripts/ci_test.py --preset jit-native
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


def _run_ctest(args: list[str], label: str) -> int:
    print(f"\n[ci_test] {label}: ctest {' '.join(args)}")
    p = subprocess.run(["ctest"] + args, cwd=ROOT, capture_output=True,
                       text=True, encoding="utf-8", errors="replace")
    tail = (p.stdout or "")[-300:] + (p.stderr or "")[-300:]
    print(f"[ci_test] {'ok' if p.returncode == 0 else 'FAILED'} (rc={p.returncode}): {label}\n{tail}")
    return p.returncode


# Each preset = list of (kind, args, label). kind "driver" → test_driver.py,
# kind "ctest" → ctest directly (for jit subsets that need their own -R filter,
# which the full integration group does not express).
PRESETS = {
    # unit tier: snapshot + codegen (the historical codegen-regression core)
    "managed-codegen": [
        ("driver", ["--layer", "unit", "--group", "snapshot"], "unit/snapshot"),
        ("driver", ["--layer", "unit", "--group", "codegen"], "unit/codegen"),
    ],
    # full unit tier: all three dotnet test projects
    "managed-full": [("driver", ["--layer", "unit"], "unit")],
    # native integration tier (CTest under build/native; CI preconfigures)
    "native-contracts": [("driver", ["--layer", "integration", "--group", "contracts-native", "--quick"],
                          "integration/contracts-native")],
    # JIT regression subset — the T4 JIT test cluster the workflows run via raw
    # ctest -R "test_jit_*". Centralized here (was scattered inline in YAML).
    # Build dir matches the integration group's cmake_build_dir (root reference preset).
    "jit-native": [("ctest",
                    ["--test-dir", "artifacts/presets/windows-x64-reference", "-C", "Debug",
                     "-R", "test_jit_native|test_jit_abi|test_jit_il_smoke|test_jit_gc_slot_map|"
                           "test_jit_osr|test_jit_seh|test_jit_unwind|test_jit_binary_ir|"
                           "test_jit_mode|test_jit_pgo",
                     "--output-on-failure"],
                    "native/jit")],
    # python e2e smoke (foundation-dll)
    "foundation-smoke": [("driver", ["--layer", "e2e", "--group", "foundation-dll-smoke"],
                          "e2e/smoke")],
    # native GC gate: the deterministic GC unit/integration group (labels unit;gc),
    # excluding the heavy stress/benchmark/soak tiers.  Wire the group via
    # suite_contract.yaml's contracts-native group + a ctest -LE filter that drops
    # only the long tiers so GC correctness is gated on every PR.
    "native-gc": [
        ("driver", ["--layer", "integration", "--group", "contracts-native-gc", "--quick", "--cases"],
         "integration/contracts-native-gc"),
    ],
    # GC stress/nightly gate: runs ONLY the stress|soak|benchmark tiers of the
    # native integration group.  --stress-only is REQUIRED here: it makes the driver
    # set CHAOS_GC_TEST_STRESS_ONLY=1, which flips the native adapter from ctest -LE
    # (exclude) to -L "stress|soak|benchmark" (only these).  Without this flag the
    # preset would silently run the fast excluded set instead of the stress tier.
    # Long-running; this is the nightly / on-demand reliability gate, never per-PR.
    "gc-stress": [("driver", ["--layer", "integration", "--group", "contracts-native",
                              "--stress-only"],
                   "integration/contracts-native stress")],
    # publish-smoke: matrix e2e for the publish path (app / jit / source-only).
    # Runs the repo-built Driver directly (dev mode) so it works even in environments
    # where the standalone embedded-SDK build has toolchain gaps.  Triggered on
    # push-to-main and release, not per-PR (codegen pipeline is the per-PR gate).
    "publish-smoke": [("driver", ["--layer", "e2e", "--group", "publish-smoke"],
                        "e2e/publish-smoke")],
}


def main() -> int:
    ap = argparse.ArgumentParser(description="Chaos IL2CPP CI test entry")
    ap.add_argument("--preset", required=True, choices=list(PRESETS) + ["all"])
    ar = ap.parse_args()

    presets = list(PRESETS) if ar.preset == "all" else [ar.preset]
    failures = 0
    for p in presets:
        for kind, args, label in PRESETS[p]:
            failures += _run_ctest(args, label) if kind == "ctest" else _run(args, label)
    print(f"\n[ci_test] OVERALL: {'OK' if failures == 0 else '%d step(s) failed' % failures}")
    return 0 if failures == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
