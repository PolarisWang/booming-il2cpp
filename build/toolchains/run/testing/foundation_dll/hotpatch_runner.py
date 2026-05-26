"""Run Hotpatch verification using il2cpp-translated entry EXE directly.

No C++ host compilation needed — the entry EXE is already a native executable
translated by il2cpp. Run it with --hotupdate to exercise the hotpatch path.

Usage:
  python hotpatch_runner.py <family-slug>
  python hotpatch_runner.py --all
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION = _REPO_ROOT / "testing" / "foundation-dll" / "System.Private.CoreLib"


def _locate_entry_exe(family_slug: str) -> Path | None:
    """Find the native entry EXE in native/entry.exe"""
    family_dir = _VERIFICATION / family_slug
    candidate = family_dir / "native" / "entry.exe"
    return candidate if candidate.exists() else None


def build_and_run(family_slug: str) -> dict:
    """Run the Hotpatch verification for a single family using entry EXE."""
    family_dir = _VERIFICATION / family_slug
    result = {"family": family_slug, "success": False}

    exe_path = _locate_entry_exe(family_slug)
    if exe_path is None:
        result["error"] = "Entry EXE not found (run convert-to-cpp first)"
        return result

    print(f"  Running Hotpatch verification for {family_slug}...")
    t1 = time.time()
    try:
        r_run = subprocess.run(
            [str(exe_path), "--hotupdate"],
            capture_output=True, text=True, timeout=120,
            errors="replace",
        )
        run_elapsed = time.time() - t1
        result["run_elapsed"] = round(run_elapsed, 1)

        output = r_run.stdout
        json_start = output.find("{")
        if json_start >= 0:
            try:
                data = json.loads(output[json_start:])
                result["test_data"] = data
                passed = data.get("passedMethods", 0)
                failed = data.get("failedMethods", 0)
                total = data.get("totalMethods", 0)
                result["passed"] = passed
                result["failed"] = failed
                result["total"] = total
                result["success"] = failed == 0
                print(f"  {passed}/{total} passed, {failed} failed ({run_elapsed:.1f}s)")
            except json.JSONDecodeError:
                result["error"] = f"JSON parse failed: {output[:200]}"
                result["raw_output"] = output[:500]
        else:
            # Fall back to parsing "Passed: N/M" from output
            import re
            for line in output.splitlines():
                m = re.search(r'Passed:\s*(\d+)/(\d+)', line)
                if m:
                    passed, total = int(m.group(1)), int(m.group(2))
                    result["passed"] = passed
                    result["total"] = total
                    result["success"] = r_run.returncode == 0
                    print(f"  {passed}/{total} passed, {total - passed} failed ({run_elapsed:.1f}s)")
                    break
            else:
                result["error"] = f"No JSON or Passed output found, rc={r_run.returncode}"
                result["stderr"] = r_run.stderr[:300]
                print(f"  No output parsed (rc={r_run.returncode})")

    except subprocess.TimeoutExpired:
        result["error"] = "Test timed out (120s)"
        print(f"  TIMEOUT")
    except OSError as e:
        result["error"] = str(e)
        print(f"  OS error: {e}")

    return result


def main() -> None:
    parser = argparse.ArgumentParser(description="Hotpatch verification runner")
    parser.add_argument("family_slug", nargs="?", help="Family slug (e.g., convert-char)")
    parser.add_argument("--all", action="store_true", help="Run all families with entry EXEs")
    parser.add_argument("--json", action="store_true", help="Output results as JSON")
    args = parser.parse_args()

    if args.all:
        families = sorted(
            d.name for d in _VERIFICATION.iterdir()
            if d.is_dir()
            and _locate_entry_exe(d.name) is not None
            and d.name != "reports"
        )
        print(f"Running Hotpatch verification for {len(families)} families...")
    elif args.family_slug:
        families = [args.family_slug]
    else:
        parser.print_help()
        sys.exit(1)

    all_results = []
    all_passed = 0
    all_failed_count = 0

    for slug in families:
        r = build_and_run(slug)
        all_results.append(r)
        if r["success"]:
            all_passed += 1
        else:
            all_failed_count += 1
        print(f"  Status: {'PASS' if r['success'] else 'FAIL'} ({r.get('passed', 0)}/{r.get('total', 0)} passed)")

    total = len(families)
    print(f"\n{'='*60}")
    print(f"Hotpatch verification Summary: {all_passed} passed, {all_failed_count} failed, {total} total")
    print(f"{'='*60}")
    for r in all_results:
        status = "PASS" if r["success"] else "FAIL"
        detail = r.get("error", f"{r.get('passed', 0)}/{r.get('total', 0)} passed")
        print(f"  {status:4s}  {r['family']:35s}  {detail}")

    if args.json:
        print(json.dumps(all_results, indent=2))

    if all_failed_count > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()