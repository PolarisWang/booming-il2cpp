"""Update batch report with fixes verified this session, then generate dashboard."""
from __future__ import annotations

import json
import subprocess
import sys
import time
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parent
_CORE_PACKAGE = _REPO_ROOT / "testing" / "foundation-dll"
if str(_CORE_PACKAGE) not in sys.path:
    sys.path.insert(0, str(_CORE_PACKAGE))

_OLD_PIPELINE = _REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll"
if str(_OLD_PIPELINE) not in sys.path:
    sys.path.insert(0, str(_OLD_PIPELINE))

from _core.python.models import UnifiedReport

_TESTING_ROOT = _CORE_PACKAGE.parent  # testing/

cli_script = _CORE_PACKAGE / "_core" / "python" / "cli.py"

# Run each remaining failed family and build a fresh report
FAMILIES_TO_RERUN = [
    # Families with "entry.exe build failed" - likely transient CMake race
    "array-indexing-copy",
    "collections-generic-core",
    "delegate-core-invocation",
    "enum-parsing",
    "math-numerics",
    "reflection-assembly",
    "reflection-field-property",
    "reflection-member-complete",
    # Known reflection fact failures (AOT limitation)
    "reflection-activation",
    "reflection-member-basics",
    "reflection-type",
    "type-runtime-handles",
    # Already passing families (to verify)
    "threading-monitor-interlocked",
    "rcw-basic",
]

# First, run families sequentially and collect results
report = UnifiedReport(assembly="System.Private.CoreLib")

for i, slug in enumerate(FAMILIES_TO_RERUN):
    print(f"[{i+1}/{len(FAMILIES_TO_RERUN)}] {slug}...", flush=True)
    t0 = time.time()
    result = subprocess.run(
        [sys.executable, str(cli_script), slug],
        capture_output=True, text=True, timeout=7200,
        cwd=str(_REPO_ROOT),
    )
    elapsed = time.time() - t0

    # Try to extract FINAL_REPORT JSON
    found = False
    for line in result.stdout.splitlines():
        if line.startswith("FINAL_REPORT:"):
            try:
                r = json.loads(line[len("FINAL_REPORT:"):])
                report.add_result(r)
                status = r.get("overall_status", "?")
                cov = r.get("coverage", {})
                print(f"  -> {status} ({cov.get('stagesPassed', 0)}/{cov.get('stagesTotal', 0)}) [{elapsed:.0f}s]", flush=True)
                found = True
            except json.JSONDecodeError:
                pass
            break

    if not found:
        # Fallback: check exit code
        passed = result.returncode == 0
        print(f"  -> {'passed' if passed else 'failed'} (exit={result.returncode}) [{elapsed:.0f}s]", flush=True)

# Write report
report_path = _TESTING_ROOT / "results" / "batch-report-final.json"
report_dict = report.to_dict()
report_path.write_text(json.dumps(report_dict, ensure_ascii=False, indent=2), encoding="utf-8")
total = report_dict.get("total_families", len(report_dict.get("results", [])))
passed = report_dict.get("passed", 0)
failed = report_dict.get("failed", 0)
print(f"\nReport: {total} families, {passed} passed, {failed} failed")

# Generate dashboard
sys.path.insert(0, str(dashboard_dir))
import importlib.util
spec = importlib.util.spec_from_file_location("deep_dashboard", dashboard_dir / "deep_dashboard.py")
deep_dashboard = importlib.util.module_from_spec(spec)
spec.loader.exec_module(deep_dashboard)

dashboard_html = deep_dashboard.generate_html(deep_dashboard.load_batch_report(report_path))
dashboard_path = _TESTING_ROOT / "results" / "deep-dashboard-final.html"
dashboard_path.write_text(dashboard_html, encoding="utf-8")
print(f"Dashboard: {dashboard_path}")
