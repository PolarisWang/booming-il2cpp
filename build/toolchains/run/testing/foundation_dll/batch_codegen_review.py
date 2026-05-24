"""Batch codegen review runner for native-aot pipeline.

Runs ai_codegen_reviewer.py across all families in an assembly,
producing a summary report of PASS/WARN/FAIL per family.
"""
from __future__ import annotations

import json
import subprocess
import sys
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_REVIEWER = _HERE / "ai_codegen_reviewer.py"
_VERIFICATION_BASE = _REPO_ROOT / "testing" / "foundation-dll"

REVIEW_TYPES = ["codegen", "hotupdate"]


def run_one_family(family_slug: str, assembly_name: str, review_type: str = "codegen") -> dict:
    """Run codegen review for one family, return result dict."""
    family_dir = _VERIFICATION_BASE / assembly_name / family_slug
    genuine_dir = family_dir / "native" / "genuine" / "generated"

    csil_path = genuine_dir / "native-aot.il-dump.txt"
    cpp_path = genuine_dir / "native-aot.generated.cpp"

    if not csil_path.exists():
        return {"family": family_slug, "verdict": "SKIP", "issues": ["no csil dump"]}
    if not cpp_path.exists():
        return {"family": family_slug, "verdict": "SKIP", "issues": ["no cpp file"]}

    result = subprocess.run(
        [sys.executable, str(_REVIEWER), family_slug,
         "--csil", str(csil_path),
         "--cpp", str(cpp_path),
         "--type", review_type],
        capture_output=True, text=True, timeout=120,
    )

    issues = []
    for line in result.stderr.splitlines():
        if line.strip():
            issues.append(line.strip())
    # Extract issues from stdout too
    in_issues = False
    for line in result.stdout.splitlines():
        if "  - " in line:
            issues.append(line.strip())
        if "Verdict:" in line:
            verdict_line = line

    verdict = "PASS"
    if result.returncode == 1:
        verdict = "WARN"
    elif result.returncode == 2:
        verdict = "FAIL"

    return {
        "family": family_slug,
        "verdict": verdict,
        "issues": issues,
        "exit_code": result.returncode,
    }


def run_batch(assembly_name: str = "System.Private.CoreLib",
              review_type: str = "codegen",
              families: list[str] | None = None) -> dict:
    """Run codegen review for all families."""
    assembly_dir = _VERIFICATION_BASE / assembly_name
    if families is None:
        families = sorted([
            d.name for d in assembly_dir.iterdir()
            if d.is_dir() and (d / "capability-family-contract.json").exists()
        ])

    results = []
    counts = {"PASS": 0, "WARN": 0, "FAIL": 0, "SKIP": 0}

    print(f"Batch codegen review ({review_type}) — {len(families)} families")
    print(f"Assembly: {assembly_name}")
    print(f"{'Family':35s} {'Verdict':>8s}")
    print("-" * 45)

    t0 = time.time()
    for slug in families:
        r = run_one_family(slug, assembly_name, review_type)
        results.append(r)
        counts[r["verdict"]] = counts.get(r["verdict"], 0) + 1
        print(f"  {slug:35s} {r['verdict']:>8s}")
    elapsed = time.time() - t0

    print(f"\nResults: {counts['PASS']} PASS, {counts['WARN']} WARN, {counts['FAIL']} FAIL, {counts['SKIP']} SKIP")
    print(f"Time: {elapsed:.1f}s")

    # Show FAIL families with their first issue
    failed = [r for r in results if r["verdict"] == "FAIL"]
    if failed:
        print(f"\nFAILED families ({len(failed)}):")
        for r in failed:
            first_issue = r["issues"][0] if r["issues"] else "(no details)"
            print(f"  {r['family']}: {first_issue}")

    return {
        "assembly": assembly_name,
        "reviewType": review_type,
        "results": results,
        "counts": counts,
        "totalFamilies": len(families),
        "elapsedSeconds": round(elapsed, 1),
    }


def main():
    import argparse
    parser = argparse.ArgumentParser(description="Batch codegen review runner")
    parser.add_argument("--families", nargs="*", help="Families to process (default: all)")
    parser.add_argument("--assembly", default="System.Private.CoreLib",
                        help="Assembly name")
    parser.add_argument("--type", default="codegen", choices=REVIEW_TYPES,
                        help="Review type")
    parser.add_argument("--output", help="Output report path")
    args = parser.parse_args()

    report = run_batch(args.assembly, args.type, args.families)

    if args.output:
        output_path = Path(args.output)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(
            json.dumps(report, indent=2, ensure_ascii=False) + "\n",
            encoding="utf-8")
        print(f"\nReport written to: {output_path}")


if __name__ == "__main__":
    main()
