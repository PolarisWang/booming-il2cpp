#!/usr/bin/env python3
"""Foundation DLL Pipeline — auto-generate + build + test + dashboard refresh.

Canonical entry point for:
  1. Phase 1 auto-generation (gap_analyzer)
  2. Phase 2 handwritten stubs  (run_phase2)
  3. dotnet build all test projects
  4. dotnet test all test projects
  5. Refresh verification dashboard & projections

Returns exit code 0 on success, 1 on any failure.
"""

import subprocess
import sys
import time
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]
PHASE1 = REPO / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "gap_analyzer.py"
PHASE2 = REPO / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "run_phase2.py"
REFRESH = ["python", str(REPO / "build" / "toolchains" / "run" / "run.py"), "verify", "verification-v1", "--json"]


def run(cmd, label):
    print(f"\n=== {label} ===")
    t0 = time.time()
    r = subprocess.run(cmd, cwd=REPO, capture_output=True, text=True)
    elapsed = time.time() - t0
    if r.returncode != 0:
        print(f"FAILED ({elapsed:.1f}s)")
        print(r.stdout[-2000:] if r.stdout else "")
        print(r.stderr[-2000:] if r.stderr else "")
        return False
    print(f"OK ({elapsed:.1f}s)")
    return True


def main():
    errors = []

    # Step 1: Phase 1 — auto-generate
    if not run(["python", str(PHASE1), "--auto-generate", "--update-ledger"], "Phase 1 — auto-generate"):
        errors.append("Phase 1 failed")

    # Step 2: Phase 2 — handwritten stubs
    if not run(["python", str(PHASE2)], "Phase 2 — handwritten stubs"):
        errors.append("Phase 2 failed")

    # Step 3: Build all test projects
    proj_dirs = sorted(REPO.glob("verification/foundation-dll/*/*/test/*.Tests.csproj"))
    for csproj in proj_dirs:
        if not run(["dotnet", "build", str(csproj.parent), "--verbosity", "quiet"],
                   f"Build {csproj.parent.name}"):
            errors.append(f"Build failed: {csproj.relative_to(REPO)}")

    # Step 4: Test all test projects
    for csproj in proj_dirs:
        label = f"Test {csproj.parent.name}"
        t0 = time.time()
        r = subprocess.run(["dotnet", "test", str(csproj.parent), "--verbosity", "quiet"],
                           cwd=REPO, capture_output=True, text=True)
        elapsed = time.time() - t0
        output = r.stdout + r.stderr
        if "Failed!" in output:
            print(f"FAILED {label} ({elapsed:.1f}s)")
            errors.append(f"Test failed: {csproj.relative_to(REPO)}")
        elif "Passed!" in output or "Skipped!" in output:
            print(f"OK {label} ({elapsed:.1f}s)")
        else:
            print(f"UNKNOWN {label} ({elapsed:.1f}s)")
            errors.append(f"Unexpected test output: {csproj.relative_to(REPO)}")

    # Step 5: Refresh dashboard
    if not run(REFRESH, "Dashboard refresh"):
        errors.append("Dashboard refresh failed")

    print("\n" + "=" * 60)
    if errors:
        print(f"FAILURES ({len(errors)}):")
        for e in errors:
            print(f"  - {e}")
        sys.exit(1)
    else:
        print("All pipeline steps passed.")
        sys.exit(0)


if __name__ == "__main__":
    main()
