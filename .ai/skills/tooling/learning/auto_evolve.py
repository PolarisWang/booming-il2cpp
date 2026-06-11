#!/usr/bin/env python3
"""Auto-evolution pipeline: health check → propose → benchmark → review → promote.

Runs the full skill evolution pipeline automatically:
1. health_engine.py check → generate snapshot
2. evolve.py auto-evolve → generate proposals
3. For each proposal: benchmark → review → promote (if both pass)

Designed to be run via cron schedule.
"""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parent.parent
PYTHON = sys.executable or "python"


def run(cmd: list[str], timeout: int = 120) -> subprocess.CompletedProcess:
    return subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, cwd=REPO_ROOT)


def main() -> int:
    print(f"[auto-evolve] Starting pipeline at {datetime.now(timezone.utc).isoformat()}")

    # Step 1: Health check
    print("[auto-evolve] Step 1: Health check...")
    result = run([PYTHON, "skills/tooling/learning/health_engine.py", "check", "--window", "30"])
    if result.returncode != 0:
        print(f"[auto-evolve] Health check warning: {result.stderr[:200]}")
    print(result.stdout[-500:] if len(result.stdout) > 500 else result.stdout)

    # Step 2: Check if there are evolution candidates
    print("[auto-evolve] Step 2: Checking evolution proposals...")
    result = run([PYTHON, "skills/tooling/learning/evolve.py", "propose"])
    print(result.stdout[-500:] if len(result.stdout) > 500 else result.stdout)

    # Step 3: Auto-evolve (generate proposals if thresholds met)
    print("[auto-evolve] Step 3: Auto-evolve...")
    result = run([PYTHON, "skills/tooling/learning/evolve.py", "auto-evolve"], timeout=300)
    if result.returncode != 0:
        print(f"[auto-evolve] Auto-evolve skipped or had no candidates: {result.stderr[:200]}")
    print(result.stdout[-500:] if len(result.stdout) > 500 else result.stdout)

    # Step 4: Check for pending proposals and try to process them
    proposals_dir = REPO_ROOT / "skills" / "lifecycle" / "evolution" / "proposals"
    if proposals_dir.exists():
        proposals = sorted(proposals_dir.iterdir()) if proposals_dir.is_dir() else []
        if proposals:
            print(f"[auto-evolve] Step 4: Found {len(proposals)} proposal(s), processing...")
            for prop in proposals:
                prop_id = prop.name
                print(f"  Processing: {prop_id}")

                # Benchmark
                result = run([PYTHON, "skills/tooling/learning/skill_learn.py", "evolve-benchmark", prop_id], timeout=300)
                if result.returncode != 0:
                    print(f"  Benchmark failed for {prop_id}, skipping promote")
                    continue

                # Review
                result = run([PYTHON, "skills/tooling/learning/skill_learn.py", "evolve-review", prop_id], timeout=120)
                if result.returncode != 0:
                    print(f"  Review failed for {prop_id}, skipping promote")
                    continue

                # Promote
                result = run([PYTHON, "skills/tooling/learning/skill_learn.py", "evolve-promote", prop_id], timeout=120)
                if result.returncode == 0:
                    print(f"  ✅ Promoted: {prop_id}")
                else:
                    print(f"  ❌ Promote failed for {prop_id}: {result.stderr[:200]}")
        else:
            print("[auto-evolve] Step 4: No proposals to process")
    else:
        print("[auto-evolve] Step 4: Proposals directory not found")

    print(f"[auto-evolve] Pipeline complete at {datetime.now(timezone.utc).isoformat()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
