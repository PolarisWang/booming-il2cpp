"""Batch re-run for failed families + generate dashboard."""
from __future__ import annotations

import json
import subprocess
import sys
import time
from pathlib import Path

_HERE = Path(__file__).resolve().parent
_CORE_PACKAGE = _HERE.parents[1]  # testing/foundation-dll/
if str(_CORE_PACKAGE) not in sys.path:
    sys.path.insert(0, str(_CORE_PACKAGE))
if str(_CORE_PACKAGE.parent) not in sys.path:
    sys.path.insert(0, str(_CORE_PACKAGE.parent))

_OLD_PIPELINE = _HERE.parents[4] / "build" / "toolchains" / "run" / "testing" / "foundation_dll"
if str(_OLD_PIPELINE) not in sys.path:
    sys.path.insert(0, str(_OLD_PIPELINE))

from _core.python.models import UnifiedReport

_FAMILIES_DIR = _CORE_PACKAGE.parents[1] / "testing" / "foundation-dll" / "System.Private.CoreLib"

SLUGS_TO_RERUN = [
    # Codegen failures (likely transient CMake race)
    "array-indexing-copy",
    "collections-generic-core",
    "delegate-core-invocation",
    "enum-parsing",
    "math-numerics",
    "reflection-assembly",
    "reflection-field-property",
    "reflection-member-complete",
    # Genuine codegen failures (need to verify fix)
    "codegen-edge-cases",
    "snapshot-prover",
    "object-equality-identity",
    "threading-monitor-interlocked",
    # JIT codegen failure
    "rcw-basic",
]

print(f"Re-running {len(SLUGS_TO_RERUN)} families...")
print()

report = UnifiedReport(assembly="System.Private.CoreLib")

for i, slug in enumerate(SLUGS_TO_RERUN):
    print(f"[{i+1}/{len(SLUGS_TO_RERUN)}] {slug}...")
    sys.stdout.flush()

    t0 = time.time()
    result = subprocess.run(
        [sys.executable, str(_HERE / "cli.py"), slug],
        capture_output=True, text=True, timeout=7200,
        cwd=str(_HERE.parents[1]),
    )
    elapsed = time.time() - t0

    # Extract UnifiedReport JSON from stdout tail
    stdout = result.stdout
    report_json = None
    for line in stdout.splitlines():
        if line.startswith("FINAL_REPORT:"):
            try:
                report_json = json.loads(line[len("FINAL_REPORT:"):])
            except json.JSONDecodeError:
                pass
            break

    if report_json:
        r = report_json
        status = r.get("overall_status", "?")
        stages = r.get("stages", {})
        passed_stages = sum(1 for s in stages.values() if isinstance(s, dict) and s.get("status") == "passed")
        failed_stages = [(k, v.get("summary", "")[:100]) for k, v in stages.items()
                        if isinstance(v, dict) and v.get("status") == "failed"]
        print(f"  Status: {status}, Stages: {passed_stages}/{len(stages)}, Elapsed: {elapsed:.0f}s")
        if failed_stages:
            for fs, summary in failed_stages:
                print(f"    FAILED: {fs}: {summary}")
        report.add_result(r)
    else:
        # Fall back to stdout tail analysis
        passed = "Fact" in stdout and "passed" in stdout
        failed = "Fact" in stdout and "failed" in stdout
        print(f"  No FINAL_REPORT found. Stdout tail: {stdout[-200:]}")
        print(f"  Exit code: {result.returncode}")
        report.add_result({"slug": slug, "status": "passed" if passed else "failed"})

    print()
    sys.stdout.flush()

print("All re-runs complete.")
print(f"Overall: {len(SLUGS_TO_RERUN)} families")
report_path = _CORE_PACKAGE.parent / "results" / "batch-report-ree.json"
report_path.write_text(json.dumps(report.to_dict(), ensure_ascii=False, indent=2), encoding="utf-8")
print(f"Report written to: {report_path}")

# Generate dashboard
from deep_dashboard import generate_html, load_batch_report
dashboard_html = generate_html(load_batch_report(report_path))
dashboard_path = _CORE_PACKAGE.parent / "results" / "deep-dashboard-ree.html"
dashboard_path.write_text(dashboard_html, encoding="utf-8")
print(f"Dashboard written to: {dashboard_path}")
